/*
 *      Copyright (C) 2005-2015 Team KODI
 *      http://kodi.tv
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with KODI; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */
// AudioDSPSettings.cpp: implementation of the CAudioSettings class.
//
//////////////////////////////////////////////////////////////////////

#include "AudioDSPSettings.h"
#include "cores/AudioEngine/Engines/ActiveAE/AudioDSPAddons/ActiveAEDSPMode.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAudioSettings::CAudioSettings()
{
  m_MasterStreamType    = AUDIODSP_ADDON_ASTREAM_AUTO;
  m_MasterStreamTypeSel = AUDIODSP_ADDON_ASTREAM_AUTO;
  m_MasterStreamBase    = AUDIODSP_ADDON_ABASE_STEREO;
}

bool CAudioSettings::operator!=(const CAudioSettings &right) const
{
  if (m_MasterStreamType      != right.m_MasterStreamType)      return true;
  if (m_MasterStreamTypeSel   != right.m_MasterStreamTypeSel)   return true;

  return false;
}
