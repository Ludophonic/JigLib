//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file materials.cpp 
//                     
//==============================================================
#include "materials.hpp"

using namespace JigLib;
using namespace std;

//==============================================================
// tMaterialTable
//==============================================================
tMaterialTable::tMaterialTable()
{
  Reset();
}

//==============================================================
// Reset
//==============================================================
void tMaterialTable::Reset()
{
  Clear();
  float normalBouncy = 0.3f;
  float normalRoughS = 0.5f;
  float normalRoughD = 0.3f;
  float roughRoughS = 0.5f;
  float roughRoughD = 0.3f;
  SetMaterialProperties(UNSET,            tMaterialProperties(0.0f, 0.0f,         0.0f));
  SetMaterialProperties(NOTBOUNCY_SMOOTH, tMaterialProperties(0.0f, 0.0f,         0.0f));
  SetMaterialProperties(NOTBOUNCY_NORMAL, tMaterialProperties(0.0f, normalRoughS, normalRoughD));
  SetMaterialProperties(NOTBOUNCY_ROUGH,  tMaterialProperties(0.0f, roughRoughD,  roughRoughD));
  SetMaterialProperties(NORMAL_SMOOTH,    tMaterialProperties(normalBouncy, 0.0f,         1.0f));
  SetMaterialProperties(NORMAL_NORMAL,    tMaterialProperties(normalBouncy, normalRoughS, normalRoughD));
  SetMaterialProperties(NORMAL_ROUGH,     tMaterialProperties(normalBouncy, roughRoughS,  roughRoughD));
  SetMaterialProperties(BOUNCY_SMOOTH,    tMaterialProperties(1.0f, 0.0f,         0.0f));
  SetMaterialProperties(BOUNCY_NORMAL,    tMaterialProperties(1.0f, normalRoughS, normalRoughD));
  SetMaterialProperties(BOUNCY_ROUGH,     tMaterialProperties(1.0f, roughRoughS,  roughRoughD));
}

//==============================================================
// Clear
//==============================================================
void tMaterialTable::Clear()
{
  mMaterials.clear();
  mMaterialPairs.clear();
}

//==============================================================
// SetMaterialProperties
//==============================================================
void tMaterialTable::SetMaterialProperties(tMaterialID id, const tMaterialProperties &properties)
{
  mMaterials[id] = properties;
  for (tMaterials::const_iterator it = mMaterials.begin() ; it != mMaterials.end() ; ++it)
  {
    tMaterialID otherId = it->first;
    const tMaterialProperties &mat = it->second;
    mMaterialPairs[std::pair<tMaterialID, tMaterialID>(otherId, id)] = mMaterialPairs[std::pair<tMaterialID, tMaterialID>(id, otherId)] = tMaterialPairProperties(
      properties.mElasticity * mat.mElasticity,
      properties.mStaticRoughness * mat.mStaticRoughness,
      properties.mDynamicRoughness * mat.mDynamicRoughness);
  }
}

//==============================================================
// SetMaterialPairProperties
//==============================================================
void tMaterialTable::SetMaterialPairProperties(tMaterialID id1, tMaterialID id2, const tMaterialPairProperties &pairProperties)
{
  mMaterialPairs[std::pair<tMaterialID, tMaterialID>(id1, id2)] = mMaterialPairs[std::pair<tMaterialID, tMaterialID>(id2, id1)] = pairProperties;
}

