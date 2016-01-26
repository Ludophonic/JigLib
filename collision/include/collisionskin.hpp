//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file collisionskin.hpp 
//                     
//==============================================================
#ifndef JIGCOLLISIONSKIN_HPP
#define JIGCOLLISIONSKIN_HPP

#include "../collision/include/materials.hpp"
#include "../maths/include/transform3.hpp"
#include "../geometry/include/aabox.hpp"

namespace JigLib
{
  /// todo make this a template param of tCollisionSkin
  struct tCollisionSkinExternalData
  {
    tCollisionSkinExternalData() : mFloat(0.0f), mInt(0), mPointer(0) {}
    float mFloat;
    int   mInt;
    void *mPointer;
  };

  /// Some skins may be owned by a physical body too.
  class tCollisionSkin
  {
  public:
    tCollisionSkin(class tBody *owner = 0);

    ~tCollisionSkin();

    /// Gets the owner of the skin
    class tBody *GetOwner() const {return mOwner;}

    /// Sets the owner of the skin
    void SetOwner(class tBody *owner);

    /// Adds a primitive to this collision skin - the primitive is
    /// copied (so you can pass in something on the stack, or delete
    /// the original) - perhaps using reference counting.  Returns the
    /// primitive index, or -1 if failure Also takes that material ID
    /// and the properties used when a collision ID is USER_DEFINED
    int AddPrimitive(const tPrimitive &prim, tMaterialTable::tMaterialID matID, tMaterialProperties matProps = tMaterialProperties(0.0f, 0.0f, 0.0f));

    /// Removes and destroys all primitives
    void RemoveAllPrimitives();

    /// Returns the number of registered primitives
    unsigned GetNumPrimitives() const {return mPrimitivesLocal.size();}

    /// Gets the primitive in local space
    const tPrimitive *GetPrimitiveLocal(unsigned iPrim) const {return mPrimitivesLocal[iPrim];}

    /// Gets the old value of primitive in world space  
    const tPrimitive *GetPrimitiveOldWorld(unsigned iPrim) const {return mPrimitivesOldWorld[iPrim];}

    /// Gets the new value of primitive in world space
    const tPrimitive *GetPrimitiveNewWorld(unsigned iPrim) const {return mPrimitivesNewWorld[iPrim];}

    /// Gets the material ID for a primitive
    tMaterialTable::tMaterialID GetMaterialID(unsigned iPrim) const {return mMaterialIDs[iPrim];}

    /// Returns the material properties for a primitive
    const tMaterialProperties &GetMaterialProperties(unsigned iPrim) const {return mMaterialProperties[iPrim];}

    /// Sets the material properties for a primitive. In this case the
    /// material ID will be automatically set to USER_DEFINED
    void SetMaterialProperties(unsigned iPrim, const tMaterialProperties & matProperties);

    /// returns the total volume
    tScalar GetVolume() const;

    /// returns the total surface area
    tScalar GetSurfaceArea() const;

    /// these get called during the collision detection
    void SetNewTransform(const tTransform3 &transform);
    void SetOldTransform(const tTransform3 &transform);
    /// set both new and old
    void SetTransform(const tTransform3 &transformOld, const tTransform3 &transformNew);

    /// Applies a transform to the local primitives (e.g. to shift
    /// everything after calculating CoM etc)
    void ApplyLocalTransform(const tTransform3 &transform);

    /// Returns pos/orient in world space
    const tVector3 &GetOldPos() const {return mTransformOld.position;}
    const tVector3 &GetNewPos() const {return mTransformNew.position;}
    const tMatrix33 &GetOldOrient() const {return mTransformOld.orientation;}
    const tMatrix33 &GetNewOrient() const {return mTransformNew.orientation;}
    const tTransform3 &GetOldTransform() const {return mTransformOld;}
    const tTransform3 &GetNewTransform() const {return mTransformNew;}

    /// Updates bounding volume of this skin 
    void UpdateWorldBoundingBox();

    /// Gets a bounding volume of this skin 
    const tAABox & GetWorldBoundingBox() const {return mWorldBoundingBox; }
   
    /// Intended for internal use by Physics - we get told about the
    /// collisions we're involved with. Used to resolve penetrations.
    std::vector<class tCollisionInfo *> & GetCollisions() {
      return mCollisions;}

    /// Each skin can contain a list of other skins it shouldn't
    /// collide with. You only need to add skins from another "family"
    /// - i.e.  don't explicitly add children/parents
    std::vector<const tCollisionSkin*> & GetNonCollidables() {return mNonCollidables;}
    const std::vector<const tCollisionSkin*> &GetNonCollidables() const {return mNonCollidables;}

    /// intended for internal use by tCollisionSystem
    void SetCollisionSystem(class tCollisionSystem * collSystem) {mCollSystem = collSystem; }

    /// Returns the collision system this skin is attached to
    class tCollisionSystem* GetCollisionSystem() const {return mCollSystem;}
    
    /// Every skin must support a ray/segment intersection test -
    /// operates on the new value of the primitives
    bool SegmentIntersect(tScalar & frac, 
                          tVector3 & pos, 
                          tVector3 & normal, 
                          const class tSegment & seg) const;

    /// Intended for internal JigLib use - but not touched by the
    /// collision system
    tCollisionSkinExternalData &GetExternalData() {return mExternalData;}
    const tCollisionSkinExternalData &GetExternalData() const {return mExternalData;}

    /// Helper to calculate the combined mass, centre of mass, and
    /// inertia tensor about the origin and the CoM (for the local
    /// primitives) primitiveProperties is an array of properties -
    /// must be the same number as there are primitives
    void GetMassProperties(const tPrimitive::tPrimitiveProperties primitiveProperties[], 
      tScalar &mass, tVector3 &centerOfMass, tMatrix33 &inertiaTensor, tMatrix33 &inertiaTensorCoM);
    /// Helper to calculate the combined mass, centre of mass, and
    /// inertia tensor about the origin and the CoM (for the local
    /// primitives) primitiveProperties indicates the properties used
    /// for all primitives - so the mass is the total mass
    void GetMassProperties(const tPrimitive::tPrimitiveProperties &primitiveProperties, 
      tScalar &mass, tVector3 &centerOfMass, tMatrix33 &inertiaTensor, tMatrix33 &inertiaTensorCoM);

  private:
    void UpdateBoundingBox();

    class tCollisionSystem *mCollSystem;

    class tBody *mOwner;

    /// Bounding box in world reference frame - includes all children
    /// too
    tAABox mWorldBoundingBox;
    
    std::vector<class tCollisionInfo *> mCollisions;
    std::vector<const tCollisionSkin*> mNonCollidables;
    
    /// old value of primitives in world space
    std::vector<tPrimitive*> mPrimitivesOldWorld;
    /// our primitives in world space
    std::vector<tPrimitive*> mPrimitivesNewWorld;
    /// Our primitives in local space
    std::vector<tPrimitive*> mPrimitivesLocal;
    /// material for each primitive
    std::vector<tMaterialTable::tMaterialID> mMaterialIDs;
    /// values used when mat ID is USER_DEFINED
    std::vector<tMaterialProperties> mMaterialProperties;

    tTransform3 mTransformOld;
    tTransform3 mTransformNew;

    tCollisionSkinExternalData mExternalData;
  };
}

#endif
