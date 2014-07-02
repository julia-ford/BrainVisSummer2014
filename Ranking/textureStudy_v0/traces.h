#ifndef TRACES_H
#define TRACES_H

#include "vec3.h"
#include "rgba.h"
#include <cmath>
#include <vector>
#include "tensoreigen.h"
using namespace std;

class Traces
{
public:
    Traces();
    ~Traces();

    void BuildFromColorFile(const char *filename);
    void BuildFromTensorFile(const char *filename);
	void SaveToObj(const char* filename) const;

	int GetNumFibers() const {return positions.size();};
	int GetNumSegs(int t) const {return positions[t].size();};
	vec3 GetPosition(int t, int s) const {return positions[t][s];};
	vec3 GetCenter() const {return (minPos+maxPos)/2;};
	vec3 GetCenter(const vector<int> &subparts) const;
	float GetAverageFA(const vec3& bottomLeft, const vec3& topRight, int sampleInterval = 1) const;
	void GetBoundingBox(vec3 &_minPos, vec3 &_maxPos){_minPos=minPos;_maxPos=maxPos;};
	rgba GetColor(int t,int s) const {return colors[t][s];};
	TensorEigen GetEigen(int t, int s) const {return eigens[t][s];};
	vec3 GetDirection(int t,int s) const{
		if(s==GetNumSegs(t)-1){
			return GetPosition(t,s)-GetPosition(t,s-1);
		}
		else return GetPosition(t,s+1)-GetPosition(t,s);
	}
	void SetZReverse(bool rez){revZ=rez;};
	const vector<vector<vec3>>& GetPositions() const{return positions;};
	const vector<vector<rgba>>& GetColors() const{return colors;};
	const vector<vector<TensorEigen>>& GetEigens() const{return eigens;};


protected:
    void bound(const vec3& p);
	void clear();
	void clearPositions();
	void clearEigens();
	void clearColors();

	vec3 minPos;
	vec3 maxPos;

	float minfa,maxfa;

	bool revZ;

    vector<vector<vec3>> positions;
    vector<vector<rgba>> colors;
	vector<vector<TensorEigen>> eigens;
};

#endif // TRACES_H
