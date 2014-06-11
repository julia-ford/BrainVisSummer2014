#pragma once

#include "rgba.h"
#include <vector>
using namespace std;

class ColorScheme{
public:
	static float clamp(float v,float s,float e);
	static int clamp(int v,int s,int e);
};
class CoolWarmColorScheme
{
public:
	// value; 0~1
	static rgba GetColorContious(const float value);
	static rgba GetColorDiscrete(const float value, const int numDiscrete);

protected:
	static float coolWarmColor[][4];
};

class BoysSurfaceColorScheme
{
public:
	// read from file
	static void LoadFromFile(const char* filename);
	static rgba GetColor(int t, int s);
	
protected:
	static vector<rgba> boysColors;
};

class SaturationColorScheme
{
public:
	// read from file
	static rgba GetColorContious(float value);
	static rgba GetColorDiscrete(const float value, const int numDiscrete);
};

class FiberColorScheme{
public:
	static rgba GetHighlightColor();
	static rgba GetHaloColor();
};

class OrdinalColorScheme
{
public:
	// support at least 6
	// read from file
	static rgba GetColor(unsigned int value);
};
