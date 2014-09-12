//------------------------------------------------------------------------------
//  Copyright 2007-2014 by Jyh-Ming Lien and George Mason University
//  See the file "LICENSE" for more information
//------------------------------------------------------------------------------


#pragma once

#include "Matrix.h"
#include "Vector.h"

namespace mathtool{

    class Quaternion {
    public:
        Quaternion(){ m_s=1; m_v.set(0,0,0); }
        Quaternion( double s, Vector3d v ){ m_v=v; m_s=s; }
        Quaternion( const Quaternion & q){ *this=q; }
        Quaternion( double r[3]){  //compute rotational quaternion
            //convert rot to quaternion
            double sin_r=(double)sin(r[0]/2);
            double cos_r=(double)cos(r[0]/2);
            Quaternion qx(cos_r,sin_r*Vector3d(1,0,0));
            sin_r=(double)sin(r[1]/2);
            cos_r=(double)cos(r[1]/2);
            Quaternion qy(cos_r,sin_r*Vector3d(0,1,0));
            sin_r=(double)sin(r[2]/2);
            cos_r=(double)cos(r[2]/2);
            Quaternion qz(cos_r,sin_r*Vector3d(0,0,1));
            *this=(qz*qy*qx).normalize();
        }

        ////////////////////////////////////////////////////////////////////////
        // Operations for Quaternion
        Quaternion operator*(const Quaternion & q) const {
            double s=q.m_s*m_s-q.m_v*m_v;
            Vector3d v=q.m_v*m_s+m_v*q.m_s+m_v%q.m_v;
            return Quaternion(s,v);
        }
        Quaternion operator*(const Vector3d & v) const { return *this*Quaternion(0,v); }
        Quaternion operator/(double s) const { return Quaternion(m_s/s,m_v/s); }
        Quaternion & operator=(const Quaternion & q){ set(q.m_s,q.m_v); return *this; }
        Quaternion operator+(const Quaternion & q) const { return Quaternion(m_s+q.m_s,m_v+q.m_v); }
        Quaternion operator-(const Quaternion & q) const { return Quaternion(m_s-q.m_s,m_v-q.m_v); }
        Quaternion operator-() const { return Quaternion(m_s,-m_v); }
        friend Quaternion operator*(const Vector3d & v, const Quaternion & q);
        friend istream& operator>>(istream & in, Quaternion & q );
        friend ostream& operator<<(ostream & out, const Quaternion & q );

        //////////////////////////////////////////////////////////////////////////
        //Normalization
        Quaternion normalize(){ 
            Quaternion q(*this);
            double l=q.norm();
            q=q/l;
            return q;
        }

        double norm(){ return sqrt(normsqr()); }
        double normsqr(){ return m_v.normsqr()+sqr(m_s); }

        //////////////////////////////////////////////////////////////////////////
        //Access

        Matrix3x3 getMatrix(){
            double x_2=2*sqr(m_v[0]); double y_2=2*sqr(m_v[1]); double z_2=2*sqr(m_v[2]);
            double xy=2*m_v[0]*m_v[1]; double yz=2*m_v[1]*m_v[2]; double zx=2*m_v[2]*m_v[0]; 
            double sx=2*m_s*m_v[0]; double sy=2*m_s*m_v[1]; double sz=2*m_s*m_v[2]; 
            return Matrix3x3(1-y_2-z_2, xy-sz, zx+sy,
                             xy+sz, 1-x_2-z_2, yz-sx,
                             zx-sy, yz+sx, 1-x_2-y_2);
        }
    

        void set(double s,const Vector3d & v){ m_v=v; m_s=s; }
        void set(double q1, double q2, double q3, double q4){ m_s=q1; m_v.set(q2,q3,q4); }
        const Vector3d& getComplex() const { return m_v; }
        double getReal() const { return m_s; }

    private:
        Vector3d m_v;
        double m_s;
    };

}//emd of nprmlib


