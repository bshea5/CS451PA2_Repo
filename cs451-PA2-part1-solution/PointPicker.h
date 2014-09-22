// PointPicker.h: interface for the CPointPicker class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Vector.h"
#include "GL/gli.h"
using namespace mathtool;

class CPointPicker  
{
public:
    //////////////////////////////////////////////////////////////////////
    //Constructors and Destructor
    CPointPicker();
    virtual ~CPointPicker();

    //////////////////////////////////////////////////////////////////////
    //Core
    void computeProjection( const Vector3d * pts, unsigned int size);
    Vector3d * unprojMousePos(int x, int y);
    void findCloest(int x, int y);

    //////////////////////////////////////////////////////////////////////
    //Core
    int getPickID() const { return m_PickedID; }
//////////////////////////////////////////////////////////////////////
//Private Data
private:
    unsigned int m_Size; //how many pt this class can handle
    const Vector3d * m_PointBuffer;
    Vector3d * m_ProjectedPt; 
    Vector3d m_MousePos;
    int m_PickedID;

    GLdouble m_ProjM[16];
    GLdouble m_ModelViewM[16];
    GLint m_ViewPortM[4];
};

