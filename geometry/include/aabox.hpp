//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file aabox.hpp 
//                     
//==============================================================
#ifndef JIGAABOX_HPP
#define JIGAABOX_HPP

#include "../geometry/include/primitive.hpp"
#include "../maths/include/transform3.hpp"
#include "../geometry/include/box.hpp"
#include "../geometry/include/sphere.hpp"
#include "../geometry/include/capsule.hpp"

namespace JigLib
{
  /// An axis-aligned box
  class tAABox : public tPrimitive
  {
  public:
    /// position based on one corner. sideLengths are the full side
    /// lengths (each element must be >= 0)
    tAABox(const tVector3 & minPos,
           const tVector3 & maxPos) :
      tPrimitive(tPrimitive::AABOX),
      mMinPos(minPos), mMaxPos(maxPos) {}

    tAABox(bool clear = true);
    
    virtual tPrimitive* Clone() const;

    virtual void GetTransform(tTransform3 &t) const {t.position = mMinPos; t.orientation = tMatrix33::Identity();}
    virtual void SetTransform(const tTransform3 &t) {mMaxPos = t.position + mMaxPos - mMinPos; mMinPos = t.position;}
    virtual bool SegmentIntersect(tScalar &frac, tVector3 &pos, tVector3 &normal, const class tSegment &seg) const;
    virtual void GetMassProperties(const tPrimitiveProperties &primitiveProperties, 
      tScalar &mass, tVector3 &centerOfMass, tMatrix33 &inertiaTensor) const;
    virtual tScalar GetVolume() const {return (mMaxPos - mMinPos).GetLengthSq();}
    virtual tScalar GetSurfaceArea() const {tVector3 sl = mMaxPos - mMinPos; return 2.0f * (sl.x * sl.y + sl.x * sl.z + sl.y * sl.z);}

    // adding points etc
    void Clear();
    void AddPoint(const tVector3 & pos);
    void AddSphere(const class tSphere & sphere);
    void AddCapsule(const class tCapsule & capsule);
    void AddBox(const class tBox & box);
    void AddSegment(const class tSegment & seg);
    void AddAABox(const tAABox &aabox);
    void AddPrimitive(const tPrimitive &prim);
    
    /// move it
    void Move(const tVector3 & delta) {mMinPos += delta; mMaxPos += delta;}

    // checking for containment
    bool IsPointInside(const tVector3 & pos) const;
    
    // overlap
    friend bool OverlapTest(const tAABox & box0, const tAABox & box1);
    friend bool OverlapTest(const tAABox & box0, const tAABox & box1, tScalar tol);
    
    tVector3 GetCentre() const {return 0.5f * (mMinPos + mMaxPos);}
    
    const tVector3 & GetMinPos() const {return mMinPos;}
    void SetMinPos(const tVector3 & minPos) {mMinPos = minPos;}
    
    const tVector3 & GetMaxPos() const {return mMaxPos;}
    void SetMaxPos(const tVector3 & maxPos) {mMaxPos = maxPos;}
    
    tVector3 GetSideLengths() const {return mMaxPos - mMinPos;}

    /// todo return sqrt of radiusSq fn
    tScalar GetRadiusAboutCentre() const {return 0.5f * (mMaxPos - mMinPos).GetLength();}
    /// todo optimise
    tScalar GetRadiusSqAboutCentre() const {return Sq(GetRadiusAboutCentre());}

    static const tAABox & HugeBox() {return mHugeBox;}
    
  private:
    tVector3 mMinPos, mMaxPos;
    
    static tAABox mHugeBox;
  };
  
  bool OverlapTest(const tAABox & box0, const tAABox & box1);
  
#include "../geometry/include/aabox.inl"
}


#endif
