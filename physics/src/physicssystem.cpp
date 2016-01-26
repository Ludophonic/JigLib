//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file physicssystem.cpp 
//                     
//==============================================================
#include "physicssystem.hpp"
#include "physicscollision.hpp"
#include "body.hpp"
#include "constraint.hpp"
#include "physicscontroller.hpp"

#include "collisionskin.hpp"
#include "collisionsystem.hpp"
#include "distance.hpp"

#include "trace.hpp"

#include <algorithm>

using namespace JigLib;
using namespace std;

tPhysicsSystem * tPhysicsSystem::mCurrentPhysicsSystem = 0;

// limit the extra velocity during collision/penetration calculations
static const tScalar maxVelMag = 0.5f;
static const tScalar maxShockVelMag = 0.05f;
static const tScalar minVelForProcessing = 0.001f;
static const tScalar penetrationShockRelaxationTimesteps = 10;

#define DO_FRICTION
#define USE_OLD_IMPULSE

//==============================================================
// tPhysicsSystem
//==============================================================
tPhysicsSystem::tPhysicsSystem()
{
  TRACE_METHOD_ONLY(ONCE_1);
  SetCurrentPhysicsSystem(this);

  mNumCollisionIterations = 4;
  mNumContactIterations = 12;
  mNumPenetrationRelaxationTimesteps = 10;
  mAllowedPenetration = 0.01f;
  mDoShockStep = false;
  mCollToll = 0.05f;
  mSolverType = SOLVER_COMBINED;
  mFreezingEnabled = true;
  SetGravity(-10.0f * tVector3::Up());
  mCollisionSystem = 0;
  mTargetTime = 0.0f;
  mOldTime = 0.0f;
  mDoingIntegration = false;

  SetCollisionFns();
}

//==============================================================
// ~tPhysicsSystem
//==============================================================
tPhysicsSystem::~tPhysicsSystem()
{
  TRACE_METHOD_ONLY(ONCE_1);
  // need to guard against our lists being modified as we go through them.
  unsigned i;
  {
    vector<tBody *> bodies = mBodies;
    for (i = 0 ; i < bodies.size() ; ++i)
      bodies[i]->DisableBody();
  }
  {
    vector<tConstraint *> constraints = mConstraints;
    for (i = 0 ; i < constraints.size() ; ++i)
      constraints[i]->DisableConstraint();
  }
  {
    vector<tPhysicsController *> controllers = mControllers;
    for (i = 0 ; i < controllers.size() ; ++i)
      controllers[i]->DisableController();
  }

  SetCurrentPhysicsSystem(0);
}

//==============================================================
// SetGravity
//==============================================================
void tPhysicsSystem::SetGravity(const tVector3 & gravity) 
{
  mGravity = gravity; 
  mGravityMagnitude = gravity.GetLength();
  if (gravity.x == gravity.y && gravity.y == gravity.z)
  {
    mGravityAxis = -1;
  }
  mGravityAxis = 0;
  if (Abs(gravity.y) > Abs(gravity.x))
    mGravityAxis = 1;
  if (Abs(gravity.z) > Abs(gravity[mGravityAxis]))
    mGravityAxis = 2;
}

//==============================================================
// add_body
//==============================================================
void tPhysicsSystem::AddBody(tBody * body)
{
  TRACE_METHOD_ONLY(FRAME_1);
  Assert(false == mDoingIntegration);
  Assert(body);
  if (mBodies.end() == find(mBodies.begin(), mBodies.end(), body))
    mBodies.push_back(body);
  else
    TRACE("Warning: tried to add body %p to physics"
          " but it's already registered", body);

  // Also add it to the collision system
  if (mCollisionSystem && body->GetCollisionSkin())
    mCollisionSystem->AddCollisionSkin(body->GetCollisionSkin());
}

//==============================================================
// RemoveBody
//==============================================================
bool tPhysicsSystem::RemoveBody(tBody * body)
{
  TRACE_METHOD_ONLY(FRAME_1);
  Assert(false == mDoingIntegration);
  Assert(body);

  // Also remove it from the collision system
  if (mCollisionSystem && body->GetCollisionSkin())
    mCollisionSystem->RemoveCollisionSkin(body->GetCollisionSkin());

  tBodies::iterator it = 
    find(mBodies.begin(), mBodies.end(), body);
  if (mBodies.end() == it)
    return false;
  mBodies.erase(it);
  return true;
}

//==============================================================
// add_constraint
//==============================================================
void tPhysicsSystem::AddConstraint(tConstraint * constraint)
{
  TRACE_METHOD_ONLY(FRAME_1);
  Assert(false == mDoingIntegration);
  if (mConstraints.end() == 
      find(mConstraints.begin(), mConstraints.end(), constraint))
    mConstraints.push_back(constraint);
  else
    TRACE("Warning: tried to add constraint %p to physics"
          " but it's already registered", constraint);
}

//==============================================================
// RemoveBody
//==============================================================
bool tPhysicsSystem::RemoveConstraint(tConstraint * constraint)
{
  TRACE_METHOD_ONLY(FRAME_1);
  Assert(false == mDoingIntegration);
  tConstraints::iterator it = 
    find(mConstraints.begin(), mConstraints.end(), constraint);
  if (mConstraints.end() == it)
    return false;
  mConstraints.erase(it);
  return true;
}

//==============================================================
// add_controller
//==============================================================
void tPhysicsSystem::AddController(tPhysicsController * controller)
{
  TRACE_METHOD_ONLY(FRAME_1);
  Assert(false == mDoingIntegration);
  if (mControllers.end() == 
      find(mControllers.begin(), mControllers.end(), controller))
    mControllers.push_back(controller);
  else
    TRACE("Warning: tried to add controller %p to physics "
          "but it's already registered", controller);
}

//==============================================================
// RemoveBody
//==============================================================
bool tPhysicsSystem::RemoveController(tPhysicsController * controller)
{
  TRACE_METHOD_ONLY(FRAME_1);
  Assert(false == mDoingIntegration);
  tControllers::iterator it = 
    find(mControllers.begin(), mControllers.end(), controller);
  if (mControllers.end() == it)
    return false;
  mControllers.erase(it);
  return true;
}

//==============================================================
// activate_object
// activate object and bring it into the collision list/do collision detection
//==============================================================
void tPhysicsSystem::ActivateObject(tBody * body)
{
  TRACE_METHOD_ONLY(FRAME_2);
  if ( body->IsActive() ||
       body->GetImmovable() )
  {
    return;
  }

  body->SetActive();

  mActiveBodies.push_back(body);

  if (!mCollisionSystem)
    return;

  if (!body->GetCollisionSkin())
    return;

  int orig_num = mCollisions.size();

  tBasicCollisionFunctor functor(mCollisions);
  tFrozenCollisionPredicate predicate(body);
  mCollisionSystem->DetectCollisions(
    *body, 
    functor, 
    &predicate,
    mCollToll + 0.01f); // make sure we get things above us

  // now check that any adjacent touching bodies wouldn't accelerate
  // towards us if we moved away
  int new_num = mCollisions.size();
  int i;
  for (i = orig_num ; i < new_num ; ++i)
  {
    // must be a body-body interaction to be interesting
    if (mCollisions[i]->mSkinInfo.skin1)
    {
      tBody * other_body = mCollisions[i]->mSkinInfo.skin0->GetOwner();
      // the collision normal pointing from body to other_body
      tVector3 thisBody_normal = mCollisions[i]->mDirToBody0;
      if (other_body == body)
      {
        other_body = mCollisions[i]->mSkinInfo.skin1->GetOwner();
        thisBody_normal.Negate();
      }
      if (other_body && !other_body->IsActive())
      {
        tVector3 force_on_other = /*other_body->GetMass() * GetGravity() + */
          other_body->GetForce();
        if (Dot(force_on_other, thisBody_normal) < -SCALAR_TINY)
        {
          // wake it up recursively. after this, the contents of our
          // mCollisions may have been relocated
          ActivateObject(other_body);
        }
      }
    }
  }
}

//==============================================================
// PreProcessCollision
//==============================================================
void tPhysicsSystem::PreProcessCollision(tCollisionInfo * collision,
                                         tScalar dt)
{
  TRACE_METHOD_ONLY(FRAME_2);
  tBody * body0 = collision->mSkinInfo.skin0->GetOwner();
  tBody * body1 = collision->mSkinInfo.skin1->GetOwner();

  // make as not satisfied
  collision->mSatisfied = false;

  // always calc the following
  const tVector3 & N = collision->mDirToBody0;
  const tScalar timescale = mNumPenetrationRelaxationTimesteps * dt;

  for (unsigned iPos = 0 ; iPos < collision->mPointInfo.Size() ; ++iPos)
  {
    tCollPointInfo& ptInfo = collision->mPointInfo[iPos];

    // some things we only calculate if there are bodies, and they are
    // movable
    if (body0->GetImmovable())
      ptInfo.mDenominator = 0.0f;
    else
      ptInfo.mDenominator = 
        body0->GetInvMass() + 
        Dot(N, Cross(body0->GetWorldInvInertia() * 
                     (Cross(ptInfo.mR0, N)), 
                     ptInfo.mR0));

    if (body1 && !body1->GetImmovable())
      ptInfo.mDenominator += body1->GetInvMass() + 
        Dot(N, Cross(body1->GetWorldInvInertia() * 
                     (Cross(ptInfo.mR1, N)), 
                     ptInfo.mR1));

    if (ptInfo.mDenominator < SCALAR_TINY)
      ptInfo.mDenominator = SCALAR_TINY;

    // calculate the world position
    ptInfo.mPosition = body0->GetOldPosition() + ptInfo.mR0;

    // per-point penetration resolution
    if (ptInfo.mInitialPenetration > mAllowedPenetration)
    {
        ptInfo.mMinSeparationVel = (ptInfo.mInitialPenetration - mAllowedPenetration) / timescale;
    }
    else
    {
      tScalar approachScale = -0.1f * (ptInfo.mInitialPenetration - mAllowedPenetration) / (SCALAR_TINY + mAllowedPenetration);
      Limit(approachScale, SCALAR_TINY, 1.0f);
      ptInfo.mMinSeparationVel = approachScale * (ptInfo.mInitialPenetration - mAllowedPenetration) / Max(dt, SCALAR_TINY);
    }
    if (ptInfo.mMinSeparationVel > maxVelMag)
      ptInfo.mMinSeparationVel = maxVelMag;
  }
}

inline bool LessCollPtDenom(const tCollPointInfo& info1, const tCollPointInfo& info2)
{
  return info1.mDenominator < info2.mDenominator;
}

//==============================================================
// PreProcessCollisionAccumulated
//==============================================================
void tPhysicsSystem::PreProcessCollisionAccumulated(tCollisionInfo * collision,
                                                    tScalar dt)
{
  TRACE_METHOD_ONLY(FRAME_2);
  tBody * body0 = collision->mSkinInfo.skin0->GetOwner();
  tBody * body1 = collision->mSkinInfo.skin1->GetOwner();

  // make as not satisfied
  collision->mSatisfied = false;

  // always calc the following
  const tVector3 & N = collision->mDirToBody0;
  const tScalar timescale = mNumPenetrationRelaxationTimesteps * dt;

#ifdef USE_OLD_IMPULSE
  tBodyPair bp(collision->mSkinInfo.skin0->GetOwner(), collision->mSkinInfo.skin1->GetOwner(), tVector3::Zero(), tVector3::Zero());
  std::multimap<tBodyPair, tCachedImpulses>::iterator cacheItBegin = mCachedContacts.lower_bound(bp);
  std::multimap<tBodyPair, tCachedImpulses>::iterator cacheItEnd   = mCachedContacts.upper_bound(bp);
#endif

  for (unsigned iPos = 0 ; iPos < collision->mPointInfo.Size() ; ++iPos)
  {
    tCollPointInfo& ptInfo = collision->mPointInfo[iPos];

    // some things we only calculate if there are bodies, and they are
    // movable
    if (body0->GetImmovable())
      ptInfo.mDenominator = 0.0f;
    else
      ptInfo.mDenominator = 
        body0->GetInvMass() + 
        Dot(N, Cross(body0->GetWorldInvInertia() * 
                     (Cross(ptInfo.mR0, N)), 
                     ptInfo.mR0));

    if (body1 && !body1->GetImmovable())
      ptInfo.mDenominator += body1->GetInvMass() + 
        Dot(N, Cross(body1->GetWorldInvInertia() * 
                     (Cross(ptInfo.mR1, N)), 
                     ptInfo.mR1));

    if (ptInfo.mDenominator < SCALAR_TINY)
      ptInfo.mDenominator = SCALAR_TINY;

    // calculate the world position
    ptInfo.mPosition = body0->GetOldPosition() + ptInfo.mR0;

    // per-point penetration resolution
    if (ptInfo.mInitialPenetration > mAllowedPenetration)
    {
        ptInfo.mMinSeparationVel = (ptInfo.mInitialPenetration - mAllowedPenetration) / timescale;
    }
    else
    {
      tScalar approachScale = -0.1f * (ptInfo.mInitialPenetration - mAllowedPenetration) / (SCALAR_TINY + mAllowedPenetration);
      Limit(approachScale, SCALAR_TINY, 1.0f);
      ptInfo.mMinSeparationVel = approachScale * (ptInfo.mInitialPenetration - mAllowedPenetration) / Max(dt, SCALAR_TINY);
    }

    ptInfo.mAccumulatedNormalImpulse = 0.0f;
    ptInfo.mAccumulatedNormalImpulseAux = 0.0f;
    ptInfo.mAccumulatedFrictionImpulse.SetToZero();
#ifdef USE_OLD_IMPULSE
    /// todo take this value from config or derive from the geometry (but don't reference the body in the cache as it
    /// may be deleted)
    static tScalar minDist = 0.2f;
    tScalar bestDistSq = Sq(minDist);
    for (std::multimap<tBodyPair, tCachedImpulses>::iterator it = cacheItBegin ; it != cacheItEnd ; ++it)
    {
      const tBodyPair &bp = it->first;
      tScalar distSq = (bp.mBodyA == collision->mSkinInfo.skin0->GetOwner()) ? 
        PointPointDistanceSq(bp.mRA, ptInfo.mR0) : PointPointDistanceSq(bp.mRA, ptInfo.mR1);
      if (distSq < bestDistSq)
      {
        bestDistSq = distSq;
        ptInfo.mAccumulatedNormalImpulse = it->second.mNormalImpulse;
        ptInfo.mAccumulatedNormalImpulseAux = it->second.mNormalImpulseAux;
        ptInfo.mAccumulatedFrictionImpulse = (bp.mBodyA == collision->mSkinInfo.skin0->GetOwner()) ? 
          it->second.mFrictionImpulse : -it->second.mFrictionImpulse;
      }
    }
    static tScalar oldScale = 1.f;
    ptInfo.mAccumulatedNormalImpulse *= oldScale;
    ptInfo.mAccumulatedFrictionImpulse *= oldScale;
    ptInfo.mAccumulatedNormalImpulseAux *= oldScale;
    if (ptInfo.mAccumulatedNormalImpulse != 0.0f)
    {
      tVector3 impulse(ptInfo.mAccumulatedNormalImpulse, N);
      impulse += ptInfo.mAccumulatedFrictionImpulse;
      body0->ApplyBodyWorldImpulse(impulse, ptInfo.mR0);
      if (body1)
        body1->ApplyNegativeBodyWorldImpulse(impulse, ptInfo.mR1);
    }
    if (ptInfo.mAccumulatedNormalImpulseAux != 0.0f)
    {
      tVector3 impulse(ptInfo.mAccumulatedNormalImpulseAux, N);
      body0->ApplyBodyWorldImpulseAux(impulse, ptInfo.mR0);
      if (body1)
        body1->ApplyNegativeBodyWorldImpulseAux(impulse, ptInfo.mR1);
    }
#endif
  }
/*
  std::sort(&collision->mPointInfo[0], 
            1 + &collision->mPointInfo[collision->mPointInfo.Size() - 1], 
            LessCollPtDenom);
            */
}

//==============================================================
// MoreCollPtPenetration
//==============================================================
inline bool MoreCollPtPenetration(const tCollPointInfo& info1, const tCollPointInfo& info2)
{
  return info1.mInitialPenetration > info2.mInitialPenetration;
}

//==============================================================
// PreProcessCollisionFast
// strips things down to just one collision point!
//==============================================================
void tPhysicsSystem::PreProcessCollisionFast(tCollisionInfo * collision,
                                             tScalar dt)
{
  TRACE_METHOD_ONLY(FRAME_2);
  tBody * body0 = collision->mSkinInfo.skin0->GetOwner();
  tBody * body1 = collision->mSkinInfo.skin1->GetOwner();

  // make as not satisfied
  collision->mSatisfied = false;

  // always calc the following
  const tVector3 & N = collision->mDirToBody0;
  const tScalar timescale = mNumPenetrationRelaxationTimesteps * dt;

  // only keep the best few collision points
  static unsigned keep = 3;
  if (collision->mPointInfo.Size()  > keep)
  {
    std::sort(&collision->mPointInfo[0], 
              1 + &collision->mPointInfo[collision->mPointInfo.Size() - 1], 
              MoreCollPtPenetration);
    // forget the rest
    collision->mPointInfo.Resize(keep);
  }
  
  for (unsigned iPos = 0 ; iPos < collision->mPointInfo.Size() ; ++iPos)
  {
    tCollPointInfo& ptInfo = collision->mPointInfo[iPos];
    // some things we only calculate if there are bodies, and they are
    // movable
    if (body0->GetImmovable())
      ptInfo.mDenominator = 0.0f;
    else
      ptInfo.mDenominator = 
        body0->GetInvMass() + 
        Dot(N, Cross(body0->GetWorldInvInertia() * 
                     (Cross(ptInfo.mR0, N)), 
                     ptInfo.mR0));

    if (body1 && !body1->GetImmovable())
      ptInfo.mDenominator += body1->GetInvMass() + 
        Dot(N, Cross(body1->GetWorldInvInertia() * 
                     (Cross(ptInfo.mR1, N)), 
                     ptInfo.mR1));

    if (ptInfo.mDenominator < SCALAR_TINY)
      ptInfo.mDenominator = SCALAR_TINY;

    // calculate the world position
    ptInfo.mPosition = body0->GetOldPosition() + ptInfo.mR0;

    // per-point penetration resolution
    if (ptInfo.mInitialPenetration > mAllowedPenetration)
    {
        ptInfo.mMinSeparationVel = (ptInfo.mInitialPenetration - mAllowedPenetration) / timescale;
    }
    else
    {
      tScalar approachScale = -0.1f * (ptInfo.mInitialPenetration - mAllowedPenetration) / (SCALAR_TINY + mAllowedPenetration);
      Limit(approachScale, SCALAR_TINY, 1.0f);
      ptInfo.mMinSeparationVel = approachScale * (ptInfo.mInitialPenetration - mAllowedPenetration) / Max(dt, SCALAR_TINY);
    }
    if (ptInfo.mMinSeparationVel > maxVelMag)
      ptInfo.mMinSeparationVel = maxVelMag;
  }
}

//==============================================================
// ProcessCollision
// do all colls individually
//==============================================================
bool tPhysicsSystem::ProcessCollision(tCollisionInfo * collision, 
                                      tScalar dt,
                                      bool firstContact)
{
  TRACE_METHOD_ONLY(MULTI_FRAME_1);
  collision->mSatisfied = true;

  tBody * body0 = collision->mSkinInfo.skin0->GetOwner();
  tBody * body1 = collision->mSkinInfo.skin1->GetOwner();

  const tVector3 & N = collision->mDirToBody0;

  bool gotOne = false;
  for (unsigned iPos = 0 ; iPos < collision->mPointInfo.Size(); ++iPos)
  {
    tCollPointInfo & ptInfo = collision->mPointInfo[iPos];
    Assert(ptInfo.mDenominator >= SCALAR_TINY);

    tScalar normalVel;
    if (body1)
      normalVel =  Dot(body0->GetVelocity(ptInfo.mR0) - body1->GetVelocity(ptInfo.mR1), N); 
    else
      normalVel =  Dot(body0->GetVelocity(ptInfo.mR0), N); 

    if (normalVel > ptInfo.mMinSeparationVel)
      continue;

    tScalar finalNormalVel = -collision->mMatPairProperties.mRestitution * normalVel;

    if (finalNormalVel < minVelForProcessing)
    {
      // could be zero elasticity in collision, or could be zero
      // elasticity in contact - don't care.  relax towards 0
      // penetration
      finalNormalVel = ptInfo.mMinSeparationVel;
    }

    tScalar deltaVel = finalNormalVel - normalVel;

    if (deltaVel <= minVelForProcessing)
      continue;

    tScalar normalImpulse = deltaVel / ptInfo.mDenominator;

    // prepare our return value
    gotOne = true;
    tVector3 impulse(normalImpulse, N);

    body0->ApplyBodyWorldImpulse(impulse, ptInfo.mR0);
    if (body1)
      body1->ApplyNegativeBodyWorldImpulse(impulse, ptInfo.mR1);
#ifdef DO_FRICTION
    // For friction, work out the impulse in the opposite direction to
    // the tangential velocity that would be required to bring this
    // point to a halt. Apply the minimum of this impulse magnitude,
    // and the one obtained from the normal impulse. This prevents
    // reversing the velocity direction.
    //
    // recalculate the velocity since it's changed.
    tVector3 Vr_new(body0->GetVelocity(ptInfo.mR0));
    if (body1)
      Vr_new -= body1->GetVelocity(ptInfo.mR1);

    tVector3 tangent_vel(Vr_new - Dot(Vr_new, N)  * N);
    tScalar tangent_speed = tangent_vel.GetLength();
    if (tangent_speed > minVelForProcessing)
    {
      tVector3 T = -tangent_vel / tangent_speed;

      // calculate an "inelastic collision" to zero the relative vel
      tScalar denominator = 0.0f;
      if (!body0->GetImmovable())
      {
        denominator = body0->GetInvMass() + 
          Dot(T, Cross(body0->GetWorldInvInertia() * 
                       (Cross(ptInfo.mR0, T)),
                       ptInfo.mR0));
      }
      if (body1 && !body1->GetImmovable())
      {
        denominator += body1->GetInvMass() + 
          Dot(T, Cross(body1->GetWorldInvInertia() * 
                       (Cross(ptInfo.mR1, T)), ptInfo.mR1));
      }

      if (denominator > SCALAR_TINY)
      {
        tScalar impulseToReverse = tangent_speed / denominator;

        tScalar impulseFromNormalImpulse = 
          collision->mMatPairProperties.mStaticFriction * normalImpulse;
        tScalar frictionImpulse;

        if (impulseToReverse < impulseFromNormalImpulse)
          frictionImpulse = impulseToReverse;
        else
          frictionImpulse = collision->mMatPairProperties.mDynamicFriction * normalImpulse;
        
        T *= frictionImpulse;
        body0->ApplyBodyWorldImpulse(T, ptInfo.mR0);
        if (body1)
          body1->ApplyNegativeBodyWorldImpulse(T, ptInfo.mR1);
      }
    } // end of friction
#endif
  }

  if (gotOne)
  {
    body0->SetConstraintsAndCollisionsUnsatisfied();
    if (body1)
      body1->SetConstraintsAndCollisionsUnsatisfied();
    //collision->mSatisfied = true;
  }
  return gotOne;
}

//==============================================================
// ProcessCollisionAccumulated
//==============================================================
bool tPhysicsSystem::ProcessCollisionAccumulated(tCollisionInfo * collision, 
                                                 tScalar dt,
                                                 bool firstContact)
{
  TRACE_METHOD_ONLY(MULTI_FRAME_1);
  collision->mSatisfied = true;

  tBody * body0 = collision->mSkinInfo.skin0->GetOwner();
  tBody * body1 = collision->mSkinInfo.skin1->GetOwner();

  bool gotOne = false;
  
  for (unsigned iPos = collision->mPointInfo.Size() ; iPos-- != 0 ; )
  {

    tCollPointInfo & ptInfo = collision->mPointInfo[iPos];
    Assert(ptInfo.mDenominator >= SCALAR_TINY);

    tScalar normalImpulse;
    // first the real impulse
    {
      tScalar normalVel;
      if (body1)
        normalVel =  Dot(body0->GetVelocity(ptInfo.mR0) - body1->GetVelocity(ptInfo.mR1), collision->mDirToBody0); 
      else
        normalVel =  Dot(body0->GetVelocity(ptInfo.mR0), collision->mDirToBody0); 

      // result in zero...
      tScalar deltaVel = -normalVel;
      // ...except that the impulse reduction to achieve the desired separation must be done
      // here - not with aux - because aux would suck objects together
      if (ptInfo.mMinSeparationVel < 0.0f) 
        deltaVel += ptInfo.mMinSeparationVel;

      if (Abs(deltaVel) > minVelForProcessing)
      {
        normalImpulse = deltaVel / ptInfo.mDenominator;

        tScalar origAccumulatedNormalImpulse = ptInfo.mAccumulatedNormalImpulse;
        ptInfo.mAccumulatedNormalImpulse = Max(ptInfo.mAccumulatedNormalImpulse + normalImpulse, 0.0f);
        tScalar actualImpulse = ptInfo.mAccumulatedNormalImpulse - origAccumulatedNormalImpulse;

        tVector3 impulse(actualImpulse, collision->mDirToBody0);
        body0->ApplyBodyWorldImpulse(impulse, ptInfo.mR0);
        if (body1)
          body1->ApplyNegativeBodyWorldImpulse(impulse, ptInfo.mR1);
        // prepare our return value
        gotOne = true;
#ifdef _DEBUG
        {
          tScalar checkNormalVel;
          if (body1)
            checkNormalVel =  Dot(body0->GetVelocity(ptInfo.mR0) - body1->GetVelocity(ptInfo.mR1), collision->mDirToBody0); 
          else
            checkNormalVel =  Dot(body0->GetVelocity(ptInfo.mR0), collision->mDirToBody0); 
          int junk = 0;
        }
#endif
      }
    }

    // now the correction impulse
    static bool doCorrection = true;
    if (doCorrection)
    {
      tScalar normalVel;
      if (body1)
        normalVel =  Dot(body0->GetVelocityAux(ptInfo.mR0) - body1->GetVelocityAux(ptInfo.mR1), collision->mDirToBody0); 
      else
        normalVel =  Dot(body0->GetVelocityAux(ptInfo.mR0), collision->mDirToBody0); 

      tScalar deltaVel = -normalVel;
      // only try to separate objects
      if (ptInfo.mMinSeparationVel > 0.0f) 
        deltaVel += ptInfo.mMinSeparationVel;

      if (Abs(deltaVel) > minVelForProcessing)
      {
        normalImpulse = deltaVel / ptInfo.mDenominator;

        tScalar origAccumulatedNormalImpulse = ptInfo.mAccumulatedNormalImpulseAux;
        ptInfo.mAccumulatedNormalImpulseAux = Max(ptInfo.mAccumulatedNormalImpulseAux + normalImpulse, 0.0f);
        tScalar actualImpulse = ptInfo.mAccumulatedNormalImpulseAux - origAccumulatedNormalImpulse;

        tVector3 impulse(actualImpulse, collision->mDirToBody0);
        body0->ApplyBodyWorldImpulseAux(impulse, ptInfo.mR0);
        if (body1)
          body1->ApplyNegativeBodyWorldImpulseAux(impulse, ptInfo.mR1);
        // prepare our return value
        gotOne = true;
      }
    }

#ifdef DO_FRICTION
    // For friction, work out the impulse in the opposite direction to
    // the tangential velocity that would be required to bring this
    // point to a halt. Apply the minimum of this impulse magnitude,
    // and the one obtained from the normal impulse. This prevents
    // reversing the velocity direction.
    //
    // recalculate the velocity since it's changed.
    if (ptInfo.mAccumulatedNormalImpulse > 0.0f)
    {
      tVector3 Vr_new(body0->GetVelocity(ptInfo.mR0));
      if (body1)
        Vr_new -= body1->GetVelocity(ptInfo.mR1);

      tVector3 tangent_vel(Vr_new - Dot(Vr_new, collision->mDirToBody0)  * collision->mDirToBody0);
      tScalar tangent_speed = tangent_vel.GetLength();
      if (tangent_speed > minVelForProcessing)
//      if (tangent_speed > SCALAR_TINY)
      {
        tVector3 T = -tangent_vel / tangent_speed;

        // calculate an "inelastic collision" to zero the relative vel
        tScalar denominator = 0.0f;
        if (!body0->GetImmovable())
        {
          denominator = body0->GetInvMass() + 
            Dot(T, Cross(body0->GetWorldInvInertia() * 
                         (Cross(ptInfo.mR0, T)),
                         ptInfo.mR0));
        }
        if (body1 && !body1->GetImmovable())
        {
          denominator += body1->GetInvMass() + 
            Dot(T, Cross(body1->GetWorldInvInertia() * 
                         (Cross(ptInfo.mR1, T)), ptInfo.mR1));
        }

        if (denominator > SCALAR_TINY)
        {
          tScalar impulseToReverse = tangent_speed / denominator;
          tVector3 frictionImpulseVec = T * impulseToReverse;

          tVector3 origAccumulatedFrictionImpulse = ptInfo.mAccumulatedFrictionImpulse;
          ptInfo.mAccumulatedFrictionImpulse += frictionImpulseVec;

          tScalar AFIMag = ptInfo.mAccumulatedFrictionImpulse.GetLength();
          tScalar maxAllowedAFIMag = collision->mMatPairProperties.mStaticFriction * ptInfo.mAccumulatedNormalImpulse;

          if (AFIMag > SCALAR_TINY && AFIMag > maxAllowedAFIMag)
            ptInfo.mAccumulatedFrictionImpulse *= maxAllowedAFIMag / AFIMag;

          tVector3 actualFrictionImpulse = ptInfo.mAccumulatedFrictionImpulse - origAccumulatedFrictionImpulse; 

          body0->ApplyBodyWorldImpulse(actualFrictionImpulse, ptInfo.mR0);
          if (body1)
            body1->ApplyNegativeBodyWorldImpulse(actualFrictionImpulse, ptInfo.mR1);
        }
      } // end of friction
    }
#endif
  }

  if (gotOne)
  {
    body0->SetConstraintsAndCollisionsUnsatisfied();
    if (body1)
      body1->SetConstraintsAndCollisionsUnsatisfied();
    //collision->mSatisfied = true;
  }
  return gotOne;
}

//==============================================================
// ProcessCollisionFast
//==============================================================
bool tPhysicsSystem::ProcessCollisionFast(tCollisionInfo * collision, 
                                          tScalar dt,
                                          bool firstContact)
{
  TRACE_METHOD_ONLY(MULTI_FRAME_1);
  collision->mSatisfied = true;

  tBody * body0 = collision->mSkinInfo.skin0->GetOwner();
  tBody * body1 = collision->mSkinInfo.skin1->GetOwner();

  bool gotOne = false;
  for (unsigned iPos = collision->mPointInfo.Size() ; 
       iPos-- != 0 ; )
  {
    tCollPointInfo & ptInfo = collision->mPointInfo[iPos];
    Assert(ptInfo.mDenominator >= SCALAR_TINY);

    tScalar normalVel;
    if (body1)
      normalVel =  Dot(body0->GetVelocity(ptInfo.mR0) - body1->GetVelocity(ptInfo.mR1), collision->mDirToBody0); 
    else
      normalVel =  Dot(body0->GetVelocity(ptInfo.mR0), collision->mDirToBody0); 

    if (normalVel > ptInfo.mMinSeparationVel)
      continue;

    tScalar finalNormalVel = -collision->mMatPairProperties.mRestitution * normalVel;

    if (finalNormalVel < minVelForProcessing)
    {
      // could be zero elasticity in collision, or could be zero
      // elasticity in contact - don't care.  relax towards 0
      // penetration
      finalNormalVel = ptInfo.mMinSeparationVel;
    }

    tScalar deltaVel = finalNormalVel - normalVel;

    if (deltaVel < minVelForProcessing)
      continue;

    tScalar normalImpulse = deltaVel / ptInfo.mDenominator;

    // prepare our return value
    gotOne = true;
    tVector3 impulse(normalImpulse, collision->mDirToBody0);

    body0->ApplyBodyWorldImpulse(impulse, ptInfo.mR0);
    if (body1)
      body1->ApplyNegativeBodyWorldImpulse(impulse, ptInfo.mR1);
#ifdef DO_FRICTION
    // recalculate the velocity since it's changed.
    tVector3 Vr_new(body0->GetVelocity(ptInfo.mR0));
    if (body1)
      Vr_new -= body1->GetVelocity(ptInfo.mR1);

    tVector3 tangent_vel(Vr_new - Dot(Vr_new, collision->mDirToBody0)  * collision->mDirToBody0);
    tScalar tangent_speed = tangent_vel.GetLength();
    if (tangent_speed > minVelForProcessing)
    {
      tVector3 T = -tangent_vel / tangent_speed;

      // calculate an "inelastic collision" to zero the relative vel
      tScalar denominator = 0.0f;
      if (!body0->GetImmovable())
      {
        denominator = body0->GetInvMass() + 
          Dot(T, Cross(body0->GetWorldInvInertia() * 
                       (Cross(ptInfo.mR0, T)),
                       ptInfo.mR0));
      }
      if (body1 && !body1->GetImmovable())
      {
        denominator += body1->GetInvMass() + 
          Dot(T, Cross(body1->GetWorldInvInertia() * 
                       (Cross(ptInfo.mR1, T)), ptInfo.mR1));
      }

      if (denominator > SCALAR_TINY)
      {
        tScalar impulseToReverse = tangent_speed / denominator;

        T *= impulseToReverse;
        body0->ApplyBodyWorldImpulse(T, ptInfo.mR0);
        if (body1)
          body1->ApplyNegativeBodyWorldImpulse(T, ptInfo.mR1);
      }
    } // end of friction
#endif
  }

  if (gotOne)
  {
    body0->SetConstraintsAndCollisionsUnsatisfied();
    if (body1)
      body1->SetConstraintsAndCollisionsUnsatisfied();
  }
  return gotOne;
}


//==============================================================
// ProcessCollisionCombined process all points before actually updating vel
// (using the average impulse etc)
//==============================================================
bool tPhysicsSystem::ProcessCollisionCombined(tCollisionInfo * collision, 
                                              tScalar dt,
                                              bool firstContact)
{
  TRACE_METHOD_ONLY(MULTI_FRAME_1);
  collision->mSatisfied = true;

  tBody * body0 = collision->mSkinInfo.skin0->GetOwner();
  tBody * body1 = collision->mSkinInfo.skin1->GetOwner();

  // the individual impulses in the same order as
  // collision->mPointInfo - for friction
  tScalar totalImpulse = 0.0f;
  unsigned iPos;

  tVector3 avPos(0.0f);
  tScalar avMinSeparationVel = 0.0f;

  tScalar impulses[tCollisionInfo::MAX_COLLISION_POINTS];
  for (iPos = collision->mPointInfo.Size() ; iPos-- != 0  ; )
  {
    Assert(collision->mPointInfo[iPos].mDenominator >= SCALAR_TINY);
    tCollPointInfo & ptInfo = collision->mPointInfo[iPos];
    impulses[iPos] = 0.0f;

    tScalar normalVel;
    if (body1)
      normalVel =  Dot(body0->GetVelocity(ptInfo.mR0) - 
                       body1->GetVelocity(ptInfo.mR1), collision->mDirToBody0); 
    else
      normalVel =  Dot(body0->GetVelocity(ptInfo.mR0), collision->mDirToBody0); 

    if (normalVel > ptInfo.mMinSeparationVel)
      continue;

    tScalar finalNormalVel = -collision->mMatPairProperties.mRestitution * normalVel;

    if (finalNormalVel < minVelForProcessing)
    {
      // could be zero elasticity in collision, or could be zero
      // elasticity in contact - don't care.  relax towards 0
      // penetration
      finalNormalVel = ptInfo.mMinSeparationVel;
    }

    const tScalar deltaVel = finalNormalVel - normalVel;
    if (deltaVel < minVelForProcessing)
      continue;

    tScalar normalImpulse = deltaVel / ptInfo.mDenominator;

    impulses[iPos] = normalImpulse;
    totalImpulse += normalImpulse;

    AddScaleVector3(avPos, avPos, normalImpulse, ptInfo.mPosition);
    avMinSeparationVel += ptInfo.mMinSeparationVel * normalImpulse;
  }

  if (totalImpulse <= SCALAR_TINY)
    return false;
  
  const tScalar scale = 1.0f / totalImpulse;

  // apply all these impulses (as well as subsequently applying an
  // impulse at an averaged position)
  for (iPos = collision->mPointInfo.Size() ; iPos-- != 0  ; )
  {
    if (impulses[iPos] > SCALAR_TINY)
    {
      tCollPointInfo & ptInfo = collision->mPointInfo[iPos];
      const tScalar sc = impulses[iPos] * scale;
      const tVector3 impulse(impulses[iPos] * sc, collision->mDirToBody0);
      body0->ApplyBodyWorldImpulse(impulse, ptInfo.mR0);
      if (body1)
        body1->ApplyNegativeBodyWorldImpulse(impulse, ptInfo.mR1);
    }
  }

  avPos *= scale;
  avMinSeparationVel *= scale;

  // now calculate the single impulse to be applied at avPos
  tVector3 R0, R1;
  SubVector3(R0, avPos, body0->GetPosition());
  tVector3 Vr(body0->GetVelocity(R0));
  if (body1)
  {
    SubVector3(R1, avPos, body1->GetPosition());
    Vr -= body1->GetVelocity(R1);
  }
  const tScalar normalVel = Dot(Vr, collision->mDirToBody0);

  tScalar normalImpulse = 0.0f;
  if (normalVel < avMinSeparationVel)
  {
    // coefficient of restitution
    tScalar finalNormalVel = -collision->mMatPairProperties.mRestitution * normalVel;

    if (finalNormalVel < minVelForProcessing)
    {
      // must be a contact - could be zero elasticity in collision, or
      // could be zero elasticity in contact - don't care.  relax
      // towards 0 penetration
      finalNormalVel = avMinSeparationVel;
    }

    const tScalar deltaVel = finalNormalVel - normalVel;

    if (deltaVel > minVelForProcessing)
    {
      tScalar denominator = 0.0f;
      if (!body0->GetImmovable())
        denominator = body0->GetInvMass() +
          Dot(collision->mDirToBody0, Cross(body0->GetWorldInvInertia() * (Cross(R0, collision->mDirToBody0)), R0));
      if (body1 && !body1->GetImmovable())
        denominator += body1->GetInvMass() + 
          Dot(collision->mDirToBody0, Cross(body1->GetWorldInvInertia() * (Cross(R1, collision->mDirToBody0)), R1));
      if (denominator < SCALAR_TINY)
        denominator = SCALAR_TINY;

      normalImpulse = deltaVel / denominator;
      const tVector3 impulse(normalImpulse, collision->mDirToBody0);

      body0->ApplyWorldImpulse(impulse, avPos);
      if (body1)
        body1->ApplyNegativeWorldImpulse(impulse, avPos);
    }
  }
#ifdef DO_FRICTION
  // now do friction point by point
  for (iPos = collision->mPointInfo.Size() ; iPos-- != 0  ; )
  {
    // For friction, work out the impulse in the opposite direction to
    // the tangential velocity that would be required to bring this
    // point to a halt. Apply the minimum of this impulse magnitude,
    // and the one obtained from the normal impulse. This prevents
    // reversing the velocity direction.
    //
    // However, recalculate the velocity since it's changed.
    const tCollPointInfo & ptInfo = collision->mPointInfo[iPos];

    tVector3 Vr_new = body1 ? 
      (body0->GetVelocity(ptInfo.mR0) - body1->GetVelocity(ptInfo.mR1)) :
      (body0->GetVelocity(ptInfo.mR0));

    tVector3 T(Vr_new - Dot(Vr_new, collision->mDirToBody0)  * collision->mDirToBody0);
    const tScalar tangent_speed = T.GetLength();
    if (tangent_speed > minVelForProcessing)
    {
      T /= -tangent_speed;

      const tScalar sc = impulses[iPos] * scale;
      const tScalar ptNormalImpulse = sc * (normalImpulse + impulses[iPos]);

      // calculate an "inelastic collision" to zero the relative vel
      tScalar denominator = 0.0f;
      if (!body0->GetImmovable())
      {
        denominator  = body0->GetInvMass() + 
          Dot(T, Cross(body0->GetWorldInvInertia() * 
                       (Cross(ptInfo.mR0, T)), ptInfo.mR0));
      }
      if (body1 && !body1->GetImmovable())
      {
        denominator += body1->GetInvMass() + 
          Dot(T, Cross(body1->GetWorldInvInertia() * 
                       (Cross(ptInfo.mR1, T)), ptInfo.mR1));
      }

      if (denominator > SCALAR_TINY)
      {
        const tScalar impulseToReverse = tangent_speed / denominator;

        const tScalar impulseFromNormalImpulse =
          collision->mMatPairProperties.mStaticFriction * ptNormalImpulse;
        tScalar frictionImpulse;

        if (impulseToReverse < impulseFromNormalImpulse)
          frictionImpulse = impulseToReverse;
        else
          frictionImpulse = collision->mMatPairProperties.mDynamicFriction * ptNormalImpulse;

        T *= frictionImpulse;
        body0->ApplyBodyWorldImpulse(T, ptInfo.mR0);
        if (body1)
          body1->ApplyNegativeBodyWorldImpulse(T, ptInfo.mR1);
      }
    } 
  } // end of friction
#endif

  body0->SetConstraintsAndCollisionsUnsatisfied();
  if (body1)
    body1->SetConstraintsAndCollisionsUnsatisfied();
  return true;
}

//==============================================================
// ProcessCollisionForShock
// for this to even sort-of work it needs the cached aux impulse
// not to be applied in the pre-processing
//==============================================================
bool tPhysicsSystem::ProcessCollisionForShock(tCollisionInfo * collision, 
                                              tScalar dt)
{
  TRACE_METHOD_ONLY(MULTI_FRAME_1);
  collision->mSatisfied = true;

//  const tVector3 & N = collision->mDirToBody0;
  tVector3 N = collision->mDirToBody0;
  N.x = N.y = 0.0f;
  N.NormaliseSafe();

  unsigned iterations = 5;

  unsigned iPos;
  const tScalar timescale = penetrationShockRelaxationTimesteps * dt;
  for (iPos = 0 ; iPos < collision->mPointInfo.Size() ; ++iPos)
  {
    tCollPointInfo& ptInfo = collision->mPointInfo[iPos];
//    ptInfo.mAccumulatedNormalImpulseAux = 0.0f;
  }

  // since this is shock, body 0 OR body1 can be immovable. Also, if
  // immovable make the constraint against a non-moving object
  tBody * body0 = collision->mSkinInfo.skin0->GetOwner();
  tBody * body1 = collision->mSkinInfo.skin1->GetOwner();
  if (body0->GetImmovable())
    body0 = 0;
  if (body1 && body1->GetImmovable())
    body1 = 0;

  if (!body0 && !body1)
    return false;

  for (unsigned iteration = 0 ; iteration < iterations ; ++iteration)
  {
    for (iPos = 0 ; iPos < collision->mPointInfo.Size() ; ++iPos)
    {
      tCollPointInfo& ptInfo = collision->mPointInfo[iPos];

      tScalar normalVel = 0.0f;
      if (body0)
        normalVel =  Dot(body0->GetVelocity(ptInfo.mR0), N) + Dot(body0->GetVelocityAux(ptInfo.mR0), N); 
      if (body1)
        normalVel -= Dot(body1->GetVelocity(ptInfo.mR1), N) + Dot(body1->GetVelocityAux(ptInfo.mR1), N); 

      tScalar finalNormalVel = (ptInfo.mInitialPenetration - mAllowedPenetration) / timescale;

      if (finalNormalVel < 0.0f)
        continue;

      tScalar impulse = (finalNormalVel - normalVel) / ptInfo.mDenominator;

      tScalar orig = ptInfo.mAccumulatedNormalImpulseAux;
      ptInfo.mAccumulatedNormalImpulseAux = Max(ptInfo.mAccumulatedNormalImpulseAux + impulse, 0.0f);
      tVector3 actualImpulse = (ptInfo.mAccumulatedNormalImpulseAux - orig) * N;

      if (body0)
        body0->ApplyBodyWorldImpulseAux(actualImpulse, ptInfo.mR0);
      if (body1)
        body1->ApplyNegativeBodyWorldImpulseAux(actualImpulse, ptInfo.mR1);

#ifdef _DEBUG
      {
        tScalar normalVelAfter = 0.0f;
        if (body0)
          normalVelAfter =  Dot(body0->GetVelocity(ptInfo.mR0), N) + Dot(body0->GetVelocityAux(ptInfo.mR0), N); 
        if (body1)
          normalVelAfter -= Dot(body1->GetVelocity(ptInfo.mR1), N) + Dot(body1->GetVelocityAux(ptInfo.mR1), N); 
        int junk = 0;
      }

#endif
    }
  }

  if (body0)
    body0->SetConstraintsAndCollisionsUnsatisfied();
  if (body1)
    body1->SetConstraintsAndCollisionsUnsatisfied();
  return true;
}

//==============================================================
// DampActiveBodies
//==============================================================
void tPhysicsSystem::DampAllActiveBodies()
{
  TRACE_METHOD_ONLY(FRAME_2);
  unsigned numBodies = mActiveBodies.size();
  for (unsigned i = 0 ; i < numBodies ; ++i)
    mActiveBodies[i]->DampForDeactivation();
}

//==============================================================
// try_to_activate_all_frozen_objects
//==============================================================
void tPhysicsSystem::TryToActivateAllFrozenObjects()
{
  TRACE_METHOD_ONLY(FRAME_2);
  unsigned numBodies = mBodies.size();
  for (unsigned i = 0 ; i < numBodies ; ++i)
  {
    if (!mBodies[i]->IsActive())
    {
      if (mBodies[i]->GetShouldBeActive())
      {
        ActivateObject(mBodies[i]);
      }
      else
      {
        if (mBodies[i]->GetVelChanged())
        {
          mBodies[i]->SetVelocity(tVector3::Zero());
          mBodies[i]->SetAngVel(tVector3::Zero());
          mBodies[i]->ClearVelChanged();
        }
      }
    }
  }
}

//==============================================================
// MoreCollisionheight
//==============================================================
bool MoreCollisionHeight(const tCollisionInfo* info0, const tCollisionInfo* info1)
{
  if (!info0->mSkinInfo.skin0->GetOwner())
    return false;
  if (!info1->mSkinInfo.skin1->GetOwner())
    return false;
  return (info0->mSkinInfo.skin0->GetOwner()->GetPosition().z > 
          info1->mSkinInfo.skin0->GetOwner()->GetPosition().z);
}

//==============================================================
// LessCollisionheight
//==============================================================
bool LessCollisionHeight(const tCollisionInfo* info0, const tCollisionInfo* info1)
{
  if (!info0->mSkinInfo.skin0->GetOwner())
    return false;
  if (!info1->mSkinInfo.skin1->GetOwner())
    return false;
  return (info0->mSkinInfo.skin0->GetOwner()->GetPosition().z < 
          info1->mSkinInfo.skin0->GetOwner()->GetPosition().z);
}

//====================================================================
// MoreCollisionDepth
//====================================================================
bool MoreCollisionDepth(const tCollisionInfo* info0, const tCollisionInfo* info1)
{
  tScalar avDepth0 = 0.0f;
  tScalar avDepth1 = 0.0f;
  unsigned n0 = info0->mPointInfo.Size();
  unsigned n1 = info1->mPointInfo.Size();
  unsigned count0 = 0;
  unsigned count1 = 0;
  for (unsigned i0 = 0 ; i0 < n0 ; ++i0)
  {
    tScalar depth = info0->mPointInfo[i0].mInitialPenetration;
    if (depth > 0.0f)
    {
      avDepth0 += depth;
      ++count0;
    }
  }
  for (unsigned i1 = 0 ; i1 < n1 ; ++i1)
  {
    tScalar depth = info1->mPointInfo[i1].mInitialPenetration;
    if (depth > 0.0f)
    {
      avDepth1 += depth;
      ++count1;
    }
  }
  if (count0 > 0)
    avDepth0 /= count0;
  if (count1 > 0)
    avDepth1 /= count1;

  return avDepth0 > avDepth1;
}

//====================================================================
// MoreCollisionDepth
//====================================================================
bool LessCollisionDepth(const tCollisionInfo* info0, const tCollisionInfo* info1)
{
  tScalar avDepth0 = 0.0f;
  tScalar avDepth1 = 0.0f;
  unsigned n0 = info0->mPointInfo.Size();
  unsigned n1 = info1->mPointInfo.Size();
  unsigned count0 = 0;
  unsigned count1 = 0;
  for (unsigned i0 = 0 ; i0 < n0 ; ++i0)
  {
    tScalar depth = info0->mPointInfo[i0].mInitialPenetration;
    if (depth > 0.0f)
    {
      avDepth0 += depth;
      ++count0;
    }
  }
  for (unsigned i1 = 0 ; i1 < n1 ; ++i1)
  {
    tScalar depth = info1->mPointInfo[i1].mInitialPenetration;
    if (depth > 0.0f)
    {
      avDepth1 += depth;
      ++count1;
    }
  }
  if (count0 > 0)
    avDepth0 /= count0;
  if (count1 > 0)
    avDepth1 /= count1;

  return avDepth0 < avDepth1;
}

//==============================================================
// detect_all_collisions
//==============================================================
void tPhysicsSystem::DetectAllCollisions(tScalar dt)
{
  TRACE_METHOD_ONLY(FRAME_1);

  if (!mCollisionSystem)
    return;

  unsigned numBodies = mBodies.size();
  unsigned numColls = mCollisions.size();
  unsigned numActiveBodies = mActiveBodies.size();

  unsigned i;

  for (i = 0 ; i < numActiveBodies ; ++i)
    mActiveBodies[i]->StoreState();

  UpdateAllVelocities(dt);
  UpdateAllPositions(dt);

  for (i = 0 ; i < numColls ; ++i)
    tCollisionInfo::FreeCollisionInfo(*mCollisions[i]);
  mCollisions.resize(0);

  for (i = 0 ; i < numBodies ; ++i)
  {
    if (mBodies[i]->GetCollisionSkin())
      mBodies[i]->GetCollisionSkin()->GetCollisions().resize(0);
  }

  tBasicCollisionFunctor functor(mCollisions);
  mCollisionSystem->DetectAllCollisions(
    mActiveBodies, 
    functor, 
    0,
    mCollToll);

  // There's a bug in MSV6.0 std::sort - accesses index -1 !!!!
  // stable_sort seems to work. However, best results with jenga
  // stacks come from random.
  std::random_shuffle(mCollisions.begin(), mCollisions.end());
//  std::stable_sort(mCollisions.begin(), mCollisions.end(), MoreCollisionHeight);
//  std::stable_sort(mCollisions.begin(), mCollisions.end(), MoreCollisionDepth);

  for (i = 0 ; i < numActiveBodies ; ++i)
    mActiveBodies[i]->RestoreState();
}

//==============================================================
// SetCollisionFns
//==============================================================
void tPhysicsSystem::SetCollisionFns()
{
  switch (mSolverType)
  {
  case SOLVER_FAST:
    mPreProcessCollisionFn = mPreProcessContactFn = &tPhysicsSystem::PreProcessCollisionFast;
    mProcessCollisionFn = mProcessContactFn = &tPhysicsSystem::ProcessCollisionFast;
    return;
  case SOLVER_NORMAL:
    mPreProcessCollisionFn = mPreProcessContactFn = &tPhysicsSystem::PreProcessCollision;
    mProcessCollisionFn = mProcessContactFn = &tPhysicsSystem::ProcessCollision;
    return;
  case SOLVER_COMBINED:
    mPreProcessCollisionFn = mPreProcessContactFn = &tPhysicsSystem::PreProcessCollision;
    mProcessCollisionFn = mProcessContactFn = &tPhysicsSystem::ProcessCollisionCombined;
    return;
  case SOLVER_ACCUMULATED:
    mPreProcessCollisionFn = &tPhysicsSystem::PreProcessCollision;
    mProcessCollisionFn = &tPhysicsSystem::ProcessCollision;
    mPreProcessContactFn = &tPhysicsSystem::PreProcessCollisionAccumulated;
    mProcessContactFn = &tPhysicsSystem::ProcessCollisionAccumulated;
    return;
  }
  
  TRACE("Bad solver type!\n");
  mPreProcessCollisionFn = &tPhysicsSystem::PreProcessCollision;
  mProcessCollisionFn = &tPhysicsSystem::ProcessCollision;
}

//==============================================================
// handle_all_collisions
//==============================================================
void tPhysicsSystem::HandleAllConstraints(tScalar dt, unsigned iter, bool forceInelastic)
{
  TRACE_METHOD_ONLY(FRAME_1);

  unsigned i;
  unsigned origNumCollisions = mCollisions.size();
  const unsigned numConstraints = mConstraints.size();

  // prepare all the constraints
  for (i = 0 ; i < numConstraints ; ++i)
  {
    mConstraints[i]->PreApply(dt);
  }

  // prepare all the collisions 
  if (forceInelastic)
  {
    for (i = 0 ; i < origNumCollisions ; ++i)
    {
     (this->*mPreProcessContactFn)(mCollisions[i], dt);
      mCollisions[i]->mMatPairProperties.mRestitution = 0.0f;
      mCollisions[i]->mSatisfied = false;
    }
  }
  else
  {
    // prepare for the collisions
    for (i = 0 ; i < origNumCollisions ; ++i)
      (this->*mPreProcessCollisionFn)(mCollisions[i], dt);
  }
  
  // iterate over the collisions
  static int dir = 1;
  for (unsigned step = 0 ; step < iter ; ++step)
  {
    bool gotOne = false;
    // step 6
    unsigned numCollisions = mCollisions.size();
    dir = !dir;
    for (i = dir ? 0 : numCollisions - 1; 
         i >= 0 && i < numCollisions; 
         dir ? ++i : --i)
    {
      if (!mCollisions[i]->mSatisfied)
      {
        if (forceInelastic)
          gotOne |= (this->*mProcessContactFn)(mCollisions[i], dt, step == 0);
        else
          gotOne |= (this->*mProcessCollisionFn)(mCollisions[i], dt, step == 0);
      }
    }
    for (i = 0 ; i < numConstraints ; ++i)
    {
      if (!mConstraints[i]->GetSatisfied())
      {
        gotOne |= mConstraints[i]->Apply(dt);
      }
    }
    // wake up any previously stationary frozen objects that were
    // frozen. 
    if (mFreezingEnabled)
      TryToActivateAllFrozenObjects();

    // number of collisions may have increased...
    numCollisions = mCollisions.size();

    // preprocess any new collisions.
    if (forceInelastic)
    {
      for (i = origNumCollisions ; i < numCollisions ; ++i)
      {
        mCollisions[i]->mMatPairProperties.mRestitution = 0.0f;
        mCollisions[i]->mSatisfied = false;
        (this->*mPreProcessContactFn)(mCollisions[i], dt);
      }
    }
    else
    {
      for (i = origNumCollisions ; i < numCollisions ; ++i)
      {
        (this->*mPreProcessCollisionFn)(mCollisions[i], dt);
      }
    }
    
    origNumCollisions = numCollisions;

    if (!gotOne)
      break;
  }
}

/// Comparisons for ordering the shock step
inline bool LessBodyX(tBody * body0, tBody * body1)
{
  return body0->GetPosition().x < body1->GetPosition().x;
}
inline bool LessBodyY(tBody * body0, tBody * body1)
{
  return body0->GetPosition().y < body1->GetPosition().y;
}
inline bool LessBodyZ(tBody * body0, tBody * body1)
{
  return body0->GetPosition().z < body1->GetPosition().z;
}

//==============================================================
// DoShockStep
//==============================================================
void tPhysicsSystem::DoShockStep(tScalar dt)
{
  const int numBodies = mBodies.size();

  if ( Abs(mGravity.x) > Abs(mGravity.y) && Abs(mGravity.x) > Abs(mGravity.z) ) 
    sort(mBodies.begin(), mBodies.end(), LessBodyX);
  else if ( Abs(mGravity.y) > Abs(mGravity.z) && Abs(mGravity.y) > Abs(mGravity.x) ) 
    sort(mBodies.begin(), mBodies.end(), LessBodyY);
  else if ( Abs(mGravity.z) > Abs(mGravity.x) && Abs(mGravity.z) > Abs(mGravity.y) ) 
    sort(mBodies.begin(), mBodies.end(), LessBodyZ);

  bool gotOne = true;
  unsigned nLoops = 0;
  while (gotOne)
  {
    gotOne = false;
    ++nLoops;
    for (int iBody = 0 ; iBody < numBodies ; ++iBody)
    {
      tBody * body = mBodies[iBody];
      if (!body->GetImmovable() && body->GetDoShockProcessing())
      {
        tCollisionSkin * skin = body->GetCollisionSkin();
        if (skin)
        {
          vector<tCollisionInfo *> & colls = skin->GetCollisions();
          int numColls = colls.size();
          if ((0 == numColls) || (!body->IsActive()))
          {
            body->InternalSetImmovable();
          }
          else
          {
            bool setImmovable = false;
            // process every collision on body that is between it and
            // another immovable... then make it immovable (temporarily).
            for (int i = 0 ; i < numColls ; ++i)
            {
              tCollisionInfo * info = colls[i];
              // only if this collision is against an immovable object then
              // process it
              if ( ( (info->mSkinInfo.skin0 == body->GetCollisionSkin()) &&
                     ( (info->mSkinInfo.skin1->GetOwner() == 0) ||
                       (info->mSkinInfo.skin1->GetOwner()->GetImmovable()) ) ) ||
                   ( (info->mSkinInfo.skin1 == body->GetCollisionSkin()) &&
                     ( (info->mSkinInfo.skin0->GetOwner() == 0) ||
                       (info->mSkinInfo.skin0->GetOwner()->GetImmovable()) ) ) )
              {
                // need to recalc denominator since immovable set
                (this->*mPreProcessCollisionFn)(info, dt); 
                ProcessCollisionForShock(info, dt);
                setImmovable = true;
              }
            }
            if (setImmovable)
            {
              body->InternalSetImmovable();
              gotOne = true;
            }
          }
        }
        else
        {
          // no skin - help early out next loop
          body->InternalSetImmovable();
        }
      }
    }
  }
  TRACE_FILE_IF(MULTI_FRAME_3)
    TRACE("loops = %d\n", nLoops);

  for (int i = 0 ; i < numBodies ; ++i)
    mBodies[i]->InternalRestoreImmovable();
}

//==============================================================
// do_all_external_forces
//==============================================================
void tPhysicsSystem::GetAllExternalForces(tScalar dt)
{
  TRACE_METHOD_ONLY(FRAME_1);
  int numBodies = mBodies.size();
  int i;
  for (i = 0 ; i < numBodies ; ++i)
  {
    mBodies[i]->AddExternalForces(dt);
  }

  int numControllers = mControllers.size();
  for (i = 0 ; i < numControllers ; ++i)
  {
    mControllers[i]->UpdateController(dt);
  }
}

//==============================================================
// update_all_velocities
// todo - currently update vel of inactive objects, in case they
// have some force which might result in them becoming active.
// maybe have a flag that they set when their force is non-zero,
// so we could skip this when they're inactive
//==============================================================
void tPhysicsSystem::UpdateAllVelocities(tScalar dt)
{
  TRACE_METHOD_ONLY(FRAME_1);
  int numBodies = mBodies.size();
  for (int i = 0 ; i < numBodies ; ++i)
  {
    if (mBodies[i]->IsActive() || mBodies[i]->GetVelChanged())
      mBodies[i]->UpdateVelocity(dt);
  }
}

//==============================================================
// update_all_positions
//==============================================================
void tPhysicsSystem::UpdateAllPositions(tScalar dt)
{
  TRACE_METHOD_ONLY(FRAME_1);
  int numBodies = mActiveBodies.size();
  for (int i = 0 ; i < numBodies ; ++i)
    mActiveBodies[i]->UpdatePositionWithAux(dt);
}

//==============================================================
// NotifyAllPostPhysics
//==============================================================
void tPhysicsSystem::NotifyAllPostPhysics(tScalar dt)
{
  TRACE_METHOD_ONLY(FRAME_1);
  int numBodies = mBodies.size();
  for (int i = 0 ; i < numBodies ; ++i)
    mBodies[i]->PostPhysics(dt);
}

//==============================================================
// CopyAllCurrentStatesToOld
//==============================================================
void tPhysicsSystem::CopyAllCurrentStatesToOld()
{
  int numBodies = mBodies.size();
  int i;
  for (i = 0 ; i < numBodies ; ++i)
  {
    if (mBodies[i]->IsActive() || mBodies[i]->GetVelChanged())
      mBodies[i]->CopyCurrentStateToOld();
  }
}

//==============================================================
// try_to_freeze_all_objects
//==============================================================
void tPhysicsSystem::TryToFreezeAllObjects(tScalar dt)
{
  TRACE_METHOD_ONLY(FRAME_1);
  int numBodies = mActiveBodies.size();
  int i;
  for (i = 0 ; i < numBodies ; ++i)
    mActiveBodies[i]->TryToFreeze(dt);
}

//==============================================================
// activate_all_frozen_objects_left_hanging
//==============================================================
void tPhysicsSystem::ActivateAllFrozenObjectsLeftHanging()
{
  TRACE_METHOD_ONLY(FRAME_1);
  const unsigned numBodies = mBodies.size();
  for (unsigned i = 0 ; i < numBodies ; ++i)
  {
    tBody * thisBody = mBodies[i];
    if ( thisBody->IsActive() &&
         thisBody->GetCollisionSkin() )
    {
      // first activate any bodies due to the movement of this body
      thisBody->DoMovementActivations();

      // now record any movement notifications that are needed
      vector<tCollisionInfo *> & collisions = 
        mBodies[i]->GetCollisionSkin()->GetCollisions();
      if (!collisions.empty())
      {
        // walk through the object's contact list
        unsigned j;
        const unsigned numCollisions = collisions.size();
        for (j = 0 ; j < numCollisions ; ++j)
        {
          const tCollisionInfo & coll = *collisions[j];
          Assert(coll.mSkinInfo.skin1);
          // must be a body-body interaction to be interesting
          if (coll.mSkinInfo.skin1->GetOwner())
          {
            tBody * other_body = coll.mSkinInfo.skin0->GetOwner();
//            tVector3 dirToOther = coll.mDirToBody0;

            if (other_body == thisBody)
            {
              other_body = coll.mSkinInfo.skin1->GetOwner();
//              dirToOther.Negate();
            }
            if (!other_body->IsActive())
            {
              // only wake up objects that would "fall" against this
              // one...  actually don't do this because it breaks if
              // an object below should rotate (e.g. removing a top
              // block from a wall)
//              if (Dot(dirToOther, other_body->GetForce()) < -SCALAR_TINY)
              {
                thisBody->AddMovementActivation(
                  thisBody->GetPosition(), 
                  other_body);
              }
            }
          }
        }
      }
    }
  }
}


//==============================================================
// enable_freezing
//==============================================================
void tPhysicsSystem::EnableFreezing(bool freeze)
{
  TRACE_METHOD_ONLY(ONCE_2);
  mFreezingEnabled = freeze;

  if (!mFreezingEnabled)
  {
    int numBodies = mBodies.size();
    int i;
    for (i = 0 ; i < numBodies ; ++i)
    {
      mBodies[i]->SetActive();
    }
  }
}

//==============================================================
// FindAllActiveBodies
// we take advantage of the fact that during most of the physics
// update we only ever activate objects - It's only in
// TryToFreezeAllObjects that objects get deactivated. Therefore
// whenever an object is activated we can add it to our list, 
// confident that it won't be added twice
//==============================================================
void tPhysicsSystem::FindAllActiveBodies()
{
  mActiveBodies.resize(0);
  int numBodies = mBodies.size();
  int i;
  for (i = 0 ; i < numBodies ; ++i)
  {
    if (mBodies[i]->IsActive())
      mActiveBodies.push_back(mBodies[i]);
  }

}

//========================================================
// LimitAllVelocities
//========================================================
void tPhysicsSystem::LimitAllVelocities()
{
  int numActiveBodies = mActiveBodies.size();
  for (int i = 0 ; i < numActiveBodies ; ++i)
  {
    mActiveBodies[i]->LimitVel();
    mActiveBodies[i]->LimitAngVel();
  }
}

//========================================================
// UpdateContactCache
//========================================================
void tPhysicsSystem::UpdateContactCache()
{
  mCachedContacts.clear();

  for (unsigned i = mCollisions.size() ; i-- != 0 ; )
  {
    tCollisionInfo * collInfo = mCollisions[i];
    for (unsigned iPos = 0 ; iPos < collInfo->mPointInfo.Size() ; ++iPos)
    {
      tCollPointInfo& ptInfo = collInfo->mPointInfo[iPos];
      mCachedContacts.insert(
        make_pair(tBodyPair(collInfo->mSkinInfo.skin0->GetOwner(), 
                            collInfo->mSkinInfo.skin1->GetOwner(), ptInfo.mR0, ptInfo.mR1), 
                  tCachedImpulses(ptInfo.mAccumulatedNormalImpulse,
                                  ptInfo.mAccumulatedNormalImpulseAux,
                                  collInfo->mSkinInfo.skin0->GetOwner() > collInfo->mSkinInfo.skin1->GetOwner() ? 
                                    ptInfo.mAccumulatedFrictionImpulse : -ptInfo.mAccumulatedFrictionImpulse)));
    }
  }
}

//==============================================================
// Integrate
//==============================================================
void tPhysicsSystem::Integrate(tScalar dt)
{
  TRACE_METHOD_ONLY(FRAME_1);
  mDoingIntegration = true;

  mOldTime = mTargetTime;
  mTargetTime += dt;

  SetCollisionFns();

  FindAllActiveBodies();

  CopyAllCurrentStatesToOld();

  GetAllExternalForces(dt);

  DetectAllCollisions(dt);

  HandleAllConstraints(dt, mNumCollisionIterations, false);

  UpdateAllVelocities(dt);

  HandleAllConstraints(dt, mNumContactIterations, true);

  // do a shock step to help stacking
  if (mDoShockStep)
    DoShockStep(dt);

  DampAllActiveBodies();

  if (mFreezingEnabled)
  {
    TryToFreezeAllObjects(dt);
    ActivateAllFrozenObjectsLeftHanging();
  }

  LimitAllVelocities();

  UpdateAllPositions(dt);

  NotifyAllPostPhysics(dt);

#ifdef USE_OLD_IMPULSE
  if (mSolverType == SOLVER_ACCUMULATED)
    UpdateContactCache();
#endif

  if (mNullUpdate)
  {
    for (unsigned i = 0 ; i < mActiveBodies.size() ; ++i)
      mActiveBodies[i]->RestoreState();
  }

  mDoingIntegration = false;
}

