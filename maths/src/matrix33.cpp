// JigLib - Copyright (C) 2004 Danny Chapman
#include "matrix33.hpp"

using namespace JigLib;

tMatrix33 tMatrix33::mIdentity = tMatrix33(1, 0, 0, 0, 1, 0, 0, 0, 1);

//==============================================================
// CalculateRotFromMatrix
//==============================================================
void CalculateRotFromMatrix(const tMatrix33 & matrix, 
                            tVector3 & axis, 
                            tScalar & degrees)
{
  tScalar factor = (Trace(matrix) - 1.0f) * 0.5f;
  if (factor > 1.0f)
    factor = 1.0f;
  else if (factor < -1.0f)
    factor = -1.0f;
  degrees = AcosDeg(factor);
  
  if (degrees == 0.0f)
  {
    axis = tVector3(1.0f, 0.0f, 0.0f);
    return;
  }
  else if (degrees == 180.0f)
  {
    if ( (matrix(0, 0) > matrix(1, 1)) && (matrix(0, 0) > matrix(2, 2)) )
    {
      axis[0] = 0.5f * Sqrt(matrix(0, 0) - matrix(1, 1) - matrix(2, 2) + 1.0f);
      axis[1] = matrix(0, 1) / (2.0f * axis[0]);
      axis[2] = matrix(0, 2) / (2.0f * axis[0]);
    }
    else if (matrix(1, 1) > matrix(2, 2))
    {
      axis[1] = 0.5f * Sqrt(matrix(1, 1) - matrix(0, 0) - matrix(2, 2) + 1.0f);
      axis[0] = matrix(0, 1) / (2.0f * axis[1]);
      axis[2] = matrix(1, 2) / (2.0f * axis[1]);
    }
    else
    {
      axis[2] = 0.5f * Sqrt(matrix(2, 2) - matrix(0, 0) - matrix(1, 1) + 1.0f);
      axis[0] = matrix(0, 2) / (2.0f * axis[2]);
      axis[2] = matrix(1, 2) / (2.0f * axis[2]);
    }
  }
  else
  {
    axis[0] = matrix(2, 1) - matrix(1, 2);
    axis[1] = matrix(0, 2) - matrix(2, 0);
    axis[2] = matrix(1, 0) - matrix(0, 1);
  }
  axis.Normalise();
}
