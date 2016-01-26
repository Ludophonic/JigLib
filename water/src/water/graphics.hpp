//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file graphics.hpp 
//                     
//==============================================================
#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include "jiglib.hpp"

#include <GL/glut.h>
#include <GL/glu.h>

// single quadric for everything... if it doesn't exist it will be
// created
inline GLUquadricObj * GetGLUQuadric()
{
  static GLUquadricObj * quadric = 0;
  if (quadric == 0)
  {
    quadric = gluNewQuadric();
//    gluQuadricNormals(quadric, GLU_FLAT);
    gluQuadricNormals(quadric, GLU_SMOOTH);
    gluQuadricDrawStyle(quadric, GLU_FILL);
  }
  Assert(quadric);
  return quadric;
}

// warning - expensive!
struct tSaveGLState
{
  tSaveGLState() {glPushMatrix(); glPushAttrib(GL_ALL_ATTRIB_BITS);}
  ~tSaveGLState() {glPopMatrix(); glPopAttrib();}
};

// cheap!
struct tSaveGLMatrixState
{
  tSaveGLMatrixState() {glPushMatrix();}
  ~tSaveGLMatrixState() {glPopMatrix();}
};

//==============================================================
// CheckGLErrors
//==============================================================
inline void CheckGLErrors(char * str="")
{
  GLenum err_code = glGetError();
  if (err_code != GL_NO_ERROR)
  {
    TRACE("OpenGL Error: %s [%s]\n", gluErrorString(err_code), str);
  }
}

#ifdef USING_DOUBLE

#define GLNORMAL3 glNormal3d
#define GLNORMAL4 glNormal4d

#define GLNORMAL2V glNormal2dv
#define GLNORMAL3V glNormal3dv
#define GLNORMAL4V glNormal4dv

#define GLVERTEX2 glVertex2d
#define GLVERTEX3 glVertex3d
#define GLVERTEX4 glVertex4d

#define GLVERTEX2V glVertex2dv
#define GLVERTEX3V glVertex3dv
#define GLVERTEX4V glVertex4dv

#define GLTEXCOORD1 glTexCoord1d
#define GLTEXCOORD2 glTexCoord2d
#define GLTEXCOORD3 glTexCoord3d

#define GLCOLOR2 glColor2d
#define GLCOLOR3 glColor3d
#define GLCOLOR4 glColor4d

#define GLTRANSLATE glTranslated
#define GLROTATE glRotated

#define GLMULTMATRIX glMultMatrixd

#else

#define GLNORMAL2 glNormal2f
#define GLNORMAL3 glNormal3f
#define GLNORMAL4 glNormal4f

#define GLNORMAL2V glNormal2fv
#define GLNORMAL3V glNormal3fv
#define GLNORMAL4V glNormal4fv

#define GLVERTEX2 glVertex2f
#define GLVERTEX3 glVertex3f
#define GLVERTEX4 glVertex4f

#define GLVERTEX2V glVertex2fv
#define GLVERTEX3V glVertex3fv
#define GLVERTEX4V glVertex4fv

#define GLTEXCOORD1 glTexCoord1f
#define GLTEXCOORD2 glTexCoord2f
#define GLTEXCOORD3 glTexCoord3f

#define GLCOLOR2 glColor2f
#define GLCOLOR3 glColor3f
#define GLCOLOR4 glColor4f

#define GLTRANSLATE glTranslatef
#define GLROTATE glRotatef

#define GLMULTMATRIX glMultMatrixf

#endif

//==============================================================
// apply_transformation
//==============================================================
inline void ApplyTransformation(const JigLib::tVector3 & translation, 
                                const JigLib::tMatrix33 & rotation)
{
  // translation's easy...
  GLTRANSLATE(translation[0], translation[1], translation[2]);
  
  JigLib::tScalar matrix[] = 
    {
      rotation(0, 0), // 1st column
      rotation(1, 0),
      rotation(2, 0),
      0,
      rotation(0, 1), // 2nd column
      rotation(1, 1),
      rotation(2, 1),
      0,
      rotation(0, 2), // 3rd column
      rotation(1, 2),
      rotation(2, 2),
      0,
      0, 0, 0, 1        // 4th column
    };
  
  GLMULTMATRIX(&matrix[0]);
}

#endif
