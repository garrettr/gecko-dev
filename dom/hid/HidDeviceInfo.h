/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=8 sts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef HidDeviceInfo_h___
#define HidDeviceInfo_h___

#include "nsIHidDeviceInfo.h"
#include "nsISupports.h"
#include "nsCOMPtr.h"
#include "nsString.h"

#include "hidapi.h"

namespace mozilla {
namespace dom {

class HidDeviceInfo final: public nsIHidDeviceInfo
{
  public:
    NS_DECL_ISUPPORTS
    NS_DECL_NSIHIDDEVICEINFO

    HidDeviceInfo();
    HidDeviceInfo(const struct hid_device_info* dev);

  private:
    ~HidDeviceInfo();

    nsCString mDeviceId;

    uint16_t mVendorId;
    uint16_t mProductId;

    nsString mManufacturerName;
    nsString mProductName;
    nsString mSerialNumber;

    uint16_t mUsagePage;
    uint16_t mUsage;
};

} // namespace dom
} // namespace mozilla

#endif /* HidDeviceInfo_h___ */
