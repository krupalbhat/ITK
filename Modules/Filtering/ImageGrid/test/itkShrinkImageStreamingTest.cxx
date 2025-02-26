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

#include <iostream>
#include "itkCastImageFilter.h"
#include "itkShrinkImageFilter.h"
#include "itkPipelineMonitorImageFilter.h"
#include "itkStreamingImageFilter.h"
#include "itkTestingMacros.h"

int
itkShrinkImageStreamingTest(int, char *[])
{

  constexpr unsigned int numberOfStreamDivisions = 4;

  // type alias to simplify the syntax
  using ShortImage = itk::Image<short, 2>;
  auto sourceImage = ShortImage::New();

  using MonitorFilter = itk::PipelineMonitorImageFilter<ShortImage>;

  // fill in an image
  constexpr ShortImage::IndexType index = { { 100, 100 } };
  constexpr ShortImage::SizeType  size = { { 8, 12 } };
  const ShortImage::RegionType    region{ index, size };
  sourceImage->SetRegions(region);
  sourceImage->Allocate();

  itk::ImageRegionIterator<ShortImage> iterator(sourceImage, region);

  short i = 0;
  for (; !iterator.IsAtEnd(); ++iterator, ++i)
  {
    iterator.Set(i);
  }


  // use caster to copy source to intermediate image of only the
  // requested region
  const itk::CastImageFilter<ShortImage, ShortImage>::Pointer caster =
    itk::CastImageFilter<ShortImage, ShortImage>::New();
  caster->SetInput(sourceImage);


  auto monitor1 = MonitorFilter::New();
  monitor1->SetInput(caster->GetOutput());

  // Create a filter, shrink by 2,3
  const itk::ShrinkImageFilter<ShortImage, ShortImage>::Pointer shrink =
    itk::ShrinkImageFilter<ShortImage, ShortImage>::New();
  shrink->SetInput(monitor1->GetOutput());

  unsigned int factors[2] = { 2, 3 };
  shrink->SetShrinkFactors(factors);


  auto monitor2 = MonitorFilter::New();
  monitor2->SetInput(shrink->GetOutput());

  const itk::StreamingImageFilter<ShortImage, ShortImage>::Pointer streamer =
    itk::StreamingImageFilter<ShortImage, ShortImage>::New();
  streamer->SetInput(monitor2->GetOutput());
  streamer->SetNumberOfStreamDivisions(numberOfStreamDivisions);
  streamer->Update();


  // this verifies that the pipeline was executed as expected along
  // with correct region propagation and output information
  if (!monitor2->VerifyAllInputCanStream(numberOfStreamDivisions))
  {
    std::cout << "Filter failed to execute as expected!" << std::endl;
    std::cout << monitor2;
    return EXIT_FAILURE;
  }

  // Verify that only the data needed is requested;
  for (const auto & j : monitor1->GetOutputRequestedRegions())
  {
    ITK_TEST_EXPECT_TRUE(j.GetSize(1) % factors[1] == 1);
  }


  return EXIT_SUCCESS;
}
