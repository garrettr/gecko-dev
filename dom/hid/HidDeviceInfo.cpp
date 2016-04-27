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

#include "unicode/unistr.h"
#include "ICUUtils.h"

#include "hidapi.h"

// Try converting wchar_t to something sane on Mac OS X with iconv
//#include <iconv.h>

namespace mozilla {
namespace dom {

NS_IMPL_ISUPPORTS(HidDeviceInfo, nsIHidDeviceInfo)

HidDeviceInfo::HidDeviceInfo()
{
}

// Copied from ICUUtils.cpp, where it's ifdef'ed out for reasons unclear.
void
ICUStringToMozString(icu::UnicodeString& aICUString, nsAString& aMozString)
{
  // Both ICU's UnicodeString and Mozilla's nsAString use UTF-16, so we can
  // cast here.
  static_assert(sizeof(UChar) == 2 && sizeof(nsAString::char_type) == 2,
                "Unexpected character size - the following cast is unsafe");

  const nsAString::char_type* buf =
    (const nsAString::char_type*)aICUString.getTerminatedBuffer();
  aMozString.Assign(buf);

  NS_ASSERTION(aMozString.Length() == (uint32_t)aICUString.length(),
               "Conversion failed");
}

void
HIDAPIStringToMozillaString(const wchar_t* aHIDAPIStr,
                            nsAString& aMozStr)
{
  // TODO Make sure this works on all platforms
  // WindowsStringUtils::safe_wcstombs might be useful?
  // froydnj: Might just be able to pass to nsString constructor on Windows
  // On Mac and Windows, wchar_t is 4 bytes *and* the encoding is not specified
  // by the standard.
  // However, maybe we can make a safe assertion about this based on what's in the HIDAPI code.

  // On Linux, it incorrectly assumes it's UTF-8. However, we can probably
  // convert it with iconv using the wchar_t converter (fingers crossed).

  /*
  int rv;
  iconv_t ic;
  ic = iconv_open("UTF−16", "wchar_t"); // or, for Mac, UTF-32LE
  if (ic == (iconv_t)-1) {
    //LOG(("iconv_open failed"));
    return;
  }

  size_t icrv;
  size_t len = wcslen(aHIDAPIStr);
  char* converted = (char*)calloc(len+1, sizeof(wchar_t));
  icrv = iconv(ic, &aHIDAPIStr, &len, &converted, &len);
	if (icrv == (size_t)-1) {
		//LOG(("iconv() failed"));
    iconv_close(ic);
    return;
	}
  icrv = iconv_close(ic);
  if (icrv == -1) {
    //LOG(("iconv_close() failed"));
  }

  aMozStr.Assign(converted);
  */

  /*
  size_t len = wcslen(aHIDAPIStr);
  char* mbs;
  mbs = (char*) calloc(len, sizeof(wchar_t));
  size_t rv = 0;
  rv = wcstombs(mbs, aHIDAPIStr, len*sizeof(wchar_t));
  aMozStr.Assign(reinterpret_cast<char16_t*>(mbs));
  free(mbs);
  */

  // This should work, it compiles but does not link on Mac OS X with an
  // "undefined symbols" error for the iconv functions.
  /*
  iconv_t ic;
  ic = iconv_open("UTF−16", "wchar_t"); // or, for Mac, UTF-32LE
  if (ic == (iconv_t)-1) {
    //LOG(("iconv_open failed"));
    return;
  }

  size_t len = wcslen(aHIDAPIStr) * sizeof(wchar_t);
  size_t conv_len = len;
  char* converted = (char*) malloc(conv_len);
  memset(converted, 0, len);
  size_t icrv;
  icrv = iconv(ic, (char**)&aHIDAPIStr, &len, &converted, &conv_len);
  if (icrv == (size_t)-1) {
    //LOG(("iconv() failed"));
    iconv_close(ic);
    return;
  }
  icrv = iconv_close(ic);
  if (icrv == -1) {
    //LOG(("iconv_close() failed"));
    return;
  }
  aMozStr.Assign(reinterpret_cast<char16_t*>(converted));
  free(converted);
  */

#ifdef XP_MACOSX
  // On Mac, wchar_t is 4 bytes and HIDAPI explicitly converts everything to
  // kCFStringEncodingUTF32LE.
  size_t len = wcslen(aHIDAPIStr);
  icu::UnicodeString converted = icu::UnicodeString::fromUTF32((UChar32*) aHIDAPIStr, len);
  ICUStringToMozString(converted, aMozStr);
#endif
}

HidDeviceInfo::HidDeviceInfo(const struct hid_device_info* dev)
{
  mDeviceId = nsCString(dev->path);

  mVendorId = dev->vendor_id;
  mProductId = dev->product_id;

  HIDAPIStringToMozillaString(dev->manufacturer_string, mManufacturerName);
  HIDAPIStringToMozillaString(dev->product_string, mProductName);
  HIDAPIStringToMozillaString(dev->serial_number, mSerialNumber);

  // TODO these won't be available on Linux
  mUsagePage = dev->usage_page;
  mUsage = dev->usage;
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
