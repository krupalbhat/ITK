/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkCompositeSpatialObject.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __CompositeSpatialObject_txx
#define __CompositeSpatialObject_txx

#include "itkCompositeSpatialObject.h"

namespace itk
{
  template < unsigned short NDimensions, class TransformType, class OutputType >
  CompositeSpatialObject< NDimensions, TransformType, OutputType >
  ::CompositeSpatialObject()
  {}

  template < unsigned short NDimensions, class TransformType, class OutputType >
  CompositeSpatialObject< NDimensions, TransformType, OutputType >
  ::~CompositeSpatialObject()
  {}


  template < unsigned short NDimensions, class TransformType, class OutputType >
  void 
  CompositeSpatialObject< NDimensions, TransformType, OutputType >
  ::AddSpatialObject( SuperclassPointer pointer )
  {
    m_Children.push_back( pointer );
    pointer->SetParent( this );
  }

  template < unsigned short NDimensions, class TransformType, class OutputType >
  void
  CompositeSpatialObject< NDimensions, TransformType, OutputType >
  ::RemoveSpatialObject( SuperclassPointer pointer )
  {
    ChildrenListType::iterator it = m_Children.begin();
    for( it = m_Children.begin(); it != m_Children.end(); ++it)
      {
      if( *it == pointer )
        {
        (*it)->SetParent(NULL);
        m_Children.erase( it );
        break;
        }
      }
  }
  
  template < unsigned short NDimensions, class TransformType, class OutputType >
  bool 
  CompositeSpatialObject< NDimensions, TransformType, OutputType >
  ::IsInside( const PointType &  point )
  {
    ChildrenListType::iterator it = m_Children.begin();
    ChildrenListType::iterator end = m_Children.end();
    
    for(; it!=end; it++)
      {
      if( (*it)->IsInside(point) ) 
        {
        return true;
        }
      }  
    return false;
  }

  template < unsigned short NDimensions, class TransformType, class OutputType >
  bool 
  CompositeSpatialObject< NDimensions, TransformType, OutputType >
  ::IsEvaluableAt( const PointType & point )
  {
    ChildrenListType::iterator it = m_Children.begin();
    ChildrenListType::iterator end = m_Children.end();
    
    for(; it!=end; it++)
      {
      if( (*it)->IsEvaluableAt(point) ) 
        {
        return true;
        }
      }  
    return false;
  }

  template < unsigned short NDimensions, class TransformType, class OutputType >
  void 
  CompositeSpatialObject< NDimensions, TransformType, OutputType >
  ::ValueAt( const PointType & point, OutputType & value )
  {
    ChildrenListType::iterator it = m_Children.begin();
    ChildrenListType::iterator end = m_Children.end();

    for(; it!=end; it++)
      {
      if( (*it)->IsEvaluableAt(point) )
        {
        (*it)->ValueAt(point,value); 
        }
      } 

    itk::ExceptionObject e("CompositeSpatialObject.txx");
    e.SetLocation("CompositeSpatialObject< NDimensions, TransformType, OutputType >::ValueAt( const PointType & )");
    e.SetDescription("This composite spatial object is not evaluable at the requested point");
    throw e;
  }

  template < unsigned short NDimensions, class TransformType, class OutputType >
  void 
  CompositeSpatialObject< NDimensions, TransformType, OutputType >
  ::DerivativeAt( const PointType & point, short unsigned int order, OutputVectorType & value )
  {
    ChildrenListType::iterator it = m_Children.begin();
    ChildrenListType::iterator end = m_Children.end();
    
    for(; it!=end; it++)
      {
      if( (*it)->IsInside(point) )
        {
        try
          {
          (*it)->DerivativeAt(point,order,value);
          }
        catch(...)
          {
          throw;
          }
        }
      } 
  }

  template < unsigned short NDimensions, class TransformType, class OutputType >
  void 
  CompositeSpatialObject< NDimensions, TransformType, OutputType >
  ::ComputeBounds( void )
  {
    ChildrenListType::iterator it = m_Children.begin();
    ChildrenListType::iterator end = m_Children.end();
    PointType pointLow,pointHigh;
    BoundingBoxType::PointsContainerPointer points, childrenPoints;
    BoundingBoxType::PointsContainerIterator childrenPointsIt;
    BoundingBoxType::PointsContainerIterator childrenPointsEnd;

    unsigned int n = NDimensions/2;

    if( this->GetMTime() > m_BoundsMTime )
      {

      if( !(points = m_Bounds->GetPoints()) )
        {
        points = BoundingBoxType::PointsContainer::New();
        m_Bounds->SetPoints(points);
        }

      unsigned int i = 0;

      for(; it!=end; it++)
        {
        childrenPoints = (*it)->GetBounds()->GetPoints();
        childrenPointsIt = childrenPoints->Begin();
        childrenPointsEnd = childrenPoints->End();

        for(; childrenPointsIt != childrenPointsEnd; childrenPointsIt++,i++ )
          {
          points->InsertElement( i, childrenPointsIt.Value() );
          }
        }

      points->Modified();
      m_Bounds->ComputeBoundingBox();
      m_BoundsMTime.Modified();
    }
  }
  
  template < unsigned short NDimensions, class TransformType, class OutputType >
  unsigned long
  CompositeSpatialObject< NDimensions, TransformType, OutputType >
  ::GetMTime( void ) const
  {
    ChildrenListType::const_iterator it = m_Children.begin();
    ChildrenListType::const_iterator end = m_Children.end();
 
    unsigned long latestTime = Superclass::GetMTime();
    unsigned long localTime;

    for(; it!=end; it++ )
      {
      localTime = (*it)->GetMTime();

      if( localTime > latestTime )
        {
        latestTime = localTime;
        }
      } 
    return latestTime;
  }

  template < unsigned short NDimensions, class TransformType, class OutputType >
  CompositeSpatialObject< NDimensions, TransformType, OutputType >::ChildrenListType &
  CompositeSpatialObject< NDimensions, TransformType, OutputType >
  ::GetChildren( void )
  {
    return m_Children;
  }

  template < unsigned short NDimensions, class TransformType, class OutputType >
  void 
  CompositeSpatialObject< NDimensions, TransformType, OutputType >
  ::SetChildren( ChildrenListType & children )
  { 
    m_Children = children;

    ChildrenListType::const_iterator it = m_Children.begin();
    ChildrenListType::const_iterator end = m_Children.end();
    
    for(; it != end; it++ )
      {
      (*it)->SetParent( this );    
      }
  }

  template < unsigned short NDimensions, class TransformType, class OutputType >
  unsigned int
  CompositeSpatialObject< NDimensions, TransformType, OutputType >
  ::GetNumberOfChildren( void )
  {
    return m_Children.size();
  } 

  template < unsigned short NDimensions, class TransformType, class OutputType >
  void
  CompositeSpatialObject< NDimensions, TransformType, OutputType >
  ::PrintSelf( std::ostream& os, Indent indent ) const
  {
    os << indent << "Number of children: " 
       << m_Children.size() << std::endl;
    os << indent << "List of children: ";

    ChildrenListType::const_iterator it = m_Children.begin();
    ChildrenListType::const_iterator end = m_Children.end();

    for(; it != end; it++ )
      {
      os << "[" << (*it).GetPointer() << "] ";
      }
    os << std::endl;

    Superclass::PrintSelf(os, indent);
  }

  template < unsigned short NDimensions, class TransformType, class OutputType >
  void 
  CompositeSpatialObject< NDimensions, TransformType, OutputType >
  ::RebuildGlobalToLocalTransformList( void )
  {
    BuildGlobalToLocalTransformList(m_GlobalToLocalTransformList,false);

    ChildrenListType::const_iterator it = m_Children.begin();
    ChildrenListType::const_iterator end = m_Children.end();

    for(; it != end; it++ )
      {
      (*it)->RebuildGlobalToLocalTransformList();
      }
  }

  template < unsigned short NDimensions, class TransformType, class OutputType >
  void 
  CompositeSpatialObject< NDimensions, TransformType, OutputType >
  ::RebuildLocalToGlobalTransformList( void )
  {
    BuildLocalToGlobalTransformList(m_LocalToGlobalTransformList,false);

    ChildrenListType::const_iterator it = m_Children.begin();
    ChildrenListType::const_iterator end = m_Children.end();

    for(; it != end; it++ )
      {
      (*it)->RebuildLocalToGlobalTransformList();
      }
  }
  
} // end of namespace itk 

#endif // __SpatialCompositeObject_txx



