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
#ifndef itkGaussianDerivativeImageFunction_hxx
#define itkGaussianDerivativeImageFunction_hxx


#include "itkImageNeighborhoodOffsets.h"
#include "itkShapedImageNeighborhoodRange.h"
#include "itkMath.h"

#include <algorithm> // For fill_n.
#include <cassert>

namespace itk
{

template <typename TInputImage, typename TOutput>
GaussianDerivativeImageFunction<TInputImage, TOutput>::GaussianDerivativeImageFunction()
{
  std::fill_n(m_Sigma, Self::ImageDimension, 1.0);
  std::fill_n(m_Extent, Self::ImageDimension, 1.0);
  m_GaussianDerivativeSpatialFunction->SetNormalized(false); // faster
}

template <typename TInputImage, typename TOutput>
void
GaussianDerivativeImageFunction<TInputImage, TOutput>::SetInputImage(const InputImageType * ptr)
{
  Superclass::SetInputImage(ptr);
  this->RecomputeGaussianKernel();
}

template <typename TInputImage, typename TOutput>
void
GaussianDerivativeImageFunction<TInputImage, TOutput>::SetSigma(const double * sigma)
{
  if (ContainerCopyWithCheck(m_Sigma, sigma, Self::ImageDimension))
  {
    this->RecomputeGaussianKernel();
    this->Modified();
  }
}

template <typename TInputImage, typename TOutput>
void
GaussianDerivativeImageFunction<TInputImage, TOutput>::SetSigma(const double sigma)
{
  if (ContainerFillWithCheck(m_Sigma, sigma, Self::ImageDimension))
  {
    this->RecomputeGaussianKernel();
    this->Modified();
  }
}

template <typename TInputImage, typename TOutput>
void
GaussianDerivativeImageFunction<TInputImage, TOutput>::SetExtent(const double * extent)
{
  if (ContainerCopyWithCheck(m_Extent, extent, Self::ImageDimension))
  {
    this->RecomputeGaussianKernel();
    this->Modified();
  }
}

template <typename TInputImage, typename TOutput>
void
GaussianDerivativeImageFunction<TInputImage, TOutput>::SetExtent(const double extent)
{
  if (ContainerFillWithCheck(m_Extent, extent, Self::ImageDimension))
  {
    this->RecomputeGaussianKernel();
    this->Modified();
  }
}

template <typename TInputImage, typename TOutput>
void
GaussianDerivativeImageFunction<TInputImage, TOutput>::RecomputeGaussianKernel()
{
  const TInputImage * const inputImage = this->GetInputImage();

  if (inputImage == nullptr)
  {
    // Do clean-up, to ensure that the operators will
    // not refer to a previous image, and to reduce memory usage.
    m_OperatorArray = OperatorArrayType();
  }
  else
  {
    using SpacingType = typename TInputImage::SpacingType;
    const SpacingType spacing = m_UseImageSpacing ? inputImage->GetSpacing() : MakeFilled<SpacingType>(1);

    for (unsigned int direction = 0; direction < Self::ImageDimension; ++direction)
    {
      // Set the derivative of the Gaussian first
      OperatorNeighborhoodType                                  dogNeighborhood;
      typename GaussianDerivativeSpatialFunctionType::InputType pt;
      typename NeighborhoodType::SizeType                       size{};
      size[direction] = static_cast<SizeValueType>(m_Sigma[direction] * m_Extent[direction]);
      dogNeighborhood.SetRadius(size);
      m_ImageNeighborhoodOffsets[direction] = GenerateRectangularImageNeighborhoodOffsets(size);

      typename GaussianDerivativeSpatialFunctionType::ArrayType s;
      s[0] = m_Sigma[direction];
      m_GaussianDerivativeSpatialFunction->SetSigma(s);

      typename OperatorNeighborhoodType::Iterator it = dogNeighborhood.Begin();

      unsigned int i = 0;

      const typename TInputImage::SpacingValueType directionSpacing = spacing[direction];
      assert(directionSpacing != 0);

      while (it != dogNeighborhood.End())
      {
        pt[0] = dogNeighborhood.GetOffset(i)[direction] * directionSpacing;
        (*it) = m_GaussianDerivativeSpatialFunction->Evaluate(pt);
        ++i;
        ++it;
      }

      m_OperatorArray[direction] = dogNeighborhood;

      // Note: A future version of ITK could possibly also set a Gaussian blurring operator
      // here, which should then be applied at EvaluateAtIndex(index).
    }
  }
}

template <typename TInputImage, typename TOutput>
auto
GaussianDerivativeImageFunction<TInputImage, TOutput>::EvaluateAtIndex(const IndexType & index) const -> OutputType
{
  OutputType gradient;

  const TInputImage * const image = this->GetInputImage();

  for (unsigned int direction = 0; direction < Self::ImageDimension; ++direction)
  {
    // Note: A future version of ITK should do Gaussian blurring here.

    double result = 0.0;

    const OperatorNeighborhoodType & operatorNeighborhood = m_OperatorArray[direction];

    const ShapedImageNeighborhoodRange<const InputImageType> neighborhoodRange(
      *image, index, m_ImageNeighborhoodOffsets[direction]);
    assert(neighborhoodRange.size() == operatorNeighborhood.Size());

    auto neighborhoodRangeIterator = neighborhoodRange.cbegin();

    for (const TOutput & kernelValue : operatorNeighborhood.GetBufferReference())
    {
      result += kernelValue * (*neighborhoodRangeIterator);
      ++neighborhoodRangeIterator;
    }
    gradient[direction] = result;
  }

  return gradient;
}

template <typename TInputImage, typename TOutput>
auto
GaussianDerivativeImageFunction<TInputImage, TOutput>::Evaluate(const PointType & point) const -> OutputType
{
  IndexType index;

  this->ConvertPointToNearestIndex(point, index);
  return this->EvaluateAtIndex(index);
}

template <typename TInputImage, typename TOutput>
auto
GaussianDerivativeImageFunction<TInputImage, TOutput>::EvaluateAtContinuousIndex(
  const ContinuousIndexType & cindex) const -> OutputType
{
  IndexType index;

  this->ConvertContinuousIndexToNearestIndex(cindex, index);
  return this->EvaluateAtIndex(index);
}

template <typename TInputImage, typename TOutput>
void
GaussianDerivativeImageFunction<TInputImage, TOutput>::PrintSelf(std::ostream & os, Indent indent) const
{
  this->Superclass::PrintSelf(os, indent);
  itkPrintSelfBooleanMacro(UseImageSpacing);

  os << indent << "Sigma: " << m_Sigma << std::endl;
  os << indent << "Extent: " << m_Extent << std::endl;

  os << indent << "OperatorArray: " << m_OperatorArray << std::endl;
  os << indent << "GaussianDerivativeSpatialFunction: " << m_GaussianDerivativeSpatialFunction << std::endl;
}

} // end namespace itk

#endif
