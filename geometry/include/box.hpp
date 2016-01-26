//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file box.hpp 
//                     
//==============================================================
#ifndef JIGBOX_HPP
#define JIGBOX_HPP

#include "../geometry/include/line.hpp"
#include "../geometry/include/primitive.hpp"
#include "../maths/include/transform3.hpp"
#include "../maths/include/matrix33.hpp"

namespace JigLib
{
  class tBox : public tPrimitive
  {
  public:
    // position/orientation are based on one corner the box. Sides are
    // the full side lengths
    tBox(const tVector3 & pos, 
         const tMatrix33 & orient,
         const tVector3 & sideLengths) :
    tPrimitive(tPrimitive::BOX),
      mTransform(pos, orient), mSideLengths(sideLengths) {}
    tBox() : tPrimitive(tPrimitive::BOX) {}
    
    virtual tPrimitive* Clone() const;
    
    /// Get the box corner/origin position
    const tVector3 & GetPos() const {return mTransform.position;}
    /// Set the box corner/origin position
    void SetPos(const tVector3 & pos) {mTransform.position = pos;}
    
    // inherited
    virtual void GetTransform(class tTransform3 &t) const {t = mTransform;}
    virtual void SetTransform(const class tTransform3 &t) {mTransform = t;}
    virtual bool SegmentIntersect(tScalar &frac, tVector3 &pos, tVector3 &normal, const class tSegment &seg) const;
    virtual void GetMassProperties(const tPrimitiveProperties &primitiveProperties, 
				   tScalar &mass, tVector3 &centerOfMass, tMatrix33 &inertiaTensor) const;
    virtual tScalar GetVolume() const {
 return mSideLengths.x * mSideLengths.y * mSideLengths.z;}
    virtual tScalar GetSurfaceArea() const {
 return 2.0f * (mSideLengths.x * mSideLengths.y + mSideLengths.x * mSideLengths.z + mSideLengths.y * mSideLengths.z);}
    

    /// Get the box centre position
    tVector3 GetCentre() const {return mTransform.position + mTransform.orientation * (0.5f * mSideLengths);}
   
    /// Get bounding radius around the centre
    tScalar GetBoundingRadiusAboutCentre() const;

    /// Get the box orientation
    const tMatrix33 & GetOrient() const {return mTransform.orientation;}
    /// Set the box orientation
    void SetOrient(const tMatrix33 & orient) {mTransform.orientation = orient;}
    
    /// Get the three side lengths of the box
    const tVector3 & GetSideLengths() const {return mSideLengths;}
    /// Set the three side lengths of the box
    void SetSideLengths(const tVector3 & sideLengths) {
      mSideLengths = sideLengths;}

    /// expands box by amount on each side (in both +ve and -ve directions)
    void Expand(const tVector3 &amount);

    /// Returns the half-side lengths
    const tVector3 GetHalfSideLengths() const {return 0.5f * mSideLengths;}

    /// returns the vector representing the edge direction 
    const tVector3 GetSide(unsigned i) const {return mTransform.orientation[i] * mSideLengths[i];}

    /// returns the squared distance 
    /// todo remove this/put it in distance fns
    tScalar GetSqDistanceToPoint(tVector3 & closestBoxPoint,
                                 const tVector3 & point) const;
    
    /// Returns the distance from the point to the box, (-ve if the
    /// point is inside the box), and optionally the closest point on
    /// the box.
    /// TODO make this actually return -ve if inside
    /// todo remove this/put it in distance fns
    tScalar GetDistanceToPoint(tVector3 & closestBoxPoint,
                               const tVector3 & point) const {
      return Sqrt(GetSqDistanceToPoint(closestBoxPoint, point));}
    
    
    /// Gets the minimum and maximum extents of the box along the
    /// axis, relative to the centre of the box.
    inline void GetSpan(tScalar & min,
                        tScalar & max,
                        const tVector3 & axis) const;
    
    /// indices into the points returned by GetCornerPoints
    enum tBoxPointIndex {BRD, BRU, BLD, BLU, FRD, FRU, FLD, FLU};
    
    /// Gets the corner points, populating pts
    inline void GetCornerPoints(tVector3 pts[8]) const;
    
    /// tEdge just contains indexes into the points returned by GetCornerPoints
    struct tEdge {tBoxPointIndex ind0, ind1;};
    // Returns a (const) list of 12 edges - at the moment in this order:
    //  {BRD, BRU}, // origin-up
    //  {BRD, BLD}, // origin-left
    //  {BRD, FRD}, // origin-fwd
    //  {BLD, BLU}, // leftorigin-up
    //  {BLD, FLD}, // leftorigin-fwd
    //  {FRD, FRU}, // fwdorigin-up
    //  {FRD, FLD}, // fwdorigin-left
    //  {BRU, BLU}, // uporigin-left
    //  {BRU, FRU}, // uporigin-fwd
    //  {BLU, FLU}, // upleftorigin-fwd
    //  {FRU, FLU}, // upfwdorigin-left
    //  {FLD, FLU}, // fwdleftorigin-up
    inline const tEdge * GetAllEdges() const {return mEdges;}
    
    /// edgeIndices will contain indexes into the result of GetAllEdges
    inline void GetEdgesAroundPoint(unsigned edgeIndices[3], tBoxPointIndex pt) const;
    
  private:
    tTransform3 mTransform;
    tVector3 mSideLengths;
    static const tEdge mEdges[12];
  };
  
  inline tScalar tBox::GetBoundingRadiusAboutCentre() const
  {
    return 0.5f * mSideLengths.GetLength();
  }


  //==============================================================
  // GetSpan
  //==============================================================
  inline void tBox::GetSpan(tScalar & min,
                            tScalar & max,
                            const tVector3 & axis) const
  {
    tScalar s = Abs(Dot(axis, mTransform.orientation.mCols[0])) * (0.5f * mSideLengths.x);
    tScalar u = Abs(Dot(axis, mTransform.orientation.mCols[1])) * (0.5f * mSideLengths.y);
    tScalar d = Abs(Dot(axis, mTransform.orientation.mCols[2])) * (0.5f * mSideLengths.z);
    tScalar r = s + u + d;
    tScalar p = Dot(GetCentre(), axis);
    min = p-r;
    max = p+r;
  }
  
  //==============================================================
  // GetCornerPoints
  //==============================================================
  inline void tBox::GetCornerPoints(tVector3 pts[8]) const
  {
    pts[BRD] = mTransform.position;
    AddScaleVector3(pts[FRD], mTransform.position, mSideLengths.x, mTransform.orientation.GetLook());
    AddScaleVector3(pts[BLD], mTransform.position, mSideLengths.y, mTransform.orientation.GetLeft());
    AddScaleVector3(pts[BRU], mTransform.position, mSideLengths.z, mTransform.orientation.GetUp());
    AddScaleVector3(pts[FLD], pts[BLD], mSideLengths.x, mTransform.orientation.GetLook());
    AddScaleVector3(pts[BLU], pts[BRU], mSideLengths.y, mTransform.orientation.GetLeft());
    AddScaleVector3(pts[FRU], pts[FRD], mSideLengths.z, mTransform.orientation.GetUp());
    AddScaleVector3(pts[FLU], pts[FLD], mSideLengths.z, mTransform.orientation.GetUp());
  }
  
  //==============================================================
  // GetEdgesAroundPoint
  //==============================================================
  inline void tBox::GetEdgesAroundPoint(unsigned edgeIndices[3], tBox::tBoxPointIndex pt) const
  {
    unsigned ind = 0;
    for (unsigned i = 0 ; i < sizeof(mEdges) / sizeof(mEdges[0]) ; ++i)
      {
	if ( (mEdges[i].ind0 == pt) || (mEdges[i].ind1 == pt) )
	  edgeIndices[ind++] = i;
	if (ind == 3)
	  return;
      }
    Assert(ind == 3);
  }

  //==============================================================
  // Expand
  //==============================================================
  inline void tBox::Expand(const tVector3 &amount)
  {
    mTransform.position -= mTransform.orientation * amount;
    AddScaleVector3(mSideLengths, mSideLengths, 2.0f, amount);
  }

}


#endif
