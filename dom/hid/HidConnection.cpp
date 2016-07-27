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

} // namespace dom
} // namespace mozilla
