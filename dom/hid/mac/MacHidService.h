/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef MacHidService_h___
#define MacHidService_h___

#include "HidService.h"
#include "nsIHidService.h"

#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/hid/IOHIDBase.h>
#include <IOKit/hid/IOHIDKeys.h>
#include <IOKit/hid/IOHIDManager.h>
#include <IOKit/IOKitLib.h>

namespace mozilla {
namespace dom {

class MacHidService final : public HidService
{
  public:
    //NS_DECL_ISUPPORTS_INHERITED
    //
    // XXX: I thought I needed this, because I'm inheriting from a class that
    // implements nsISupports, but it appeared to cause a linker error:
    //
    // 0:50.82   "vtable for mozilla::dom::MacHidService", referenced from:
    // 0:50.82       mozilla::dom::MacHidService::MacHidService() in Unified_cpp_dom_hid_mac0.o
    // 0:50.83   NOTE: a missing vtable usually means the first non-inline virtual member function has no definition.

    MacHidService() : mManager(nullptr) {};

  private:
    ~MacHidService() {};

    virtual void NativeInit() override;
    virtual void NativeShutdown() override;
    virtual nsresult NativeGetDevices(GetDevicesCallbackHandle aCallback) override;
    virtual nsresult NativeConnect(nsIHidDeviceInfo* aDeviceInfo,
                                   ConnectCallbackHandle aCallback) override;

    IOHIDManagerRef mManager;
};

} // namespace dom
} // namespace mozilla

#endif /* MacHidService_h___ */
