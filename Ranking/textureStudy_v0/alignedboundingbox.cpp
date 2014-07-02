#include "alignedboundingbox.h"
#include <fstream>
#include "myGL.h"
using namespace std;

AlignedBoundingBox::AlignedBoundingBox(void)
{
}

AlignedBoundingBox::AlignedBoundingBox(string filename)
{
	buildFromFile(filename);
}

AlignedBoundingBox::~AlignedBoundingBox(void)
{
}

bool AlignedBoundingBox::isPointInside(const vec3& point){
	if(point.x < lowerEnd.x) return false;
	if(point.y < lowerEnd.y) return false;
	if(point.z < lowerEnd.z) return false;
	if(point.x > higherEnd.x) return false;
	if(point.y > higherEnd.y) return false;
	if(point.z > higherEnd.z) return false;
	return true;
}
  
void AlignedBoundingBox::buildFromFile(string filename){
	ifstream infile;
	infile.open(filename.c_str());
	// ignore the first line
	infile.ignore(100,'\n');
	infile >> lowerEnd.x >> lowerEnd.y >> lowerEnd.z;

	infile.ignore(100,'\n');
	infile >> higherEnd.x >> higherEnd.y >> higherEnd.z;
	infile.close();

	vertices[0] = vec3(higherEnd.x, lowerEnd.y, lowerEnd.z);
	vertices[1] = vec3(higherEnd.x,higherEnd.y, lowerEnd.z);
	vertices[2] = vec3(higherEnd.x,higherEnd.y, higherEnd.z);
	vertices[3] = vec3(higherEnd.x, lowerEnd.y, higherEnd.z);
	vertices[4] = vec3(lowerEnd.x, lowerEnd.y,lowerEnd.z);
	vertices[5] = vec3(lowerEnd.x,higherEnd.y,lowerEnd.z);
	vertices[6] = vec3(lowerEnd.x,higherEnd.y,higherEnd.z);
	vertices[7] = vec3(lowerEnd.x, lowerEnd.y,higherEnd.z);

	/*
	vertices[0] = vec3(lowerEnd.x, lowerEnd.y, lowerEnd.z);
	vertices[1] = vec3(higherEnd.x,lowerEnd.y, lowerEnd.z);
	vertices[2] = vec3(higherEnd.x,lowerEnd.y, higherEnd.z);
	vertices[3] = vec3(lowerEnd.x, lowerEnd.y, higherEnd.z);
	vertices[4] = vec3(lowerEnd.x, higherEnd.y,lowerEnd.z);
	vertices[5] = vec3(higherEnd.x,higherEnd.y,lowerEnd.z);
	vertices[6] = vec3(higherEnd.x,higherEnd.y,higherEnd.z);
	vertices[7] = vec3(lowerEnd.x, higherEnd.y,higherEnd.z);
	*/

	for(int i = 0;i<8;i++){
		colors[i] = rgba(1,1,0,1);
	}

	// front
	faces[0] = int4(0,1,2,3);
	// back
	faces[1] = int4(4,7,6,5);
	// left
	faces[2] = int4(0,3,7,4);
	// right
	faces[3] = int4(5,6,2,1);
	// top
	faces[4] = int4(2,6,7,3);
	// bottom
	faces[5] = int4(0,4,5,1);

	for(int i = 0;i<6;i++){
		vec3& a = vertices[faces[i].x];
		vec3& b = vertices[faces[i].y];
		vec3& c = vertices[faces[i].z];
		normals[i] = ((b-a)^(c-a)).normalized();
	}

	edges[0][0] = 0;  edges[0][1] = 1;
	edges[1][0] = 1;  edges[1][1] = 2;
	edges[2][0] = 2;  edges[2][1] = 3;
	edges[3][0] = 3;  edges[3][1] = 0;
	edges[4][0] = 4;  edges[4][1] = 5;
	edges[5][0] = 5;  edges[5][1] = 6;
	edges[6][0] = 6;  edges[6][1] = 7;
	edges[7][0] = 7;  edges[7][1] = 4;
	edges[8][0] = 0;  edges[8][1] = 4;
	edges[9][0] = 1;  edges[9][1] = 5;
	edges[10][0] = 2; edges[10][1] = 6;
	edges[11][0] = 3; edges[11][1] = 7;
}

void AlignedBoundingBox::build(const vector<float>& points){
	lowerEnd.set(points[0],points[1],points[2]);
	higherEnd.set(points[3],points[4],points[5]);

	vertices[0] = vec3(higherEnd.x, lowerEnd.y, lowerEnd.z);
	vertices[1] = vec3(higherEnd.x,higherEnd.y, lowerEnd.z);
	vertices[2] = vec3(higherEnd.x,higherEnd.y, higherEnd.z);
	vertices[3] = vec3(higherEnd.x, lowerEnd.y, higherEnd.z);
	vertices[4] = vec3(lowerEnd.x, lowerEnd.y,lowerEnd.z);
	vertices[5] = vec3(lowerEnd.x,higherEnd.y,lowerEnd.z);
	vertices[6] = vec3(lowerEnd.x,higherEnd.y,higherEnd.z);
	vertices[7] = vec3(lowerEnd.x, lowerEnd.y,higherEnd.z);

	/*
	vertices[0] = vec3(lowerEnd.x, lowerEnd.y, lowerEnd.z);
	vertices[1] = vec3(higherEnd.x,lowerEnd.y, lowerEnd.z);
	vertices[2] = vec3(higherEnd.x,lowerEnd.y, higherEnd.z);
	vertices[3] = vec3(lowerEnd.x, lowerEnd.y, higherEnd.z);
	vertices[4] = vec3(lowerEnd.x, higherEnd.y,lowerEnd.z);
	vertices[5] = vec3(higherEnd.x,higherEnd.y,lowerEnd.z);
	vertices[6] = vec3(higherEnd.x,higherEnd.y,higherEnd.z);
	vertices[7] = vec3(lowerEnd.x, higherEnd.y,higherEnd.z);
	*/

	for(int i = 0;i<8;i++){
		colors[i] = rgba(1,1,0,1);
	}

	// front
	faces[0] = int4(0,1,2,3);
	// back
	faces[1] = int4(4,7,6,5);
	// left
	faces[2] = int4(0,3,7,4);
	// right
	faces[3] = int4(5,6,2,1);
	// top
	faces[4] = int4(2,6,7,3);
	// bottom
	faces[5] = int4(0,4,5,1);

	for(int i = 0;i<6;i++){
		vec3& a = vertices[faces[i].x];
		vec3& b = vertices[faces[i].y];
		vec3& c = vertices[faces[i].z];
		normals[i] = ((b-a)^(c-a)).normalized();
	}

	edges[0][0] = 0;  edges[0][1] = 1;
	edges[1][0] = 1;  edges[1][1] = 2;
	edges[2][0] = 2;  edges[2][1] = 3;
	edges[3][0] = 3;  edges[3][1] = 0;
	edges[4][0] = 4;  edges[4][1] = 5;
	edges[5][0] = 5;  edges[5][1] = 6;
	edges[6][0] = 6;  edges[6][1] = 7;
	edges[7][0] = 7;  edges[7][1] = 4;
	edges[8][0] = 0;  edges[8][1] = 4;
	edges[9][0] = 1;  edges[9][1] = 5;
	edges[10][0] = 2; edges[10][1] = 6;
	edges[11][0] = 3; edges[11][1] = 7;

}

void AlignedBoundingBox::Render(){
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glColor4f(0.8,0.8,0.8,0.5);
	for(int iface=0;iface<6;iface++){
		glBegin(GL_POLYGON);{
			int4 vidx = faces[iface];
			glNormal3f(normals[vidx.x].x,normals[vidx.x].y,normals[vidx.x].z);
			glVertex3f(vertices[vidx.x].x,vertices[vidx.x].y,vertices[vidx.x].z);
			glNormal3f(normals[vidx.y].x,normals[vidx.y].y,normals[vidx.y].z);
			glVertex3f(vertices[vidx.y].x,vertices[vidx.y].y,vertices[vidx.y].z);
			glNormal3f(normals[vidx.z].x,normals[vidx.z].y,normals[vidx.z].z);
			glVertex3f(vertices[vidx.z].x,vertices[vidx.z].y,vertices[vidx.z].z);
			glNormal3f(normals[vidx.w].x,normals[vidx.w].y,normals[vidx.w].z);
			glVertex3f(vertices[vidx.w].x,vertices[vidx.w].y,vertices[vidx.w].z);
		}glEnd();
	}
	glDisable(GL_CULL_FACE);

	glLineWidth(2);
	glColor3f(1,1,0);
	for(int iedge=0;iedge<12;iedge++){
		glBegin(GL_LINES);{
			int vidx1 = edges[iedge][0];
			int vidx2 = edges[iedge][1];
			glVertex3f(vertices[vidx1].x,vertices[vidx1].y,vertices[vidx1].z);
			glVertex3f(vertices[vidx2].x,vertices[vidx2].y,vertices[vidx2].z);
		}glEnd();
	}
}