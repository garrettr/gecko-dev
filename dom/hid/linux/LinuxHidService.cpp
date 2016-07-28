/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "LinuxHidService.h"

#include "HidService.h"
#include "nsIHidService.h"
#include "HidDeviceInfo.h"
#include "HidConnection.h"
#include "LinuxHidConnection.h"

#include "nsArrayEnumerator.h"
#include "nsCOMArray.h"
#include "nsCOMPtr.h"
#include "nsError.h"
#include "nsISimpleEnumerator.h"

#include <vector>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../../gamepad/linux/udev.h" // TODO(adma): properly export udev

namespace mozilla {
namespace dom {

extern LazyLogModule gHidServiceLog;
#define LOG(args) MOZ_LOG(gHidServiceLog, mozilla::LogLevel::Debug, args)
#define LOG_ENABLED() MOZ_LOG_TEST(gHidServiceLog, mozilla::LogLevel::Debug)

already_AddRefed<nsIHidDeviceInfo>
LinuxHidService::GetHidDeviceInfo(struct udev_device *dev) {

  char *saveptr = NULL;
  char *line;
  char *key;
  char *value;

  uint16_t bus_type;

  nsAutoCString deviceId;
  uint16_t vendorId;
  uint16_t productId;
  nsAutoString manufacturerName;
  nsAutoString productName;
  nsAutoString serialNumber;
  uint16_t usagePage = 0;
  uint16_t usage = 0;
  uint16_t maxInputReportSize = 0;
  uint16_t maxOutputReportSize = 0;
  uint16_t maxFeatureReportSize = 0;

  int found_id = 0;
  int found_serial = 0;
  int found_name = 0;

  LOG(("Found device %s", mUdev.udev_device_get_devnode(dev)));

  deviceId.AssignASCII(mUdev.udev_device_get_syspath(dev));

  struct udev_device* parent =
    mUdev.udev_device_get_parent_with_subsystem_devtype(dev,
                                                        "hid",
                                                        nullptr);

  const char *uevent = mUdev.udev_device_get_sysattr_value(parent, "uevent");
  char *tmp = strdup(uevent);

  /* Start by parsing a hid uvent line. It looks like this:
   *
   *     DRIVER=hid-generic
   *     HID_ID=XXXX:YYYYYYYY:ZZZZZZZZ
   *     HID_NAME=
   *     HID_PHYS=usb-0000:00:14.0-3.2.4/input0
   *     HID_UNIQ=
   *     MODALIAS=hid:bXXXXg0001vYYYYYYYYpZZZZZZZZ
   *
   */

  line = strtok_r(tmp, "\n", &saveptr);
  while (line != NULL) {
    /* line: "KEY=value" */
    key = line;
    value = strchr(line, '=');
    if (!value) {
      goto next_line;
    }
    *value = '\0';
    value++;

    if (strcmp(key, "HID_ID") == 0) {
      /*
       *        type vendor   product
       * HID_ID=0003:000005AC:00008242
       */
      int ret = sscanf(value, "%hx:%hx:%hx", &bus_type, &vendorId, &productId);
      if (ret == 3) {
        found_id = 1;
      }
    } else if (strcmp(key, "HID_NAME") == 0) {
      productName.AssignASCII(value, strlen(value));
      found_name = 1;
    }  else if (strcmp(key, "HID_UNIQ") == 0) {
      serialNumber.AssignASCII(value, strlen(value));
      found_serial = 1;
    }

    next_line:
    line = strtok_r(NULL, "\n", &saveptr);
  }

  free(tmp);

  /* Next parse the usb parent device */
  parent =
      mUdev.udev_device_get_parent_with_subsystem_devtype(parent,
                                                          "usb",
                                                          "usb_device");

  /*LOG((mUdev.udev_device_get_sysattr_value(parent, "idVendor")));
  LOG((mUdev.udev_device_get_sysattr_value(parent, "idProduct")));
  LOG((mUdev.udev_device_get_sysattr_value(parent, "manufacturer")));
  LOG((mUdev.udev_device_get_sysattr_value(parent, "product")));
  LOG((mUdev.udev_device_get_sysattr_value(parent, "uevent")));*/

  tmp = (char *) mUdev.udev_device_get_sysattr_value(parent, "manufacturer");
  manufacturerName.AssignASCII(tmp, strlen(tmp));

  struct udev_device *parent_device = mUdev.udev_device_get_parent(dev);

  // TODO(adma): surely there is a Mozilla fancy way of creating a
  // path and reading a file. Find it and use it
  nsCString report_descriptor_path;
  report_descriptor_path.AssignASCII(mUdev.udev_device_get_syspath(parent_device));
  report_descriptor_path.AppendASCII("/report_descriptor", strlen("/report_descriptor"));
  char *report_descriptor_cpath = ToNewCString(report_descriptor_path);

  uint8_t buf[512];
  const size_t buf_size = 512;
  size_t len;
  size_t size = 0;
  std::vector<uint8_t> report_descriptor;

  FILE *fp = fopen(report_descriptor_cpath, "rb");
  while ((len = fread(buf, 1, buf_size, fp)) > 0) {
    report_descriptor.insert(report_descriptor.end(), buf, buf + len);

    size += len;
  }
  fclose(fp);

  // TODO(adma): should we actually parse the report descriptor right
  // away or just carry it around and parse it later, when needed?

  // TODO(adma): get usage page, usage, input-, output- and feature-
  // report size by looking at the report descriptor. Hack it for now
  if (manufacturerName.EqualsASCII("Yubico")) {
    usagePage = 0xf1d0;
    usage = 0x0001;
    maxInputReportSize = 0x00ff;
    maxOutputReportSize = 0x00ff;
    maxFeatureReportSize = 0x00ff;
    LOG(("%s is a U2F device", mUdev.udev_device_get_devnode(dev)));
  }

  nsCOMPtr<nsIHidDeviceInfo> deviceInfo =
    new HidDeviceInfo(
                      deviceId,
                      vendorId,
                      productId,
                      manufacturerName,
                      productName,
                      serialNumber,
                      usagePage,
                      usage,
                      maxInputReportSize,
                      maxOutputReportSize,
                      maxFeatureReportSize);

  return deviceInfo.forget();

}

void
LinuxHidService::NativeInit() {

  if (!mUdev) {
    LOG(("libudev is not initalized"));
    return;
  }

}

void
LinuxHidService::NativeShutdown() {
  LOG(("TODO"));
}

nsresult
LinuxHidService::NativeGetDevices(GetDevicesCallbackHandle aCallback) {

  nsCOMArray<nsIHidDeviceInfo> deviceInfoArray;

  struct udev_enumerate* en = mUdev.udev_enumerate_new(mUdev.udev);
  mUdev.udev_enumerate_add_match_subsystem(en, "hidraw");
  mUdev.udev_enumerate_scan_devices(en);

  struct udev_list_entry* dev_list_entry;
  for (dev_list_entry = mUdev.udev_enumerate_get_list_entry(en);
       dev_list_entry != nullptr;
       dev_list_entry = mUdev.udev_list_entry_get_next(dev_list_entry)) {

    const char* path = mUdev.udev_list_entry_get_name(dev_list_entry);
    struct udev_device* dev = mUdev.udev_device_new_from_syspath(mUdev.udev, path);

    nsCOMPtr<nsIHidDeviceInfo> deviceInfo = GetHidDeviceInfo(dev);
    deviceInfoArray.AppendObject(deviceInfo);

    mUdev.udev_device_unref(dev);
  }

  mUdev.udev_enumerate_unref(en);

  NS_DispatchToMainThread(NS_NewRunnableFunction(
    [aCallback, deviceInfoArray] () mutable -> void {
      // nsISimpleEnumerator is not thread-safe, so it has to be constructed on
      // the main thread.
      nsCOMPtr<nsISimpleEnumerator> deviceEnumerator;
      nsresult rv = NS_NewArrayEnumerator(getter_AddRefs(deviceEnumerator),
                                          deviceInfoArray);
      aCallback->Callback(rv, deviceEnumerator);
    }
  ));

  return NS_OK;
}

nsresult
LinuxHidService::NativeConnect(nsIHidDeviceInfo* aDeviceInfo,
                               ConnectCallbackHandle aCallback) {

  LOG(("In LinuxHidService::NativeConnect()"));
  MOZ_ASSERT(aDeviceInfo);
  MOZ_ASSERT(aCallback);

  nsresult rv = NS_OK;

  nsAutoCString deviceId;
  rv = aDeviceInfo->GetDeviceId(deviceId);
  NS_ENSURE_SUCCESS(rv, rv);

  LOG(("Connecting to device %s", deviceId.get()));

  // Create the Device
  struct udev_device * deviceRef = mUdev.udev_device_new_from_syspath(mUdev.udev, deviceId.get());
  if (deviceRef == NULL) {
    LOG(("Failed to Create device"));
    aCallback->Callback(NS_ERROR_FAILURE, nullptr);
  }

  // Open the HID device
  int fd = open(mUdev.udev_device_get_devnode(deviceRef), O_RDWR);
  if (fd < 0) {
    LOG(("Failed to Open device"));
    aCallback->Callback(NS_ERROR_FAILURE, nullptr);
  }

  // Create the LinuxHidConnection
  nsCOMPtr<nsIHidConnection> conn = new LinuxHidConnection(fd,
                                                           aDeviceInfo);
  if (!conn) {
    LOG(("Failed to create new LinuxHidConnection"));
    aCallback->Callback(NS_ERROR_FAILURE, nullptr);
  }

  NS_DispatchToMainThread(NS_NewRunnableFunction(
    [aCallback, conn] () mutable -> void {
      LOG(("About to call aCallback on main thread..."));
      aCallback->Callback(NS_OK, conn);
    }
  ));

  return NS_OK;
}

} // namespace dom
} // namespace mozilla
