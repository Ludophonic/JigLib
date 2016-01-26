//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file heightmap.hpp 
//                     
//==============================================================
#ifndef JIGHEIGHTMAP_HPP
#define JIGHEIGHTMAP_HPP

#include "../geometry/include/primitive.hpp"
#include "../geometry/include/line.hpp"
#include "../geometry/include/plane.hpp"
#include "../maths/include/transform3.hpp"
#include "../utils/include/array2d.hpp"

namespace JigLib
{
  /// Defines a heightmap that has up in the "z" direction indexs go
  /// from "bottom right" - i.e. (0, 0) -> (xmin, ymin)
  /// heights/normals are obtained by interpolation over triangles,
  /// with each quad being divided up in the same way - the diagonal
  /// going from (i, j) to (i+1, j+1)
  class tHeightmap : public tPrimitive
  {    
  public:
    /// pass in an array of heights, and the axis that represents up
    /// Also the centre of the heightmap (assuming z is up), and the
    /// grid size
    tHeightmap(const tArray2D<tScalar> & heights, 
               tScalar x0, tScalar y0,
               tScalar dx, tScalar dy);
    
    virtual tPrimitive* Clone() const;

    virtual void GetTransform(class tTransform3 &t) const {t.position.SetTo(0.0f); t.orientation.SetToIdentity();}
    virtual void SetTransform(const class tTransform3 &t) {}
    virtual bool SegmentIntersect(tScalar &frac, tVector3 &pos, tVector3 &normal, const class tSegment &seg) const;
    virtual void GetMassProperties(const tPrimitiveProperties &primitiveProperties, 
      tScalar &mass, tVector3 &centerOfMass, tMatrix33 &inertiaTensor) const;
    virtual tScalar GetVolume() const {return 0.0f;}
    virtual tScalar GetSurfaceArea() const {return 0.0f;}

    unsigned GetNx() const {return mHeights.GetNx();}
    unsigned GetNy() const {return mHeights.GetNy();}
    
    /// Get the height at a particular index
    /// indices are clamped
    tScalar GetHeight(int i, int j) const;
    /// Get the normal
    tVector3 GetNormal(int i, int j) const;
    /// get height and normal (quicker than calling both)
    void GetHeightAndNormal(tScalar & h, tVector3 & normal, int i, int j) const;
    /// Get the full position at a particular index
    void GetSurfacePos(tVector3 & pos, int i, int j) const;
    /// Get the full position and normal at a particular index
    void GetSurfacePosAndNormal(tVector3 & pos,
                                tVector3 & normal, 
                                int i, int j) const;
    
    /// height of a world position
    tScalar GetHeight(const tVector3 & point) const;
    /// Get the normal below a world position
    tVector3 GetNormal(const tVector3 & point) const;
    /// get height and normal (quicker than calling both)
    void GetHeightAndNormal(tScalar & h, 
                            tVector3 & normal, 
                            const tVector3 & point) const;
    /// Get the full position below a world position
    void GetSurfacePos(tVector3 & pos, const tVector3 & point) const;
    /// Get the full position and normal below a world position
    void GetSurfacePosAndNormal(tVector3 & pos, 
                                tVector3 & normal, 
                                const tVector3 & point) const;
  private:
    tArray2D<tScalar> mHeights;
    
    tScalar mX0, mY0;
    tScalar mDx, mDy;
    tScalar mXMin, mYMin;
    tScalar mXMax, mYMax;
  };
}

#endif
