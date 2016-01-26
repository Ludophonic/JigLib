#ifndef MATERIALS_HPP
#define MATERIALS_HPP

#include "../maths/include/precision.hpp"

#include <map>
#include <algorithm>

namespace JigLib
{
  class tMaterialPairProperties
  {
  public:
    tMaterialPairProperties(tScalar r = 0.0f, tScalar sf = 0.0f, tScalar df = 0.0f) : mRestitution(r), mStaticFriction(sf), mDynamicFriction(df) {}
    tScalar mRestitution;
    tScalar mStaticFriction;
    tScalar mDynamicFriction;
  };
  
  class tMaterialProperties
  {
  public:
    tMaterialProperties(tScalar e = 0.0f, tScalar sr = 0.0f, tScalar dr = 0.0f) : mElasticity(e), mStaticRoughness(sr), mDynamicRoughness(dr) {}
    tScalar mElasticity;
    tScalar mStaticRoughness;
    tScalar mDynamicRoughness;
  };
  
  

  /// This handles the properties of interactions between different
  /// materials
  class tMaterialTable
  {
  public:
    typedef unsigned int tMaterialID;

    /// Some default materials that get added automatically User
    /// materials should start at NUM_JIGLIB_MATERIAL_TYPES, or else
    /// ignore this and over-ride everything. User-refined values can
    /// get used so should not assume the values come form this enum -
    /// use tMaterialID
    enum 
    {
      UNSET,
      USER_DEFINED, ///< individual values should be used/calculated at run-time
      NOTBOUNCY_SMOOTH,
      NOTBOUNCY_NORMAL,
      NOTBOUNCY_ROUGH,
      NORMAL_SMOOTH,
      NORMAL_NORMAL,
      NORMAL_ROUGH,
      BOUNCY_SMOOTH,
      BOUNCY_NORMAL,
      BOUNCY_ROUGH,
      NUM_JIGLIB_MATERIAL_TYPES
    };
    
    /// On construction all the tJigLibMaterials get added
    tMaterialTable();
    
    /// clear everything except tJigLibMaterials 
    void Reset();
    
    /// clear everything
    void Clear();
    
    /// This adds/overrides a material, and sets all the pairs for
    /// existing materials using some sensible heuristic
    void SetMaterialProperties(tMaterialID id, const tMaterialProperties &properties);
    
    /// Returns properties of a material - defaults on inelastic
    /// frictionless.
    const tMaterialProperties &GetMaterialProperties(tMaterialID id) const {return mMaterials[id];}
    
    /// Gets the properties for a pair of materials. Same result even
    /// if the two ids are swapped
    const tMaterialPairProperties &GetPairProperties(tMaterialID id1, tMaterialID id2) const {return mMaterialPairs[std::pair<tMaterialID, tMaterialID>(id1, id2)];}
    
    /// This overrides the result for a single pair of materials. It's
    /// recommended that you add all materials first. Order of ids
    /// doesn't matter
    void SetMaterialPairProperties(tMaterialID id1, tMaterialID id2, const tMaterialPairProperties &pairProperties);
    
    private:
      const tMaterialPairProperties &GetCalculatedPairProperties(tMaterialID id1, tMaterialID id2) const {return mMaterialPairs[std::pair<tMaterialID, tMaterialID>(id1, id2)];}

      typedef std::map<tMaterialID, tMaterialProperties> tMaterials;
      typedef std::map< std::pair<tMaterialID, tMaterialID>, tMaterialPairProperties> tMaterialPairs;
      
      mutable tMaterials mMaterials; ///< for default initialisation in lookup
      mutable tMaterialPairs mMaterialPairs; ///< for default initialisation in lookup
  };
  
}
#endif
