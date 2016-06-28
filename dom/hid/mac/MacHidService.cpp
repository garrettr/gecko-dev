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

nsresult
MacHidService::NativeGetDevices(GetDevicesCallbackHandle aCallback) {
  // Stub, just proving out the OOP architecture right now
  LOG(("In MacHidService::NativeGetDevices()"));
  MOZ_ASSERT(mManager);

  nsresult rv = NS_OK;

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

    // XXX: do we need this? from hidapi mac/hid.c
    /*
    if (!device) {
      continue;
    }
    */

    // Gather information for HidDeviceInfo
    /*
    CFNumberRef vendorIdRef =
      (CFNumberRef) IOHIDDeviceGetProperty(device, CFSTR(kIOHIDVendorIDKey));
    CFNumberRef productIdRef =
      (CFNumberRef) IOHIDDeviceGetProperty(device, CFSTR(kIOHIDProductIDKey));
    int vendorId, productId;
    CFNumberGetValue(vendorIdRef, kCFNumberIntType, &vendorId);
    CFNumberGetValue(productIdRef, kCFNumberIntType, &productId);
    */

    nsCOMPtr<nsIHidDeviceInfo> deviceInfo = new HidDeviceInfo();
    deviceInfoArray.AppendObject(deviceInfo);
  }

  nsCOMPtr<nsISimpleEnumerator> deviceEnumerator;
  rv = NS_NewArrayEnumerator(getter_AddRefs(deviceEnumerator),
                             deviceInfoArray);
  NS_ENSURE_SUCCESS(rv, rv);

  NS_DispatchToMainThread(NS_NewRunnableFunction(
    [aCallback, deviceEnumerator] () mutable -> void {
      aCallback->Callback(NS_OK, deviceEnumerator);
    }
  ));
  return NS_OK;
}

} // namespace dom
} // namespace mozilla
