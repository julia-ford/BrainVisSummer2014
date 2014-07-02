// alignMethod: 0 left, x is left
#include"myGLexp.h"
#include <cmath>

// 1 middle, x is middle
// 2 right, x is right
void myGLexp::myGLPrintString(int x, int y, const string &str, const int alignMethod, float fontSize){
	// GLUT_STROKE_MONO_ROMAN
	// The maximum top character in the font is 119.05 units;
	// the bottom descends 33.33 units.
	// Each character is 104.76 units wide.
	float offset = 0;
	float scale = fontSize/104.76;
	if(alignMethod == 0){
		offset = 0;
	}
	else if(alignMethod == 1){
		offset = -(float)str.size()*fontSize/2;
	}
	else{
		offset = -(float)str.size()*fontSize;
	}
	int winWidth = glutGet(GLUT_WINDOW_WIDTH);
	int winHeight = glutGet(GLUT_WINDOW_HEIGHT);

	glPushAttrib(GL_VIEWPORT_BIT | GL_LIGHTING_BIT);{
		glViewport(0,0,winWidth,winHeight);
		glDisable(GL_LIGHTING);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();{
			glLoadIdentity();
			glOrtho(0,winWidth,0,winHeight,-5,5);
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();{
				//glRasterPos2i(x,y);
				glLoadIdentity();
				glTranslatef(x+offset,y,0);
				glScalef(scale,scale,scale);
				for(int i = 0;i<str.size();i++){
					//glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,str[i]);
					glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN,str[i]);
					//glutStrokeCharacter(GLUT_STROKE_ROMAN,str[i]);
				}
			}glPopMatrix();
			glMatrixMode(GL_PROJECTION);
		}glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	}glPopAttrib();
};

void myGLexp::esMatrixTranspose3x3 (float src[3][3], float dest[3][3]){
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

void myGLexp::MatrixTranspose4x4 (float src[4][4], float dest[4][4]){
	float tmp[4][4];
	for(int i = 0;i<4;i++){
		for(int j = 0;j<4;j++){
			tmp[i][j] = src[j][i];
		}
	}

	for(int i = 0;i<4;i++){
		for(int j = 0;j<4;j++){
			dest[i][j] = tmp[i][j];
		}
	}
}

void myGLexp::esMultiply3x3(float src1[3][3], float src2[3], float dest[3]){
	float tmp[3];
	for (int i=0;i<3;i++){
			tmp[i]=0;
	}

	for (int i=0;i<3;i++){
		for (int j=0;j<3;j++){
			tmp[i]+=( src1[j][i]*src2[j]);
		}
	}
	
	for (int i=0;i<3;i++){
			dest[i]=tmp[i];
	}
}

void myGLexp::crossProduct(const float x[3], const float y[3], float z[3]){
	z[0] = x[1]*y[2]-x[2]*y[1];
    z[1] = x[2]*y[0]-x[0]*y[2];
    z[2] = x[0]*y[1]-x[1]*y[0];
}

void myGLexp::esMultiply4x4(float src1[4][4], float src2[4], float dest[4]){
	float tmp[4];
	for (int i=0;i<4;i++){
			tmp[i]=0;
	}

	for (int i=0;i<4;i++){
		for (int j=0;j<4;j++){
			tmp[i]+=( src1[j][i]*src2[j]);
		}
	}
	
	for (int i=0;i<4;i++){
			dest[i]=tmp[i];
	}
}

// no scaling !!!
// input and output are both opengl tradition: column major
void myGLexp::matrixInverse4x4(float OpenGLmatIn[16], float matOut[4][4])
{
	float matIn[4][4];
	// OpenGL matrix is column major matrix in 1x16 array. Convert it to row major 4x4 matrix
	for(int m=0, k=0; m<=3, k<16; m++)
		for(int n=0;n<=3;n++)
		{
		matIn[m][n] = OpenGLmatIn[k];
		k++;
		}
	// 3x3 rotation Matrix Transpose ( it is equal to invering rotations) . Since rotation matrix is anti-symmetric matrix, transpose is equal to Inverse.
	for(int i=0 ; i<3; i++){
	for(int j=0; j<3; j++){
		matOut[j][i] = matIn[i][j];
		}
	}
	// Negate the translations ( equal to inversing translations)
	float vTmp[3];
 
	vTmp[0] = -matIn[3][0];
	vTmp[1] = -matIn[3][1];
	vTmp[2] = -matIn[3][2];
	// Roatate this vector using the above newly constructed rotation matrix
	matOut[3][0] = vTmp[0]*matOut[0][0] + vTmp[1]*matOut[1][0] + vTmp[2]*matOut[2][0];
	matOut[3][1] = vTmp[0]*matOut[0][1] + vTmp[1]*matOut[1][1] + vTmp[2]*matOut[2][1];
	matOut[3][2] = vTmp[0]*matOut[0][2] + vTmp[1]*matOut[1][2] + vTmp[2]*matOut[2][2];
 
	// Take care of the unused part of the OpenGL 4x4 matrix
	matOut[0][3] = matOut[1][3] = matOut[2][3] = 0.0f;
	matOut[3][3] = 1.0f;
}

inline void myGLexp::Matrix4x4MultiplyBy4x4 (float src1[4][4], float src2[4][4], float dest[4][4]){
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

void myGLexp::esRotateMatrix4x4(float result[4][4], GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
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

void myGLexp::rotateMatrix3x3(float result[3][3], GLfloat angleRad, GLfloat x, GLfloat y, GLfloat z)
{
	GLfloat mag = sqrtf(x * x + y * y + z * z);
	x/=mag;
	y/=mag;
	z/=mag;
	float s = sin(angleRad),
            c = cos(angleRad),
            t = 1 - c;

    result[0][0] = t*x*x+c;
    result[1][0] = t*x*y-s*z;
    result[2][0] = t*x*z+s*y;
    result[0][1] = t*x*y+s*z;
    result[1][1] = t*y*y+c;
    result[2][1] = t*y*z-s*x;
    result[0][2] = t*x*z-s*y;
    result[1][2] = t*y*z+s*x;
    result[2][2] = t*z*z+c;
}