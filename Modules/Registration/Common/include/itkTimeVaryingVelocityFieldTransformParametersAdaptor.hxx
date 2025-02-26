/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef itkTimeVaryingVelocityFieldTransformParametersAdaptor_hxx
#define itkTimeVaryingVelocityFieldTransformParametersAdaptor_hxx


#include "itkIdentityTransform.h"
#include "itkResampleImageFilter.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkMath.h"

namespace itk
{

template <typename TTransform>
TimeVaryingVelocityFieldTransformParametersAdaptor<TTransform>::TimeVaryingVelocityFieldTransformParametersAdaptor()
{
  this->m_RequiredFixedParameters.SetSize(TotalDimension * (TotalDimension + 3));
  this->m_RequiredFixedParameters.Fill(0.0);
}

template <typename TTransform>
void
TimeVaryingVelocityFieldTransformParametersAdaptor<TTransform>::SetRequiredSize(const SizeType & size)
{
  bool isModified = false;
  for (SizeValueType d = 0; d < TotalDimension; ++d)
  {
    if (Math::NotExactlyEquals(this->m_RequiredFixedParameters[d], size[d]))
    {
      isModified = true;
    }
    this->m_RequiredFixedParameters[d] = size[d];
  }

  if (isModified)
  {
    itkDebugMacro("Setting size to " << size);
    this->Modified();
  }
}

template <typename TTransform>
auto
TimeVaryingVelocityFieldTransformParametersAdaptor<TTransform>::GetRequiredSize() const -> const SizeType
{
  SizeType size;
  for (SizeValueType d = 0; d < TotalDimension; ++d)
  {
    size[d] = static_cast<SizeValueType>(this->m_RequiredFixedParameters[d]);
  }
  return size;
}

template <typename TTransform>
void
TimeVaryingVelocityFieldTransformParametersAdaptor<TTransform>::SetRequiredOrigin(const PointType & origin)
{
  bool isModified = false;
  for (SizeValueType d = 0; d < TotalDimension; ++d)
  {
    if (Math::NotExactlyEquals(this->m_RequiredFixedParameters[TotalDimension + d], origin[d]))
    {
      isModified = true;
    }
    this->m_RequiredFixedParameters[TotalDimension + d] = origin[d];
  }

  if (isModified)
  {
    itkDebugMacro("Setting origin to " << origin);
    this->Modified();
  }
}

template <typename TTransform>
auto
TimeVaryingVelocityFieldTransformParametersAdaptor<TTransform>::GetRequiredOrigin() const -> const PointType
{
  PointType origin;
  for (SizeValueType d = 0; d < TotalDimension; ++d)
  {
    origin[d] = this->m_RequiredFixedParameters[TotalDimension + d];
  }
  return origin;
}

template <typename TTransform>
void
TimeVaryingVelocityFieldTransformParametersAdaptor<TTransform>::SetRequiredSpacing(const SpacingType & spacing)
{
  bool isModified = false;
  for (SizeValueType d = 0; d < TotalDimension; ++d)
  {
    if (Math::NotExactlyEquals(this->m_RequiredFixedParameters[2 * TotalDimension + d], spacing[d]))
    {
      isModified = true;
    }
    this->m_RequiredFixedParameters[2 * TotalDimension + d] = spacing[d];
  }

  if (isModified)
  {
    itkDebugMacro("Setting spacing to " << spacing);
    this->Modified();
  }
}

template <typename TTransform>
auto
TimeVaryingVelocityFieldTransformParametersAdaptor<TTransform>::GetRequiredSpacing() const -> const SpacingType
{
  SpacingType spacing;
  for (SizeValueType d = 0; d < TotalDimension; ++d)
  {
    spacing[d] = this->m_RequiredFixedParameters[2 * TotalDimension + d];
  }
  return spacing;
}

template <typename TTransform>
void
TimeVaryingVelocityFieldTransformParametersAdaptor<TTransform>::SetRequiredDirection(const DirectionType & direction)
{
  bool isModified = false;
  for (SizeValueType di = 0; di < TotalDimension; ++di)
  {
    for (SizeValueType dj = 0; dj < TotalDimension; ++dj)
    {
      if (Math::NotExactlyEquals(this->m_RequiredFixedParameters[3 * TotalDimension + (di * TotalDimension + dj)],
                                 direction[di][dj]))
      {
        isModified = true;
      }
      this->m_RequiredFixedParameters[3 * TotalDimension + (di * TotalDimension + dj)] = direction[di][dj];
    }
  }

  if (isModified)
  {
    itkDebugMacro("Setting direction to " << direction);
    this->Modified();
  }
}

template <typename TTransform>
auto
TimeVaryingVelocityFieldTransformParametersAdaptor<TTransform>::GetRequiredDirection() const -> const DirectionType
{
  DirectionType direction;
  for (SizeValueType di = 0; di < TotalDimension; ++di)
  {
    for (SizeValueType dj = 0; dj < TotalDimension; ++dj)
    {
      direction[di][dj] = this->m_RequiredFixedParameters[3 * TotalDimension + (di * TotalDimension + dj)];
    }
  }
  return direction;
}

template <typename TTransform>
void
TimeVaryingVelocityFieldTransformParametersAdaptor<TTransform>::AdaptTransformParameters()
{
  if (!this->m_Transform)
  {
    itkExceptionMacro("Transform has not been set.");
  }

  if (this->m_RequiredFixedParameters == this->m_Transform->GetFixedParameters())
  {
    return;
  }

  const SizeType      newFieldSize = this->GetRequiredSize();
  const PointType     newFieldOrigin = this->GetRequiredOrigin();
  const SpacingType   newFieldSpacing = this->GetRequiredSpacing();
  const DirectionType newFieldDirection = this->GetRequiredDirection();

  using IdentityTransformType = IdentityTransform<ParametersValueType, TotalDimension>;
  auto identityTransform = IdentityTransformType::New();
  identityTransform->SetIdentity();

  using LinearInterpolatorType = LinearInterpolateImageFunction<TimeVaryingVelocityFieldType, ParametersValueType>;
  auto interpolator = LinearInterpolatorType::New();
  interpolator->SetInputImage(this->m_Transform->GetVelocityField());

  using ResamplerType =
    ResampleImageFilter<TimeVaryingVelocityFieldType, TimeVaryingVelocityFieldType, ParametersValueType>;
  auto resampler = ResamplerType::New();
  resampler->SetInput(this->m_Transform->GetVelocityField());
  resampler->SetOutputDirection(newFieldDirection);
  resampler->SetOutputOrigin(newFieldOrigin);
  resampler->SetOutputSpacing(newFieldSpacing);
  resampler->SetSize(newFieldSize);
  resampler->SetTransform(identityTransform);
  resampler->SetInterpolator(interpolator);

  const typename TimeVaryingVelocityFieldType::Pointer newTimeVaryingVelocityField = resampler->GetOutput();
  newTimeVaryingVelocityField->Update();
  newTimeVaryingVelocityField->DisconnectPipeline();

  this->m_Transform->SetVelocityField(newTimeVaryingVelocityField);
  this->m_Transform->SetLowerTimeBound(0.0);
  this->m_Transform->SetUpperTimeBound(1.0);
  this->m_Transform->IntegrateVelocityField();
}

} // namespace itk

#endif
