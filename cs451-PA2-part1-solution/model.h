//------------------------------------------------------------------------------
//  Copyright 2007-2009 by Jyh-Ming Lien and George Mason University
//  See the file "LICENSE" for more information
//------------------------------------------------------------------------------

#ifndef _BF_MODEL_H_
#define _BF_MODEL_H_

#include <Point.h>
#include <Vector.h>
#include <Matrix.h>
#include <Quaternion.h>
using namespace mathtool;

#include <string>
#include <cassert>
#include <set>
using namespace std;

#include "objReader.h"

//shorten some notations
typedef unsigned int uint;

//a triangle of the model
struct triangle
{
    triangle(){facet_id=0;}

    uint v[3]; //vertex id
    uint e[3]; //edge id

    Vector3d n; //normal

    uint facet_id; //id of the corresponding facet in kway-union.h

    //backups
    Vector3d bk_n;
};

//a vertex of the model
struct vertex
{
    vertex(){ concave=false; }
    Point3d p;  //position
    list<uint> m_f;
    list<uint> m_e; //a list of edges

    //backups
    Point3d bk_p; 

    //if concave, set to true
    bool concave;
};

//an edge of the model
struct edge
{
    edge(){ type='x'; vid[0]=vid[1]=UINT_MAX; }
    uint vid[2];
    vector<uint> fid;

    Vector3d v;       //parallel vector
    Vector3d in_n[2]; //inface normals

    //backups
    Vector3d bk_v;       //parallel vector
    Vector3d bk_in_n[2]; //inface normals

    //type, c-convex, r-reflex, p-plane, b-border
    char type;
};

struct model
{
    //initialization
    model()
	{ 
        v_size=e_size=t_size=0;
        vertices=NULL;
        edges=NULL;
        tris=NULL; 

        for(int i=0;i<3;i++) for(int j=0;j<3;j++) current_rot[i][j]=0;
        current_rot[0][0]=current_rot[1][1]=current_rot[2][2]=1;
    }

    ~model(){}

    void destroy()
	{
        delete [] tris;     tris=NULL;
        delete [] edges;    edges=NULL;
        delete [] vertices; vertices=NULL;
        v_size=e_size=t_size=0;
    }

    //build this model
    bool build(const string & name);

    //rotate points
    void rotate(const Matrix2x2& m);
    void rotate(const Matrix3x3& M);

    //scale the model
    void scale(double s);

    //negate point/facets ...
    void negate();

    //reverse facets ...
    void reverse();
    
    //data
    vertex   * vertices;  //vertices
    triangle * tris;      //triangles
    edge     * edges;     //edges
    uint v_size, e_size, t_size;

    //current orientation
    double   current_rot[3][3];

};



#endif //_BF_MODEL_H_
