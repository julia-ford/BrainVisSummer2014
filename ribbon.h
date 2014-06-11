#pragma once

#include "vec3.h"
#include "rgba.h"
#include <vector>
using namespace std;

class Ribbon
{
public:
	Ribbon(void);
	~Ribbon(void);

	void SetPositions(const vector<vec3>* positions);
	void SetColors(const vector<rgba>* colors);
	void SetSizes(const vector<float>* sizes);
	void SetUs(const vector<float>* us);

	void SetHaloColor(const rgba& haloColor){m_haloColor = haloColor;};
	void SetDrawhalo(bool d){m_drawHalo = d;};

	void Reset();

	void DrawGeometry(const vec3& normal, const vector<int> &lefts) const;
	void DrawGeometry(const vector<vec3>& normals, const vector<int> &lefts) const;

protected:
	const vector<vec3>* m_positions;
	const vector<rgba>* m_colors;
	const vector<float>* m_sizes;

	rgba m_haloColor;
	bool m_drawHalo;

	// textureCoords
	// for FA maps
	const vector<float>* m_us;

	vec3 getDirection(int idx) const;
};

