// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/shell/shell_browser_main_parts.h"

#include "base/bind.h"
#include "base/command_line.h"
#include "base/files/file_path.h"
#include "base/message_loop.h"
#include "base/threading/thread.h"
#include "base/threading/thread_restrictions.h"
#include "cc/base/switches.h"
#include "content/public/common/content_switches.h"
#include "content/public/common/main_function_params.h"
#include "content/public/common/url_constants.h"
#include "content/shell/shell.h"
#include "content/shell/shell_browser_context.h"
#include "content/shell/shell_devtools_delegate.h"
#include "content/shell/shell_switches.h"
#include "googleurl/src/gurl.h"
#include "grit/net_resources.h"
#include "net/base/net_module.h"
#include "net/base/net_util.h"
#include "ui/base/resource/resource_bundle.h"

#if defined(OS_ANDROID)
#include "net/android/network_change_notifier_factory_android.h"
#include "net/base/network_change_notifier.h"
#endif

#if defined(USE_AURA) && defined(USE_X11)
#include "ui/base/touch/touch_factory_x11.h"
#endif

namespace content {

namespace {

static GURL GetStartupURL() {
  CommandLine* command_line = CommandLine::ForCurrentProcess();
  if (command_line->HasSwitch(switches::kContentBrowserTest))
    return GURL();
  const CommandLine::StringVector& args = command_line->GetArgs();

#if defined(OS_ANDROID)
  // Delay renderer creation on Android until surface is ready.
  return GURL();
#endif

  if (args.empty())
    return GURL("http://www.google.com/");

  GURL url(args[0]);
  if (url.is_valid() && url.has_scheme())
    return url;

  return net::FilePathToFileURL(base::FilePath(args[0]));
}

base::StringPiece PlatformResourceProvider(int key) {
  if (key == IDR_DIR_HEADER_HTML) {
    base::StringPiece html_data =
        ui::ResourceBundle::GetSharedInstance().GetRawDataResource(
            IDR_DIR_HEADER_HTML);
    return html_data;
  }
  return base::StringPiece();
}

}  // namespace

ShellBrowserMainParts::ShellBrowserMainParts(
    const MainFunctionParams& parameters)
    : BrowserMainParts(),
      parameters_(parameters),
      run_message_loop_(true),
      devtools_delegate_(NULL) {
}

ShellBrowserMainParts::~ShellBrowserMainParts() {
}

#if !defined(OS_MACOSX)
void ShellBrowserMainParts::PreMainMessageLoopStart() {
#if defined(USE_AURA) && defined(USE_X11)
  ui::TouchFactory::SetTouchDeviceListFromCommandLine();
#endif
}
#endif

void ShellBrowserMainParts::PostMainMessageLoopStart() {
#if defined(OS_ANDROID)
  base::MessageLoopForUI::current()->Start();
#endif
}

void ShellBrowserMainParts::PreEarlyInitialization() {
#if defined(OS_ANDROID)
  net::NetworkChangeNotifier::SetFactory(
      new net::NetworkChangeNotifierFactoryAndroid());

  CommandLine::ForCurrentProcess()->AppendSwitch(
      cc::switches::kCompositeToMailbox);
#endif
}

void ShellBrowserMainParts::PreMainMessageLoopRun() {
  browser_context_.reset(new ShellBrowserContext(false));
  off_the_record_browser_context_.reset(new ShellBrowserContext(true));

  Shell::Initialize();
  net::NetModule::SetResourceProvider(PlatformResourceProvider);

  devtools_delegate_ = new ShellDevToolsDelegate(browser_context_.get());

  if (!CommandLine::ForCurrentProcess()->HasSwitch(switches::kDumpRenderTree)) {
    Shell::CreateNewWindow(browser_context_.get(),
                           GetStartupURL(),
                           NULL,
                           MSG_ROUTING_NONE,
                           gfx::Size());
  }

  if (parameters_.ui_task) {
    parameters_.ui_task->Run();
    delete parameters_.ui_task;
    run_message_loop_ = false;
  }
}

bool ShellBrowserMainParts::MainMessageLoopRun(int* result_code)  {
  return !run_message_loop_;
}

void ShellBrowserMainParts::PostMainMessageLoopRun() {
#if defined(USE_AURA)
  Shell::PlatformExit();
#endif
  if (devtools_delegate_)
    devtools_delegate_->Stop();
  browser_context_.reset();
  off_the_record_browser_context_.reset();
}

}  // namespace
