/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "HidService.h"
#include "nsIHidService.h"

#include "mozilla/Logging.h"
#include "mozilla/ModuleUtils.h"
#include "mozilla/StaticPtr.h"

#include "nsAppRunner.h" // for XRE_IsParentProcess
#include "nsArrayEnumerator.h"
#include "nsAutoPtr.h"
#include "nsCOMArray.h"
#include "nsCOMPtr.h"
#include "nsDebug.h"
#include "nsError.h"
#include "nsISimpleEnumerator.h"
#include "nsNativeCharsetUtils.h"
#include "nsString.h"

#include "hidapi.h"

namespace mozilla {
namespace dom {

// NSPR_LOG_MODULES=HidService:5
LazyLogModule gHidServiceLog("HidService");
#define LOG(args) MOZ_LOG(gHidServiceLog, mozilla::LogLevel::Debug, args)
#define LOG_ENABLED() MOZ_LOG_TEST(gHidServiceLog, mozilla::LogLevel::Debug)

StaticRefPtr<HidService> HidService::sHidService;

already_AddRefed<HidService>
HidService::GetInstance()
{
  // Avoid creating HidService in content process.
  if (!XRE_IsParentProcess()) {
    MOZ_CRASH("Non-chrome processes should not get here.");
  }

  if (!sHidService) {
    sHidService = new HidService();
  }
  RefPtr<HidService> hidService = sHidService.get();
  return hidService.forget();
}

HidService::HidService()
{
  LOG(("Hid Service started up"));
}

HidService::~HidService()
{
  LOG(("Hid Service shutting down"));
}

NS_IMETHODIMP
HidService::GetDevices(nsIHidGetDevicesCallback* aCallback)
{
  nsresult rv = NS_OK;

  struct hid_device_info *devs, *cur_dev;
  nsCOMArray<nsIHidDeviceInfo> deviceInfoArray;

  // Get all connected HID devices
  devs = hid_enumerate(0, 0);
  cur_dev = devs;
  while(cur_dev)
  {
    nsCOMPtr<nsIHidDeviceInfo> deviceInfo = new HidDeviceInfo(cur_dev);
    deviceInfoArray.AppendObject(deviceInfo);
    cur_dev = cur_dev->next;
  }
  hid_free_enumeration(devs);

  nsCOMPtr<nsISimpleEnumerator> deviceEnumerator;
  rv = NS_NewArrayEnumerator(getter_AddRefs(deviceEnumerator),
                             deviceInfoArray);
  NS_ENSURE_SUCCESS(rv, rv);

  aCallback->Callback(NS_OK, deviceEnumerator);
  return NS_OK;
}

NS_IMETHODIMP
HidService::Connect(nsIHidDeviceInfo* aDeviceInfo,
                    nsIHidConnectCallback* aCallback)
{
  /*
  // Create the device here so we can avoid handling an error in the constructor
  hid_device *dev = hid_open_path(aDeviceId);
  if (!dev) {
    aCallback->OnComplete(NS_FAILED, nullptr);
  }
  nsCOMPtr<nsIHidConnection> conn = new HidConnection(aDeviceId, dev);
  aCallback->OnComplete(NS_OK, conn);
  return NS_OK;
  */
  return NS_ERROR_NOT_IMPLEMENTED;
}

// b7a8f508-b76a-41ad-a959-7204aac2802f
#define HIDSERVICE_CID                                  \
  { 0xb7a8f508,                                         \
    0xb76a,                                             \
    0x41ad,                                             \
    { 0xa9, 0x59, 0x72, 0x04, 0xaa, 0xc2, 0x80, 0x2f }  \
  }
#define HIDSERVICE_CONTRACTID "@mozilla.org/dom/hid-service;1"

NS_IMPL_ISUPPORTS(HidService, nsIHidService)

NS_GENERIC_FACTORY_SINGLETON_CONSTRUCTOR(HidService,
                                         HidService::GetInstance)

NS_DEFINE_NAMED_CID(HIDSERVICE_CID);

static const mozilla::Module::CIDEntry kHidServiceCIDs[] = {
  { &kHIDSERVICE_CID, false, nullptr, HidServiceConstructor },
  { nullptr }
};

static const mozilla::Module::ContractIDEntry kHidServiceContracts[] = {
  { HIDSERVICE_CONTRACTID, &kHIDSERVICE_CID },
  { nullptr }
};

static const mozilla::Module kHidServiceModule = {
  mozilla::Module::kVersion,
  kHidServiceCIDs,
  kHidServiceContracts,
  nullptr
};

NSMODULE_DEFN(HidServiceModule) = &kHidServiceModule;

} // namespace dom
} // namespace mozilla
