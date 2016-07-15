/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "MacHidConnection.h"

namespace mozilla {
namespace dom {

extern LazyLogModule gHidServiceLog;
#define LOG(args) MOZ_LOG(gHidServiceLog, mozilla::LogLevel::Debug, args)
#define LOG_ENABLED() MOZ_LOG_TEST(gHidServiceLog, mozilla::LogLevel::Debug)

MacHidConnection::MacHidConnection(IOHIDDeviceRef aDeviceRef,
                                   nsIHidDeviceInfo* aDeviceInfo)
  : mDeviceRef(aDeviceRef),
    mDeviceInfo(aDeviceInfo)
{
  //nsresult rv = NS_OK;

  // TODO CFRunLoopGetCurrent vs CFRunLoopGetMain
  // We're on the worker thread now. Does it automatically have a run loop, or
  // do I have to start one?
  IOHIDDeviceScheduleWithRunLoop(mDeviceRef, CFRunLoopGetCurrent(),
      kCFRunLoopDefaultMode);
  
  uint32_t maxInputReportSize;
  mDeviceInfo->GetMaxInputReportSize(&maxInputReportSize);
  // TODO does maxInputReportSize include a byte for the report id, if one is used?
  // Chrome adds a byte if the device has a report id, see:
  // https://cs.chromium.org/chromium/src/device/hid/hid_connection_mac.cc?type=cs&sq=package:chromium&l=41
  mInputReportBuffer.SetCapacity(maxInputReportSize);

  IOHIDDeviceRegisterInputReportCallback(
      mDeviceRef,
      mInputReportBuffer.Elements(),
      mInputReportBuffer.Length(),
      &MacHidConnection::InputReportCallback,
      this);
}

void
MacHidConnection::InputReportCallback(void* context,
                                      IOReturn result,
                                      void* sender,
                                      IOHIDReportType type,
                                      uint32_t report_id,
                                      uint8_t* report_bytes,
                                      CFIndex report_length)
{
  MacHidConnection* connection = static_cast<MacHidConnection*>(context);
  if (result != kIOReturnSuccess) {
    LOG(("Failed to read input report"));
  } else {
    LOG(("Got an input report!"));
  }
}

} // namespace dom
} // namespace mozilla
