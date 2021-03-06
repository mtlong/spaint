/**
 * grove: ScoreRelocaliserFactory.cpp
 * Copyright (c) Torr Vision Group, University of Oxford, 2017. All rights reserved.
 */

#include "relocalisation/ScoreRelocaliserFactory.h"
using namespace ORUtils;
using namespace tvgutil;

#include "relocalisation/cpu/ScoreRelocaliser_CPU.h"
#ifdef WITH_CUDA
#include "relocalisation/cuda/ScoreRelocaliser_CUDA.h"
#endif

namespace grove {

//#################### PUBLIC STATIC MEMBER FUNCTIONS ####################

ScoreRelocaliser_Ptr ScoreRelocaliserFactory::make_score_relocaliser(const std::string& forestFilename, const tvgutil::SettingsContainer_CPtr& settings, DeviceType deviceType)
{
  return make_score_relocaliser(forestFilename, settings, "ScoreRelocaliser.", deviceType);
}

ScoreRelocaliser_Ptr ScoreRelocaliserFactory::make_score_relocaliser(const std::string& forestFilename, const SettingsContainer_CPtr& settings,
                                                                     const std::string& settingsNamespace, DeviceType deviceType)
{
  ScoreRelocaliser_Ptr relocaliser;

  if(deviceType == DEVICE_CUDA)
  {
#ifdef WITH_CUDA
    relocaliser.reset(new ScoreRelocaliser_CUDA(forestFilename, settings, settingsNamespace));
#else
    throw std::runtime_error("Error: CUDA support not currently available. Reconfigure in CMake with the WITH_CUDA option set to on.");
#endif
  }
  else
  {
    relocaliser.reset(new ScoreRelocaliser_CPU(forestFilename, settings, settingsNamespace));
  }

  return relocaliser;
}

}
