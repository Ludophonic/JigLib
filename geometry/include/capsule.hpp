//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file capsule.hpp 
//                     
//==============================================================
#ifndef JIGCAPSULE_HPP
#define JIGCAPSULE_HPP

#include "../geometry/include/primitive.hpp"
#include "../maths/include/precision.hpp"
#include "../maths/include/transform3.hpp"
#include "../maths/include/matrix33.hpp"

namespace JigLib
{
  /// defines a capsule that is orientated along its body x direction, with
  /// its start at its position.
  class tCapsule : public tPrimitive
  {
  public:
    tCapsule(const tVector3 & pos, 
             const tMatrix33 & orient,
             tScalar radius,
             tScalar length) :
      tPrimitive(tPrimitive::CAPSULE),
      mTransform(pos, orient), 
      mLength(length),
      mRadius(radius) {}
    
    virtual tPrimitive* Clone() const;

    virtual void GetTransform(class tTransform3 &t) const {t = mTransform;}
    virtual void SetTransform(const class tTransform3 &t) {mTransform = t;}
    virtual bool SegmentIntersect(tScalar &frac, tVector3 &pos, tVector3 &normal, const class tSegment &seg) const;
    virtual void GetMassProperties(const tPrimitiveProperties &primitiveProperties, 
      tScalar &mass, tVector3 &centerOfMass, tMatrix33 &inertiaTensor) const;
    virtual tScalar GetVolume() const {return (4.0f / 3.0f) * PI * Cube(mRadius) + mLength * PI * Sq(mRadius);}
    virtual tScalar GetSurfaceArea() const {return 4.0f * PI * Sq(mRadius) + mLength * 2.0f * PI * mRadius;}

    const tVector3 & GetPos() const {return mTransform.position;}
    void SetPos(const tVector3 & pos) {mTransform.position = pos;}

    tVector3 GetEnd() const {return mTransform.position + mLength * mTransform.orientation.GetLook();}

    const tMatrix33 & GetOrient() const {return mTransform.orientation;}
    void SetOrient(const tMatrix33 & orient) {mTransform.orientation = orient;}
    
    const tScalar & GetLength() const {return mLength;}
    void SetLength(const tScalar & length) {mLength = length;}

    const tScalar & GetRadius() const {return mRadius;}
    void SetRadius(const tScalar & radius) {mRadius = radius;}

  private:
    tTransform3 mTransform;
    tScalar mLength;
    tScalar mRadius;
  };
}


#endif
