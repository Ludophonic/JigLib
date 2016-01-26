//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file body.hpp 
//                     
//==============================================================
#ifndef JIGBODY_HPP
#define JIGBODY_HPP

#include "../maths/include/transform3.hpp"
#include "../physics/include/physicssystem.hpp"
#include "../physics/include/constraint.hpp"

namespace JigLib
{
  /// Basic rigid body - can be used as is, or a derived class can
  /// over-ride certain behaviours.
  /// A body can only be added to one physics system at a time!
  class tBody
  {
  public:
    tBody();
    virtual ~tBody();

    //==============================================================
    // A few virtual functions that can be over-ridden
    //==============================================================

    /// allow the body to add on any additional forces (including
    /// gravity)/impulses etc. Default behaviour sets to gravity.
    virtual void AddExternalForces(tScalar dt);
    
    /// Called right at the end of the timestep to notify the derived class
    virtual void PostPhysics(tScalar dt) {}

    /// register with the physics system
    void EnableBody();

    /// deregister from the physics system
    void DisableBody();

    /// are we registered with the physics system?
    bool GetBodyEnabled() const {return mBodyEnabled;}

    /// allowed to return 0 if this body doen't engage in collisions
    class tCollisionSkin * GetCollisionSkin() { return mCollSkin; }
    
    /// The body doesn't actually own the collision skin
    void SetCollisionSkin(class tCollisionSkin * skin) { mCollSkin = skin; }
    
    /// This sets the position (sets the vel to 0), but it also tries
    /// to make sure that any frozen bodies resting against this one
    /// get activated if necessary.  Not very efficient. Be a little
    /// careful about when you call it - it will mess with the physics
    /// collision list.  Also, after this call the object will be
    /// active.
    void MoveTo(const tVector3 & pos, const tMatrix33 & orientation);
    
    void SetTransform(const tTransform3 &t) {mTransform = t;}
    void SetTransformRate(const tTransform3Rate &rate) {mTransformRate = rate;}
    const tTransform3 &GetTransform() const {return mTransform;}
    const tTransform3 &GetOldTransform() const {return mOldTransform;}
    const tTransform3Rate &GetTransformRate() const {return mTransformRate;}
    const tTransform3Rate &GetOldTransformRate() const {return mOldTransformRate;}

    void SetPosition(const tVector3 & pos) {mTransform.position = pos; }
    const tVector3 & GetPosition() const { return mTransform.position; }
    const tVector3 & GetOldPosition() const { return mOldTransform.position; }
    
    void SetOrientation(const tMatrix33 & orient);
    const tMatrix33 & GetOrientation() const { return mTransform.orientation; }
    const tMatrix33 & GetOldOrientation() const { return mOldTransform.orientation; }
    
    void SetVelocity(const tVector3 & vel) { mTransformRate.velocity = vel; }
    void SetVelocityAux(const tVector3 & vel) { mTransformRateAux.velocity = vel; }
    const tVector3 & GetVelocity() const { return mTransformRate.velocity; }
    const tVector3 & GetOldVelocity() const { return mOldTransformRate.velocity; }
    const tVector3 & GetVelocityAux() const { return mTransformRateAux.velocity; }

    void SetAngVel(const tVector3 & angVel) { mTransformRate.angVelocity = angVel; }
    void SetAngVelAux(const tVector3 & angVel) { mTransformRateAux.angVelocity = angVel; }
    const tVector3 & GetAngVel() const { return mTransformRate.angVelocity; }
    const tVector3 & GetOldAngVel() const { return mOldTransformRate.angVelocity; }
    const tVector3 & GetAngVelAux() const { return mTransformRateAux.angVelocity; }
    
    void SetForce(const tVector3 & f) { mForce = f; }
    const tVector3 & GetForce() const { return mForce; }
    
    void SetTorque(const tVector3 & t) { mTorque = t; }
    const tVector3 & GetTorque() const { return mTorque; }

    /// Returns the velocity of a point at body-relative position
    /// (in world frame) relPos
    tVector3 GetVelocity(const tVector3& relPos) const;
    /// As GetVelocity but just uses the aux velocities
    tVector3 GetVelocityAux(const tVector3& relPos) const;
    
    /// Sets the force on this object to the global gravitational force 
    /// (taken from the physics system)
    void SetForceToGravity();
    
    /// if mass = 0, then inv_mass will be very very large, but not
    /// infinite. Similarly if inv_mass = 0
    void SetMass(tScalar mass);
    void SetInvMass(tScalar invMass);
    tScalar GetMass() const {return mMass;}
    tScalar GetInvMass() const {return mInvMass;}
    
    void SetBodyInertia(tScalar Ixx, tScalar Iyy, tScalar Izz);
    void SetBodyInertia(const tMatrix33 &bodyInertia);
    void SetBodyInvInertia(tScalar invIxx, tScalar invIyy, tScalar invIzz);
    const tMatrix33 & GetBodyInertia() const {return mBodyInertia;}
    const tMatrix33 & GetBodyInvInertia() const {return mBodyInvInertia;}
    const tMatrix33 & GetWorldInvInertia() const { return mWorldInvInertia; }
    const tMatrix33 & GetWorldInertia() const { return mWorldInertia; }
    
    // functions to add forces in the world coordinate frame
    void AddWorldForce(const tVector3 & force);
    void AddWorldForce(const tVector3 & force, const tVector3 & pos);
    void AddWorldTorque(const tVector3 & torque);
    
    // functions to add impulses in the world coordinate frame
    void ApplyWorldImpulse(const tVector3 & impulse);
    void ApplyNegativeWorldImpulse(const tVector3 & impulse);
    void ApplyWorldImpulse(const tVector3 & impulse, 
                           const tVector3 & pos);
    void ApplyNegativeWorldImpulse(const tVector3 & impulse, 
                                   const tVector3 & pos);
    void ApplyWorldAngImpulse(const tVector3 & angImpulse);
    
    void ApplyWorldImpulseAux(const tVector3 & impulse);
    void ApplyNegativeWorldImpulseAux(const tVector3 & impulse);
    void ApplyWorldImpulseAux(const tVector3 & impulse, 
                              const tVector3 & pos);
    void ApplyNegativeWorldImpulseAux(const tVector3 & impulse, 
                                      const tVector3 & pos);

    // functions to add forces in the body coordinate frame
    void AddBodyForce(const tVector3 & force);
    void AddBodyForce(const tVector3 & force, const tVector3 & pos);
    void AddBodyTorque(const tVector3 & torque);
    
    // functions to add impulses in the body coordinate frame
    void ApplyBodyImpulse(const tVector3 & impulse);
    void ApplyNegativeBodyImpulse(const tVector3 & impulse);
    void ApplyBodyImpulse(const tVector3 & impulse, const tVector3 & pos);
    void ApplyNegativeBodyImpulse(const tVector3 & impulse, const tVector3 & pos);
    void ApplyBodyAngImpulse(const tVector3 & angImpulse);

    // functions to add impulses at a position offset in world space
    void ApplyBodyWorldImpulse(const tVector3 & impulse, const tVector3 & delta);
    void ApplyNegativeBodyWorldImpulse(const tVector3 & impulse, const tVector3 & delta);

    void ApplyBodyWorldImpulseAux(const tVector3 & impulse, const tVector3 & delta);
    void ApplyNegativeBodyWorldImpulseAux(const tVector3 & impulse, const tVector3 & delta);

    /// This just sets all forces/impulses etc to zero
    void ClearForces();
    
    /// copies the current position etc to old - normally called only
    /// by tPhysicsSystem.
    void CopyCurrentStateToOld();
    
    /// ensures that this object never moves, and reduces collision
    /// checking
    void SetImmovable(bool immovable);

    /// indicates if we ever move (change our position - may still
    /// have a non-zero velocity for conveyor-belts!)
    bool GetImmovable() const {return mImmovable;}
    
    enum tActivity {ACTIVE, INACTIVE};
    bool IsActive() const {return mActivity == ACTIVE;}
    
    /// Allow the activity to be explicitly set - be careful about
    /// explicitly freezing an object (may become unfrozen anyway).
    /// If setting to ACTIVE, then the activity factor indicates how
    /// active the object is considered to be - a value of 1 means
    /// completely active - a value nearer 0 means that freezing will
    /// happen much sooner (assuming no further movement).
    void SetActive(tScalar activity_factor = 1.0f);
    void SetInactive();
    
    /// indicates if the velocity is above the threshold for freezing
    bool GetShouldBeActive() {
      return ( (mTransformRate.velocity.GetLengthSq() > 
                mSqVelocityActivityThreshold) ||
               (mTransformRate.angVelocity.GetLengthSq() > 
                mSqAngVelActivityThreshold) );}
    bool GetShouldBeActiveAux() {
      return ( (mTransformRateAux.velocity.GetLengthSq() > 
                mSqVelocityActivityThreshold) ||
               (mTransformRateAux.angVelocity.GetLengthSq() > 
                mSqAngVelActivityThreshold) );}
    
    //==============================================================
    // Configuration
    //==============================================================

    /// set how long it takes to deactivate
    void SetDeactivationTime(tScalar seconds);
    
    /// set what the velocity threshold is for activation
    /// rot is in deg per second.
    void SetActivityThreshold(tScalar vel, tScalar angVel);

    /// set the thresholds for deactivation
    void SetDeactivationThreshold(tScalar posThreshold,
                                  tScalar orientThreshold);

    /// values > 1 make the body less likely to wake up following an impulse.
    void SetActivationFactor(tScalar factor);
    
    /// allows setting of whether this body ever freezes
    void SetAllowFreezing(bool allow);
    
    /// Bodies that use a lot of constraints don't behave very well during the
    /// physics shock step, so they can bypass it
    void SetDoShockProcessing(bool doit) {mDoShockProcessing = doit;}
    bool GetDoShockProcessing() const {return mDoShockProcessing;}

    /// Marks all constraints/collisions as being unsatisfied
    void SetConstraintsAndCollisionsUnsatisfied();

    /// Allow constraints to "register" themselves with this body
    void AddConstraint(class tConstraint * constraint);

    /// Allow constraints to "deregister" themselves with this body
    void RemoveConstraint(class tConstraint * constraint);
  private:
    // some functions really just for internal use 
    friend class tPhysicsSystem;

    /// Copy our current state (position, velocity etc) into the stored state
    void StoreState();
    /// restore from the stored state into our current state.
    void RestoreState();

    /// implementation updates the velocity/angular rotation with the
    /// force/torque.
    void UpdateVelocity(tScalar dt);
    
    /// implementation updates the position/orientation with the
    /// current velocties. 
    void UpdatePosition(tScalar dt);
    /// Updates the position with the auxilary velocities, and zeros them
    void UpdatePositionWithAux(tScalar dt);

    /// function provided for the use of Physics system
    inline void TryToFreeze(tScalar dt);
    
    /// damp movement as the body approaches deactivation
    void DampForDeactivation();

    /// function provided for use of physics system. Activates any
    /// body in its list if it's moved more than a certain distance,
    /// in which case it also clears its list.
    void DoMovementActivations();
    
    /// adds the other body to the list of bodies to be activated if
    /// this body moves more than a certain distance from either a
    /// previously stored position, or the position passed in.
    void AddMovementActivation(const tVector3 & pos, 
                               tBody * other_body);
    
    /// Used by physics to temporarily make an object immovable -
    /// needs to restore afterwars!
    void InternalSetImmovable() {
      mOrigImmovable = mImmovable; mImmovable = true;}
    void InternalRestoreImmovable() {
      mImmovable = mOrigImmovable;}

    bool GetVelChanged() const {return mVelChanged;}
    void ClearVelChanged() {mVelChanged = false;}

    void LimitVel();
    void LimitAngVel();
    
  private:
    /// Helper to stop the velocities getting silly
    static tScalar mVelMax;
    static tScalar mAngVelMax;
    bool mBodyEnabled;
    
    /// don't actually own the skin...
    tCollisionSkin * mCollSkin;
    
    /// the "working" state
    tTransform3 mTransform;
    tTransform3Rate mTransformRate;
    tTransform3Rate mTransformRateAux;

    /// Prevent velocity updates etc 
    bool mImmovable;
    /// Flag that gets set whenever our velocity/angular velocity might have been changed.
    /// Used by physics to speed up checking for activation
    bool mVelChanged;
    tScalar mInvMass;

    /// inverse inertia in world frame
    tMatrix33 mWorldInvInertia;
    
    /// the previous state - copied explicitly using
    /// CopyCurrentStateToOld.
    tTransform3 mOldTransform;
    tTransform3Rate mOldTransformRate;
    
    /// stored state - used internally by physics during the updates
    tTransform3 mStoredTransform;
    tTransform3Rate mStoredTransformRate;

    tMatrix33 mInvOrientation;
    
    tScalar mMass;
    
    bool mOrigImmovable;
    
    bool mDoShockProcessing;

    /// inertia in body space (not necessarily diagonal)
    tMatrix33 mBodyInertia;
    
    /// inertia tensor in world space
    tMatrix33 mWorldInertia;
    
    /// inverse inertia in body frame
    tMatrix33 mBodyInvInertia;
    
    /// force etc in world frame.
    tVector3 mForce;
    /// torque in world frame
    tVector3 mTorque;
    
    /// for deactivation
    tActivity mActivity;
    /// How long we've been still
    tScalar mInactiveTime;
    
    /// last position for when trying the deactivate
    tVector3 mLastPositionForDeactivation;
    /// last orientation for when trying to deactivate
    tMatrix33 mLastOrientationForDeactivation;
    
    /// change for detecting position changes during deactivation
    tScalar mSqDeltaPosThreshold;
    /// change (todo - hacky) for detecting orientation changes during
    /// deactivation.
    tScalar mSqDeltaOrientThreshold;

    /// how long it takes to go from active to frozen when stationary
    tScalar mDeactivationTime;
    
    /// velocity below which we're considered still
    tScalar mSqVelocityActivityThreshold;
    /// ang velocity below which we're considered still - in (radians
    /// per sec)^2
    tScalar mSqAngVelActivityThreshold;
    
    /// The position stored when we need to notify other bodies
    tVector3 mStoredPositionForActivation;
    /// The list of bodies that need to be activated when we move away
    /// from our stored position
    std::vector<tBody *> mBodiesToBeActivatedOnMovement;
    
    /// whether this body can freeze (assuming Physics freezing is
    /// enabled)
    bool mAllowFreezing;

    /// list of constraints that act on this body
    std::vector<class tConstraint *> mConstraints;
  };
  
//==============================================================
// Inline implementations
//==============================================================
#include "../physics/include/body.inl"
  
}

#endif
