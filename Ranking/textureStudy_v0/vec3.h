#ifndef vec3_h
#define vec3_h

#include <cmath>

class vec3{
public:
    vec3(float _x,float _y,float _z):x(_x),y(_y),z(_z){};
    vec3() {x = y = z = 0.0;};

    friend vec3 operator+(const vec3 &a,const vec3 &b){
        vec3 t(a.x+b.x,a.y+b.y,a.z+b.z);
        return t;
    };

    friend vec3 operator-(const vec3 &a,const vec3 &b){
        return vec3(a.x-b.x,a.y-b.y,a.z-b.z);
    }

    friend float operator*(const vec3 &a,const vec3 &b){
        return a.dotMultiply(b);
    }

    friend vec3 operator^(const vec3 &a,const vec3 &b){
        return a.crossMultiply(b);
    }

    friend vec3 operator*(const vec3 &a,const float b){
        return vec3(a.x*b,a.y*b,a.z*b);
    }

    friend vec3 operator*(const float b, const vec3 &a){
        return vec3(a.x*b,a.y*b,a.z*b);
    }

    friend vec3 operator/(const vec3 &a,const float n){
        return vec3(a.x/n,a.y/n,a.z/n);
    }

    friend bool operator==(const vec3 &a, const vec3 &b){
        if(a.x != b.x) return false;
        if(a.y != b.y) return false;
        if(a.z != b.z) return false;
        return true;
    }
	
    friend bool operator>=(const vec3 &a, const vec3 &b){
        if(a.x < b.x) return false;
        if(a.y < b.y) return false;
        if(a.z < b.z) return false;
        return true;
    }
	
    friend bool operator<=(const vec3 &a, const vec3 &b){
        if(a.x > b.x) return false;
        if(a.y > b.y) return false;
        if(a.z > b.z) return false;
        return true;
    }

    static bool isIntersected(const vec3 &a1, const vec3 &b1, const vec3 &a2, const vec3 &b2){
        vec3 t1 = (b1-a1).crossMultiply(a2-a1);
        vec3 t2 = (b1-a1).crossMultiply(b2-a1);
        if (t1*t2 >= 0) return false;

        vec3 t3 = (b2-a2).crossMultiply(a1-a2);
                vec3 t4 = (b2-a2).crossMultiply(b1-a2);
                if (t3*t4 >= 0) return false;

        return true;
    }

    static bool isParallel(const vec3 &a, const vec3 &b){
        return a.crossMultiply(b) == vec3(0.0,0.0,0.0);
    }

	void set(float _x, float _y, float _z){
		x = _x;
		y = _y;
		z = _z;
	}

	void boundAsMin(const vec3 &p){
		if(x>p.x) x = p.x;
		if(y>p.y) y = p.y;
		if(z>p.z) z = p.z;
	}
	
	void boundAsMax(const vec3 &p){
		if(x<p.x) x = p.x;
		if(y<p.y) y = p.y;
		if(z<p.z) z = p.z;
	}

	void fromSphereCoord(float longi, float lati, float r = 1.f){
		x=cos(longi)*cos(lati)*r;
        y=sin(longi)*cos(lati)*r;
        z=sin(lati)*r;
	}

    vec3 operator-() const{
        return vec3(-x,-y,-z);
    }

    void operator+=(const vec3 &other){
        x+=other.x;
        y+=other.y;
        z+=other.z;
    };
	    
	void operator-=(const vec3 &other){
        x-=other.x;
        y-=other.y;
        z-=other.z;
    };

    void operator*=(const float n){
        x*=n;
        y*=n;
        z*=n;
    }

    void operator/=(const float n){
        x/=n;
        y/=n;
        z/=n;
    }

    vec3 operator=(const vec3 &other){
                x=other.x;
                y=other.y;
                z=other.z;
        return *this;
        };

    float dotMultiply(const vec3 &other) const{
        return x*other.x+y*other.y+z*other.z;
    }

    vec3 crossMultiply(const vec3 &other) const{
        vec3 t;
        t.x = y*other.z-z*other.y;
        t.y = -(x*other.z-z*other.x);
        t.z = x*other.y-y*other.x;
        return t;
    }

    void scale(const vec3 &_size){
        x *= _size.x;
        y *= _size.y;
        z *= _size.z;
    }

    void scale(const float _x, const float _y, const float _z){
        x *= _x;
        y *= _y;
        z *= _z;
    }

    void normalize(){
        float n = norm();
        if(n != 0) (*this)/=n;
    }

    vec3 normalized() const{
        float n = norm();
        if(n != 0) return (*this)/n;
        return (*this);
    }

    float norm() const{
        return sqrt(squared());
    }

    float squared() const{
        return pow(x,2)+pow(y,2)+pow(z,2);
    }

public:
    float x;
    float y;
    float z;
};

#endif //vec3_h
