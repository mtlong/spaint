/**
 * spaint: SpaintRaycaster.cpp
 */

#include "core/SpaintRaycaster.h"

#include <stdexcept>

#include <ITMLib/Engine/ITMTrackerFactory.h>
#include <ITMLib/Engine/ITMVisualisationEngine.cpp>
#include <ITMLib/Engine/DeviceSpecific/CPU/ITMVisualisationEngine_CPU.cpp>

namespace spaint {

//#################### CONSTRUCTORS ####################

SpaintRaycaster::SpaintRaycaster(const SpaintModel_CPtr& model)
: m_model(model)
{
  // Set up the InfiniTAM visualisation engine.
  if(model->get_settings().deviceType == ITMLibSettings::DEVICE_CUDA)
  {
#ifdef WITH_CUDA
    // Use the GPU implementation of the visualisation engine.
    m_visualisationEngine.reset(new ITMVisualisationEngine_CUDA<SpaintVoxel,ITMVoxelIndex>);
#else
    // This should never happen as things stand - we set deviceType to DEVICE_CPU if CUDA support isn't available.
    throw std::runtime_error("Error: CUDA support not currently available. Reconfigure in CMake with the WITH_CUDA option set to on.");
#endif
  }
  else
  {
    // Use the CPU implementation of the visualisation engine.
    m_visualisationEngine.reset(new ITMVisualisationEngine_CPU<SpaintVoxel,ITMVoxelIndex>);
  }

  // Set up the live render state.
  Vector2i trackedImageSize = ITMTrackerFactory::GetTrackedImageSize(model->get_settings(), model->get_rgb_image_size(), model->get_depth_image_size());
  m_liveRenderState.reset(m_visualisationEngine->CreateRenderState(model->get_scene().get(),trackedImageSize));
}

//#################### PUBLIC MEMBER FUNCTIONS ####################

void SpaintRaycaster::generate_free_raycast(const UChar4Image_Ptr& output, RenderState_Ptr& renderState, const ITMPose& pose) const
{
  SpaintModel::Scene_CPtr scene = m_model->get_scene();
  SpaintModel::View_CPtr view = m_model->get_view();
  const ITMLibSettings& settings = m_model->get_settings();

  if(!renderState) renderState.reset(m_visualisationEngine->CreateRenderState(scene.get(), m_model->get_depth_image_size()));

  m_visualisationEngine->FindVisibleBlocks(scene.get(), &pose, &view->calib->intrinsics_d, renderState.get());
  m_visualisationEngine->CreateExpectedDepths(scene.get(), &pose, &view->calib->intrinsics_d, renderState.get());
  m_visualisationEngine->RenderImage(scene.get(), &pose, &view->calib->intrinsics_d, renderState.get(), renderState->raycastImage, false);

  prepare_to_copy_visualisation(renderState->raycastImage->noDims, output);
  output->SetFrom(
    renderState->raycastImage,
    settings.deviceType == ITMLibSettings::DEVICE_CUDA ? ORUtils::MemoryBlock<Vector4u>::CUDA_TO_CPU : ORUtils::MemoryBlock<Vector4u>::CPU_TO_CPU
  );
}

void SpaintRaycaster::get_default_raycast(const UChar4Image_Ptr& output) const
{
  const ITMLibSettings& settings = m_model->get_settings();
  prepare_to_copy_visualisation(m_liveRenderState->raycastImage->noDims, output);
  output->SetFrom(
    m_liveRenderState->raycastImage,
    settings.deviceType == ITMLibSettings::DEVICE_CUDA ? ORUtils::MemoryBlock<Vector4u>::CUDA_TO_CPU : ORUtils::MemoryBlock<Vector4u>::CPU_TO_CPU
  );
}

void SpaintRaycaster::get_depth_input(const UChar4Image_Ptr& output) const
{
  prepare_to_copy_visualisation(m_model->get_view()->depth->noDims, output);
  if(m_model->get_settings().deviceType == ITMLibSettings::DEVICE_CUDA) m_model->get_view()->depth->UpdateHostFromDevice();
  m_visualisationEngine->DepthToUchar4(output.get(), m_model->get_view()->depth);
}

const SpaintRaycaster::RenderState_Ptr& SpaintRaycaster::get_live_render_state()
{
  return m_liveRenderState;
}

void SpaintRaycaster::get_rgb_input(const UChar4Image_Ptr& output) const
{
  prepare_to_copy_visualisation(m_model->get_view()->rgb->noDims, output);
  if(m_model->get_settings().deviceType == ITMLibSettings::DEVICE_CUDA) m_model->get_view()->rgb->UpdateHostFromDevice();
  output->SetFrom(m_model->get_view()->rgb, ORUtils::MemoryBlock<Vector4u>::CPU_TO_CPU);
}

const SpaintRaycaster::VisualisationEngine_Ptr& SpaintRaycaster::get_visualisation_engine()
{
  return m_visualisationEngine;
}

//#################### PRIVATE MEMBER FUNCTIONS ####################

void SpaintRaycaster::prepare_to_copy_visualisation(const Vector2i& inputSize, const UChar4Image_Ptr& output) const
{
  output->Clear();
  output->ChangeDims(inputSize);
}

}