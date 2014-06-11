#pragma once
#include "rgba.h"
#include "vec3.h"
#include <vector>
#include <string>
#include "colortotexturemap.h"
using namespace std;

//===========================================================================//
// This file contains class definitions for all of the legends used in the   //
// various trials. The implementations for the functions are in legend.cpp.  //
//===========================================================================//

class BallLegend
{
public:
	BallLegend();
	~BallLegend();

	void SetColorTextureMap( const ColorToTextureMap* maps){textureMaps=maps;}
	void Render();
	void SetDrawColorBall(bool d){drawColorBall=d;};
	void SetDrawTextureBall(bool d){drawTextureBall=d;};
	void SetDrawSaturationBall(bool d){drawSaturationBall=d;};

	void LoadFromFile(const char* filename);
	rgba GetColorByDirection(const vec3& dir) const;
	void DrawAxes();
	void BuildDisplayList();

protected:
	void deSetDrawBall() {
		drawColorBall = false;
		drawTextureBall = false;
		drawSaturationBall = false;
	}
	void destory();

	bool drawColorBall;
	bool drawTextureBall;
	bool drawSaturationBall;

	rgba** colors;
	vec3** points;

	int numLati;
	int numLongi;
	int gl_DisplayList;
	int gl_displayListTex;
	int gl_displayListSaturation;

	const ColorToTextureMap*  textureMaps;
};

class BarLegend
{
public:
	BarLegend();
	~BarLegend();
	void Render();
	void SetDiscrete(bool d){m_discrete = d;};
	void SetColors(const vector<rgba> & newColors);
	void SetTitle(const string& title){m_title=title;};
	void SetRange(float start, float end){
		m_startValue = start;
		m_endValue = end;
	}
	
protected:
	bool m_discrete;
	vector<rgba> m_colors;
	string m_title;
	float m_startValue;
	float m_endValue;
};

class TextureLegend{
public:
	void Render(int type=1);

	void SetTexture(int tex[8]){
		for(int i = 0;i<8;i++){
			m_textureName[i] = tex[i];
		}
	};
	void SetRange(float start, float end){
		m_startValue = start;
		m_endValue = end;
	}
	void SetTitle(const string& title){m_title=title;};

protected:
	string m_title;
	int m_textureName[8];
	float m_startValue;
	float m_endValue;
};
