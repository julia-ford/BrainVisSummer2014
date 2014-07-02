#ifndef myGLexp_h
#define myGLexp_h

#include <string>
#include "myGL.h"
using namespace std;

namespace myGLexp{
	
	const double PI = 3.14159265358979323846264338327950288419716939;
	// alignMethod: 0 left, x is left
	// 1 middle, x is middle
	// 2 right, x is right
	void myGLPrintString(int x, int y, const string &str, const int alignMethod = 0, float fontSize = 30.f);

	void esMatrixTranspose3x3 (float src[3][3], float dest[3][3]);

	void MatrixTranspose4x4 (float src[4][4], float dest[4][4]);

	void esMultiply3x3(float src1[3][3], float src2[3], float dest[3]);

	// z = x^y
	void crossProduct(const float x[3], const float y[3], float z[3]);

	void esMultiply4x4(float src1[4][4], float src2[4], float dest[4]);

	// no scaling !!!
	// input and output are both opengl tradition: column major
	// tested
	void matrixInverse4x4(float OpenGLmatIn[16], float matOut[4][4]);

	inline void Matrix4x4MultiplyBy4x4 (float src1[4][4], float src2[4][4], float dest[4][4]);

	// not tested
	void esRotateMatrix4x4(float result[4][4], GLfloat angle, GLfloat x, GLfloat y, GLfloat z);

	// tested
	void rotateMatrix3x3(float result[3][3], GLfloat angleRad, GLfloat x, GLfloat y, GLfloat z);
};

#endif