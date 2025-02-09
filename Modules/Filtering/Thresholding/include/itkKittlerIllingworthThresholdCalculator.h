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

#ifndef itkKittlerIllingworthThresholdCalculator_h
#define itkKittlerIllingworthThresholdCalculator_h

#include "itkHistogramThresholdCalculator.h"

namespace itk
{

/**
 * \class KittlerIllingworthThresholdCalculator
 * \brief Computes the KittlerIllingworth's threshold for an image.
 *
 * For algorithm description see \cite kittler1986 and \cite glasbey1993.
 * Original Matlab code Copyright (C) 2004 Antti Niemisto. See
 * https://www.cs.tut.fi/~ant/histthresh/ for an excellent slide presentation and the original Matlab code.
 *
 * This class is templated over the input histogram type.
 * \warning This calculator assumes that the input histogram has only one dimension.
 *
 * \author Richard Beare. Department of Medicine, Monash University,
 * Melbourne, Australia.
 * \author Gaetan Lehmann. Biologie du Developpement et de la Reproduction, INRA de Jouy-en-Josas, France.
 *
 * This implementation was taken from the Insight Journal paper:
 * https://doi.org/10.54294/efycla
 *
 * \ingroup Operators
 * \ingroup ITKThresholding
 */
template <typename THistogram, typename TOutput = double>
class ITK_TEMPLATE_EXPORT KittlerIllingworthThresholdCalculator
  : public HistogramThresholdCalculator<THistogram, TOutput>
{
public:
  ITK_DISALLOW_COPY_AND_MOVE(KittlerIllingworthThresholdCalculator);

  /** Standard class type aliases. */
  using Self = KittlerIllingworthThresholdCalculator;
  using Superclass = HistogramThresholdCalculator<THistogram, TOutput>;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** \see LightObject::GetNameOfClass() */
  itkOverrideGetNameOfClassMacro(KittlerIllingworthThresholdCalculator);

  /** Type definition for the input image. */
  using HistogramType = THistogram;
  using OutputType = TOutput;

protected:
  KittlerIllingworthThresholdCalculator() = default;
  ~KittlerIllingworthThresholdCalculator() override = default;

  void
  GenerateData() override;

  using TotalAbsoluteFrequencyType = typename HistogramType::TotalAbsoluteFrequencyType;
  using AbsoluteFrequencyType = typename HistogramType::AbsoluteFrequencyType;
  using InstanceIdentifier = typename HistogramType::InstanceIdentifier;
  using SizeValueType = typename HistogramType::SizeValueType;

private:
  IndexValueType
  Mean();
  double
  A(InstanceIdentifier j);
  double
  B(InstanceIdentifier j);
  double
  C(InstanceIdentifier j);
};

} // end namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#  include "itkKittlerIllingworthThresholdCalculator.hxx"
#endif

#endif
