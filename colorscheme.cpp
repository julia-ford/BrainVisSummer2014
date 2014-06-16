#include <cassert>
#include <fstream>
#include "colorscheme.h"
using namespace std;

#define NUMCOLORS 33
#define MAXNUMORDINALCOLORS 6

float CoolWarmColorScheme::coolWarmColor[NUMCOLORS][4]={
0.00000f,0.229805700f,0.298717966f,0.753683153f,
0.03125f,0.266233880f,0.353094838f,0.801466763f,
0.06250f,0.303868910f,0.406535296f,0.844958670f,
0.09375f,0.342804478f,0.458757618f,0.883725899f,
0.12500f,0.383013340f,0.509419040f,0.917387822f,
0.15625f,0.424369608f,0.558148092f,0.945619588f,
0.18750f,0.466667080f,0.604562568f,0.968154911f,
0.21875f,0.509635204f,0.648280772f,0.984788140f,
0.25000f,0.552953156f,0.688929332f,0.995375608f,
0.28125f,0.596262162f,0.726149107f,0.999836203f,
0.31250f,0.639176211f,0.759599947f,0.998151185f,
0.34375f,0.681291281f,0.788964712f,0.990363227f,
0.37500f,0.722193294f,0.813952739f,0.976574709f,
0.40625f,0.761464949f,0.834302879f,0.956945269f,
0.43750f,0.798691636f,0.849786142f,0.931688648f,
0.46875f,0.833466556f,0.860207984f,0.901068838f,
0.50000f,0.865395197f,0.865410210f,0.865395561f,
0.53125f,0.897787179f,0.848937047f,0.820880546f,
0.56250f,0.924127593f,0.827384882f,0.774508472f,
0.59375f,0.944468518f,0.800927443f,0.726736146f,
0.62500f,0.958852946f,0.769767752f,0.678007945f,
0.65625f,0.967328030f,0.734132809f,0.628751763f,
0.68750f,0.969954137f,0.694266682f,0.579375448f,
0.71875f,0.966811177f,0.650421156f,0.530263762f,
0.75000f,0.958003065f,0.602842431f,0.481775914f,
0.78125f,0.943660866f,0.551750968f,0.434243684f,
0.81250f,0.923944917f,0.497308560f,0.387970225f,
0.84375f,0.899046170f,0.439559467f,0.343229596f,
0.87500f,0.869186849f,0.378313092f,0.300267182f,
0.90625f,0.834620542f,0.312874446f,0.259301199f,
0.93750f,0.795631745f,0.241283790f,0.220525627f,
0.96875f,0.752534934f,0.157246067f,0.184115123f,
1.00000f,0.705673158f,0.015556160f,0.150232812f};

vector<rgba> BoysSurfaceColorScheme::boysColors;

rgba ordinalColors[] = {
	rgba(1,0,0),
	rgba(1,1,0),
	rgba(0,1,0),
	rgba(0,1,1),
	rgba(0,0,1),
	rgba(1,0,1)
};

float ColorScheme::clamp(float v, float s, float e){
	if(v>e) return e;
	if(v<s) return s;
	return v;
}

int ColorScheme::clamp(int v, int s, int e){
	if(v>e) return e;
	if(v<s) return s;
	return v;
}

rgba CoolWarmColorScheme::GetColorContious(const float value){
	assert(value>=0 && value<=1);
	int idx = ColorScheme::clamp((NUMCOLORS-1)*value+0.5,0,NUMCOLORS-1);
	return rgba(coolWarmColor[idx][1],coolWarmColor[idx][2],coolWarmColor[idx][3],1);
}

rgba CoolWarmColorScheme::GetColorDiscrete(const float value, const int numDiscrete){
	assert(numDiscrete<NUMCOLORS/2);
	float scale = (NUMCOLORS-1)/(float)numDiscrete;
	int idx = value*numDiscrete+0.5;
	idx = ColorScheme::clamp(idx*scale+0.5,0,NUMCOLORS-1);
	return rgba(coolWarmColor[idx][1],coolWarmColor[idx][2],coolWarmColor[idx][3],1);
}

void BoysSurfaceColorScheme::LoadFromFile(const char* filename){
    ifstream input( filename );
	boysColors.clear();
	int n;
    input >> n;
	boysColors.reserve(n);
    for (int t = 0; t < n; ++t) {
		int ts;
        input >> ts;
        float x,y,z;
        input >> x >> y >> z;
		rgba color;
        input >> color.r >> color.g >> color.b ;
        input >> x;
        for (int s = 1; s < ts; ++s){
            input.ignore( 200, '\n');
        }
		boysColors.push_back(color);
    }
}

rgba BoysSurfaceColorScheme::GetColor(int t, int s){
	return boysColors[t];
}

rgba SaturationColorScheme::GetColorContious(float value){
	rgba c;
	float v = ColorScheme::clamp(value,0.f,1.f);
	c.fromHSV(10,v,1);
	return c;
}

rgba SaturationColorScheme::GetColorDiscrete(const float value, const int numDiscrete){
	rgba c;
	int t=value*numDiscrete;
	float tt = ColorScheme::clamp((float)t/numDiscrete,0.f,1.f);
	c.fromHSV(10,tt,1);
	return c;
}

rgba FiberColorScheme::GetHighlightColor(){
	return rgba(1,1,0);
}

rgba FiberColorScheme::GetHaloColor(){
	return rgba(0,0,0);
}

rgba OrdinalColorScheme::GetColor(unsigned int value){
	if(value >=  MAXNUMORDINALCOLORS) return ordinalColors[MAXNUMORDINALCOLORS-1];
	return ordinalColors[ColorScheme::clamp(value,0,MAXNUMORDINALCOLORS-1)];
}
