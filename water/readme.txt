This is a modified implementation of the Smoothed Particle
Hydrodynamics method described by Muller, Charypar and Gross
("Particle-based fluid simulation for interactive applications"
Eurographics/SIGGRAPH 2003).

Many parameters can be tweaked in the configuration file water.cfg. If 
it runs slowly try (in this order)

1. reducing nRenderSamplesX and nRenderSamplesY
2. setting renderBlobs to true
3. increasing the timestep to 0.02 (i.e. 50Hz)
4. reducing the number of particles

Clicking and dragging with the left/right mouse buttons move the
container/box. Clicking with the middle button generates a "fountain".
"wasd" push the fluid.

There are three batch files (they just select a different config file 
that you can try that are set up to work on low, medium and high-powered 
machines. If your processor supports SSE2 then you can try the opt .exe
(it runs about 25/50% faster... just from setting a compilation flag :) - 
just drag n drop a config file onto the .exe.

The basic idea is to represent the fluid of interest (i.e. the liquid,
not the air) as a collection of particles. Each particle represents
the fluid in its immediate vicinity, and interacts with a subset of
the other particles using a kernel function to weight the
interaction. By making the kernel function have a limited range the
algorithm complexity can be made O(NM) (where M is a constant related
to the kernel size), rather than O(N^2).

One problem with this algorithm is that the fluid cannot be completely
incompressible - hence the slightly springy behaviour - since the
pressure forces, which support the liquid, are derived from the
density so the density needs to vary within the liquid. Unfortunately
the state equation relating pressure to density, required for nice
fluid behaviour, doesn't provide a "real" pressure so when it's used
in the buoyancy force calculation it needs an artifical adjustment -
my workarounds make it look OK most of the time, but I don't like
them!

Another problem is that in an attempt to reduce the compressibility of
the fluid as much as possible, the stability of the system is pushed
to the limit. Consequently, changing a parameter such as the number of
particles generally requires retuning all the other parameters!

I used a verlet/particle integration scheme for both the fluid and the
buoyant object - this is probably an ideal situation for such a scheme
because of the simple collision situations.

Rendering is done in two ways (depending on the configuration file):

(1) each particle is rendered with a constant radius (for debugging),

(2) a grid is set up and the density evaluated at each point on the
grid. The values are then scaled and rendered as shaded quads.

The second method looks nice, but is very expensive in terms of CPU.

My opinion is that for writing a 2D demo of fluid inside a container
containing a buoyant object, grid based methods, if you can get them
to work(!) are probably superior, though much harder to
implement. However, in the general (game) case I believe that particle
methods have a lot of promise - for one thing they guarantee
conservation of mass/volume, and also processing is naturally
concentrated where the fluid exists. There are solutions for all the
issues outlined above, though I believe it would take some work to get
a scheme that is really stable, realistic and flexible enough to be of
use in games. The advantages over grid-based methods are that it is
easy to let the fluid flow wherever it likes, without needing to
support massive 3D arrays that would be needed in a grid-based
simulation. Also bear in mind that one of the major visual artifacts - 
holes being generated in the body of the fluid - is only really an issue
in 2D. A 3D implementation wouldn't really "suffer" from this because they 
would be obscured by the surface rendering.

- Danny Chapman

Dec 2004
