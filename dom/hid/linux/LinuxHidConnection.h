/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef LinuxHidConnection_h___
#define LinuxHidConnection_h___

#include "HidConnection.h"
#include "nsIHidConnection.h"
#include "nsIHidDeviceInfo.h"

#include "mozilla/Logging.h"

#include "nsCOMPtr.h"
#include "nsTArray.h"

namespace mozilla {
namespace dom {

class LinuxHidConnection final : public HidConnection
{
  public:
    LinuxHidConnection(int aDeviceRef,
                       nsIHidDeviceInfo* aDeviceInfo);

  private:
    ~LinuxHidConnection() {};

    static void InputReportCallback();

    int mDeviceRef; // TODO(adma): have a type for this?
    nsCOMPtr<nsIHidDeviceInfo> mDeviceInfo;
    nsTArray<uint8_t> mInputReportBuffer;
};

} // namespace dom
} // namespace mozilla

#endif /* LinuxHidConnection_h___ */
