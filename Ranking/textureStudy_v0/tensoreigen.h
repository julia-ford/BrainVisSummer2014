#ifndef tensoreigen_h
#define tensoreigen_h

#include "vec3.h"

class TensorEigen{
public:
	float value[3];
	vec3  vectors[3];
	float fa;

	float GetCl() const {return (value[0]-value[1])/GetValueSum();};
	float GetCp() const	{return 2*(value[1]-value[2])/GetValueSum();};
	float GetCs() const {return 3*value[2]/GetValueSum();};
	float GetFA() const {return fa; };

	float ComputeFA() const {
		float d1 = value[0]-value[1];
		float d2 = value[1]-value[2];
		float d3 = value[2]-value[0];
		float sqardRt = 0.5f*(d1*d1+d2*d2+d3*d3)/(value[0]*value[0]+value[1]*value[1]+value[2]*value[2]);
		return sqrt(sqardRt);
	};
	float GetValueSum() const {return value[0]+value[1]+value[2];}

	void Modify(){
		for(int i = 0; i < 3; i++){
			if(value[i] < 0){
				  value[i] *= -1;
				vectors[i] *= -1;
			}
		}

		if(value[0]<value[1]) swap(0,1);
		if(value[1]<value[2]) swap(1,2);
		if(value[0]<value[1]) swap(0,1);
		fa = ComputeFA();
	}

	/// Forces the FA value to be discrete and range from 0.2 to 0.9.
	/// In other words, after this, the FA value will always be one of
	/// eight values: 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, or 0.9.
	// TODO: This seems very wrong! Why would you make FA discrete?
	void DiscreteFA(){
		if(fa >= 1) { fa = 0.99f; }
		else if(fa < 0.2) { fa = 0.2f; }
		int intFA = fa*10;
		fa = (float)intFA/10;
	}

private:
	void swap(int i,int j){
		float tmp = value[j];
		value[j]=value[i];
		value[i]=tmp;
		vec3 tmp3 = vectors[j];
		vectors[j]=vectors[i];
		vectors[i]=tmp3;
	}
};
#endif