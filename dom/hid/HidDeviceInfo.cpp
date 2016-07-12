/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "HidDeviceInfo.h"
#include "nsIHidDeviceInfo.h"

#include "mozilla/ModuleUtils.h"

#include "nsAutoPtr.h"
#include "nsCOMPtr.h"
#include "nsDebug.h"
#include "nsError.h"

namespace mozilla {
namespace dom {

NS_IMPL_ISUPPORTS(HidDeviceInfo, nsIHidDeviceInfo)

HidDeviceInfo::HidDeviceInfo()
{
}

HidDeviceInfo::HidDeviceInfo(const nsACString& aDeviceId,
                             uint16_t aVendorId,
                             uint16_t aProductId,
                             const nsAString& aManufacturerName,
                             const nsAString& aProductName,
                             const nsAString& aSerialNumber,
                             uint16_t aUsagePage,
                             uint16_t aUsage,
                             uint32_t aMaxInputReportSize,
                             uint32_t aMaxOutputReportSize,
                             uint32_t aMaxFeatureReportSize)
  : mDeviceId(aDeviceId),
    mVendorId(aVendorId),
    mProductId(aProductId),
    mManufacturerName(aManufacturerName),
    mProductName(aProductName),
    mSerialNumber(aSerialNumber),
    mUsagePage(aUsagePage),
    mUsage(aUsage),
    mMaxInputReportSize(aMaxInputReportSize),
    mMaxOutputReportSize(aMaxOutputReportSize),
    mMaxFeatureReportSize(aMaxFeatureReportSize)
{
}

HidDeviceInfo::~HidDeviceInfo()
{
}

NS_IMETHODIMP
HidDeviceInfo::GetDeviceId(nsACString& aDeviceId)
{
  aDeviceId = mDeviceId;
  return NS_OK;
}

NS_IMETHODIMP
HidDeviceInfo::GetVendorId(uint16_t* aVendorId)
{
  *aVendorId = mVendorId;
  return NS_OK;
}

NS_IMETHODIMP
HidDeviceInfo::GetProductId(uint16_t* aProductId)
{
  *aProductId = mProductId;
  return NS_OK;
}

NS_IMETHODIMP
HidDeviceInfo::GetManufacturerName(nsAString &aManufacturerName)
{
  aManufacturerName = mManufacturerName;
  return NS_OK;
}

NS_IMETHODIMP
HidDeviceInfo::GetProductName(nsAString &aProductName)
{
  aProductName = mProductName;
  return NS_OK;
}

NS_IMETHODIMP
HidDeviceInfo::GetSerialNumber(nsAString &aSerialNumber)
{
  aSerialNumber = mSerialNumber;
  return NS_OK;
}

NS_IMETHODIMP
HidDeviceInfo::GetUsagePage(uint16_t* usagePage)
{
  *usagePage = mUsagePage;
  return NS_OK;
}

NS_IMETHODIMP
HidDeviceInfo::GetUsage(uint16_t* usage)
{
  *usage= mUsage;
  return NS_OK;
}

NS_IMETHODIMP
HidDeviceInfo::GetMaxInputReportSize(uint32_t* aMaxInputReportSize)
{
  *aMaxInputReportSize = mMaxInputReportSize;
  return NS_OK;
}

NS_IMETHODIMP
HidDeviceInfo::GetMaxOutputReportSize(uint32_t* aMaxOutputReportSize)
{
  *aMaxOutputReportSize = mMaxOutputReportSize;
  return NS_OK;
}

NS_IMETHODIMP
HidDeviceInfo::GetMaxFeatureReportSize(uint32_t* aMaxFeatureReportSize)
{
  *aMaxFeatureReportSize = mMaxFeatureReportSize;
  return NS_OK;
}

// 3caf9413-dd1b-45d7-a2e9-b1f972a40a77
#define HIDDEVICEINFO_CID                               \
  { 0x3caf9413,                                         \
    0xdd1b,                                             \
    0x45d7,                                             \
    { 0xa2, 0xe9, 0xb1, 0xf9, 0x72, 0xa4, 0x0a, 0x77 }  \
  }
#define HIDDEVICEINFO_CONTRACTID "@mozilla.org/dom/hid-device-info;1"

// TODO This should only be construct-able from C++, it doesn't make sense to do it from JS
// Well, it potentially might, for testing purposes, but not in normal use.
// What invocations of macros do I need to chant?
NS_GENERIC_FACTORY_CONSTRUCTOR(HidDeviceInfo)

NS_DEFINE_NAMED_CID(HIDDEVICEINFO_CID);

static const mozilla::Module::CIDEntry kHidDeviceInfoCIDs[] = {
  { &kHIDDEVICEINFO_CID, false, nullptr, HidDeviceInfoConstructor },
  { nullptr }
};

static const mozilla::Module::ContractIDEntry kHidDeviceInfoContracts[] = {
  { HIDDEVICEINFO_CONTRACTID, &kHIDDEVICEINFO_CID },
  { nullptr }
};

static const mozilla::Module kHidDeviceInfoModule = {
  mozilla::Module::kVersion,
  kHidDeviceInfoCIDs,
  kHidDeviceInfoContracts,
  nullptr
};

NSMODULE_DEFN(HidDeviceInfoModule) = &kHidDeviceInfoModule;

} // namespace dom
} // namespace mozilla
