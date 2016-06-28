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
#include "nsISupports.h"
#include "nsRefPtrHashtable.h"
#include "nsString.h"
#include "nsThreadUtils.h"
#include "nsProxyRelease.h" // nsMainThreadPtr*

#include "nsIHidService.h"

namespace mozilla {
namespace dom {

typedef nsMainThreadPtrHandle<nsIHidGetDevicesCallback> GetDevicesCallbackHandle;

class HidService : public nsIHidService
{
  public:
    NS_DECL_THREADSAFE_ISUPPORTS
    NS_DECL_NSIHIDSERVICE

    static already_AddRefed<HidService> GetInstance();

  protected:
    // This is a singleton, so disallow construction. They are protected
    // instead of private so the native subclasses can call them.
    HidService();
    virtual ~HidService();

  private:

    void Init();
    void Shutdown();

    // Platform-specific methods
    virtual void NativeInit() = 0;
    virtual void NativeShutdown() = 0;
    virtual nsresult NativeGetDevices(GetDevicesCallbackHandle aCallback) = 0;

    // Make the runnables friend class so they can call the private Native*
    // methods on HidService.
    friend class GetDevicesTask;

    // HID Service singleton
    static StaticRefPtr<HidService> sHidService;

    // HID worker thread, used to avoid doing IO on the main thread
    nsCOMPtr<nsIThread> mThread;

    // XXX: This is a bit weird. HidService's constructor is private in order
    // to enforce the use of a singleton design pattern. However, since we want
    // to inherit from HidService to implement the native services, we run into
    // a problem: you cannot inherit from a class with a private or protected
    // constructor. There are only two possible workarounds:
    //
    // 1. Make the constructor public
    // 2. Make friends
    //
    // Since I would like to use the private constructor to enforce the
    // singleton pattern if possible, I am going with using a friend class for
    // now.
    //friend class MacHidService;
    //
    // XXX update: trying to use protected instead of friend classes here
};

} // namespace dom
} // namespace mozilla

#endif /* HidService_h___ */
