#ifndef SuperQuadric_H
#define SuperQuadric_H

#include "vec3.h"
#include "rgba.h"
#include <vector>
using namespace std;

class SuperQuadric
{
public:

	SuperQuadric();
	~SuperQuadric();

	void Update();
	void Draw() const;
	void DrawAtOrigin() const;
	void AppendShape(vector<float> &vertices, vector<float> &texCoords, vector<float> &normals) const;
	// return vertives appended
	void AppendShape(vector<float> &vertices, vector<float> &texCoords, vector<float> &normals, vector<unsigned int> &indices, int &indexOffset) const;
	void Render();

	// won't cause redraw
	void SetColor(const rgba& newColor);
	
	// cause redraw
    void SetAlpha(float a);
	void SetBeta(float b);
	void SetGama(float g);
	void SetDirection(const vec3& newDirection);
	void SetPosition(const vec3& newPosition);
	void SetSize(const vec3& newSize);
	void SetDetail(const float newDetailTangent, const float newDetailAxial=10);
	
	//tmp sol
	void SetFAForTexureCoord(const float newFA);

	void Build();

	float GetGama() const{return gama;};
	vec3 GetPosition() const{return position;};

	vector<float> m_normals;
	vector<float> m_texCoords;
	vector<float> m_vertices;

protected:
	void drawGeometry() const;
    void buildDisplayList();
    void drawCanonicalGeometry() const;
	void changed();

    int gl_displayList;
	bool m_needRebuild;

    int m_detailTangent;
	int m_detailAxial;

	//tmp sol
	float fa;

    float alpha;
    float beta;
    float gama;
    vec3 position;
    vec3 direction;
    rgba color;
	vec3 size;


private:
    vec3 getOneVertexOnCanonicalSurface(double _theta1, double _theta2) const;

};

#endif // SuperQuadric_H
