#include "cores/AudioEngine/Engines/ActiveAE/ActiveAudioDSP/AudioDSPAddonModeNode.h"
#include "cores/AudioEngine/Engines/ActiveAE/AudioDSPAddons/ActiveAEDSPAddon.h"

using namespace DSP;
using namespace DSP::AUDIO;
using namespace ActiveAE;

CAudioDSPAddonModeNode::CAudioDSPAddonModeNode(ADDON_HANDLE_STRUCT &Handle, AE_DSP_ADDON Addon, uint64_t ID, int32_t AddonModeID) :
  m_addon(Addon),
  IADSPBufferNode(Addon->ID(), ID),
  m_handle(Handle)
{
  memset(&m_handle, 0, sizeof(ADDON_HANDLE_STRUCT));
  //memset(&m_DllFunctions, 0, sizeof(m_DllFunctions));
}

DSPErrorCode_t CAudioDSPAddonModeNode::CreateInstance(AEAudioFormat &InputFormat, AEAudioFormat &OutputFormat)
{
  InputFormat.m_dataFormat = AE_FMT_FLOATP;
  OutputFormat.m_dataFormat = AE_FMT_FLOATP;
  //if (!m_Addon->GetAddonProcessingCallbacks(m_DllFunctions))
  //{
  //  return DSP_ERR_FATAL_ERROR;
  //}

  //! @todo AudioDSP V2 simplify add-on mode creation API
  AE_DSP_SETTINGS addonSettings;
  addonSettings.iStreamID;                /*!< @brief id of the audio stream packets */
  addonSettings.iStreamType;              /*!< @brief the input stream type source eg, Movie or Music */
  addonSettings.iInChannels;              /*!< @brief the amount of input channels */
  addonSettings.lInChannelPresentFlags;   /*!< @brief the exact channel mapping flags of input */
  addonSettings.iInFrames;                /*!< @brief the input frame size from KODI */
  addonSettings.iInSamplerate;            /*!< @brief the basic sample rate of the audio packet */
  addonSettings.iProcessFrames;           /*!< @brief the processing frame size inside add-on's */
  addonSettings.iProcessSamplerate;       /*!< @brief the sample rate after input resample present in master processing */
  addonSettings.iOutChannels;             /*!< @brief the amount of output channels */
  addonSettings.lOutChannelPresentFlags;  /*!< @brief the exact channel mapping flags for output */
  addonSettings.iOutFrames;               /*!< @brief the final out frame size for KODI */
  addonSettings.iOutSamplerate;           /*!< @brief the final sample rate of the audio packet */
  addonSettings.bInputResamplingActive;   /*!< @brief if a re-sampling is performed before master processing this flag is set to true */
  addonSettings.bStereoUpmix;             /*!< @brief true if the stereo upmix setting on kodi is set */
  addonSettings.iQualityLevel;            /*!< @brief the from KODI selected quality level for signal processing */

  AE_DSP_STREAM_PROPERTIES pProperties;
  pProperties.iStreamID;
  pProperties.iStreamType;
  pProperties.iBaseType;
  pProperties.strName;
  pProperties.strCodecId;
  pProperties.strLanguage;
  pProperties.iIdentifier;
  pProperties.iChannels;
  pProperties.iSampleRate;
  pProperties.Profile;

  if (m_addon->StreamInitialize(&m_handle, &addonSettings) != AE_DSP_ERROR_NO_ERROR)
  {
    return DSP_ERR_FATAL_ERROR;
  }

  AE_DSP_STREAMTYPE addonStreamType = AE_DSP_ASTREAM_BASIC;// StreamProperties.streamType; //! @todo AudioDSP V2 add translation method
  unsigned int addonModeID = 0; //! @todo AudioDSP V2 set correct addonModeID
  if (m_addon->MasterProcessSetMode(&m_handle, addonStreamType, addonModeID, ID) != AE_DSP_ERROR_NO_ERROR)
  {
    return DSP_ERR_FATAL_ERROR;
  }

  unsigned long outChannelFlags = 0x0;
  int outputChannelAmount = 0;
  outputChannelAmount = m_addon->MasterProcessGetOutChannels(&m_handle, outChannelFlags);

  OutputFormat.m_channelLayout.Reset();
  for (unsigned int ch = 0; ch < AE_DSP_CH_MAX; ch++)
  {
    if (outChannelFlags & 1 << ch)
    {
      OutputFormat.m_channelLayout += static_cast<AEChannel>(ch); //! @todo AudioDSP V2 add conversion method
    }
  }

  return DSP_ERR_NO_ERR;
}

int CAudioDSPAddonModeNode::ProcessInstance(const uint8_t **In, uint8_t **Out)
{
  return m_addon->MasterProcess(&m_handle, reinterpret_cast<const float**>(In), reinterpret_cast<float**>(Out), m_InputFormat.m_frames);
}

DSPErrorCode_t CAudioDSPAddonModeNode::DestroyInstance()
{
  return DSP_ERR_NO_ERR;
}
