//------------------------------------------------------------------------------
//  Copyright 2007-2014 by Jyh-Ming Lien and George Mason University
//  See the file "LICENSE" for more information
//------------------------------------------------------------------------------

#include "model.h"
#include "ModelGraph.h"

#ifdef WIN32
#pragma warning(disable:4244)
#endif

//
// build the model from a file
// the file will contain an obj model
// once obj file is read, vertices and facets will be built
// other information associated with facets and vertices,
// as well as edges will be build also using model graph
//

bool model::build(const string & name)
{
    //if(pts!=NULL) return false; //built

    {//build/copy model
        objReader reader(name);
        if( !reader.Read() )
            return false;
        objModel& data=reader.getModel();

        //allocate memory
        v_size=data.pts.size();
        t_size=data.polys.size();
        vertices=new vertex[v_size];   //
        tris=new triangle[t_size];     //
        assert(vertices&&tris);        //make sure enough memory

        //copy vertices
        for(uint i=0;i<v_size;i++){
            vertices[i].p.set(&data.pts[i].x);
            vertices[i].bk_p=vertices[i].p;  //backup for modification
        }

        //copy triangles
        int tid=0;
        for(list<polygon>::iterator i=data.polys.begin();i!=data.polys.end();i++){
            list<int>& ids=i->pts;
            //check if triangle
            if(ids.size()!=3){
                cerr<<"! Error: polygon "<<tid<<" is not a triangle."<<endl;
                return false;
            }
            int vid=0;
            for(list<int>::iterator j=ids.begin();j!=ids.end();j++){
                tris[tid].v[vid++]=*j;
                vertices[*j].m_f.push_back(tid);
            }
            tid++;
        }
    }//end build/copy model


    {//build model graph and collect informations
        CModelGraph G;
        G.doInit(this);

        //create edges
        e_size=G.getEdgeSize();
        CModelEdge * ptrE=G.getEdges();
        edges=new edge[e_size];
        assert(edges);
        for(uint i=0;i<e_size;i++,ptrE=ptrE->getNext()){
            int v1=edges[i].vid[0]=ptrE->getStartPt();
            int v2=edges[i].vid[1]=ptrE->getEndPt();

            const vector<int>&tmp_fid=ptrE->getFacets();
            edges[i].fid.insert(edges[i].fid.end(),tmp_fid.begin(),tmp_fid.end());

			if( tmp_fid.size()<2 ){ //check if boundary edge
				edges[i].fid.push_back(tmp_fid.back());
				edges[i].type='b'; //bd
			}

            //compute parallel vector
            edges[i].v=edges[i].bk_v=ptrE->getV();
            
			//compute edge normals
			edges[i].in_n[0]=ptrE->getInNormal(0); 

			if (edges[i].type == 'b')
				edges[i].in_n[1] = edges[i].in_n[0];
			else
				edges[i].in_n[1] = ptrE->getInNormal(1); 
			
			//backup edge normals
			edges[i].bk_in_n[0] = edges[i].in_n[0];
			edges[i].bk_in_n[1] = edges[i].in_n[1];
			
            vertices[v1].m_e.push_back(i);
            vertices[v2].m_e.push_back(i);
        }//end i
        
        //facets
        vector<CModelFacet>& facets=G.getFacets();
        for(uint i=0;i<t_size;i++)
		{
            tris[i].n=tris[i].bk_n=facets[i].n;
            for(int j=0;j<3;j++){
                tris[i].e[j]=facets[i].m_Edge[j]->getID();
            }//end j
        }//end i

        //edge type
        ptrE=G.getEdges();
        for(uint i=0;i<e_size;i++,ptrE=ptrE->getNext())
		{
			if(edges[i].type=='b') continue; //already know
            Vector3d& n1=tris[edges[i].fid[0]].n;
            Vector3d& n2=tris[edges[i].fid[1]].n;
			//
			double d=n1*n2;
            if(fabs(1-d)<SMALLNUMBER) 
				edges[i].type='p'; //plane
            else{
                Vector3d vec=(n1%n2).normalize();
                if(vec*edges[i].v>0) edges[i].type='c'; //convex
                else edges[i].type='r'; //reflex
            }
        }

		//vertex type
		typedef list<uint>::iterator IT;
		for(uint i=0;i<v_size;i++)
		{
			vertex& v=vertices[i];

			Vector3d evec;
			for(IT ie=v.m_e.begin();ie!=v.m_e.end();ie++){
				edge& e=edges[*ie];
				Vector3d vec=e.v;
				if(e.vid[1]==i) vec=-vec;
				evec=evec+vec;
			}//end ir

			Vector3d fvec;
			for(IT f=v.m_f.begin();f!=v.m_f.end();f++){
				triangle& t=tris[*f];
				fvec=fvec+t.n;
			}//end ir

			if(evec*fvec>0) v.concave=true;
		}//end i
    }

    return true;
}

//
//
//
// Transformation operations
// 
// Rotation
// Negation
// Reverse (turn inside out)
//


void model::rotate(const Matrix2x2& M)
{
    Vector2d tmp;

    //rotate vertices
    for(uint i=0;i<v_size;i++){
        tmp.set(vertices[i].bk_p[0],vertices[i].bk_p[1]);
        tmp=M*tmp;
        vertices[i].p.set(tmp[0],tmp[1]);
    }
    
    //rotate edges
    for(uint i=0;i<e_size;i++){
        for(int j=0;j<2;j++){
            tmp.set(edges[i].bk_in_n[j][0],edges[i].bk_in_n[j][1]);
            tmp=M*tmp;
            edges[i].in_n[j].set(tmp[0],tmp[1]);
        }
        
        tmp.set(edges[i].v[0],edges[i].v[1]);
        tmp=M*tmp;
        edges[i].v.set(tmp[0],tmp[1]);
    }

    //rotate facets
    for(uint i=0;i<t_size;i++){
        tmp.set(tris[i].n[0],tris[i].n[1]);
        tmp=M*tmp;
        tris[i].n.set(tmp[0],tmp[1]);
    }
}




void model::rotate(const Matrix3x3& M)
{
    Vector3d tmp;

    //rotate vertices
    for(uint i=0;i<v_size;i++){
        tmp.set(vertices[i].bk_p.get());
        vertices[i].p=M*tmp;
    }
    
    //rotate edges
    for(uint i=0;i<e_size;i++){
        edges[i].in_n[0]=(M*edges[i].bk_in_n[0]).normalize();
        edges[i].in_n[1]=(M*edges[i].bk_in_n[1]).normalize();
        edges[i].v=(M*edges[i].bk_v).normalize();
    }
    //rotate facets
    for(uint i=0;i<t_size;i++)
        tris[i].n=(M*tris[i].bk_n).normalize();
}


void model::scale(double s)
{
    //rotate vertices
	Point3d tmp;
    for(uint i=0;i<v_size;i++){
        tmp.set(vertices[i].bk_p.get());
        vertices[i].p.set(tmp[0]*s,tmp[1]*s,tmp[2]*s);
		vertices[i].bk_p.set(tmp[0]*s,tmp[1]*s,tmp[2]*s);
    }
}

void model::negate()
{

    for(uint i=0;i<v_size;i++){
        Point3d& pt=vertices[i].p;
        pt.set(-pt[0],-pt[1],-pt[2]);

        Point3d& pt2=vertices[i].bk_p;
        pt2.set(-pt2[0],-pt2[1],-pt2[2]);
    }

    for(uint i=0;i<t_size;i++){
        tris[i].n=-tris[i].n;
        tris[i].bk_n=-tris[i].bk_n;
		swap(tris[i].v[1],tris[i].v[2]);
		//swap(tris[i].e[1],tris[i].e[2]);
    }

    for(uint i=0;i<e_size;i++){
        edge& e=edges[i];
        e.v=-e.v;
        e.in_n[0]=-e.in_n[0];
        e.in_n[1]=-e.in_n[1];
        e.bk_v=-e.bk_v;
        e.bk_in_n[0]=-e.bk_in_n[0];
        e.bk_in_n[1]=-e.bk_in_n[1];
    }
}


void model::reverse()
{
    for(uint i=0;i<t_size;i++){
        tris[i].n=-tris[i].n;
        tris[i].bk_n=-tris[i].bk_n;
		swap(tris[i].v[1],tris[i].v[2]);
    }

    for(uint i=0;i<e_size;i++){
        edge& e=edges[i];
		if(e.type=='r') e.type='c';
		else if(e.type=='c') e.type='r';
    }
}

