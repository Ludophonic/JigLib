#ifndef WATER_HPP
#define WATER_HPP

#include "jiglib.hpp"

//#define CALC_NORMAL_FIELD

// representation of each particle.
// position/vel are in world space.
// The order of fields has a significant impact on speed because of
// cache misses
class tParticle
{
public:
  // The next particle when were stored in a linked list (the spatial grid)
  tParticle * mNext;
  // position
  JigLib::tVector2 mR;
  // previous position
  JigLib::tVector2 mOldR;
  // density calculated at this particle
  JigLib::tScalar mDensity;
  // pressure - diagnosed from density
  JigLib::tScalar mP;
  // velocity - diagnosed from position since we use verlet/particle
  // integration
  JigLib::tVector2 mV;
  // pressure force
  JigLib::tVector2 mPressureForce;
  // gViscosity force
  JigLib::tVector2 mViscosityForce;
  // body force - gravity + some other forces later?
  JigLib::tVector2 mBodyForce;
#ifdef CALC_NORMAL_FIELD
  // the "color field" for the normal
  JigLib::tScalar mCs;
  // the normal field (grad(mCs) so not normalised)
  JigLib::tVector2 mN;
#endif
};

#endif
