//------------------------------------------------------------------------------
//  Copyright 2007-2014 by Jyh-Ming Lien and George Mason University
//  See the file "LICENSE" for more information
//------------------------------------------------------------------------------


#include "gli.h"
#include <iostream>
#include <math.h>

using namespace std;

GLfloat gli::m_CameraPos[3] = {0,0,100};
GLfloat gli::m_deltaDis[3]  = {0,0,0};

bool gli::m_DisalbeMouseControl=false;

GLfloat gli::m_currentAzim = 0;
GLfloat gli::m_deltaAzim   = 0;
GLfloat gli::m_currentElev = 0; 
GLfloat gli::m_deltaElev   = 0;

int gli::m_StartX        =0; 
int gli::m_StartY        =0; 
int gli::m_PressedButton =0;

gli::GLI_DISPLAY_FUNC  gli::m_dislpayFunc  = NULL;
gli::GLI_MOUSE_FUNC    gli::m_mouseFunc    = NULL;
gli::GLI_MOTION_FUNC   gli::m_motionFunc   = NULL;

float gli::m_WindowX[3]={1,0,0};
float gli::m_WindowY[3]={0,1,0};

void gli::gliInit()
{
    glutDisplayFunc( gliDisplay );
    glutMouseFunc( gliMouse );
    glutMotionFunc( gliMotion );
}

void gli::gliDisplay( void )
{
    //Draw scene
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    if( !m_DisalbeMouseControl ){ //mouse control is enabled
        glTranslatef( (m_CameraPos[0]+m_deltaDis[0]),
                     -(m_CameraPos[1]+m_deltaDis[1]), 
                     -(m_CameraPos[2]+m_deltaDis[2]) );
        glRotated(m_currentElev+m_deltaElev, 1.0, 0.0, 0.0);
        glRotated(m_currentAzim+m_deltaAzim, 0.0, 1.0, 0.0);
    }

    if( m_dislpayFunc!=NULL ){
        m_dislpayFunc();
    }

    //draw reference axis
    glMatrixMode(GL_PROJECTION); //change to Ortho view
    glPushMatrix(); 
    glLoadIdentity();
    gluOrtho2D(0,20,0,20);

    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glLoadIdentity();

    glTranslatef(1.1f,1,0);
    glRotated(m_currentElev+m_deltaElev, 1.0, 0.0, 0.0);
    glRotated(m_currentAzim+m_deltaAzim, 0.0, 1.0, 0.0);
    glBegin(GL_LINES);
    glColor3f(1,0,0);
    glVertex3f(0,0,0);
    glVertex3f(1,0,0);
    glColor3f(0,1,0);
    glVertex3f(0,0,0);
    glVertex3f(0,1,0);
    glColor3f(0,0,1);
    glVertex3f(0,0,0);
    glVertex3f(0,0,1);
    glEnd();
    glPopMatrix();
    
    //pop GL_PROJECTION
    glMatrixMode(GL_PROJECTION); //change to Pers view
    glPopMatrix();

    glutSwapBuffers();
}

void gli::
gliMouse( int button, int state, int x, int y ){

    if( !m_DisalbeMouseControl ){
        if( glutGetModifiers()!=GLUT_ACTIVE_CTRL ){
            if( state == GLUT_UP ) //reset every thing
            {
                for( int iD=0;iD<3;iD++ ){
                    m_CameraPos[iD] += m_deltaDis[iD];
                    m_deltaDis[iD]=0;
                }

                m_currentElev += m_deltaElev;
                m_currentAzim += m_deltaAzim;

                m_deltaElev = 0.0;
                m_deltaAzim = 0.0;
            }
            else { //press down

                m_StartY = y;
                m_StartX = x;
                m_StartY = y;

                m_PressedButton = button;
            }
        }
        else m_PressedButton=-1;
    }

    //call it when alt is not pressed
    if( m_mouseFunc!=NULL )
        m_mouseFunc(button, state, x, y);

    //redraw any way
    glutPostRedisplay();
}

void gli::
gliMotion( int x, int y ){

    if( !m_DisalbeMouseControl ){

        if( m_PressedButton == GLUT_RIGHT_BUTTON )
        {
			m_deltaDis[2] = (GLfloat)((m_CameraPos[2]>10) ? m_CameraPos[2] : 10) * ((GLfloat)(y - m_StartY)) / 100.0f;
        }
        else if( m_PressedButton == GLUT_MIDDLE_BUTTON ) {
			m_deltaDis[0] = (GLfloat)((m_CameraPos[0]>5) ? m_CameraPos[0] : 5) * ((GLfloat)(x - m_StartX)) / 20.0f;
			m_deltaDis[1] = (GLfloat)((m_CameraPos[1]>5) ? m_CameraPos[1] : 5) * ((GLfloat)(y - m_StartY)) / 20.0f;
          }
        else if(m_PressedButton == GLUT_LEFT_BUTTON)
        {
            m_deltaAzim = ((GLfloat)(x - m_StartX))/5.0f;
            m_deltaElev = ((GLfloat)(y - m_StartY))/5.0f;
            //compute window x, y dir
            m_WindowX[0]=1; m_WindowX[1]=0; m_WindowX[2]=0;
            m_WindowY[0]=0; m_WindowY[1]=1; m_WindowY[2]=0;
            gliRotateX(m_WindowX, m_currentElev+m_deltaElev);
            gliRotateY(m_WindowX, m_currentAzim+m_deltaAzim);
            m_WindowX[2]=-m_WindowX[2];
            gliRotateX(m_WindowY, m_currentElev+m_deltaElev);
            gliRotateY(m_WindowY, m_currentAzim+m_deltaAzim);   
            m_WindowY[2]=-m_WindowY[2];
        }
    }

    if( m_motionFunc!=NULL )
        m_motionFunc(x, y);

    //redraw any way
    glutPostRedisplay();
}

void gli::gliRotateX(float v[3], float degree){
    float c=(float)cos(3.1415926f*degree/180);
	float s=(float)sin(3.1415926f*degree/180);
    float v1=v[1]*c-v[2]*s;
    float v2=v[1]*s+v[2]*c;
    v[1]=v1; v[2]=v2;
}

void gli::gliRotateY(float v[3], float degree){
	float c = (float)cos(3.1415926f*degree / 180);
	float s = (float)sin(3.1415926f*degree / 180);
    float v0=v[0]*c+v[2]*s;
    float v2=-v[0]*s+v[2]*c;
    v[0]=v0; v[2]=v2;
}

