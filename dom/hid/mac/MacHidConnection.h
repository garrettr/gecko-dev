/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef MacHidConnection_h___
#define MacHidConnection_h___

#include "HidConnection.h"
#include "nsIHidConnection.h"
#include "nsIHidDeviceInfo.h"

#include "mozilla/Logging.h"

#include "nsCOMPtr.h"
#include "nsTArray.h"

#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/hid/IOHIDBase.h>
#include <IOKit/hid/IOHIDKeys.h>
#include <IOKit/hid/IOHIDManager.h>

namespace mozilla {
namespace dom {

class MacHidConnection final : public HidConnection
{
  public:
    MacHidConnection(IOHIDDeviceRef aDeviceRef,
                     nsIHidDeviceInfo* aDeviceInfo);

  private:
    ~MacHidConnection() {};

    virtual nsresult NativeDisconnect() override;

    static void InputReportCallback(void* context,
                                    IOReturn result,
                                    void* sender,
                                    IOHIDReportType type,
                                    uint32_t report_id,
                                    uint8_t* report_bytes,
                                    CFIndex report_length);

    IOHIDDeviceRef mDeviceRef;
    nsCOMPtr<nsIHidDeviceInfo> mDeviceInfo;
    nsTArray<uint8_t> mInputReportBuffer;
};

} // namespace dom
} // namespace mozilla

#endif /* MacHidConnection_h___ */
