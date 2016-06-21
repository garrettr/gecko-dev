/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef LinuxHidService_h___
#define LinuxHidService_h___

#include "HidService.h"
#include "nsIHidService.h"

namespace mozilla {
namespace dom {

class LinuxHidService final : public HidService
{
  public:

    LinuxHidService() {};

  private:
    ~LinuxHidService() {};

    virtual nsresult NativeGetDevices() override;
};

} // namespace dom
} // namespace mozilla

#endif /* LinuxHidService_h___ */
