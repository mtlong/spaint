/**
 * grove: RGBDPatchFeatureCalculator_CUDA.h
 * Copyright (c) Torr Vision Group, University of Oxford, 2017. All rights reserved.
 */

#ifndef H_GROVE_RGBDPATCHFEATURECALCULATOR_CUDA
#define H_GROVE_RGBDPATCHFEATURECALCULATOR_CUDA

#include "../interface/RGBDPatchFeatureCalculator.h"

namespace grove {

/**
 * \brief An instance of this class can be used to compute features based on depth and colour differences in RGBD images using CUDA.
 *
 * The features are computed as described by Valentin et al. in "Exploiting Uncertainty in Regression Forests for Accurate Camera Relocalization".
 *
 * \param KeypointType    The type of keypoint computed by this class.
 * \param DescriptorType  The type of descriptor computed by this class.
 */
template <typename KeypointType, typename DescriptorType>
class RGBDPatchFeatureCalculator_CUDA : public RGBDPatchFeatureCalculator<KeypointType, DescriptorType>
{
  //#################### TYPEDEFS ####################
public:
  using typename RGBDPatchFeatureCalculator<KeypointType,DescriptorType>::DescriptorsImage;
  using typename RGBDPatchFeatureCalculator<KeypointType,DescriptorType>::KeypointsImage;

  //#################### CONSTRUCTORS ####################
private:
  /**
   * \brief Constructs a CUDA-based RGBD patch feature calculator.
   *
   * Note: This is private to force clients to make use of FeatureCalculatorFactory, which knows the correct values to use for the arguments.
   *
   * \param depthAdaptive      Whether or not to compute the depth-normalised version of the features.
   * \param depthFeatureCount  The number of features to compute from the depth image.
   * \param depthFeatureOffset The offset in the descriptor after which we store the depth features.
   * \param rgbFeatureCount    The number of features to compute from the RGB image.
   * \param rgbFeatureOffset   The offset in the descriptor after which we store the colour features.
   *
   * \throws std::invalid_argument If depthFeatureCount + rgbFeatureCount > DescriptorType::FEATURE_COUNT, or if the offsets cause out-of-bounds access.
   */
  RGBDPatchFeatureCalculator_CUDA(bool depthAdaptive, uint32_t depthFeatureCount, uint32_t depthFeatureOffset, uint32_t rgbFeatureCount, uint32_t rgbFeatureOffset);

  //#################### PUBLIC MEMBER FUNCTIONS ####################
public:
  /** Override */
  virtual void compute_feature(const ITMUChar4Image *rgbImage, const ITMFloatImage *depthImage,
                               const Matrix4f& cameraPose, const Vector4f& intrinsics,
                               KeypointsImage *keypointsImage, DescriptorsImage *descriptorsImage) const;

  //#################### FRIENDS ####################

  friend struct FeatureCalculatorFactory;
};

}

#endif