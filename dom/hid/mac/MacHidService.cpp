/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "MacHidService.h"

#include "HidService.h"
#include "nsIHidService.h"
#include "HidDeviceInfo.h"

#include "nsArrayEnumerator.h"
#include "nsCOMArray.h"
#include "nsCOMPtr.h"
#include "nsISimpleEnumerator.h"

namespace mozilla {
namespace dom {

extern LazyLogModule gHidServiceLog;
#define LOG(args) MOZ_LOG(gHidServiceLog, mozilla::LogLevel::Debug, args)
#define LOG_ENABLED() MOZ_LOG_TEST(gHidServiceLog, mozilla::LogLevel::Debug)

void
MacHidService::NativeInit()
{
  if (mManager != nullptr) {
    return;
  }

  IOHIDManagerRef manager = IOHIDManagerCreate(kCFAllocatorDefault,
                                                  kIOHIDOptionsTypeNone);
  // TODO: check success, if IOHIDManagerCreate fails manager == nullptr

  // Match all HID devices
  IOHIDManagerSetDeviceMatching(manager, NULL);

  // TODO: register various callbacks
  // DeviceAddedCallback, DeviceRemovedCallback, InputValueChangedCallback

  IOHIDManagerScheduleWithRunLoop(manager,
                                  CFRunLoopGetCurrent(),
                                  kCFRunLoopDefaultMode);
  IOReturn rv = IOHIDManagerOpen(manager, kIOHIDOptionsTypeNone);
  if (rv != kIOReturnSuccess) {
    CFRelease(manager);
    return;
  }

  mManager = manager;
}

void
MacHidService::NativeShutdown()
{
  IOHIDManagerRef manager = mManager;
  if (manager) {
    IOHIDManagerClose(manager, 0);
    CFRelease(manager);
    mManager = nullptr;
  }
}

bool
TryGetHidIntProperty(IOHIDDeviceRef device,
                     CFStringRef key,
                     int32_t* result)
{
  CFNumberRef ref = (CFNumberRef) IOHIDDeviceGetProperty(device, key);
  return ref && CFNumberGetValue(ref, kCFNumberSInt32Type, result);
}

int32_t
GetHidIntProperty(IOHIDDeviceRef device,
                  CFStringRef key)
{
  int32_t value;
  if (TryGetHidIntProperty(device, key, &value)) {
    return value;
  }
  return 0;
}

void
CFStringToMozString(CFStringRef cfstring,
                    nsAString& converted)
{
  AutoTArray<UniChar, 32> buffer;
  int size = CFStringGetLength(cfstring);
  buffer.SetLength(size + 1);
  CFRange range = CFRangeMake(0, size);
  CFStringGetCharacters(cfstring, range, buffer.Elements());
  buffer[size] = 0;
  converted.Assign(reinterpret_cast<char16_t*>(buffer.Elements()));
}

bool
TryGetHidStringProperty(IOHIDDeviceRef device,
                        CFStringRef key,
                        nsAString& result)
{
  CFStringRef ref = (CFStringRef) IOHIDDeviceGetProperty(device, key);
  if (!ref) {
    return false;
  }
  CFStringToMozString(ref, result);
}

// XXX: having separate Get and TryGet functions seems redundant for string properties.
void
GetHidStringProperty(IOHIDDeviceRef device,
                     CFStringRef key,
                     nsAString& result)
{
  TryGetHidStringProperty(device, key, result);
}

void
GetDeviceIdFromIOHIDDeviceRef(IOHIDDeviceRef device,
                              nsACString& result)
{
  // There are (at least) two options for getting a unique per-HID-interface
  // identifier on Mac: IORegistryEntryGetPath and
  // IORegistryEntryGetRegistryEntryID. Chrome uses the RegistryEntryID, which
  // is a uint64_t, but as far as I can tell the RegistryEntryPath, a C string,
  // is equivalent. We're using the path for now because it allows us to use a
  // consistent type for the deviceId cross-platform.
  kern_return_t res;
  io_string_t path;
  // IOHIDDeviceGetService is only available on Mac OS 10.6+, but that's ok
  // because Firefox is only supported on 10.6+.
  const io_service_t ioService = IOHIDDeviceGetService(device);
  res = IORegistryEntryGetPath(ioService, kIOServicePlane, path);
  if (res == KERN_SUCCESS) {
    result.Assign(path);
  }
  // TODO: Should we indicate if this failed? Best thing to do is probably
  // return NS_FAILED or similar, and pass the error condition up the stack so
  // the device does not get added to the enumerator.
}

already_AddRefed<nsIHidDeviceInfo>
GetHidDeviceInfoFromIOHIDDeviceRef(IOHIDDeviceRef device)
{
  nsAutoCString deviceId;
  GetDeviceIdFromIOHIDDeviceRef(device, deviceId);

  // Int properties
  // TODO: int16_t vs. int32_t?
  int32_t vendorId, productId, usagePage, usage, maxInputReportSize,
          maxOutputReportSize, maxFeatureReportSize;
  vendorId = GetHidIntProperty(device, CFSTR(kIOHIDVendorIDKey));
  productId = GetHidIntProperty(device, CFSTR(kIOHIDProductIDKey));
  // TODO: Do we need to disambiguate the usages for interfaces vs devices here?
  // See: "Matching HID Devices" on https://developer.apple.com/library/mac/documentation/DeviceDrivers/Conceptual/HID/new_api_10_5/tn2187.html
  usagePage = GetHidIntProperty(device, CFSTR(kIOHIDPrimaryUsagePageKey));
  usage = GetHidIntProperty(device, CFSTR(kIOHIDPrimaryUsageKey));
  maxInputReportSize = GetHidIntProperty(device, CFSTR(kIOHIDMaxInputReportSizeKey));
  maxOutputReportSize = GetHidIntProperty(device, CFSTR(kIOHIDMaxOutputReportSizeKey));
  maxFeatureReportSize = GetHidIntProperty(device, CFSTR(kIOHIDMaxFeatureReportSizeKey));

  // String properties
  nsAutoString manufacturerName, productName, serialNumber;
  GetHidStringProperty(device, CFSTR(kIOHIDManufacturerKey), manufacturerName);
  GetHidStringProperty(device, CFSTR(kIOHIDProductKey), productName);
  GetHidStringProperty(device, CFSTR(kIOHIDSerialNumberKey), serialNumber);

  nsCOMPtr<nsIHidDeviceInfo> deviceInfo = new HidDeviceInfo(
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

nsresult
MacHidService::NativeGetDevices(GetDevicesCallbackHandle aCallback) {
  LOG(("In MacHidService::NativeGetDevices()"));
  MOZ_ASSERT(mManager);
  MOZ_ASSERT(aCallback);

  // TODO: Do I need to process_pending_events() like hidapi does?
  // See c43255b4 in hidapi
  IOHIDManagerSetDeviceMatching(mManager, NULL);
  CFSetRef deviceSet = IOHIDManagerCopyDevices(mManager);

  // XXX: for now, we'll do this the CFSetGetValues way, same as hidapi.
  // Consider using CFSetApplyFunction instead:
  // https://developer.apple.com/library/mac/documentation/CoreFoundation/Reference/CFSetRef/index.html#//apple_ref/c/func/CFSetApplyFunction
  CFIndex numDevices;
  numDevices = CFSetGetCount(deviceSet);
  IOHIDDeviceRef *deviceArray = (IOHIDDeviceRef *) calloc(numDevices, sizeof(IOHIDDeviceRef));
  CFSetGetValues(deviceSet, (const void **) deviceArray);
  
  nsCOMArray<nsIHidDeviceInfo> deviceInfoArray;
  for (uint32_t i = 0; i < numDevices; i++) {
    IOHIDDeviceRef device = deviceArray[i];
    nsCOMPtr<nsIHidDeviceInfo> deviceInfo = GetHidDeviceInfoFromIOHIDDeviceRef(device);
    deviceInfoArray.AppendObject(deviceInfo);
  }

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

} // namespace dom
} // namespace mozilla
