/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkMembershipSampleGenerator.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

Copyright (c) 2001 Insight Consortium
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

 * The name of the Insight Consortium, nor the names of any consortium members,
   nor of any contributors, may be used to endorse or promote products derived
   from this software without specific prior written permission.

  * Modified source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS ``AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/
#ifndef __itkMembershipSampleGenerator_h
#define __itkMembershipSampleGenerator_h


#include "itkObject.h"
#include "itkMembershipSample.h"

namespace itk{
  namespace Statistics{

/** \class MembershipSampleGenerator
 * \brief MembershipSampleGenerator generates a MembershipSample object
 * using a class mask sample.
 *
 * You can see it as a filter that takes two samples as input and generates 
 * a MembershipSample. There are two class template arguments first one 
 * is the type of basic sample that has measurement vectors in it, and
 * the second one is the type of sample that its measurement vectors has 
 * only one component - class label. What it does is that it merges 
 * measurement vectors and class labels in a single MembershipSample
 * object.
 *
 * To get the MembershipSample object you should plug in the input sample
 * and the class mask sample using SetInput and SetClassMask methods 
 * respectively.
 *
 * NOTE: This generator expects the class labels in the class mask sample
 * to be integer value where the value starts 0 and increases without
 * any missing value between 0 to the last class label value.
 *
 * \sa MembershipSample
 */

template< class TInputSample, class TClassMaskSample >
class ITK_EXPORT MembershipSampleGenerator : 
      public Object
{
public:
  /** Standard class typedefs. */
  typedef MembershipSampleGenerator Self;
  typedef Object Superclass ;
  typedef SmartPointer<Self> Pointer;

  /** Standard Macros */
  itkTypeMacro(MembershipSampleGenerator, Object);
  itkNewMacro(Self) ;

  /** Input sample types alias */
  typedef TInputSample InputType ;
  typedef typename TInputSample::Pointer InputPointer ;
  typedef typename TInputSample::MeasurementVectorType MeasurementVectorType ;

  /** Class mask sample types alias */
  typedef TClassMaskSample ClassMaskType ;
  typedef typename TClassMaskSample::Pointer ClassMaskPointer ;

  /** Output: MembershipSample types */
  typedef MembershipSample< InputType > OutputType ;
  typedef OutputType::Pointer OutputPointer ;

  /** Stores the input sample */
  void SetInput(InputPointer sample) ;
  
  /** Returns the input sample pointer */
  InputPointer GetInput() ;

  /** Stores the class mask sample */
  void SetClassMask(ClassMaskPointer classMask) ;

  /** Returns the class mask sample pointer*/
  ClassMaskPointer GetClassMask() ;

  /** Returns the pointer to the MembershipSample object
   * that has been generated by this */
  OutputPointer GetOutput() ;

  /** The actual output generation procedure. */
  void GenerateData() ;

protected:
  MembershipSampleGenerator() ;
  virtual ~MembershipSampleGenerator() {}
  void PrintSelf(std::ostream& os, Indent indent) const;

private:
  InputPointer m_Input ;
  ClassMaskPointer m_ClassMask ;
  OutputPointer m_Output ;
} ; // end of class

  } // end namespace Statistics
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMembershipSampleGenerator.txx"
#endif

#endif
