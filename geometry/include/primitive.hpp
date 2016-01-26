//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file primitive.hpp 
//                     
//==============================================================
#ifndef JIGPRIMITIVE_HPP
#define JIGPRIMITIVE_HPP

#include "../utils/include/assert.hpp"
#include "../maths/include/vector3.hpp"
#include "../maths/include/matrix33.hpp"

namespace JigLib
{
  /// All geometry primitives should derive from this so that it's possible to 
  /// cast them into the correct type without the overhead/hassle of RTTI or 
  /// virtual fns specific to just one class of primitive. Just do a static cast
  /// based on the type, or use the Get functions
  ///
  /// However, destruction requires virtual functions really, as does supporting other
  /// user-defined primitives
  class tPrimitive
  {
  public:
    enum tType 
    {
      AABOX,
      BOX,
      CAPSULE,
      HEIGHTMAP,
      PLANE,
      SPHERE,
      TRIANGLEMESH,
      NUM_TYPES // can add more user-defined types
    };

    tPrimitive(unsigned type) : mType(type) {}
    virtual ~tPrimitive() {}

    /// returns a copy on the heap
    virtual tPrimitive* Clone() const = 0;

    virtual void GetTransform(class tTransform3 &t) const = 0;
    virtual void SetTransform(const class tTransform3 &t) = 0;

    /// must support intersection with a segment (ray cast)
    virtual bool SegmentIntersect(tScalar &frac, tVector3 &pos, tVector3 &normal, const class tSegment &seg) const = 0;

    struct tPrimitiveProperties
    {
      enum tMassDistribution {SOLID, SHELL};
      /// density is mass per volume of SOLID, otherwise mass per surface area
      enum tMassType {MASS, DENSITY};
      tPrimitiveProperties(tMassDistribution massDistribution, tMassType massType, tScalar massOrDensity)
        : mMassDistribution(massDistribution), mMassType(massType), mMassOrDensity(massOrDensity) {}
      tMassDistribution mMassDistribution;
      tMassType mMassType;
      tScalar mMassOrDensity;
    };

    /// Calculate and return the volume
    virtual tScalar GetVolume() const = 0;
    
    /// Calculate and return the surface area
    virtual tScalar GetSurfaceArea() const = 0;

    /// Returns the mass, center of mass, and intertia tensor around the origin
    virtual void GetMassProperties(const tPrimitiveProperties &primitiveProperties, 
      tScalar &mass, tVector3 &centerOfMass, tMatrix33 &inertiaTensor) const = 0;

    ///  returns a bounding box that covers this primitive. Default returns a huge box, so
    /// implement this in the derived class for for efficiency
    virtual const class tAABox &GetBoundingBox() const;

    unsigned GetType() const {return mType;}

    class tAABox &GetAABox() {Assert(mType == AABOX); return *((tAABox*) this);}
    const class tAABox &GetAABox() const {Assert(mType == AABOX); return *((const tAABox*) this);}

    class tBox &GetBox() {Assert(mType == BOX); return *((tBox*) this);}
    const class tBox &GetBox() const {Assert(mType == BOX); return *((const tBox*) this);}

    class tCapsule &GetCapsule() {Assert(mType == CAPSULE); return *((tCapsule*) this);}
    const class tCapsule &GetCapsule() const {Assert(mType == CAPSULE); return *((const tCapsule*) this);}

    class tHeightmap &GetHeightmap() {Assert(mType == HEIGHTMAP); return *((tHeightmap*) this);}
    const class tHeightmap &GetHeightmap() const {Assert(mType == HEIGHTMAP); return *((const tHeightmap*) this);}

    class tPlane &GetPlane() {Assert(mType == PLANE); return *((tPlane*) this);}
    const class tPlane &GetPlane() const {Assert(mType == PLANE); return *((const tPlane*) this);}

    class tSphere &GetSphere() {Assert(mType == SPHERE); return *((tSphere*) this);}
    const class tSphere &GetSphere() const {Assert(mType == SPHERE); return *((const tSphere*) this);}

    class tTriangleMesh &GetTriangleMesh() {Assert(mType == TRIANGLEMESH); return *((tTriangleMesh*) this);}
    const class tTriangleMesh &GetTriangleMesh() const {Assert(mType == TRIANGLEMESH); return *((const tTriangleMesh*) this);}

  protected:
    const tPrimitive &operator=(const tPrimitive &rhs) const {Assert(mType == rhs.mType); return *this;}

  private:
    const unsigned mType;
 };
}

#endif
