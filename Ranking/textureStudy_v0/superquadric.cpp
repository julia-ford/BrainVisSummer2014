
#include "SuperQuadric.h"
#include <gl/glut.h>
#include "rgba.h"
#include "vec3.h"
#include <cassert>
#include "mystdexp.h"
#include <cmath>
using namespace mystdexp;


#define PI 3.141592653589793238462643
#define isignf(x) (x<0?-1:1)

inline void esMatrixTranspose3x3 (float src[3][3], float dest[3][3]){
	float tmp[3][3];
	for(int i = 0;i<3;i++){
		for(int j = 0;j<3;j++){
			tmp[i][j] = src[j][i];
		}
	}

	for(int i = 0;i<3;i++){
		for(int j = 0;j<3;j++){
			dest[i][j] = tmp[j][i];
		}
	}
}

inline void MatrixTranspose4x4 (float src[4][4], float dest[4][4]){
	float tmp[4][4];
	for(int i = 0;i<4;i++){
		for(int j = 0;j<4;j++){
			tmp[i][j] = src[j][i];
		}
	}

	for(int i = 0;i<4;i++){
		for(int j = 0;j<4;j++){
			dest[i][j] = tmp[j][i];
		}
	}
}

void esMultiply3x3(float src1[3][3], float src2[3], float dest[3]){
	float tmp[3];
	for (int i=0;i<3;i++){
         tmp[i]=0;
    }

    for (int i=0;i<3;i++){
        for (int j=0;j<3;j++){
            tmp[i]+=( src1[i][j]*src2[j]);
        }
    }
	
	for (int i=0;i<3;i++){
         dest[i]=tmp[i];
    }
}

inline void Matrix4x4MultiplyBy4x4 (float src1[4][4], float src2[4][4], float dest[4][4]){
dest[0][0] = src1[0][0] * src2[0][0] + src1[0][1] * src2[1][0] + src1[0][2] * src2[2][0] + src1[0][3] * src2[3][0]; 
dest[0][1] = src1[0][0] * src2[0][1] + src1[0][1] * src2[1][1] + src1[0][2] * src2[2][1] + src1[0][3] * src2[3][1]; 
dest[0][2] = src1[0][0] * src2[0][2] + src1[0][1] * src2[1][2] + src1[0][2] * src2[2][2] + src1[0][3] * src2[3][2]; 
dest[0][3] = src1[0][0] * src2[0][3] + src1[0][1] * src2[1][3] + src1[0][2] * src2[2][3] + src1[0][3] * src2[3][3]; 
dest[1][0] = src1[1][0] * src2[0][0] + src1[1][1] * src2[1][0] + src1[1][2] * src2[2][0] + src1[1][3] * src2[3][0]; 
dest[1][1] = src1[1][0] * src2[0][1] + src1[1][1] * src2[1][1] + src1[1][2] * src2[2][1] + src1[1][3] * src2[3][1]; 
dest[1][2] = src1[1][0] * src2[0][2] + src1[1][1] * src2[1][2] + src1[1][2] * src2[2][2] + src1[1][3] * src2[3][2]; 
dest[1][3] = src1[1][0] * src2[0][3] + src1[1][1] * src2[1][3] + src1[1][2] * src2[2][3] + src1[1][3] * src2[3][3]; 
dest[2][0] = src1[2][0] * src2[0][0] + src1[2][1] * src2[1][0] + src1[2][2] * src2[2][0] + src1[2][3] * src2[3][0]; 
dest[2][1] = src1[2][0] * src2[0][1] + src1[2][1] * src2[1][1] + src1[2][2] * src2[2][1] + src1[2][3] * src2[3][1]; 
dest[2][2] = src1[2][0] * src2[0][2] + src1[2][1] * src2[1][2] + src1[2][2] * src2[2][2] + src1[2][3] * src2[3][2]; 
dest[2][3] = src1[2][0] * src2[0][3] + src1[2][1] * src2[1][3] + src1[2][2] * src2[2][3] + src1[2][3] * src2[3][3]; 
dest[3][0] = src1[3][0] * src2[0][0] + src1[3][1] * src2[1][0] + src1[3][2] * src2[2][0] + src1[3][3] * src2[3][0]; 
dest[3][1] = src1[3][0] * src2[0][1] + src1[3][1] * src2[1][1] + src1[3][2] * src2[2][1] + src1[3][3] * src2[3][1]; 
dest[3][2] = src1[3][0] * src2[0][2] + src1[3][1] * src2[1][2] + src1[3][2] * src2[2][2] + src1[3][3] * src2[3][2]; 
dest[3][3] = src1[3][0] * src2[0][3] + src1[3][1] * src2[1][3] + src1[3][2] * src2[2][3] + src1[3][3] * src2[3][3]; 
};

void esRotate(float result[4][4], GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
   GLfloat sinAngle, cosAngle;
   GLfloat mag = sqrtf(x * x + y * y + z * z);
 
   sinAngle = sinf ( angle * PI / 180.0f );
   cosAngle = cosf ( angle * PI / 180.0f );
   if ( mag > 0.0f )
   {
      GLfloat xx, yy, zz, xy, yz, zx, xs, ys, zs;
      GLfloat oneMinusCos;
      GLfloat rotMat[4][4];
 
      x /= mag;
      y /= mag;
      z /= mag;
 
      xx = x * x;
      yy = y * y;
      zz = z * z;
      xy = x * y;
      yz = y * z;
      zx = z * x;
      xs = x * sinAngle;
      ys = y * sinAngle;
      zs = z * sinAngle;
      oneMinusCos = 1.0f - cosAngle;
 
      rotMat[0][0] = (oneMinusCos * xx) + cosAngle;
      rotMat[0][1] = (oneMinusCos * xy) - zs;
      rotMat[0][2] = (oneMinusCos * zx) + ys;
      rotMat[0][3] = 0.0F; 
 
      rotMat[1][0] = (oneMinusCos * xy) + zs;
      rotMat[1][1] = (oneMinusCos * yy) + cosAngle;
      rotMat[1][2] = (oneMinusCos * yz) - xs;
      rotMat[1][3] = 0.0F;
 
      rotMat[2][0] = (oneMinusCos * zx) - ys;
      rotMat[2][1] = (oneMinusCos * yz) + xs;
      rotMat[2][2] = (oneMinusCos * zz) + cosAngle;
      rotMat[2][3] = 0.0F; 
 
      rotMat[3][0] = 0.0F;
      rotMat[3][1] = 0.0F;
      rotMat[3][2] = 0.0F;
      rotMat[3][3] = 1.0F;
 
      Matrix4x4MultiplyBy4x4( result, rotMat, result );
   }
}
void esRotateMatrix4x4(float result[4][4], GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
   GLfloat sinAngle, cosAngle;
   GLfloat mag = sqrtf(x * x + y * y + z * z);
 
   sinAngle = sinf ( angle * PI / 180.0f );
   cosAngle = cosf ( angle * PI / 180.0f );
   if ( mag > 0.0f )
   {
      GLfloat xx, yy, zz, xy, yz, zx, xs, ys, zs;
      GLfloat oneMinusCos;
 
      x /= mag;
      y /= mag;
      z /= mag;
 
      xx = x * x;
      yy = y * y;
      zz = z * z;
      xy = x * y;
      yz = y * z;
      zx = z * x;
      xs = x * sinAngle;
      ys = y * sinAngle;
      zs = z * sinAngle;
      oneMinusCos = 1.0f - cosAngle;
 
      result[0][0] = (oneMinusCos * xx) + cosAngle;
      result[0][1] = (oneMinusCos * xy) - zs;
      result[0][2] = (oneMinusCos * zx) + ys;
      result[0][3] = 0.0F; 
 
      result[1][0] = (oneMinusCos * xy) + zs;
      result[1][1] = (oneMinusCos * yy) + cosAngle;
      result[1][2] = (oneMinusCos * yz) - xs;
      result[1][3] = 0.0F;
 
      result[2][0] = (oneMinusCos * zx) - ys;
      result[2][1] = (oneMinusCos * yz) + xs;
      result[2][2] = (oneMinusCos * zz) + cosAngle;
      result[2][3] = 0.0F; 
 
      result[3][0] = 0.0F;
      result[3][1] = 0.0F;
      result[3][2] = 0.0F;
      result[3][3] = 1.0F;
   }
}

void esRotateMatrix3x3(float result[3][3], GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
   GLfloat sinAngle, cosAngle;
   GLfloat mag = sqrtf(x * x + y * y + z * z);
 
   sinAngle = sinf ( angle * PI / 180.0f );
   cosAngle = cosf ( angle * PI / 180.0f );
   if ( mag > 0.0f )
   {
      GLfloat xx, yy, zz, xy, yz, zx, xs, ys, zs;
      GLfloat oneMinusCos;
 
      x /= mag;
      y /= mag;
      z /= mag;
 
      xx = x * x;
      yy = y * y;
      zz = z * z;
      xy = x * y;
      yz = y * z;
      zx = z * x;
      xs = x * sinAngle;
      ys = y * sinAngle;
      zs = z * sinAngle;
      oneMinusCos = 1.0f - cosAngle;
 
      result[0][0] = (oneMinusCos * xx) + cosAngle;
      result[0][1] = (oneMinusCos * xy) - zs;
      result[0][2] = (oneMinusCos * zx) + ys;
 
      result[1][0] = (oneMinusCos * xy) + zs;
      result[1][1] = (oneMinusCos * yy) + cosAngle;
      result[1][2] = (oneMinusCos * yz) - xs;
 
      result[2][0] = (oneMinusCos * zx) - ys;
      result[2][1] = (oneMinusCos * yz) + xs;
      result[2][2] = (oneMinusCos * zz) + cosAngle;
   }
}

SuperQuadric::SuperQuadric(){
    gl_displayList = -1;
    m_detailTangent = 10;
    m_detailAxial = 10;
    alpha = 0.5;
    beta = 0.5;
    gama = 6.0;
	size = vec3(0.35,0.35,0.35);
	changed();
}

SuperQuadric::~SuperQuadric(){
	glDeleteLists(gl_displayList,1);
}

void SuperQuadric::Update(){
	if(m_needRebuild){
		buildDisplayList();
	}
	m_needRebuild = false;
}

void SuperQuadric::Draw() const{
	//glColor3f(color.r,color.g,color.b);
	drawGeometry();
}

void SuperQuadric::DrawAtOrigin() const{
	glPushMatrix();{
		//glRotatef(rotateAngle,rotateAxis.x,rotateAxis.y,rotateAxis.z);
		drawCanonicalGeometry();
	}glPopMatrix();
}

void SuperQuadric::AppendShape(vector<float> &vertices, vector<float> &texCoords, vector<float> &normals) const{
	append(vertices,m_vertices);
	append(texCoords,m_texCoords);
	append(normals,m_normals);
}

void SuperQuadric::AppendShape(vector<float> &vertices, vector<float> &texCoords, vector<float> &normals, vector<unsigned int> &indices, int &indexOffset) const{
	
	assert(vertices.size()/3 == indexOffset);
	for(int h = 0;h<m_detailTangent;h++){
		for(int v = 0;v<=m_detailAxial;v++){
			int pidx = (h*(m_detailAxial+1)+v)*2;
			normals.push_back(m_normals[pidx*3+0]);
			normals.push_back(m_normals[pidx*3+1]);
			normals.push_back(m_normals[pidx*3+2]);
			texCoords.push_back(m_texCoords[pidx*2+0]);
			texCoords.push_back(m_texCoords[pidx*2+1]);
			vertices.push_back(m_vertices[pidx*3+0]);
			vertices.push_back(m_vertices[pidx*3+1]);
			vertices.push_back(m_vertices[pidx*3+2]);
		}
	}
	for(int h = 0;h<m_detailTangent;h++){
		for(int v = 0;v<m_detailAxial;v++){
			int idx = h*(m_detailAxial+1)+v;
			indices.push_back(idx+indexOffset);
			indices.push_back(idx+1+indexOffset);
			
			if(h==m_detailTangent-1){
				indices.push_back(v+1+indexOffset);
				indices.push_back(v+indexOffset);
			}
			else{
				indices.push_back(idx+1+(m_detailAxial+1)+indexOffset);
				indices.push_back(idx+(m_detailAxial+1)+indexOffset);
			}
		}
	}
	indexOffset +=  m_detailTangent*(m_detailAxial+1) ;
}

void SuperQuadric::Render(){
	glColor3f(color.r,color.g,color.b);
	glCallList(gl_displayList);
}

void SuperQuadric::SetAlpha(float a){
	alpha = a;
	changed();
}

void SuperQuadric::SetBeta(float b){
	beta = b;
	changed();
}

void SuperQuadric::SetGama(float g){
	gama = g;
	changed();
}

void SuperQuadric::SetDirection(const vec3& newDirection) {
	direction = newDirection;
	changed();
}
void SuperQuadric::SetPosition(const vec3& newPosition){
	position = newPosition;
	changed();
}

void SuperQuadric::SetColor(const rgba& newColor){
	color=newColor;
}

void SuperQuadric::SetSize(const vec3& newSize){
	size=newSize;
	changed();
}

void SuperQuadric::SetDetail(const float newDetailTangent, const float newDetailAxial){
	m_detailTangent=newDetailTangent;
	m_detailAxial=newDetailAxial;
	changed();
}

void SuperQuadric::SetFAForTexureCoord(const float newFA){
	fa=newFA;
	changed();
}

void SuperQuadric::Build(){
	double delta = delta = 0.02 * PI / (m_detailTangent>m_detailAxial?m_detailTangent:m_detailAxial);
	
	m_normals.resize(m_detailTangent*(m_detailAxial+1)*2*3);
	m_texCoords.resize(m_detailTangent*(m_detailAxial+1)*2*2);
	m_vertices.resize(m_detailTangent*(m_detailAxial+1)*2*3);

	
	const vec3 mainAxis(0,0,1);
	vec3 rotateAxis = mainAxis^direction;
	float rotateAngle = acos(mainAxis*(direction.normalized()))/PI*180;
	if(direction.x == 0 && direction.z == 0){	// if along z axis
		rotateAngle = 0;
		rotateAxis = vec3(1,0,0);	// does not matter what value
	}
	// glRotatef(rotateAngle,rotateAxis.x,rotateAxis.y,rotateAxis.z);

	float rotateMatrix[3][3];
	float mormalRotateMatrix[3][3];
	esRotateMatrix3x3(rotateMatrix,rotateAngle,rotateAxis.x,rotateAxis.y,rotateAxis.z);
	esMatrixTranspose3x3(rotateMatrix,mormalRotateMatrix);

	for(int h = 0;h<m_detailTangent;h++){
		float hagl1 = h*PI*2/m_detailTangent;
		float hagl2 = (h+1)*PI*2/m_detailTangent;
		for(int v = 0;v<=m_detailAxial;v++){
			float vagl = v*PI/m_detailAxial;
			vec3 p1;// = getOneVertexOnCanonicalSurface(vagl,hagl1);
			vec3 p2;// = getOneVertexOnCanonicalSurface(vagl,hagl2);
			vec3 norm1;
			vec3 norm2;
			if(v==0){
				norm1 = vec3(0,0,1);
				norm2 = vec3(0,0,1);
				p1.set(0,0,1);
				p2.set(0,0,1);
			}
			else if(v==m_detailAxial){
				norm1 = vec3(0,0,-1);
				norm2 = vec3(0,0,-1);
				p1.set(0,0,-1);
				p2.set(0,0,-1);
			}
			else{
				p1 = getOneVertexOnCanonicalSurface(vagl,hagl1);
				p2 = getOneVertexOnCanonicalSurface(vagl,hagl2);
				vec3 p11 = getOneVertexOnCanonicalSurface(vagl+delta,hagl1);
				vec3 p12 = getOneVertexOnCanonicalSurface(vagl,hagl1+delta);
				norm1 = ((p11-p1)^(p12-p1)).normalized();
				
				vec3 p21 = getOneVertexOnCanonicalSurface(vagl+delta,hagl2);
				vec3 p22 = getOneVertexOnCanonicalSurface(vagl,hagl2+delta);
				norm2 = ((p21-p2)^(p22-p2)).normalized();
			}
			float u = (float)v/m_detailAxial;
			if(u>0.45) u=(u-0.4)/fa+0.4;
			p1.scale(size);
			p2.scale(size);

			int pidx = (h*(m_detailAxial+1)+v)*2;

			//esMultiply3x3(mormalRotateMatrix,&(norm1.x),&(norm1.x));
			//esMultiply3x3(mormalRotateMatrix,&(norm2.x),&(norm2.x));
			//esMultiply3x3(rotateMatrix,&(p1.x),&(p1.x));
			//esMultiply3x3(rotateMatrix,&(p2.x),&(p2.x));

			m_normals[pidx*3+0] = norm1.x;
			m_normals[pidx*3+1] = norm1.y;
			m_normals[pidx*3+2] = norm1.z;
			m_normals[pidx*3+3] = norm2.x;
			m_normals[pidx*3+4] = norm2.y;
			m_normals[pidx*3+5] = norm2.z;
			m_texCoords[pidx*2+0] = u;
			m_texCoords[pidx*2+1] = (float)h/m_detailTangent;
			m_texCoords[pidx*2+2] = u;
			m_texCoords[pidx*2+3] = (float)(h+1)/m_detailTangent;
			m_vertices[pidx*3+0] = p1.x;
			m_vertices[pidx*3+1] = p1.y;
			m_vertices[pidx*3+2] = p1.z;
			m_vertices[pidx*3+3] = p2.x;
			m_vertices[pidx*3+4] = p2.y;
			m_vertices[pidx*3+5] = p2.z;
			
			esMultiply3x3(mormalRotateMatrix,&m_normals[pidx*3],&m_normals[pidx*3]);
			esMultiply3x3(mormalRotateMatrix,&m_normals[pidx*3+3],&m_normals[pidx*3+3]);
			esMultiply3x3(rotateMatrix,&m_vertices[pidx*3],&m_vertices[pidx*3]);
			esMultiply3x3(rotateMatrix,&m_vertices[pidx*3+3],&m_vertices[pidx*3+3]);
		}
    }
}

void SuperQuadric::drawGeometry() const{
	const vec3 mainAxis(0,0,1);
	vec3 rotateAxis = mainAxis^direction;
	float rotateAngle = acos(mainAxis*(direction.normalized()))/PI*180;
	if(direction.x == 0 && direction.z == 0){	// if along z axis
		rotateAngle = 0;
		rotateAxis = vec3(1,0,0);	// does not matter what value
	}
	glPushMatrix();{
		glTranslatef(position.x,position.y,position.z);
		glRotatef(rotateAngle,rotateAxis.x,rotateAxis.y,rotateAxis.z);
		drawCanonicalGeometry();
	}glPopMatrix();
}

void SuperQuadric::buildDisplayList(){
	glDeleteLists(gl_displayList,1);
    gl_displayList = glGenLists(1);
    glNewList(gl_displayList,GL_COMPILE);{
		drawGeometry();
	}glEndList();
}

void SuperQuadric::drawCanonicalGeometry() const{
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glNormalPointer(GL_FLOAT, 0, &m_normals[0]);
	glTexCoordPointer(2, GL_FLOAT, 0, &m_texCoords[0]);
	glVertexPointer(3, GL_FLOAT, 0, &m_vertices[0]);

	glDrawArrays(GL_QUAD_STRIP, 0, m_detailTangent*(m_detailAxial+1)*2);

	glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}

/*
void SuperQuadric::drawCanonicalGeometry(){
    double delta = delta = 0.02 * PI / (m_detailTangent>m_detailAxial?m_detailTangent:m_detailAxial);
	
	float *normals = new float[m_detailTangent*(m_detailAxial+1)*2*3];
	float *texCoords = new float[m_detailTangent*(m_detailAxial+1)*2*2];
	float *vertices = new float[m_detailTangent*(m_detailAxial+1)*2*3];

	for(int h = 0;h<m_detailTangent;h++){
		float hagl1 = h*PI*2/m_detailTangent;
		float hagl2 = (h+1)*PI*2/m_detailTangent;
		//glBegin(GL_QUAD_STRIP);{
			for(int v = 0;v<=m_detailAxial;v++){
				float vagl = v*PI/m_detailAxial;
				vec3 p1 = getOneVertexOnCanonicalSurface(vagl,hagl1);
				vec3 p2 = getOneVertexOnCanonicalSurface(vagl,hagl2);
				vec3 norm1;
				vec3 norm2;
				if(v==0){
					norm1 = vec3(0,0,1);
					norm2 = vec3(0,0,1);
				}
				else if(v==m_detailAxial){
					norm1 = vec3(0,0,-1);
					norm2 = vec3(0,0,-1);
				}
				else{
					vec3 p11 = getOneVertexOnCanonicalSurface(vagl+delta,hagl1);
					vec3 p12 = getOneVertexOnCanonicalSurface(vagl,hagl1+delta);
					norm1 = ((p11-p1)^(p12-p1)).normalized();
				
					vec3 p21 = getOneVertexOnCanonicalSurface(vagl+delta,hagl2);
					vec3 p22 = getOneVertexOnCanonicalSurface(vagl,hagl2+delta);
					norm2 = ((p21-p2)^(p22-p2)).normalized();
				}
				float u = (float)v/m_detailAxial;
				if(u>0.45) u=(u-0.4)/fa+0.4;
				p1.scale(size);
				p2.scale(size);

				int pidx = (h*(m_detailAxial+1)+v)*2;
				normals[pidx*3+0] = norm1.x;
				normals[pidx*3+1] = norm1.y;
				normals[pidx*3+2] = norm1.z;
				normals[pidx*3+3] = norm2.x;
				normals[pidx*3+4] = norm2.y;
				normals[pidx*3+5] = norm2.z;
				texCoords[pidx*2+0] = u;
				texCoords[pidx*2+1] = (float)h/m_detailTangent;
				texCoords[pidx*2+2] = u;
				texCoords[pidx*2+3] = (float)(h+1)/m_detailTangent;
				vertices[pidx*3+0] = p1.x;
				vertices[pidx*3+1] = p1.y;
				vertices[pidx*3+2] = p1.z;
				vertices[pidx*3+3] = p2.x;
				vertices[pidx*3+4] = p2.y;
				vertices[pidx*3+5] = p2.z;
			}
		//}glEnd();
    }
	
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glNormalPointer(GL_FLOAT, 0, normals);
	glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
	glVertexPointer(3, GL_FLOAT, 0, vertices);

	glDrawArrays(GL_QUAD_STRIP, 0, m_detailTangent*(m_detailAxial+1)*2);

	glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

}
*/

/*
// backup
void SuperQuadric::drawCanonicalGeometry(){
    double delta = delta = 0.02 * PI / (m_detailTangent>m_detailAxial?m_detailTangent:m_detailAxial);
	for(int h = 0;h<m_detailTangent;h++){
		float hagl1 = h*PI*2/m_detailTangent;
		float hagl2 = (h+1)*PI*2/m_detailTangent;
		glBegin(GL_QUAD_STRIP);{
			for(int v = 0;v<=m_detailAxial;v++){
				float vagl = v*PI/m_detailAxial;
				vec3 p1 = getOneVertexOnCanonicalSurface(vagl,hagl1);
				vec3 p2 = getOneVertexOnCanonicalSurface(vagl,hagl2);
				vec3 norm1;
				vec3 norm2;
				if(v==0){
					norm1 = vec3(0,0,1);
					norm2 = vec3(0,0,1);
				}
				else if(v==m_detailAxial){
					norm1 = vec3(0,0,-1);
					norm2 = vec3(0,0,-1);
				}
				else{
					vec3 p11 = getOneVertexOnCanonicalSurface(vagl+delta,hagl1);
					vec3 p12 = getOneVertexOnCanonicalSurface(vagl,hagl1+delta);
					norm1 = ((p11-p1)^(p12-p1)).normalized();
				
					vec3 p21 = getOneVertexOnCanonicalSurface(vagl+delta,hagl2);
					vec3 p22 = getOneVertexOnCanonicalSurface(vagl,hagl2+delta);
					norm2 = ((p21-p2)^(p22-p2)).normalized();
				}
				glNormal3f(norm1.x,norm1.y,norm1.z);
				float u = (float)v/m_detailAxial;
				if(u>0.45) u=(u-0.4)/fa+0.4;
				glTexCoord2f(u,(float)h/m_detailTangent);
				p1.scale(size);
				glVertex3f(p1.x,p1.y,p1.z);

				glNormal3f(norm2.x,norm2.y,norm2.z);
				glTexCoord2f(u,(float)(h+1)/m_detailTangent);
				p2.scale(size);
				glVertex3f(p2.x,p2.y,p2.z);
			}
		}glEnd();
    }
}
*/
void SuperQuadric::changed(){
	m_needRebuild = true;
}

vec3 SuperQuadric::getOneVertexOnCanonicalSurface(double _theta1, double _theta2) const
{
	// polar is z axis
    double ct1 = cos(_theta1);
    double ct2 = cos(_theta2);
    double st1 = sin(_theta1);
    double st2 = sin(_theta2);
	vec3 p;
    p.x = isignf(ct2)*isignf(st1)*powf(fabs(ct2),alpha)*powf(fabs(st1),beta);
    p.y = isignf(st2)*isignf(st1)*powf(fabs(st2),alpha)*powf(fabs(st1),beta);
    p.z = isignf(ct1)*powf(fabs(ct1),beta);

	assert(p.x>-100&&p.x<200);
	assert(p.y>-100&&p.y<200);
	assert(p.z>-100&&p.z<200);
    return p;
}

