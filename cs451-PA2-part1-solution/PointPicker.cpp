// PointPicker.cpp: implementation of the CPointPicker class.
//
//////////////////////////////////////////////////////////////////////

#include "PointPicker.h"
#include "GL/gli.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPointPicker::CPointPicker()
{
    m_ProjectedPt=NULL;
    m_PointBuffer=NULL;
    m_Size=0;
    m_PickedID=-1;
    memset(m_ProjM,0,sizeof(GLdouble)*16);
}

CPointPicker::~CPointPicker()
{
    delete [] m_ProjectedPt;
}

//////////////////////////////////////////////////////////////////////
// Core
//////////////////////////////////////////////////////////////////////
void CPointPicker::computeProjection( const Vector3d * pts, unsigned int size){

    if( pts==NULL ) return;
    //check if size is big enough
    if( m_Size!=size ){ delete m_ProjectedPt; m_Size=size; }
    if( (m_ProjectedPt=new Vector3d[size])==NULL ) return;
    m_PointBuffer=pts;

    //Get matrix info
    glGetIntegerv( GL_VIEWPORT, m_ViewPortM );
    glGetDoublev( GL_MODELVIEW_MATRIX, m_ModelViewM );
    glGetDoublev( GL_PROJECTION_MATRIX, m_ProjM );

    //find projection
    for( unsigned int iP=0;iP<size;iP++ ){
        gluProject(pts[iP][0], pts[iP][1], pts[iP][2], 
                   m_ModelViewM, m_ProjM, m_ViewPortM, 
                   &m_ProjectedPt[iP][0],&m_ProjectedPt[iP][1],&m_ProjectedPt[iP][2]);
    }
}

Vector3d * CPointPicker::unprojMousePos(int x, int y){

    if( m_PickedID==-1 || m_PointBuffer==NULL) return NULL; //pick a point first

	//Get matrix info
	//glGetIntegerv(GL_VIEWPORT, m_ViewPortM);
	//glGetDoublev(GL_MODELVIEW_MATRIX, m_ModelViewM);
	//glGetDoublev(GL_PROJECTION_MATRIX, m_ProjM);

    double real_y = m_ViewPortM[3]-y;
    Vector3d s,e; //start and end of ray

    /* unproject to plane defined by current x and y direction*/
    gluUnProject((GLdouble)x, (GLdouble)real_y, 0.0,m_ModelViewM, 
                 m_ProjM, m_ViewPortM, &s[0], &s[1], &s[2]);
    gluUnProject((GLdouble)x, (GLdouble)real_y, 1.0,m_ModelViewM, 
                 m_ProjM, m_ViewPortM, &e[0], &e[1], &e[2]);
    
    const float * tmp_nx=gli::getWindowX();
	const float * tmp_ny = gli::getWindowY();
    Vector3d nx(tmp_nx[0],tmp_nx[1],tmp_nx[2]);
    Vector3d ny(tmp_ny[0],tmp_ny[1],tmp_ny[2]);
    Vector3d n=nx%ny;
    double t=(m_PointBuffer[m_PickedID]*n-s*n)/((e-s)*n);

	m_MousePos = s*(1 - t) + t*e;

    return &m_MousePos;
}

void CPointPicker::findCloest(int x, int y)
{
    m_PickedID=-1;
    if( m_ProjectedPt==NULL ) return;
    double real_y = m_ViewPortM[3]-y;
    double dist=1e10; double z_buffer=1e10;

    for( unsigned int i=0;i<m_Size;i++ ){
        double wx=m_ProjectedPt[i][0]; 
        double wy=m_ProjectedPt[i][1]; 
        double wz=m_ProjectedPt[i][2];
        double d= (wx-x)*(wx-x)+(wy-real_y)*(wy-real_y);
        if( (d<dist) || (abs(d-dist)<0.00001 && wz<z_buffer) ){
            dist=d; z_buffer=wz;
            m_PickedID=i;
        }
    }
}
