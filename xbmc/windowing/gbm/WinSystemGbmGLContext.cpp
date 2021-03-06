/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "WinSystemGbmGLContext.h"

#include "OptionalsReg.h"
#include "cores/RetroPlayer/process/gbm/RPProcessInfoGbm.h"
#include "cores/RetroPlayer/rendering/VideoRenderers/RPRendererGBM.h"
#include "cores/RetroPlayer/rendering/VideoRenderers/RPRendererOpenGL.h"
#include "cores/VideoPlayer/DVDCodecs/DVDFactoryCodec.h"
#include "cores/VideoPlayer/VideoRenderers/LinuxRendererGL.h"
#include "cores/VideoPlayer/VideoRenderers/RenderFactory.h"
#include "rendering/gl/ScreenshotSurfaceGL.h"
#include "utils/log.h"

#include "platform/posix/XTimeUtils.h"

#include <EGL/egl.h>
#include <EGL/eglext.h>

using namespace KODI::WINDOWING::GBM;

CWinSystemGbmGLContext::CWinSystemGbmGLContext()
: CWinSystemGbmEGLContext(EGL_PLATFORM_GBM_MESA, "EGL_MESA_platform_gbm")
{}

std::unique_ptr<CWinSystemBase> CWinSystemBase::CreateWinSystem()
{
  std::unique_ptr<CWinSystemBase> winSystem(new CWinSystemGbmGLContext());
  return winSystem;
}

bool CWinSystemGbmGLContext::InitWindowSystem()
{
  VIDEOPLAYER::CRendererFactory::ClearRenderer();
  CDVDFactoryCodec::ClearHWAccels();
  CLinuxRendererGL::Register();
  RETRO::CRPProcessInfoGbm::Register();
  RETRO::CRPProcessInfoGbm::RegisterRendererFactory(new RETRO::CRendererFactoryOpenGL);

  if (!CWinSystemGbmEGLContext::InitWindowSystemEGL(EGL_OPENGL_BIT, EGL_OPENGL_API))
  {
    return false;
  }

  bool general, deepColor;
  m_vaapiProxy.reset(VaapiProxyCreate(m_DRM->GetRenderNodeFileDescriptor()));
  VaapiProxyConfig(m_vaapiProxy.get(), m_eglContext.GetEGLDisplay());
  VAAPIRegisterRender(m_vaapiProxy.get(), general, deepColor);

  if (general)
  {
    VAAPIRegister(m_vaapiProxy.get(), deepColor);
  }

  CScreenshotSurfaceGL::Register();

  return true;
}

bool CWinSystemGbmGLContext::SetFullScreen(bool fullScreen, RESOLUTION_INFO& res, bool blankOtherDisplays)
{
  if (res.iWidth != m_nWidth ||
      res.iHeight != m_nHeight)
  {
    CLog::Log(LOGDEBUG, "CWinSystemGbmGLContext::%s - resolution changed, creating a new window", __FUNCTION__);
    CreateNewWindow("", fullScreen, res);
  }

  if (!m_eglContext.TrySwapBuffers())
  {
    CEGLUtils::Log(LOGERROR, "eglSwapBuffers failed");
    throw std::runtime_error("eglSwapBuffers failed");
  }

  CWinSystemGbm::SetFullScreen(fullScreen, res, blankOtherDisplays);
  CRenderSystemGL::ResetRenderSystem(res.iWidth, res.iHeight);

  return true;
}

void CWinSystemGbmGLContext::PresentRender(bool rendered, bool videoLayer)
{
  if (!m_bRenderCreated)
    return;

  if (rendered || videoLayer)
  {
    if (rendered)
    {
      if (!m_eglContext.TrySwapBuffers())
      {
        CEGLUtils::Log(LOGERROR, "eglSwapBuffers failed");
        throw std::runtime_error("eglSwapBuffers failed");
      }
    }
    CWinSystemGbm::FlipPage(rendered, videoLayer);

    if (m_dispReset && m_dispResetTimer.IsTimePast())
    {
      CLog::Log(LOGDEBUG, "CWinSystemGbmGLContext::%s - Sending display reset to all clients",
                __FUNCTION__);
      m_dispReset = false;
      CSingleLock lock(m_resourceSection);

      for (auto resource : m_resources)
        resource->OnResetDisplay();
    }
  }
  else
  {
    Sleep(10);
  }
}

bool CWinSystemGbmGLContext::CreateContext()
{
  const EGLint glMajor = 3;
  const EGLint glMinor = 2;

  CEGLAttributesVec contextAttribs;
  contextAttribs.Add({{EGL_CONTEXT_MAJOR_VERSION_KHR, glMajor},
                      {EGL_CONTEXT_MINOR_VERSION_KHR, glMinor},
                      {EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR}});

  if (!m_eglContext.CreateContext(contextAttribs))
  {
    CEGLAttributesVec fallbackContextAttribs;
    fallbackContextAttribs.Add({{EGL_CONTEXT_CLIENT_VERSION, 2}});

    if (!m_eglContext.CreateContext(fallbackContextAttribs))
    {
      CLog::Log(LOGERROR, "EGL context creation failed");
      return false;
    }
    else
    {
      CLog::Log(LOGWARNING, "Your OpenGL drivers do not support OpenGL {}.{} core profile. Kodi will run in compatibility mode, but performance may suffer.", glMajor, glMinor);
    }
  }

  return true;
}
