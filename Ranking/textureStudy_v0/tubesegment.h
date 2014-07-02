#ifndef TubeSegment_H
#define TubeSegment_H

#include "vec3.h"
#include "rgba.h"
#include <vector>
#include "int4.h"
using namespace std;


class TubeSegment
{
public:
    TubeSegment();
	~TubeSegment();

    void DrawGeometry();
	void DrawHere(vector<float> &vertices, vector<float> &texCoords,
		vector<unsigned char> &colors, vector<float> &normals,
		vector<float> &capvertices, vector<float> &captexCoords,
		vector<unsigned char> &capcolors, vector<float> &capnormals) const;

	void DrawHalo();
	void RenderTubeSegment();
	void RenderHalo();

	void SetSegments(const float numSegs);
	void SetDirections(const vec3& d1, const vec3& d2);
	void SetColors(const rgba& color1, const rgba& color2);
	void SetSizes(const float size1, const float size2);
	void ScaleSizes(const float scale1, const float scale2);
	void SetPositions(const vec3& p1, const vec3& p2);
	void SetU(const float startU,const float endU);
	void SetCaps(const bool capHead, const bool capTail);
	void UpdateTubeSegment();
	void UpdateHalo();

	// geometry writer
	void WriteGeometry(vector<vec3>& vertices,vector<vec3>& normals,vector<rgba>& colors, vector<int4>& meshes);

protected:

	bool bCapHead;
	bool bCapTail;
	int gl_displayList;
	int gl_displayList_halo;

    vec3 pa,pb;
    vec3 da,db;
    vec3 direction;
	float ua,ub;
    float size_a,size_b;
    rgba color_a,color_b;
    int segments;

	bool m_needRebuildTubeSegment;
	bool m_needRebuildHalo;
	
	void buildTubeSegmentDisplayList();
	void buildHaloDisplayList();
	void changed();
	static void setGLContext(int _mode); //1 normal; 2 halo

private:
    void rotation_matrix(const vec3& axis, const float angle, vec3 matrix_row[3]) const;
};

#endif // TubeSegment_H
