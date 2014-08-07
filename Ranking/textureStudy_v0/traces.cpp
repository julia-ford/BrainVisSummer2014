
#include <iostream>
#include "traces.h"
#include <fstream>
#include <string>
#include <sstream>
#include <cassert>
using namespace std;

#define COLORCHANNEL 256
#define MAXCOORD 9999
#define MINCOORD -9999

//#define DEBUG_LESS_TRACES

inline string convertInt(int number)
{
	stringstream ss;//create a stringstream
	ss << number;//add number to the stream
	return ss.str();//return a string with the contents of the stream
};

inline string convertFloat(float number)
{
	stringstream ss;//create a stringstream
	ss << number;//add number to the stream
	return ss.str();//return a string with the contents of the stream
};

Traces::Traces()
{
	// TODO: minPos and maxPos seem to be reversed.
	minPos.set(MAXCOORD,MAXCOORD,MAXCOORD);
	maxPos.set(MINCOORD,MINCOORD,MINCOORD);
	revZ = true;
}

Traces::~Traces()
{
	clear();
}

/// Used by training mode. Always indirectly called by main().
void Traces::BuildFromColorFile( const char *filename )
{
    ifstream input( filename);
    cerr << "file name: " << filename << endl;
	int numTrace;
    input >> numTrace;
	cerr << "num of traces: " << numTrace << endl;

	clearPositions();
	clearColors();
    positions.resize(numTrace);
    colors.resize(numTrace);
    for (int t = 0; t < numTrace; ++t){
		int numSegs;
        input >> numSegs;
        positions[t].resize(numSegs);
        colors[t].resize(numSegs);
        for (int s = 0; s < numSegs; ++s){
            vec3 p;
            input >> p.x >> p.y >> p.z;
            positions[t][s] = p;
            bound(p);
            rgba c;
            input >> c.r >> c.g >> c.b ;
            colors[t][s] = c;
            input.ignore( 200, '\n');
        }
		if(t%50 == 0)cerr << "\rTrace: Loading traces: " << (float)t/(numTrace-1)*100 <<"%";
    }
	cerr << "\rTrace: Loading traces: " << "100%      " << endl;
	input.close();
}

/// Used by non-training mode. Always indirectly called by main().
void Traces::BuildFromTensorFile( const char *filename )
{
    ifstream input( filename);
    cerr << "file name: " << filename << endl;
	int numTrace;
    input >> numTrace;
	cerr << "num of traces: " << numTrace << endl;

	clearPositions();
	clearColors();
	clearEigens();
    positions.resize(numTrace);
    colors.resize(numTrace);
    eigens.resize(numTrace);
	minfa = 100;
	maxfa = -1;
    for (int t = 0; t < numTrace; ++t){
		int numSegs;
        input >> numSegs;
        positions[t].resize(numSegs);
           colors[t].resize(numSegs);
           eigens[t].resize(numSegs);
        for (int s = 0; s < numSegs; ++s){
            vec3 p;
            input >> p.x >> p.y >> p.z;
			if(revZ)p.z = -p.z;
            positions[t][s] = p;
            bound(p);
			TensorEigen& te = eigens[t][s];
			input >> te.fa
				>> te.value[0] >> te.value[1] >> te.value[2]
				>> te.vectors[0].x >> te.vectors[0].y >> te.vectors[0].z
				>> te.vectors[1].x >> te.vectors[1].y >> te.vectors[1].z
				>> te.vectors[2].x >> te.vectors[2].y >> te.vectors[2].z;
			te.Modify();
			// TODO: other line to comment out
			//te.DiscreteFA();
			assert(fabs(te.value[0])>=fabs(te.value[1]) && fabs(te.value[1]) >=fabs(te.value[2]));
			if(te.fa>maxfa) maxfa = te.fa;
			if(te.fa<minfa) minfa = te.fa;
            input.ignore( 200, '\n');
        }
		if(t%50 == 0)cerr << "\rTrace: Loading traces: " << (float)t/(numTrace-1)*100 <<"%";
    }
	cerr << "\rTrace: Loading traces: " << "100%      " << endl;
	input.close();
}

void Traces::SaveToObj( const char* filename ) const
{
	// build material lib
	const char* mtllibName = "materialByFa.mtl";
	ofstream mtllibfile;
	mtllibfile.open(mtllibName);
	if (!mtllibfile.is_open()){
		cout<<"file "<<mtllibName<<" open failed!\n";
		return;
	}

	string mtlnameprefix("materialByFA_");
	string newmtl("newmtl");
	for (int i = 0;i<COLORCHANNEL+1;i++){
		float fa = (float)i/COLORCHANNEL;
		string FAstr = convertInt(i);
		string mtlname = mtlnameprefix+FAstr;
		mtllibfile << newmtl << ' ' << mtlname <<endl;
		mtllibfile << "Ka " << fa << ' '<< fa << ' ' << fa << endl;
		mtllibfile << "Kd " << fa << ' '<< fa << ' ' << fa << endl;
		mtllibfile << "Ks " << fa << ' '<< fa << ' ' << fa << endl;
		mtllibfile << "Ns 10.000 "<< endl;
		mtllibfile << endl;
	}
	mtllibfile << endl << "#end of file\n";
	mtllibfile.close();

	// write obj file
	ofstream objfile;
	objfile.open(filename);
	if (!objfile.is_open()){
		cout<<"file "<<filename<<" open failed!\n";
		return;
	}

	objfile<< "mtllib "<<mtllibName<<endl;
	objfile <<endl;

	int traceNum = positions.size();
	
	// write vertices
	for (int i = 0;i<traceNum;i++){
		int numSeg = positions[i].size();
		objfile << "#trace " << i <<" has "
			<< numSeg << " vertics" << endl;
		for (int j = 0;j<numSeg;j++){
			objfile<< "v "<< positions[i][j].x<<' '
				<< positions[i][j].y<<' '
				<< positions[i][j].z<<endl;
		}
	}
	
	int currentIdx = 1;
	int currentFA = -100;
	string usemtl("usemtl ");
	// write lines
	for (int i = 0;i<traceNum;i++){
		int numSeg = positions[i].size();
		objfile << endl << "#trace " << i <<" has "
			<< numSeg << " vertics" << endl;
		for (int j = 0;j<numSeg-1;j++){
			int fa = (colors[i][j].g + colors[i][j+1].g)* COLORCHANNEL/2 + 0.5;
			if (fa == currentFA){
				objfile<<' '<<currentIdx + j + 1;
				currentFA = fa;
			}
			else{
				string FAstr = convertInt(fa);
				string mtlname = mtlnameprefix+FAstr;
				objfile << endl << usemtl << mtlname << endl;
				objfile << "l "<< currentIdx+j<< ' '<<currentIdx+j+1;
			}
		}
		currentIdx += numSeg;
	}
	objfile << endl << "#end of file\n";
	objfile.close();
}

vec3 Traces::GetCenter(const vector<int> &subparts) const{
	vec3 tmpMin(MAXCOORD,MAXCOORD,MAXCOORD);
	vec3 tmpMax(MINCOORD,MINCOORD,MINCOORD);
	if (subparts.empty()){
		return vec3(0,0,0);
	}
	for(unsigned int i = 0;i<subparts.size();i++){
		int t = subparts[i];
		int numSegs = positions[t].size();
		for(unsigned int s = 0;s<numSegs;s++){
			tmpMin.boundAsMin(positions[t][s]);
			tmpMax.boundAsMax(positions[t][s]);
		}
	}
	return (tmpMin+tmpMax)/2;
}

float Traces::GetAverageFA(const vec3& bottomLeft, const vec3& topRight, int sampleInterval) const{
	float faSum = 0;
	int numSamples = 0;

	// brutal force
	for(int t = 0;t<positions.size();t++){
		for(int s = 0;s<positions[t].size();s+=sampleInterval){
			if(bottomLeft <= positions[t][s] && positions[t][s] <= topRight){
				numSamples ++;
				faSum += eigens[t][s].GetFA();
			}
		}
	}

	if(numSamples == 0) return 0;
	return faSum/numSamples;
}

void Traces::bound( const vec3& p )
{
	minPos.boundAsMin(p);
	maxPos.boundAsMax(p);
}

void Traces::clear(){
	minPos.set(MAXCOORD,MAXCOORD,MAXCOORD);
	maxPos.set(MINCOORD,MINCOORD,MINCOORD);
	clearPositions();
	clearEigens();
	clearColors();
}


void Traces::clearPositions(){
	if(!positions.empty()){
		for(unsigned int i = 0;i<positions.size();i++){
			positions[i].clear();
		}
		positions.clear();
	}
}
void Traces::clearEigens(){
	if(!eigens.empty()){
		for(unsigned int i = 0;i<eigens.size();i++){
			eigens[i].clear();
		}
		eigens.clear();
	}
}
void Traces::clearColors(){
	if(!colors.empty()){
		for(unsigned int i = 0;i<colors.size();i++){
			colors[i].clear();
		}
		colors.clear();
	}
}