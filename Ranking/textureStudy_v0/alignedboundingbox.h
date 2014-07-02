#pragma once

#include "rgba.h"
#include "int4.h"
#include "vec3.h"
#include <string>
#include <vector>
using namespace std;

class AlignedBoundingBox
{
public:
	AlignedBoundingBox(void);
	AlignedBoundingBox(string filename);
	~AlignedBoundingBox(void);

	bool isPointInside(const vec3& point);
	void buildFromFile(string filename);
	void build(const vector<float>& points);
	void Render();

	vec3 lowerEnd;
	vec3 higherEnd;

	vec3 vertices[8];
	rgba colors[8];
	int4 faces[6];
	vec3 normals[6];
	int edges[12][2];
};

