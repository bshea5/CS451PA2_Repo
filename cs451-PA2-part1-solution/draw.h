//------------------------------------------------------------------------------
//  Copyright 2007-2014 by Jyh-Ming Lien and George Mason University
//  See the file "LICENSE" for more information
//------------------------------------------------------------------------------

#pragma once

#include <GL/gli.h>
#include <time.h>

#include <map>
using namespace std;

#include "Basic.h"
#include "model.h"
using namespace mathtool;

#include "UserSpring.h"

//-----------------------------------------------------------------------------
//variables used in rendering

bool showWire=false; //on/off wireframe
bool showSharpEdgeOnly=false;
bool randomColor=false;
bool background=true; //on/off background
bool light=true; //on/off background

//Store display IDs and model colors
map<model*,int> model_solid_gids;
map<model*,int> model_wire_gids;
map<model*,Vector3d> model_colors;

//this is for point selection...
CUserSpring user_spring;

//-----------------------------------------------------------------------------
//this defines the size of the lattice
extern unsigned int lattice_nx, lattice_ny, lattice_nz;
extern vector<Point3d> FFD_lattice; //This stores all lattice nodes, FFD_lattice has size = (lattice_nx X lattice_ny X lattice_nz)
extern vector<double> FFD_parameterization; //This stores all parameterized coordinates of all vertices from the model
//FFD_parameterization has size = models.front().v_size x (nx*ny*nz)


//-----------------------------------------------------------------------------

inline void DisplayLattice()
{
	glDisable(GL_LIGHTING);

	//draw nodes
	glPointSize(8);
	glBegin(GL_POINTS);
	glColor3d(1, 0, 0);
	int i = 0;
	for (int ix = 0; ix < lattice_nx; ix++)
	{
		for (int iy = 0; iy < lattice_ny; iy++)
		{
			for (int iz = 0; iz < lattice_nz; iz++)
			{
				glVertex3dv(FFD_lattice[i++].get());
			}//end for z
		}//end for y
	}//end for x
	glEnd();
	glPointSize(1);

	//draw edges
	glBegin(GL_LINES);
	glColor3d(0, 0, 1);
	for (int ix = 0; ix < lattice_nx; ix++)
	{
		for (int iy = 0; iy < lattice_ny; iy++)
		{
			for (int iz = 0; iz < lattice_nz; iz++)
			{
				//draw an edge along x-axis if (ix!=lattice_nx-1)
				if (ix != lattice_nx - 1)
				{
					glVertex3dv(FFD_lattice[ix*lattice_ny*lattice_nz + iy*lattice_nz+iz].get());
					glVertex3dv(FFD_lattice[(ix+1)*lattice_ny*lattice_nz + iy*lattice_nz + iz].get());
				}

				//draw an edge along y-axis if (iy!=lattice_ny-1)
				if (iy != lattice_ny - 1)
				{
					glVertex3dv(FFD_lattice[ix*lattice_ny*lattice_nz + iy*lattice_nz + iz].get());
					glVertex3dv(FFD_lattice[ix*lattice_ny*lattice_nz + (iy+1)*lattice_nz + iz].get());
				}

				//draw an edge along x-axis if (iz!=lattice_nz-1)
				if (iz != lattice_nz - 1)
				{
					glVertex3dv(FFD_lattice[ix*lattice_ny*lattice_nz + iy*lattice_nz + iz].get());
					glVertex3dv(FFD_lattice[ix*lattice_ny*lattice_nz + iy*lattice_nz + iz+1].get());
				}
			}//end for z
		}//end for y
	}//end for x

	glEnd();


}

inline void DisplayModel(model& M, bool randcolor=false)
{
	//draw
	if (randcolor){
		if (model_colors.find(&M) == model_colors.end())
			model_colors[&M] = Vector3d(drand48() + 0.5, drand48() + 0.5, drand48(), +0.5).normalize() + Vector3d(0.25, 0.25, 0.25);
		glColor3dv(model_colors[&M].get());
	}
	
	//Draw facets
	glEnable( GL_POLYGON_OFFSET_FILL );
	glPolygonOffset( 0.5f, 0.5f );
	//for(list<polygon>::iterator i=M.polys.begin();i!=M.polys.end();i++)
	glBegin(GL_TRIANGLES);
	for(unsigned int i=0;i<M.t_size;i++)
	{
        const triangle & t=M.tris[i];
        glNormal3dv(M.tris[i].n.get());
        for(int k=0;k<3;k++){
            const Point3d& pt=M.vertices[t.v[k]].p;
            glVertex3d(pt[0],pt[1],pt[2]);
        }
	}
	glEnd();
	glDisable( GL_POLYGON_OFFSET_FILL );
}

inline void DisplayModelWireFrame(model& M, bool randcolor=false)
{
    //Draw Edges
    if(showWire)
	{
		glBegin(GL_LINES);
        for(uint i=0;i<M.e_size;i++){
            glColor3f(0,0,0);
            const edge & e=M.edges[i];
            if(e.fid.size()==2){//normal case, check if e is sharp
                triangle& f1=M.tris[e.fid.front()];
                triangle& f2=M.tris[e.fid.back()];
                if(fabs(1-f1.n*f2.n)<1e-2){
                    if(showSharpEdgeOnly) continue; //not sharp
                    else
                        glColor3f(0.7f,0.7f,0.7f);
                }
            }

            Point3d& p1=M.vertices[e.vid[0]].p;
            Point3d& p2=M.vertices[e.vid[1]].p;
            glVertex3d(p1[0],p1[1],p1[2]);
            glVertex3d(p2[0],p2[1],p2[2]);
        }
        glEnd();
    }
}


//copied from meshlab
void DisplayBackground(void)
{
	float topcolor[]={1,1,1};
	float bottomcolor[]={1,1,0.5};
	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-1,1,-1,1,-1,1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_TRIANGLE_STRIP);
		glColor3fv(topcolor);  	glVertex2f(-1, 1);
		glColor3fv(bottomcolor);	glVertex2f(-1,-1);
		glColor3fv(topcolor);	glVertex2f( 1, 1);
		glColor3fv(bottomcolor);	glVertex2f( 1,-1);
	glEnd();
	
	glPopAttrib();
	glPopMatrix(); // restore modelview
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void drawAll()
{
    glEnable(GL_LIGHTING);

    //show the inputs
    glColor3f(1,1,1);
    if(light) glEnable(GL_LIGHTING);
    else glDisable(GL_LIGHTING);

    for(list<model>::iterator i=models.begin();i!=models.end();i++){
        DisplayModel(*i,randomColor);
    }
    for(list<model>::iterator i=models.begin();i!=models.end();i++){
        DisplayModelWireFrame(*i);
    }

	//draw user selection...
	user_spring.doDraw();

	//draw lattice
	DisplayLattice();
}

//-----------------------------------------------------------------------------
void Display( void )
{
    //Init Draw
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    if(background) DisplayBackground();
    
    
    glPushMatrix();
    glLoadIdentity();
    static GLfloat light_position1[] = {  100, 100, 100.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position1);
    static GLfloat light_position2[] = { -100, -100, 50.0f, 1.0f };
    glLightfv(GL_LIGHT1, GL_POSITION, light_position2);
    glPopMatrix();

    drawAll();

    glDisable(GL_LIGHTING);
}


//-----------------------------------------------------------------------------
// regular openGL callback functions
bool InitGL()
{
    // transparent
    glShadeModel(GL_SMOOTH);
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glEnable( GL_LINE_SMOOTH );
    glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );

    // others
    glEnable( GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
    glClearColor(1.0, 1.0, 1.0, 0.0);

    //Let's have light!
    GLfloat Diffuse[] =  { 0.9f, 0.9f, 0.9f, 1.0f };
    glMaterialfv(GL_FRONT, GL_DIFFUSE, Diffuse);
    glColorMaterial(GL_FRONT, GL_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    GLfloat WhiteLight[] =  { 0.75f, 0.75f, 0.75f, 1.0f };
    glLightfv(GL_LIGHT0,GL_DIFFUSE,WhiteLight);
    glLightfv(GL_LIGHT1,GL_DIFFUSE,WhiteLight);

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

    return true;
}

void Reshape( int w, int h)
{
    glViewport( 0, 0, (GLsizei)w, (GLsizei)h );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

	//Perspective view
    gluPerspective( 60, 1.0f*w/h, R/100, R*100 );

	//Othogonal view
	//glOrtho(-R * 1.5, R * 1.5, -R * 1.5, R * 1.5, -R * 100, R * 100);


    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

	user_spring.ReComputeInfomation();
}

void Mouse(int button, int state, int x, int y)
{
	//mouse up
	if (state == GLUT_UP && button == GLUT_LEFT_BUTTON)
		user_spring.deleteSpring();
	if (glutGetModifiers() != GLUT_ACTIVE_CTRL) return;
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON)
		user_spring.buildSpring();
}


void Motion(int x, int y)
{
	//user_spring.ReComputeInfomation();

	if (user_spring.isSpringBuild() == false) return;
	if (user_spring.getPickID() < 0) return;

	//user_spring.ReComputeInfomation();
	user_spring.FindMousePosition(x, y);
	const Vector3d * new_pos = user_spring.getMousePos();
	if (new_pos == NULL) return;


	Vector3d trans = Point3d(new_pos->get()) - FFD_lattice[user_spring.getPickID()];
	FFD_lattice[user_spring.getPickID()] =
		user_spring.getPointArray()[user_spring.getPickID()] = *new_pos;

	/*
	float transf[3] = { trans[0], trans[1], trans[2] };
	dmvc->moveOneId(user_spring.getPickID(), transf);
	*/

	glutPostRedisplay();
}

void PassiveMotion(int x, int y)
{

	/////////////////////////////////////////////////////////
	//User Spring
	user_spring.ReComputeInfomation();
	user_spring.FindClosestPoint(x, y);
	user_spring.FindMousePosition(x, y);

	glutPostRedisplay();
}

//Used for simulation/anitmation. 
void TimerCallback(int value)
{
    //in simuation state
    glutPostRedisplay();
    glutTimerFunc(30, TimerCallback,value);
}

//Handle keyboard events
void Keyboard( unsigned char key, int x, int y )
{
    // find closest colorPt3D if ctrl is pressed...
    switch( key ){
        case 27: exit(0);
        case 'w' : showWire=!showWire; break;
        case 'r' : randomColor=!randomColor; break;
		case 'R' : model_colors.clear(); break;
		case 'L' : light=!light; break;
		case 'b' : background=!background; break;
		case 'S' : showSharpEdgeOnly=!showSharpEdgeOnly;
		           for(map<model*,int>::iterator i=model_wire_gids.begin();i!=model_wire_gids.end();i++) glDeleteLists(i->second,1);
		           model_wire_gids.clear();
		           break;
    }
    glutPostRedisplay();
}



