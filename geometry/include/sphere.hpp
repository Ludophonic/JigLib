//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file sphere.hpp 
//                     
//==============================================================
#ifndef JIGSPHERE_HPP
#define JIGSPHERE_HPP

#include "../geometry/include/primitive.hpp"
#include "../maths/include/precision.hpp"
#include "../maths/include/transform3.hpp"

namespace JigLib
{
  class tSphere : public tPrimitive
  {
  public:
    tSphere(const tVector3 & pos, tScalar radius) :
        tPrimitive(tPrimitive::SPHERE), 
        mPos(pos), mRadius(radius) {}
    tSphere() : tPrimitive(tPrimitive::SPHERE) {}
    
    virtual tPrimitive* Clone() const;

    virtual void GetTransform(class tTransform3 &t) const {t.position = mPos; t.orientation.SetToIdentity();}
    virtual void SetTransform(const class tTransform3 &t) {mPos = t.position;}
    virtual bool SegmentIntersect(tScalar &frac, tVector3 &pos, tVector3 &normal, const class tSegment &seg) const;
    virtual void GetMassProperties(const tPrimitiveProperties &primitiveProperties, 
      tScalar &mass, tVector3 &centerOfMass, tMatrix33 &inertiaTensor) const;
    virtual tScalar GetVolume() const {return (4.0f / 3.0f) * PI * Cube(mRadius);}
    virtual tScalar GetSurfaceArea() const {return 4.0f * PI * Sq(mRadius);}

    const tVector3 & GetPos() const {return mPos;}
    void SetPos(const tVector3 & pos) {mPos = pos;}
    
    const tScalar & GetRadius() const {return mRadius;}
    void SetRadius(const tScalar & radius) {mRadius = radius;}
    
    static const tSphere & HugeSphere() {return mHugeSphere;}
  private:
    tVector3 mPos;
    tScalar mRadius;
    
    static tSphere mHugeSphere;
  };
}


#endif
