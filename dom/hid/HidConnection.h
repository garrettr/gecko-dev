/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=8 sts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef HidConnection_h___
#define HidConnection_h___

#include "nsIHidConnection.h"
#include "nsISupports.h"
#include "nsITimer.h"

#include "nsProxyRelease.h"
#include "nsTArray.h"

#include "hidapi.h"

namespace mozilla {
namespace dom {

struct PendingHidReport {
  PendingHidReport();
  ~PendingHidReport();

  nsTArray<uint8_t> buffer;
  size_t size;
};

struct PendingHidReceive {
  PendingHidReceive();
  ~PendingHidReceive();

  nsMainThreadPtrHolder<nsIHidReceiveCallback> callback;
};

class HidConnection final : public nsIHidConnection
{
  public:
    NS_DECL_ISUPPORTS
    NS_DECL_NSIHIDCONNECTION

    HidConnection();

  private:
    ~HidConnection();

    // The unique device ID
    char* aDeviceId;

    // Underlying HIDAPI struct
    hid_device *mDevice;

    // Keep track of connection state; return an error to calls that try to use
    // a disconnected device
    bool mConnected;

    // Timer to check for incoming reads
    nsCOMPtr<nsITimer> mInputReportPollTimer;

    /*
    nsTArray<PendingHidReport> mPendingReports;
    nsTArray<PendingHidReceive> mPendingReceives;
    */
};

} // namespace dom
} // namespace mozilla

#endif /* HidConnection_h___ */
