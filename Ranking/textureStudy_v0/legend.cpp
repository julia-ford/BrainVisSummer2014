#include <cmath>
#include <fstream>
#include <gl/glut.h>

#include "ColorConverter.h"
#include "legend.h"
#include "myGLexp.h"
#include "mystdexp.h"

using namespace std;
using namespace myGLexp;
using namespace mystdexp;

/// Draws the x, y, and z axes which are superimposed
/// on the BallLegend instance.
void BallLegend::DrawAxes(){
	float length = 3.f;
    //x
	glColor3f(1,0,0);
	glBegin(GL_LINES);
	glVertex3f(-length,0,0);
	glVertex3f(length,0,0);
	glEnd();

    //y
	glColor3f(0,1,0);
	glBegin(GL_LINES);
	glVertex3f(0,-length,0);
	glVertex3f(0,length,0);
	glEnd();

    //z	
	glColor3f(0,0,1);
	glBegin(GL_LINES);
	glVertex3f(0,0,-length);
	glVertex3f(0,0,length);
	glEnd();

	GLint    viewport  [ 4]; // Storage for the viewport values.
	GLdouble modelview [16]; // Storage for the doubles of the modelview matrix.
	GLdouble projection[16]; // Storage for the doubles of the projection matrixm
	glGetIntegerv(GL_VIEWPORT,          viewport);   // Retrieves the Viewport Values (X, Y, Width, Height)
	glGetDoublev (GL_MODELVIEW_MATRIX,  modelview);  // Retrieves the Modelview Matrix
	glGetDoublev (GL_PROJECTION_MATRIX, projection); // Retrieves the Projection Matrix
	GLdouble winX, winY, winZ;
	vector<vec3> wins;
	
	// draw text icon
	glColor3f(1,0,0);
	gluProject(length, 0, 0,
		modelview, projection, viewport, 
		&winX, &winY, &winZ);
	wins.push_back(vec3(winX,winY,winZ));
	myGLPrintString(winX,winY,"Right",1,9);
	
	glColor3f(1,0,0);
	gluProject(-length, 0, 0,
		modelview, projection, viewport, 
		&winX, &winY, &winZ);
	wins.push_back(vec3(winX,winY,winZ));
	myGLPrintString(winX,winY,"Left",1,9);

	glColor3f(0,1,0);
	gluProject( 0,length, 0,
		modelview, projection, viewport, 
		&winX, &winY, &winZ);
	wins.push_back(vec3(winX,winY,winZ));
	myGLPrintString(winX,winY,"Posterior",1,9);
	
	glColor3f(0,1,0);
	gluProject(0, -length, 0,
		modelview, projection, viewport, 
		&winX, &winY, &winZ);
	wins.push_back(vec3(winX,winY,winZ));
	myGLPrintString(winX,winY,"Anterior",1,9);
	
	glColor3f(0,0,1);
	gluProject( 0, 0, length,
		modelview, projection, viewport, 
		&winX, &winY, &winZ);
	wins.push_back(vec3(winX,winY,winZ));
	myGLPrintString(winX,winY,"Superior",1,9);
	
	glColor3f(0,0,1);
	gluProject( 0, 0, -length,
		modelview, projection, viewport, 
		&winX, &winY, &winZ);
	wins.push_back(vec3(winX,winY,winZ));
	myGLPrintString(winX,winY,"Inferior",1,9);

}

/// Constructor for the BallLegend.
BallLegend::BallLegend()
{
	colors = NULL;
	points = NULL;
	numLati  = 0;
	numLongi = 0;
}

/// Default destructor for BallLegend.
/// Simply calls the BallLegend::destroy() function.
BallLegend::~BallLegend()
{
	destory();
}

void BallLegend::Render(){
	if(glIsList(gl_DisplayList) && drawColorBall){
		glCallList(gl_DisplayList);
	}
	else if(glIsList(gl_DisplayList) && drawTextureBall){
		glCallList(gl_displayListTex);
	}
	else if(glIsList(gl_displayListSaturation) && drawSaturationBall){
		glCallList(gl_displayListSaturation);
	}
	DrawAxes();
}

void BallLegend::LoadFromFile(const char* filename){
	destory();
	ifstream infile( filename);
	infile>>numLongi>>numLati;
	colors = new rgba*[numLongi];
	points = new vec3*[numLongi];
	for(int i=0;i<numLongi;i++){
		colors[i]=new rgba[numLati];
		points[i]=new vec3[numLati];
		for(int j=0;j<numLati;j++){
			infile>>points[i][j].x>>points[i][j].y>>points[i][j].z;
			infile>>colors[i][j].r>>colors[i][j].g>>colors[i][j].b;
            infile.ignore( 200, '\n');
		}
	}
	infile.close();
	//destory();
}

rgba BallLegend::GetColorByDirection(const vec3& _dir) const{
	vec3 dir = _dir.normalized();
	const float PI = 3.1415926;
	float h = atan2(dir.y,dir.x);
	float v = asin(dir.z);
	int hIdx = (h/PI*180+180)/10+0.5;
	int vIdx = (v/PI*180+90)/10+0.5;

	return colors[hIdx%36][vIdx];
}

// must have the textureMaps ready first!!
void BallLegend::BuildDisplayList(){
	if(glIsList(gl_DisplayList)){
		glDeleteLists(gl_DisplayList,1);
	}
	float r=2.f;
	gl_DisplayList = glGenLists(1);
	glNewList(gl_DisplayList,GL_COMPILE);
	for(int i = 0;i<numLongi;i++){
		glBegin(GL_QUAD_STRIP);
		for(int j = 0;j<numLati;j++){
			rgba &color1 = colors[i][j];
			rgba &color2 = colors[(i+1)%numLongi][j];
			vec3 &point1 = points[i][j]*r;
			vec3 &point2 = points[(i+1)%numLongi][j]*r;

			glColor3f(color1.r,color1.g,color1.b);
			glNormal3f(point1.x,point1.y,point1.z);
			glVertex3f(point1.x,point1.y,point1.z);
			
			glColor3f(color2.r,color2.g,color2.b);
			glNormal3f(point2.x,point2.y,point2.z);
			glVertex3f(point2.x,point2.y,point2.z);
		}
		glEnd();
	}
	glEndList();
		
	
	float deltaLongi = 360.f/numLongi;
	float deltaLati = 180/(numLati-1);
	const float PI = 3.1415926f;
	float repx = 1;
	float repy = 1;


	// saturation
	if(glIsList(gl_displayListSaturation)){
		glDeleteLists(gl_displayListSaturation,1);
	}
	
	gl_displayListSaturation = glGenLists(1);
	glNewList(gl_displayListSaturation,GL_COMPILE);
	for(int i = 0;i<numLongi;i++){
		for(int j = 0;j<numLati;j++){
			float longi1 = i*deltaLongi/180*PI;
			float lati1 = (j*deltaLati-90)/180*PI;
			vec3 point1;
			point1.fromSphereCoord(longi1,lati1,r);
			
			float longi2 = (i+1)*deltaLongi/180*PI;
			float lati2 = (j*deltaLati-90)/180*PI;
			vec3 point2;
			point2.fromSphereCoord(longi2,lati2,r);
			
			float longi3 = (i+1)*deltaLongi/180*PI;
			float lati3 = ((j+1)*deltaLati-90)/180*PI;
			vec3 point3;
			point3.fromSphereCoord(longi3,lati3,r);
			
			float longi4 = i*deltaLongi/180*PI;
			float lati4 = ((j+1)*deltaLati-90)/180*PI;
			vec3 point4;
			point4.fromSphereCoord(longi4,lati4,r);

			rgba color, tcolor;
			int texIdx;
			glBegin(GL_POLYGON);{
				tcolor = GetColorByDirection(point1.normalized());
				texIdx = ColorConverter::getIdxByBoysColor(tcolor);
				color.fromHSV(10,texIdx/9.f,1);
				glColor3f(color.r,color.g,color.b);
				glNormal3f(point1.x,point1.y,point1.z);
				glVertex3f(point1.x,point1.y,point1.z);
			
				tcolor = GetColorByDirection(point2.normalized());
				texIdx = ColorConverter::getIdxByBoysColor(tcolor);
				color.fromHSV(10,texIdx/9.f,1);
				glColor3f(color.r,color.g,color.b);
				glNormal3f(point2.x,point2.y,point2.z);
				glVertex3f(point2.x,point2.y,point2.z);
			
				tcolor = GetColorByDirection(point3.normalized());
				texIdx = ColorConverter::getIdxByBoysColor(tcolor);
				color.fromHSV(10,texIdx/9.f,1);
				glColor3f(color.r,color.g,color.b);
				glNormal3f(point3.x,point3.y,point3.z);
				glVertex3f(point3.x,point3.y,point3.z);
			
				tcolor = GetColorByDirection(point4.normalized());
				texIdx = ColorConverter::getIdxByBoysColor(tcolor);
				color.fromHSV(10,texIdx/9.f,1);
				glColor3f(color.r,color.g,color.b);
				glNormal3f(point4.x,point4.y,point4.z);
				glVertex3f(point4.x,point4.y,point4.z);
			}glEnd();
		}
	}
	glEndList();

	//int numLongi = 18;
	//int numLati = 8+1;
	if(glIsList(gl_displayListTex)){
		glDeleteLists(gl_displayListTex,1);
	}
	gl_displayListTex = glGenLists(1);
	glNewList(gl_displayListTex,GL_COMPILE);
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
	for(int i = 0;i<numLongi;i++){
		for(int j = 0;j<numLati;j++){
			float longi1 = i*deltaLongi/180*PI;
			float lati1 = (j*deltaLati-90)/180*PI;
			vec3 point1;
			point1.fromSphereCoord(longi1,lati1,r);
			
			float longi2 = (i+1)*deltaLongi/180*PI;
			float lati2 = (j*deltaLati-90)/180*PI;
			vec3 point2;
			point2.fromSphereCoord(longi2,lati2,r);
			
			float longi3 = (i+1)*deltaLongi/180*PI;
			float lati3 = ((j+1)*deltaLati-90)/180*PI;
			vec3 point3;
			point3.fromSphereCoord(longi3,lati3,r);
			
			float longi4 = i*deltaLongi/180*PI;
			float lati4 = ((j+1)*deltaLati-90)/180*PI;
			vec3 point4;
			point4.fromSphereCoord(longi4,lati4,r);

			vec3 cenDir = (point1+point2+point3+point4)/4;
			rgba color = GetColorByDirection(cenDir);
			GLuint tex = textureMaps->GetTextureNameByColor(color);
			//glDisable(GL_COLOR_MATERIAL);
			//assert(glIsTexture(tex) == GL_TRUE);
			glBindTexture(GL_TEXTURE_2D, tex);
			glBegin(GL_POLYGON);{
				glTexCoord2f(0,repy);
				glNormal3f(point1.x,point1.y,point1.z);
				glVertex3f(point1.x,point1.y,point1.z);
			
				glTexCoord2f(repx,repy);
				glNormal3f(point2.x,point2.y,point2.z);
				glVertex3f(point2.x,point2.y,point2.z);
			
				glTexCoord2f(repx,0);
				glNormal3f(point3.x,point3.y,point3.z);
				glVertex3f(point3.x,point3.y,point3.z);
			
				glTexCoord2f(0,0);
				glNormal3f(point4.x,point4.y,point4.z);
				glVertex3f(point4.x,point4.y,point4.z);
			}glEnd();
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
	glDisable(GL_TEXTURE_2D);
	glEndList();


	
}

/// Thoroughly cleans up the BallLegend instance in preparation for
/// the instance's destruction.
void BallLegend::destory(){
	if(colors){
		for(int i = 0; i < numLongi; i++){
			delete[] colors[i]; }
		colors = NULL; }
	if(points){
		for(int i = 0; i < numLongi; i++){
			delete[] points[i]; }
		points = NULL; }
}

/// Creates a new BarLegend instance with default values for
/// m_discrete, m_startValue, and m_endValue.
BarLegend::BarLegend(){
	m_discrete = false;
	m_startValue = 0;
	m_endValue = 1;
}

/// Destructor for BarLegend.
/// Clears m_colors.
BarLegend::~BarLegend(){
	m_colors.clear();
}

void BarLegend::Render(){
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	GLdouble modelview[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	GLdouble projection[16];
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	GLdouble winX, winY, winZ;
	glPushAttrib(GL_LIGHTING_BIT);

	if(m_discrete){
		glShadeModel(GL_FLAT);
		for(int i = 0;i<m_colors.size();i++){
			glColor3f(m_colors[i].r,m_colors[i].g,m_colors[i].b);
			float yc1 = (float)i/(m_colors.size())*2-1;
			float yc2 = (float)(i+1)/(m_colors.size())*2-1;
			glBegin(GL_POLYGON);{
				glNormal3f(0,0,1);
				glVertex3f(-1,yc1,0);
				glNormal3f(0,0,1);
				glVertex3f(0.5,yc1,0);
				glNormal3f(0,0,1);
				glVertex3f(0.5,yc2,0);
				glNormal3f(0,0,1);
				glVertex3f(-1,yc2,0);
			}glEnd();
		}
	}
	else{
		glShadeModel(GL_SMOOTH);
		glBegin(GL_QUAD_STRIP);{
			for(int i = 0;i<m_colors.size();i++){
				glColor3f(m_colors[i].r,m_colors[i].g,m_colors[i].b);
				float yc = (float)i/(m_colors.size()-1)*2-1;
				glNormal3f(0,0,1);
				glVertex3f(-1,yc,0);
				glNormal3f(0,0,1);
				glVertex3f(0.5,yc,0);
			}
		}glEnd();
	}
	glPopAttrib();

	if(m_title == "Fiber" && m_colors.size()>0){
		glPushMatrix();{
			float xx,yy,zz;
			glColor3f(1,1,1);
			gluProject( 0.5, -1, 0,
				modelview, projection, viewport, 
				&winX, &winY, &winZ);
			myGLPrintString(winX+5,winY,"tail",0,12);
		} glPopMatrix();
		glPushMatrix();{
			float xx,yy,zz;
			glColor3f(1,1,1);
			gluProject( 0.5, 1, 0,
				modelview, projection, viewport, 
				&winX, &winY, &winZ);
			myGLPrintString(winX+5,winY,"head",0,12);
		} glPopMatrix();
	}
	else if(m_colors.size()>0){
		const int numTags = 9;
		vec3 loc[numTags];
		for(int i = 0;i<numTags;i++){
			loc[i].set(0.5,(float)i/(numTags-1)*2-1,0);
		}
		// draw text icon
		float range = m_endValue-m_startValue;
		for(int i = 0;i<numTags;i++){
			glPushMatrix();{
				glColor3f(1,1,1);
				gluProject( loc[i].x, loc[i].y, loc[i].z,
					modelview, projection, viewport, 
					&winX, &winY, &winZ);
				myGLPrintString(winX+5,winY,convertFloat((float)i/(numTags-1)*range+m_startValue),0,12);
			} glPopMatrix();
		}
	
		// here is the title
		gluProject( 0,1.2, 0,
			modelview, projection, viewport, 
			&winX, &winY, &winZ);
		myGLPrintString(winX+15,winY,m_title,1,16);
	}
}

void BarLegend::SetColors(const vector<rgba> & newColors){
	m_colors.clear();
	m_colors = newColors;
}

void TextureLegend::Render(int type){
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	GLdouble modelview[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	GLdouble projection[16];
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	GLdouble winX, winY, winZ;

	glPushAttrib(GL_LIGHTING_BIT | GL_TEXTURE_BIT);
	
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	int numToShow = 8;
	float xCoord = 1;
	glColor3f(0.9,0.9,0.9);
	for(int i = 0;i<numToShow;i++){
		glBindTexture(GL_TEXTURE_2D,m_textureName[i]);
		//glColor3f(m_colors[i].r,m_colors[i].g,m_colors[i].b);

		float yc1 = (float)i/(numToShow)*2-1;
		float yc2 = (float)(i+1)/(numToShow)*2-1;
		float range = m_endValue-m_startValue;
		float value = 1.f/((float)i/(numToShow-1)*range+m_startValue);
		if(type==2){
			value = 1.f/(range+m_startValue);
		}
		glBegin(GL_POLYGON);{
			glNormal3f(0,0,1);
			glTexCoord2f(0,xCoord);
			glVertex3f(-1,yc1,0);

			glNormal3f(0,0,1);
			glTexCoord2f(0,0);
			glVertex3f(1,yc1,0);

			glNormal3f(0,0,1);
			glTexCoord2f(value,0);
			glVertex3f(1,yc2,0);

			glNormal3f(0,0,1);
			glTexCoord2f(value,xCoord);
			glVertex3f(-1,yc2,0);

		}glEnd();
	}

	glPopAttrib();

	if(numToShow==8){
		const int numTags = 9;
		vec3 loc[numTags];
		for(int i = 0;i<numTags;i++){
			loc[i].set(1,(float)i/(numTags-1)*2-1,0);
		}
		// draw text icon
		float range = m_endValue-m_startValue;
		for(int i = 0;i<numTags;i++){
			glPushMatrix();{
				glColor3f(1,1,1);
				gluProject( loc[i].x, loc[i].y, loc[i].z,
					modelview, projection, viewport, 
					&winX, &winY, &winZ);
				myGLPrintString(winX+5,winY,convertFloat((float)i/(numTags-1)*range+m_startValue),0,12);
			} glPopMatrix();
		}
	
		// here is the title
		gluProject( 0,1.2, 0,
			modelview, projection, viewport, 
			&winX, &winY, &winZ);
		myGLPrintString(winX+15,winY,m_title,1,16);
	}
}