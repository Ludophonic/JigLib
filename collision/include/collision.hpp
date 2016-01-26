// JigLib - Copyright (C) 2004 Danny Chapman
#ifndef JIGCOLLISION_HPP
#define JIGCOLLISION_HPP

#include "../collision/include/materials.hpp"

// Each skin contains a number of of basic geometry primitives, and it
// also stores the "old" and "new" position/orientations of the primitives.
#include "../collision/include/collisionskin.hpp"

#include "../collision/include/colldetectboxbox.hpp"
#include "../collision/include/colldetectboxheightmap.hpp"
#include "../collision/include/colldetectboxplane.hpp"
#include "../collision/include/colldetectcapsulebox.hpp"
#include "../collision/include/colldetectcapsulecapsule.hpp"
#include "../collision/include/colldetectcapsuleheightmap.hpp"
#include "../collision/include/colldetectcapsuleplane.hpp"
#include "../collision/include/colldetectspherebox.hpp"
#include "../collision/include/colldetectspherecapsule.hpp"
#include "../collision/include/colldetectsphereheightmap.hpp"
#include "../collision/include/colldetectsphereplane.hpp"
#include "../collision/include/colldetectspheresphere.hpp"
#include "../collision/include/colldetectspherestaticmesh.hpp"
#include "../collision/include/colldetectcapsulestaticmesh.hpp"
#include "../collision/include/colldetectboxstaticmesh.hpp"

#include "../collision/include/collisioninfo.hpp"
#include "../collision/include/collisionsystem.hpp"
#include "../collision/include/collisionsystembrute.hpp"
#include "../collision/include/collisionsystemgrid.hpp"

#endif

