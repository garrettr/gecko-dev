/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "HidService.h"
#include "nsIHidService.h"

#include "mozilla/Logging.h"
#include "mozilla/ModuleUtils.h"
#include "mozilla/StaticPtr.h"

#include "nsAppRunner.h" // for XRE_IsParentProcess
#include "nsArrayEnumerator.h"
#include "nsAutoPtr.h"
#include "nsCOMArray.h"
#include "nsCOMPtr.h"
#include "nsDebug.h"
#include "nsError.h"
#include "nsISimpleEnumerator.h"
#include "nsNativeCharsetUtils.h"
#include "nsString.h"
#include "nsThreadUtils.h"

#ifdef XP_MACOSX
#include "MacHidService.h"
#elif defined(XP_WIN)
#include "WindowsHidService.h"
#elif defined(XP_LINUX)
#include "LinuxHidService.h"
#endif

namespace mozilla {
namespace dom {

// NSPR_LOG_MODULES=HidService:5
LazyLogModule gHidServiceLog("HidService");
#define LOG(args) MOZ_LOG(gHidServiceLog, mozilla::LogLevel::Debug, args)
#define LOG_ENABLED() MOZ_LOG_TEST(gHidServiceLog, mozilla::LogLevel::Debug)

StaticRefPtr<HidService> HidService::sHidService;

already_AddRefed<HidService>
HidService::GetInstance()
{
  // Avoid creating HidService in content process.
  if (!XRE_IsParentProcess()) {
    MOZ_CRASH("Non-chrome processes should not get here.");
  }

  if (!sHidService) {
    // TODO: Do I need to do anything else to avoid potential issues from
    // concurrent access? NS_DECL_THREADSAFE_ISUPPORTS appears to have disabled
    // the "not thread-safe" check that was tripping me up earlier, but I'm not
    // sure if that means that I need to ensure thread safety on my own nowl
    NS_ASSERTION(NS_IsMainThread(), "Main thread only");

#ifdef XP_MACOSX
    sHidService = new MacHidService();
#elif defined(XP_WIN)
    sHidService = new WindowsHidService();
#elif defined(XP_LINUX)
    sHidService = new LinuxHidService();
#endif

    // TODO: error handling for Init?
    sHidService->Init();
  }
  RefPtr<HidService> hidService = sHidService.get();
  return hidService.forget();
}

HidService::HidService()
{
  LOG(("Hid Service started up"));
}

HidService::~HidService()
{
  LOG(("Hid Service shutting down"));
}

void
HidService::Init()
{
  nsresult rv = NS_NewNamedThread("HID Service", getter_AddRefs(mThread));
  NS_ENSURE_SUCCESS_VOID(rv);
}

void
HidService::Shutdown()
{
  // TODO: observe global shutdown event
  // e.g. AsyncLatencyLogger::Observe
  // also useful: nsINetworkLinkService, see netwerk/system
  // nsNotifyAddrListener::calculateNetworkId looks horrifying
  if (mThread) {
    mThread->Shutdown();
  }
}

// XXX: There's a lot of repetitive boilerplate with this style of implementing runnables.
// Is there a simpler way to do it?
// Some things to investigate:
// - NewRunnable{Function,Method}

class GetDevicesTask final : public mozilla::Runnable
{
  public:
    GetDevicesTask(nsIHidGetDevicesCallback* aCallback)
      : mCallback(new nsMainThreadPtrHolder<nsIHidGetDevicesCallback>(aCallback))
    {
      LOG(("In GetDevicesTask::GetDevicesTask"));
    }

    NS_IMETHOD Run()
    {
      LOG(("In GetDevicesTask::Run"));
      // TODO: We can make this safer by explicitly checking if we are on the
      // expected HID Service worker thread. For an example, see
      // WalkMemoryCacheRunnable::Run in CacheStorageService.cpp
      if (!NS_IsMainThread()) {
        // TODO: all the actual work (call the corresponding Native* function)
        // TODO: This is a little awkward. Can we avoid having to get the HidService again?
        RefPtr<HidService> hs = HidService::GetInstance();
        nsresult rv = hs->NativeGetDevices();
        NS_DispatchToMainThread(this);
      } else {
        mCallback->Callback(NS_OK, nullptr);
      }
      return NS_OK;
    }

  private:
    nsMainThreadPtrHandle<nsIHidGetDevicesCallback> mCallback;
};

NS_IMETHODIMP
HidService::GetDevices(nsIHidGetDevicesCallback* aCallback)
{
  LOG(("In HidService::GetDevices"));
  nsCOMPtr<nsIRunnable> r = new GetDevicesTask(aCallback);
  mThread->Dispatch(r, nsIEventTarget::DISPATCH_NORMAL);
  return NS_OK;
}

NS_IMETHODIMP
HidService::Connect(nsIHidDeviceInfo* aDeviceInfo,
                    nsIHidConnectCallback* aCallback)
{
  return NS_ERROR_NOT_IMPLEMENTED;
}

// b7a8f508-b76a-41ad-a959-7204aac2802f
#define HIDSERVICE_CID                                  \
  { 0xb7a8f508,                                         \
    0xb76a,                                             \
    0x41ad,                                             \
    { 0xa9, 0x59, 0x72, 0x04, 0xaa, 0xc2, 0x80, 0x2f }  \
  }
#define HIDSERVICE_CONTRACTID "@mozilla.org/dom/hid-service;1"

NS_IMPL_ISUPPORTS(HidService, nsIHidService)

NS_GENERIC_FACTORY_SINGLETON_CONSTRUCTOR(HidService,
                                         HidService::GetInstance)

NS_DEFINE_NAMED_CID(HIDSERVICE_CID);

static const mozilla::Module::CIDEntry kHidServiceCIDs[] = {
  { &kHIDSERVICE_CID, false, nullptr, HidServiceConstructor },
  { nullptr }
};

static const mozilla::Module::ContractIDEntry kHidServiceContracts[] = {
  { HIDSERVICE_CONTRACTID, &kHIDSERVICE_CID },
  { nullptr }
};

static const mozilla::Module kHidServiceModule = {
  mozilla::Module::kVersion,
  kHidServiceCIDs,
  kHidServiceContracts,
  nullptr
};

NSMODULE_DEFN(HidServiceModule) = &kHidServiceModule;

} // namespace dom
} // namespace mozilla
