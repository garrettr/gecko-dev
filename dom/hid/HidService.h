/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=8 sts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef HidService_h___
#define HidService_h___

#include "mozilla/Logging.h"
#include "mozilla/StaticPtr.h"

#include "nsCOMPtr.h"
#include "nsIHidService.h"
#include "nsISupports.h"
#include "nsRefPtrHashtable.h"
#include "nsString.h"

namespace mozilla {
namespace dom {

class HidService final : public nsIHidService
{
  public:
    NS_DECL_ISUPPORTS
    NS_DECL_NSIHIDSERVICE

    static already_AddRefed<HidService> GetInstance();

  private:
    // This is a singleton, so disallow construction.
    HidService();
    ~HidService();

    // Global singleton object for holding this factory service.
    static StaticRefPtr<HidService> sHidService;
};

} // namespace dom
} // namespace mozilla

#endif /* HidService_h___ */
