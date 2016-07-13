/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "HidConnection.h"
#include "nsIHidConnection.h"

#include "mozilla/ModuleUtils.h"

namespace mozilla {
namespace dom {

NS_IMPL_ISUPPORTS(HidConnection, nsIHidConnection)

HidConnection::HidConnection()
{
}

HidConnection::~HidConnection() {
}

NS_IMETHODIMP
HidConnection::Disconnect(nsIHidDisconnectCallback* aCallback)
{
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
HidConnection::Receive(nsIHidReceiveCallback* aCallback)
{
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
HidConnection::Send(const uint8_t* aData,
                    uint32_t aLen,
                    nsIHidSendCallback* aCallback)
{
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
HidConnection::ReceiveFeatureReport(nsIHidReceiveFeatureReportCallback* aCallback)
{
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
HidConnection::SendFeatureReport(const uint8_t* aData,
                                 uint32_t aLen,
                                 nsIHidSendFeatureReportCallback* aCallback)
{
  return NS_ERROR_NOT_IMPLEMENTED;
}

// 9ba6015d-4154-43c1-bba7-e7a7ccf36474
#define HIDCONNECTION_CID                               \
  { 0x9ba6015d,                                         \
    0x4154,                                             \
    0x43c1,                                             \
    { 0xbb, 0xa7, 0xe7, 0xa7, 0xcc, 0xf3, 0x64, 0x74 }  \
  }
#define HIDCONNECTION_CONTRACTID "@mozilla.org/dom/hid-connection;1"

NS_GENERIC_FACTORY_CONSTRUCTOR(HidConnection)

NS_DEFINE_NAMED_CID(HIDCONNECTION_CID);

static const mozilla::Module::CIDEntry kHidConnectionCIDs[] = {
  { &kHIDCONNECTION_CID, false, nullptr, HidConnectionConstructor },
  { nullptr }
};

static const mozilla::Module::ContractIDEntry kHidConnectionContracts[] = {
  { HIDCONNECTION_CONTRACTID, &kHIDCONNECTION_CID },
  { nullptr }
};

static const mozilla::Module kHidConnectionModule = {
  mozilla::Module::kVersion,
  kHidConnectionCIDs,
  kHidConnectionContracts,
  nullptr
};

NSMODULE_DEFN(HidConnectionModule) = &kHidConnectionModule;

} // namespace dom
} // namespace mozilla
