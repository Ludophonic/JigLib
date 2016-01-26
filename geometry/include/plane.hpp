//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file plane.hpp 
//                     
//==============================================================
#ifndef JIGPLANE_HPP
#define JIGPLANE_HPP

#include "../geometry/include/primitive.hpp"
#include "../geometry/include/line.hpp"
#include "../maths/include/precision.hpp"
#include "../maths/include/transform3.hpp"

namespace JigLib
{
  class tPlane : public tPrimitive
  {    
  public:
    tPlane() : tPrimitive(tPrimitive::PLANE) {}
    /// the full plane representation - n must be normalised!
    tPlane(const tVector3 & n, const tScalar d);
    /// plane with a specified normal passing through a point
    tPlane(const tVector3 & n, const tVector3 & pos);
    /// plane passing through 3 points
    tPlane(const tVector3 & pos0, 
                  const tVector3 & pos1, 
                  const tVector3 & pos2);
    
    virtual tPrimitive* Clone() const;

    virtual void GetTransform(class tTransform3 &t) const {t.position.SetTo(0.0f); t.orientation.SetToIdentity();}
    virtual void SetTransform(const class tTransform3 &t) {}
    virtual bool SegmentIntersect(tScalar &frac, tVector3 &pos, tVector3 &normal, const class tSegment &seg) const;
    virtual void GetMassProperties(const tPrimitiveProperties &primitiveProperties, 
      tScalar &mass, tVector3 &centerOfMass, tMatrix33 &inertiaTensor) const;
    virtual tScalar GetVolume() const {return 0.0f;}
    virtual tScalar GetSurfaceArea() const {return 0.0f;}

    const tVector3 & GetN() const {return mN;}
    void SetN(const tVector3 n) {mN = n;}
    
    const tScalar & GetD() const {return mD;}
    void SetD(tScalar d) {mD = d;}
    
    void Set(const tVector3 & n, const tScalar d);
    void Set(const tVector3 & n, const tVector3 & pos);
    void Set(const tVector3 & pos0, 
                    const tVector3 & pos1, 
                    const tVector3 & pos2);
    
    bool GetLineIntersection(tScalar & t,
                                    const tLine & line) const;
    
    bool GetRayIntersection(tScalar & t,
                                   const tRay & ray) const;
    
    bool GetSegmentIntersection(tScalar & t,
                                       const tSegment & segment) const;
    
    /// Make the plane stay in the same place but point the other
    /// direction.
    tPlane & Invert() { mN.Negate(); mD = -mD; return *this;}
    /// get an inverted copy
    tPlane GetInverse() const { 
      tPlane result(*this); result.Invert(); return result;}
    
    /// Transform plane from one reference frame to another
    tPlane & Transform(const tMatrix33 & mat, 
                              const tVector3 & vec);
    tPlane GetTransform(const tMatrix33 & mat, 
                               const tVector3 & vec) const;
    
  private:
    /// private because everyone else should use the one in distance.hpp, 
    /// but we can't use that definition in our inline implementation
    tScalar GetDistanceToPoint(const tVector3 & pos) const;
    
    tVector3 mN;
    tScalar mD;
  };
  
#include "../geometry/include/plane.inl"
}

#endif
