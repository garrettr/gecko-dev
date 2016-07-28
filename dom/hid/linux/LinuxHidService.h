/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef LinuxHidService_h___
#define LinuxHidService_h___

#include "HidService.h"
#include "nsIHidService.h"

#include "mozilla/dom/udev.h"

namespace mozilla {
namespace dom {

using mozilla::udev_lib;

class LinuxHidService final : public HidService
{
  public:

    LinuxHidService() {};

  private:

    ~LinuxHidService() {};

    virtual void NativeInit() override;
    virtual void NativeShutdown() override;
    virtual nsresult NativeGetDevices(GetDevicesCallbackHandle aCallback) override;
    virtual nsresult NativeConnect(nsIHidDeviceInfo* aDeviceInfo,
                                   ConnectCallbackHandle aCallback) override;

    already_AddRefed<nsIHidDeviceInfo> GetHidDeviceInfo(struct udev_device *dev);

    udev_lib mUdev;
};

} // namespace dom
} // namespace mozilla

#endif /* LinuxHidService_h___ */
