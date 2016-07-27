/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=8 sts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef HidConnection_h__
#define HidConnection_h__

#include "nsIHidConnection.h"

namespace mozilla {
namespace dom {

class HidConnection : public nsIHidConnection
{
  public:
    NS_DECL_THREADSAFE_ISUPPORTS
    NS_DECL_NSIHIDCONNECTION

  protected:
    HidConnection();
    virtual ~HidConnection();

  private:
    // Plaform-specific methods
    virtual nsresult NativeDisconnect() = 0;

};

} // namespace dom
} // namespace mozilla

#endif // HidConnection_h__
