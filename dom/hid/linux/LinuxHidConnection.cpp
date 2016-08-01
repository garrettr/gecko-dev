/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "LinuxHidConnection.h"

namespace mozilla {
namespace dom {

extern LazyLogModule gHidServiceLog;
#define LOG(args) MOZ_LOG(gHidServiceLog, mozilla::LogLevel::Debug, args)
#define LOG_ENABLED() MOZ_LOG_TEST(gHidServiceLog, mozilla::LogLevel::Debug)

LinuxHidConnection::LinuxHidConnection(int aDeviceRef,
                                       nsIHidDeviceInfo* aDeviceInfo)
  : mDeviceRef(aDeviceRef),
    mDeviceInfo(aDeviceInfo)
{
  LOG(("TODO LinuxHidConnection ctor"));
}

void
LinuxHidConnection::InputReportCallback()
{
  LOG(("Got an input report!"));
}

} // namespace dom
} // namespace mozilla
