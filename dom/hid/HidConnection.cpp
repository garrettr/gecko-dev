/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "HidConnection.h"
#include "nsIHidConnection.h"

#include "mozilla/Logging.h"
#include "mozilla/ModuleUtils.h"

#include "nsITimer.h"

#include "hidapi.h"

namespace mozilla {
namespace dom {

extern LazyLogModule gHidServiceLog;
#define LOG(type, msg) MOZ_LOG(gHidServiceLog, type, msg)

// HIDAPI is a synchronous API, so we need to poll it periodically.
// 100ms is arbitrarily chosen.
const uint32_t kInputReportPollInterval = 100;

// TODO Use NS_ENSURE_ARG_POINTER?
// e.g. https://dxr.mozilla.org/mozilla-central/source/dom/base/DOMParser.cpp#170

NS_IMPL_ISUPPORTS(HidConnection, nsIHidConnection)

HidConnection::HidConnection()
{
}

/*
HidConnection::HidConnection(char *aDeviceId,
                             hid_device *aDevice)
  : mDeviceId(aDeviceId),
    mDevice(aDevice),
    mConnected(true)
{
  nsresult rv;
  mInputReportPollTimer = do_CreateInstance("@mozilla.org/timer;1", &rv);
  mInputReportPollTimer->InitWithFuncCallback(InputReportTimerCallback,
                                              this,
                                              kInputReportPollInterval,
                                              nsITimer::TYPE_REPEATING_SLACK);
}
*/

HidConnection::~HidConnection()
{
  // TODO:
  // Stop timer
  // close hid_device
}

/*
void
HidConnection::InputReportTimerCallback(nsITimer *aTimer,
                                     void *aClosure)
{
  HidConnection *self = reinterpret_cast<HidConnection*>(aClosure);
  self->PollInputReports();
}

void
HidConnection::PollInputReports()
{
  // Test by just logging here to make sure everything is wired up correctly
  LOG(LogLevel::Debug, ("In PollInputReports()"));
}
*/

NS_IMETHODIMP
HidConnection::Disconnect(nsIHidDisconnectCallback *aCallback)
{
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
HidConnection::Receive(nsIHidReceiveCallback *aCallback)
{
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
HidConnection::Send(int32_t aReportId,
                    const uint8_t *aData,
                    uint32_t aLen,
                    nsIHidSendCallback *aCallback)
{
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
HidConnection::ReceiveFeatureReport(int32_t aReportId,
                                    nsIHidReceiveFeatureReportCallback *aCallback)
{
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
HidConnection::SendFeatureReport(int32_t aReportId,
                                 const uint8_t *aData,
                                 uint32_t aLen,
                                 nsIHidSendFeatureReportCallback *aCallback)
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

// TODO This should only be construct-able from C++, it doesn't make sense to do it from JS
// Well, it potentially might, for testing purposes, but not in normal use.
// What invocations of macros do I need to chant?
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
