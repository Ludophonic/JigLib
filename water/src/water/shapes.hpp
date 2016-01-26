#ifndef SHAPES_HPP
#define SHAPES_HPP

#include "jiglib.hpp"

#include <vector>

/// point used in a verlet/particle representation of a shape
class tShapeParticle
{
public:
  tShapeParticle() {}
  tShapeParticle(const JigLib::tVector2 & pos,
                 JigLib::tScalar mass) : 
    mPos(pos), mOldPos(pos), mForce(0.0f, 0.0f), mInvMass(1.0f / mass) {}
  JigLib::tVector2 mPos;
  JigLib::tVector2 mOldPos;
  JigLib::tVector2 mForce;
  JigLib::tScalar mInvMass;
};

class tStickConstraint
{
public:
  tStickConstraint() {}
  // explicitly set the length
  tStickConstraint(int i1, int i2, JigLib::tScalar len) :
    mI1(i1), mI2(i2), mRestLen(len) {}
  // make the constraint calculate the length from the shape (points
  // must be set!)
  tStickConstraint(int i1, int i2, class tShape * shape);
  // indices for the points
  int mI1, mI2;
  // how far apart the points should be
  JigLib::tScalar mRestLen;
};

/// virtual base class passed in when asking the object to resolve its
/// constraints.
class tPenetrationResolver
{
public:
  virtual ~tPenetrationResolver() {}
  // should return true if the particle was modified
  virtual bool ResolvePenetration(tShapeParticle & particle) = 0;
};

/// Defines a shape that is suitable for use in a verlet/particle
/// integration scheme.
class tShape
{
public:
  tShape();
  virtual ~tShape() {}

  /// adds the particle to our list, and returns the index
  int AddParticle(const tShapeParticle & particle);

  /// adds the constraint to our list and returns the index
  int AddStickConstraint(const tStickConstraint & constraint);

  /// derived class should be able to draw itself
  virtual void Draw() = 0;

  /// Set the centre of gravity position
  void SetCoGPosition(const JigLib::tVector2 & pos);

  /// move by a delta position (no rotation)
  void Move(const JigLib::tVector2 & delta);

  /// Set forces that are applied to all particles
  void SetParticleForces(const JigLib::tVector2 & f);

  /// integrate positions, applying forces
  void Integrate(JigLib::tScalar dt);

  /// resolve the distance constraints
  void ResolveConstraints(unsigned nIter, tPenetrationResolver & resolver);

  /// Get all the particles defining this shape
  tShapeParticle * GetParticles(int & num) {
    num = mNParticles ; return mParticles;}

  /// modify the point passed in so that it lies on the shape
  /// surface. Return value is true if the object is moved, and the
  /// normal (pointing out of the object) is set
  virtual bool MovePointOutOfObject(JigLib::tVector2 & pos, 
                                    JigLib::tVector2 & normal) = 0;

private:
  int mNParticles;
  int mNConstraints;
  enum {MAX_PARTICLES = 256, MAX_CONSTRAINTS = 256};
  tShapeParticle mParticles[MAX_PARTICLES];
  tStickConstraint mConstraints[MAX_CONSTRAINTS];
};

/// a simple rectangle, starting with it's CoM at the origin
class tRectangle : public tShape
{
public:
  tRectangle(JigLib::tScalar width, 
             JigLib::tScalar height, 
             JigLib::tScalar density);

  void Draw();

  bool MovePointOutOfObject(JigLib::tVector2 & pos, 
                            JigLib::tVector2 & normal);
};

#endif
