// UserSpring.cpp: implementation of the CUserSpring class.
//
//////////////////////////////////////////////////////////////////////

#include "UserSpring.h"
#include <algorithm>    // std::max

//-----------------------------------------------------------------------------
//this defines the size of the lattice
extern float BoundingBox[6];
//-----------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUserSpring::CUserSpring():m_MAX_SpringDist(1e5)
{
    m_bSpringBuild=false;
    m_RestLength=-1;
    m_CurrentLength=0;
    m_MousePos=NULL;
    m_ParticlePos=NULL;
	m_marker_size = 1;
}

CUserSpring::~CUserSpring()
{
    m_MousePos=NULL;
	delete [] m_ParticlePos;
    m_ParticlePos=NULL;
}

//////////////////////////////////////////////////////////////////////
// Core Functions
//////////////////////////////////////////////////////////////////////
bool CUserSpring::doInit(vector<Point3d>& m)
{
	m_ParticlePos_size = m.size();
	m_ParticlePos = new Vector3d[m_ParticlePos_size];
	assert(m_ParticlePos);

	for (int i = 0; i < m.size(); i++)
	{
		m_ParticlePos[i] = Vector3d(m[i].get());
	}

	float envBox[6];
	Point3d extend((BoundingBox[1] - BoundingBox[0]) / 2, (BoundingBox[3] - BoundingBox[2]) / 2, (BoundingBox[5] - BoundingBox[4]) / 2);
	m_marker_size = max(max(extend[0], extend[1]), extend[2])/25;

    return true;
}

void CUserSpring::doDraw()
{
	if (m_Picker.getPickID() < 0) return;

	if (m_ParticlePos == NULL) return;

	if (isSpringBuild())
		glColor4d(0.95,0,0,0.5);
	else
		glColor4d(0.25, 0.25, 0.25, 0.5);
    const Vector3d & pickedPos=m_ParticlePos[m_Picker.getPickID()];


    glPushMatrix();
    glTranslated(pickedPos[0],pickedPos[1],pickedPos[2]);
	glutSolidSphere(m_marker_size, 15, 15);
    glPopMatrix();

	if (m_MousePos == NULL) return;

    glPushMatrix();
    glTranslated( (*m_MousePos)[0],(*m_MousePos)[1],(*m_MousePos)[2] );
	glutSolidSphere(m_marker_size, 15, 15);
    glPopMatrix();


    glBegin(GL_LINES);
    glVertex3d(pickedPos[0],pickedPos[1],pickedPos[2]);
	glVertex3d((*m_MousePos)[0], (*m_MousePos)[1], (*m_MousePos)[2]);
    glEnd();
}

void CUserSpring::ReComputeInfomation()
{
	/*
    m_ParticlePos=getParticleSystem().getVertices();
    int size=getParticleSystem().getVertexSize();
	*/

	//
	if (m_ParticlePos == NULL) return;

	//
    m_Picker.computeProjection(m_ParticlePos,m_ParticlePos_size);
}

void CUserSpring::FindMousePosition(int x, int y)
{
    //do unprojection
    m_MousePos=m_Picker.unprojMousePos(x,y);
    if( m_MousePos==NULL ) return;

    //get picked id
    int id=m_Picker.getPickID();
    const Vector3d & pickedPos=m_ParticlePos[id];

    //check collision
	/*
    Vector3d n; double t;
    if( m_Env->getCollisionInfo(pickedPos, *m_MousePos, n, t) )
	{
        *m_MousePos=(1-t)*pickedPos+t*(*m_MousePos);
    }
	*/

    //check distance
    m_CurrentLength=(*m_MousePos-pickedPos).norm();
    if( m_bSpringBuild==true ){
        if( m_RestLength==-1 ) m_RestLength=m_CurrentLength;
    }
    else if(m_CurrentLength>m_MAX_SpringDist )
        m_MousePos=NULL;
}

void CUserSpring::FindClosestPoint(int x, int y)
{
    m_Picker.findCloest(x,y);
}

