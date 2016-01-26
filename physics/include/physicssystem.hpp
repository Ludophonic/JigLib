//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file physicssystem.hpp 
//                     
//==============================================================
#ifndef JIGPHYSICSSYSTEM_HPP
#define JIGPHYSICSSYSTEM_HPP

#include "../maths/include/vector3.hpp"
#include "../maths/include/matrix33.hpp"
#include "../collision/include/collisioninfo.hpp"

#include <map>

namespace JigLib
{
  /// Looks after (but doesn't own) a collection of bodies and runs
  /// their updates. Doesn't deal with collision detection - it will
  /// get passes a collision detector to do that.
  ///
  /// In the vast majority of cases there will be only one physics system,
  /// and it is consequently very annoying if every object has to keep
  /// track of which physics system it's associated with. Therefore,
  /// tPhysicsSystem supports a "singleton" style use, but it also lets
  /// the user change the "current" physics system (bad things will
  /// happen if you set it to zero whilst there are still physical objects!).
  /// 
  /// If you want more than one physics system, then in your update loop set
  /// the physics system to the first one, run physics, then set it to
  /// the second one etc.
  /// 
  /// Note that the physics system constructor and destructor will set the
  /// current physics system variable, so under normal circumstances you don't
  /// need to worry about this.
  class tPhysicsSystem
  {
  public:
    static tPhysicsSystem * GetCurrentPhysicsSystem() {return mCurrentPhysicsSystem;}
    static void SetCurrentPhysicsSystem(tPhysicsSystem * physics) {mCurrentPhysicsSystem = physics;}

    tPhysicsSystem();
    ~tPhysicsSystem();
    
    /// if there is to be any collision detection, this physics system
    /// needs to know how to collide objects. In the absence of a
    /// collision system, no collisions will occur (surprise
    /// surprise).
    void SetCollisionSystem(class tCollisionSystem * coll) {
      mCollisionSystem = coll;}
    class tCollisionSystem * GetCollisionSystem() const {
      return mCollisionSystem;}
    
    /// Integrates the system forwards by dt - the caller is
    /// responsible for making sure that repeated calls to this use
    /// the same dt (if desired)
    void Integrate(tScalar dt);
    /// Get the physics idea of the time we're advancing towards
    tScalar GetTargetTime() const {return mTargetTime;}
    /// Gets the physics idea of the time we've left behind
    tScalar GetOldTime() const {return mOldTime;}
    
    /// Allow resetting of the physics idea of time
    void ResetTime(tScalar time) {mTargetTime = mOldTime = time;}
    
    void SetNumCollisionIterations(int num) {mNumCollisionIterations = num;}
    void SetNumContactIterations(int num) {mNumContactIterations = num;}
    void SetNumPenetrationRelaxationTimesteps(int num) {mNumPenetrationRelaxationTimesteps = num;}
    void SetAllowedPenetration(tScalar dist) {mAllowedPenetration = dist;}
    void SetDoShockStep(bool shock) {mDoShockStep = shock;}
    void SetCollToll(tScalar toll) {mCollToll = toll;}
    enum tSolverType {SOLVER_FAST, SOLVER_NORMAL, SOLVER_COMBINED, SOLVER_ACCUMULATED};
    void SetSolverType(tSolverType type) {mSolverType = type;}
		/// if nullUpdate then all updates will use dt = 0 (for debugging/profiling)
		void SetNullUpdate(bool nullUpdate) {mNullUpdate = nullUpdate;}

    const tVector3 & GetGravity() const {return mGravity;}
    void SetGravity(const tVector3 & gravity);
    tScalar GravityMagnitude() const {return mGravityMagnitude;}
    /// returns 0, 1 or 2 depending on the main gravity axis, or -1 if they're all equal
    int GetMainGravityAxis() const {return mGravityAxis;}
    
    // activate the object, and also pick up any collisions between it
    // and the rest of the world.  Also activates any adjacent objects
    // that would move if this object moved away Be careful about when
    // you use this function - bear in mind that it adds elements to the
    // internal list of collisions, which can relocate the collision
    // list. I.e. be careful about calling it from within a traversal of
    // the collision list.
    void ActivateObject(class tBody * body);
    
    /// enable/disable object freezing. if freezing is disabled, all
    /// frozen object will be activated
    void EnableFreezing(bool freeze);
    
    /// indicates if freezing is currently allowed
    bool IsFreezingEnabled() const {return mFreezingEnabled;}
    
    /// allow others to peek at the collisions we detected last
    /// timestep
    const std::vector<tCollisionInfo *> & GetCollisions() const {
      return mCollisions;}
    
  private:
    friend class tBody;
    friend class tConstraint;
    friend class tPhysicsController;
    // bodies/constraints/controllers should only be added/removed
    // outside of the main physics integration - this will get
    // asserted.
    /// Add a tBody to the simulation
    void AddBody(class tBody * body);
    /// return val indicates if body was removed
    bool RemoveBody(class tBody * body);
    /// Add a constraint to the simulation
    void AddConstraint(class tConstraint * constraint);
    /// return val indicates if constraint was removed (i.e. existed)
    bool RemoveConstraint(class tConstraint * constraint);
    /// Add a physics controlled to the simulation
    void AddController(class tPhysicsController * controller);
    /// return val indicates if controller was removed (i.e. existed)
    bool RemoveController(class tPhysicsController * controller);

  private:
    // functions working on multiple bodies etc
    void FindAllActiveBodies();
    void HandleAllConstraints(tScalar dt, unsigned iter, bool forceInelastic);
    void DoShockStep(tScalar dt);
    void GetAllExternalForces(tScalar dt);
    void UpdateAllVelocities(tScalar dt);
    void UpdateAllPositions(tScalar dt);
    void CopyAllCurrentStatesToOld();
    void DetectAllCollisions(tScalar dt);
    void NotifyAllPostPhysics(tScalar dt);
    void TryToFreezeAllObjects(tScalar dt);
    void DampAllActiveBodies();
    void TryToActivateAllFrozenObjects();
    /// try to activate frozen objects that are affected by a touching
    /// active object moving away from them
    void ActivateAllFrozenObjectsLeftHanging();
    void LimitAllVelocities();
    
    // ======== helpers for individual cases =========
    
    /// Handle an individual collision by classifying it, calculating
    /// impulse, applying impulse and updating the velocities of the
    /// objects. Allows over-riding of the elasticity. Ret val indicates
    /// if an impulse was applied
    bool ProcessCollision(tCollisionInfo * collision, 
                          tScalar dt,
                          bool firstContact);
    
    /// Ideally a fast-but-inaccurate response
    bool ProcessCollisionFast(tCollisionInfo * collision, 
                              tScalar dt,
                              bool firstContact);

    /// Accumulated and clamp impulses
    bool ProcessCollisionAccumulated(tCollisionInfo * collision, 
                                     tScalar dt,
                                     bool firstContact);
    
    /// Combines all the collision positions before actually 
    /// applying impulses.
    bool ProcessCollisionCombined(tCollisionInfo * collision, 
                                  tScalar dt,
                                  bool firstContact);
    
    /// Special simplified "collision" for the shock propogation
    bool ProcessCollisionForShock(tCollisionInfo * collision, 
                                  tScalar dt);

    /// Sets up any parameters that will not change as the collision
    /// gets processed - e.g. the relative position, elasticity etc.
    void PreProcessCollision(tCollisionInfo * collision, tScalar dt);

    /// Special pre-processor for the fast solver
    void PreProcessCollisionFast(tCollisionInfo * collision, tScalar dt);

    /// Special pre-processor for the accumulated solver
    void PreProcessCollisionAccumulated(tCollisionInfo * collision, tScalar dt);

    /// Sets the function pointers for collision processing
    void SetCollisionFns();

    class tCollisionSystem * mCollisionSystem;
    
    typedef std::vector<class tBody *> tBodies;
    typedef std::vector<tCollisionInfo *> tCollisions;
    typedef std::vector<class tConstraint *> tConstraints;
    typedef std::vector<class tPhysicsController *> tControllers;
    
    tBodies mBodies;
    tBodies mActiveBodies;
    tCollisions mCollisions;
    tConstraints mConstraints;
    tControllers mControllers;
    
    struct tStoredData
    {
      inline tStoredData(class tBody * body);
      tStoredData() {}
      tVector3 position;
      tMatrix33 orientation;
      tVector3 velocity;
      tVector3 angVel;
    };
    std::vector<tStoredData> mStoredData;
    
    /// Our idea of time
    tScalar mTargetTime;
    tScalar mOldTime;
    
    /// number of collision iterations
    unsigned mNumCollisionIterations;
    /// number of contact iteratrions
    unsigned mNumContactIterations;
    /// number of timesteps to resolve penetration over
    unsigned mNumPenetrationRelaxationTimesteps;
    /// How much penetration to allow (encourages contacts to be preserved
    tScalar mAllowedPenetration;
    // should we do a shock step?
    bool mDoShockStep;
    
    /// the tolerance for collision detection - depends on the typical scales 
    /// used in the system.
    tScalar mCollToll;
    
    /// Solver type (can get changed on the fly)
    tSolverType mSolverType;

    /// global gravity acceleration
    tVector3 mGravity;
    tScalar mGravityMagnitude;
    int mGravityAxis;
    
    /// allow objects to freeze
    bool mFreezingEnabled;
    
		/// Force null updates - i.e. dt = 0
		bool mNullUpdate;

    /// flag used internally to see if we're doing the integration,
    /// during which time certain things mustn't happen (i.e. objects
    /// be added etc).
    bool mDoingIntegration;

    /// The current system - sort-of singleton support.
    static tPhysicsSystem * mCurrentPhysicsSystem;

    struct tBodyPair
    {
      /// only the bodies are used for the comparison
      /// Note that bodyB is likely to be 0
      tBodyPair(const class tBody *bodyA, const class tBody *bodyB, const tVector3 &rA, const tVector3 &rB) 
      {
        if (bodyA > bodyB) {mBodyA = bodyA; mBodyB = bodyB; mRA = rA;}
        else {mBodyA = bodyB; mBodyB = bodyA; mRA = rB;}
      }
      bool operator<(const tBodyPair &other) const
      {
        if (mBodyA < other.mBodyA) return true;
        else if (mBodyA > other.mBodyA) return false;
        if (mBodyB < other.mBodyB) return true;
        else return false;
      }
      const class tBody * mBodyA;
      const class tBody * mBodyB;
      /// position relative to mBodyA in world space
      tVector3 mRA;
    };
    struct tCachedImpulses
    {
      tCachedImpulses(tScalar normalImpulse, tScalar normalImpulseAux, const tVector3 &frictionImpulse) 
        : mNormalImpulse(normalImpulse), mNormalImpulseAux(normalImpulseAux), mFrictionImpulse(frictionImpulse) {}
      tScalar mNormalImpulse;
      tScalar mNormalImpulseAux;
      tVector3 mFrictionImpulse;
    };
    /// Some solver methods cache a "typical" contact inpulse between body pairs to warm start
    std::multimap<tBodyPair, tCachedImpulses> mCachedContacts;
    void UpdateContactCache();

    typedef bool (tPhysicsSystem::*tProcessCollisionFn)(tCollisionInfo * collision, 
                                                        tScalar dt,
                                                        bool firstContact);
    typedef void (tPhysicsSystem::*tPreProcessCollisionFn)(tCollisionInfo * collision, 
                                                           tScalar dt);
    tPreProcessCollisionFn mPreProcessContactFn;
    tProcessCollisionFn mProcessContactFn;
    tPreProcessCollisionFn mPreProcessCollisionFn;
    tProcessCollisionFn mProcessCollisionFn;
  };
  
  
}

#endif
