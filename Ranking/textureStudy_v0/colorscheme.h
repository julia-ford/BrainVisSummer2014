#pragma once

#include <vector>
#include "rgba.h"
using namespace std;

#define NUMCOLORS /*33*/37

class ColorScheme{
public:
	static float clamp(float v,float s,float e);
	static int   clamp(int v,int s,int e);
};

class CoolWarmColorScheme
{
public:
	// value; 0~1
	static rgba GetColorContinuous(const float value);
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
	static rgba GetColorContinuous(float value);
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
