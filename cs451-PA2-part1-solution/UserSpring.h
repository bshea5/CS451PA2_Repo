// UserSpring.h: interface for the CUserSpring class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include "PointPicker.h"
#include "model.h"

class CUserSpring  
{
public:
    //////////////////////////////////////////////////////////////////////
    CUserSpring();
    virtual ~CUserSpring();

    //////////////////////////////////////////////////////////////////////
    //Core
	bool doInit(vector<Point3d>& m);
    void doDraw();
    void ReComputeInfomation();
    void FindMousePosition(int x, int y);
    void FindClosestPoint(int x, int y);

    //////////////////////////////////////////////////////////////////////
    //Access
    void buildSpring(){ 
        m_bSpringBuild=true; 
    }
    bool isSpringBuild(){ return m_bSpringBuild; }
    void deleteSpring(){ m_bSpringBuild=false; m_RestLength=-1; }

    const Vector3d * getMousePos(){ return m_MousePos; }
    const Vector3d * getPickPos(){ return &m_ParticlePos[m_Picker.getPickID()]; }
    int getPickID(){ return m_Picker.getPickID(); }

    //following mehtod is not valide when no spring is build
    double getRestLength() const { return m_RestLength; }
    double getCurrentLength() const { return m_CurrentLength; }

	Vector3d * getPointArray(){ return m_ParticlePos; }

private:

    CPointPicker m_Picker; 

    Vector3d * m_ParticlePos; //this is an array
	int m_ParticlePos_size;   //this is the size of m_ParticlePos_size
    Vector3d * m_MousePos;

    const double m_MAX_SpringDist;

    bool m_bSpringBuild;      // Spring is build by user
    double m_RestLength; //spring rest length
    double m_CurrentLength; //spring's current length
	float m_marker_size;
};

