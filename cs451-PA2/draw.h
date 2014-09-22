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


//-----------------------------------------------------------------------------
//this defines the size of the lattice
extern unsigned int lattice_nx, lattice_ny, lattice_nz;
extern vector<Point3d> FFD_lattice; //This stores all lattice nodes, FFD_lattice has size = (lattice_nx X lattice_ny X lattice_nz)
extern vector<double> FFD_parameterization; //This stores all parameterized coordinates of all vertices from the model
                                             //FFD_parameterization has size = models.front().v_size
Point3d *clickedNode = NULL;	//selected node from lattice
//-----------------------------------------------------------------------------

inline void DisplayLattice()
{
	glDisable(GL_LIGHTING); //disable lighting

	//DONE: draw lattice nodes using FFD_lattice
	glPointSize(4);
	glBegin(GL_POINTS);
	glColor3f(0.9, 0.0, 0.0);
	for (int i = 0; i < FFD_lattice.size(); i++)
	{
		glVertex3d(FFD_lattice[i][0], FFD_lattice[i][1], FFD_lattice[i][2]);
	}
	glEnd();

	//draw something around the selected node
	if (clickedNode != NULL)
	{
		glPointSize(15);
		glBegin(GL_POINTS);
		glColor3f(0.2, 0.8, 0.4);
		glVertex3d(clickedNode[0][0], clickedNode[0][1], clickedNode[0][2]);
		glEnd();
	}

	//DONE: draw lattice edges using FFD_lattice
	glBegin(GL_LINES);
	glColor3f(0.0, 0.3, 0.9);
	int count = 0, index = 0;
	for (int k = 0; k < lattice_nz; k++)
	{
		for (int j = 0; j < lattice_ny; j++)
		{
			for (int i = 0; i < lattice_nx; i++)
			{
				if (k != 0) //draw z-axis lines
				{
					glVertex3d(FFD_lattice[count][0], FFD_lattice[count][1], FFD_lattice[count][2]);
					index = count - lattice_ny * lattice_nx;
					glVertex3d(FFD_lattice[index][0], FFD_lattice[index][1], FFD_lattice[index][2]);
				}
				if (j != 0)	//draw vertical line
				{
					glVertex3d(FFD_lattice[count][0], FFD_lattice[count][1], FFD_lattice[count][2]);
					index = count - lattice_nx;
					glVertex3d(FFD_lattice[index][0], FFD_lattice[index][1], FFD_lattice[index][2]);
				}
				if (i != 0) //draw horizontal line
				{
					//count = i + lattice_nx * j;
					glVertex3d(FFD_lattice[count][0], FFD_lattice[count][1], FFD_lattice[count][2]);
					index = count - 1;
					glVertex3d(FFD_lattice[index][0], FFD_lattice[index][1], FFD_lattice[index][2]);
				}
				count++;
			}
		}
	}
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
        for(int k=0;k<3;k++)
		{
            const Point3d& pt=M.vertices[t.v[k]].p;

			//send pt to OpenGL
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
}

void Mouse(int button, int state, int x, int y)
{
	// This handles mouse click 
	// 
	// DONE: check if the user clicks on one of the lattice nodes
	//		TOFIX: overlapping vertices
	//

	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		//std::cout << "mouse x,y : " << x << ", " << y << std::endl;
		GLint viewport[4];			// Where The Viewport Values Will Be Stored
		GLdouble modelview[16];     // Where The 16 Doubles Of The Modelview Matrix Are To Be Stored
		GLdouble projection[16];    // Where The 16 Doubles Of The Projection Matrix Are To Be Stored
		GLdouble winX, winY, winZ;	// Holds the x,y,z coordinates in the window space
		GLdouble posX, posY, posZ;  // Hold The lattice coordinates

		glGetDoublev(GL_MODELVIEW_MATRIX, modelview);       // Retrieve The Modelview Matrix
		glGetDoublev(GL_PROJECTION_MATRIX, projection);     // Retrieve The Projection Matrix
		glGetIntegerv(GL_VIEWPORT, viewport);				// Retrieves The Viewport Values (X, Y, Width, Height)

		//look through lattice vertices and find one near the mouse coordinates
		for (int i = 0; i < FFD_lattice.size(); i++)
		{
			//project lattice point into screen space
			posX = FFD_lattice[i][0];
			posY = FFD_lattice[i][1];
			posZ = FFD_lattice[i][2];
			gluProject(posX, posY, posZ, 
				modelview, projection, viewport, &winX, &winY, &winZ); 
			winY = (double)viewport[3] - winY - 9; //seems to be 9 off? odd....

			//compare mouse values with lattice point projected on screen
			//does where the mouse clicks differ for OS's?
			double xDif = winX - x;
			double yDif = winY - y;
			if ( ( -8 < xDif) && (xDif < 8) ) //mouseX is near a vertex x value
			{
				if ( (-8 < yDif) && (yDif < 8) )	///mouseY is near a vertex y value
				{
					std::cout << "index: " << i << std::endl;
					clickedNode = &FFD_lattice[i];	//select node
					gli::selectedNode();	//disable mouse camera control
					return;
				}
			}
		} //end for

		clickedNode = NULL;		//deselect
		gli::deselectedNode();	//enable mouse camera control

	} //end if
}


void Motion(int x, int y)
{
	//
	// This handles mouse motion when a button is pressed 
	// 
	// DONE: check if the user is moving a selected lattice node
	//       if so move the node to a new location
	//
	if ( clickedNode == NULL ) {
		return;
	}

	GLint viewport[4];			// Where The Viewport Values Will Be Stored
	GLdouble modelview[16];     // Where The 16 Doubles Of The Modelview Matrix Are To Be Stored
	GLdouble projection[16];    // Where The 16 Doubles Of The Projection Matrix Are To Be Stored
	GLdouble winX, winY, winZ;	// Holds the x,y,z coordinates from mouse
	GLdouble objX, objY, objZ;  // Hold values for the new vertex location

	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);       // Retrieve The Modelview Matrix
	glGetDoublev(GL_PROJECTION_MATRIX, projection);     // Retrieve The Projection Matrix
	glGetIntegerv(GL_VIEWPORT, viewport);				// Retrieves The Viewport Values (X, Y, Width, Height)

	gluProject(clickedNode[0][0], clickedNode[0][1], clickedNode[0][2],
		modelview, projection, viewport, &winX, &winY, &winZ);	//running this function just to get the z val
																//sad code is sad
	//assign winX and winY to mouse coordinates
	winX = (double)x;
	winY = (double)viewport[3] - y - 9; //seems to be 9 off? Very Strange? 
	//glReadPixels( x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_DOUBLE, &winZ );

	//map the mouse click coordinates to object coordinates
	gluUnProject( winX, winY, winZ,
			modelview, projection, viewport, &objX, &objY, &objZ );

	//assign object coordinates to current node
	clickedNode[0][0] = objX;
	clickedNode[0][1] = objY;
	clickedNode[0][2] = objZ;


	// TODO: recompute the position of every vertex in the model 
	//       i.e., using FFD_parameterization and FFD_lattice 
	//       Note, you only need to do this to the first model in "models"
	//

	model& m = models.front();

	glutPostRedisplay();
}

void PassiveMotion(int x, int y)
{
	// This handles mouse motion when mouse button is NOT pressed
	// does nothing now...

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



