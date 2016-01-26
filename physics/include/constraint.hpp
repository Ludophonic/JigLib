//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file constraint.hpp 
//                     
//==============================================================
#ifndef JIGCONSTRAINT_HPP
#define JIGCONSTRAINT_HPP
#include "../maths/include/precision.hpp"

namespace JigLib
{
  class tConstraint
  {
  public:
    tConstraint();
    virtual ~tConstraint();
    
    /// register with the physics system
    void EnableConstraint();

    /// deregister from the physics system
    void DisableConstraint();

    /// are we registered with the physics system?
    bool GetConstraintEnabled() const {return mConstraintEnabled;}
   
  protected:
    friend class tPhysicsSystem;
    friend class tBody;

    /// prepare for applying constraints - the subsequent calls to
    /// apply will all occur with a constant position i.e. precalculate
    /// everything possible
    virtual void PreApply(tScalar dt) {SetUnsatisfied();}
    
    /// apply the constraint by adding impulses. Return value
    /// indicates if any impulses were applied. If impulses were applied
    /// the derived class should call SetConstraintsUnsatisfied() on each
    /// body that is involved.
    virtual bool Apply(tScalar dt) = 0;
    
    /// implementation should remove all references to bodies etc - they've 
    /// been destroyed. Called from ~tBody
    virtual void Destroy() = 0;

  protected:
    /// Derived class should call this when Apply has been called on 
    /// this constraint
    void SetSatisfied() {mSatisfied = true;}
    /// Body will call this from SetConstraintsUnsatisfied.
    void SetUnsatisfied() {mSatisfied = false;}
    /// Used by physics to bypass calling this
    bool GetSatisfied() const {return mSatisfied;}
    
  private:
    bool mConstraintEnabled;
    bool mSatisfied;
  };
}

#endif
