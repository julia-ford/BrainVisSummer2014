#include <cassert>
#include <cmath>
#include <iostream>

#include <gl/glew.h>
#include <GL/glut.h>

#include "ColorConverter.h"
#include "colorscheme.h"
#include "displaybase.h"
#include "myGLexp.h"
#include "mystdexp.h"
#include "ribbon.h"
#include "superquadric.h"
#include "tubesegment.h"

using namespace mystdexp;
using namespace myGLexp;

#define COLORDISCRETE 8
#define COLORCONTINOUS 8
#define TEXHEIGHT 128
#define TEXWIDTH 128

#define OPACITY 1
#define TRAINING_BUNDLE_OPACITY 0.2
#define DEFAULTSIZE 0.35

#define HALOSIZE 1.5
#define HIGHLIGHTHALOSIZE 1.5
#define LINEHALOWIDTH 3
#define LINEWIDTH 1

#define TRAININGBUNDLEQUEST 5

rgba textureBackColor(200.0f/255.0f, 200.0f/255.0f, 200.0f/255.0f);

float DisplayBase::clamp(float v, float min,float max) const{
	return v>max?max:(v<min?min:v);
}

void DisplayBase::encodeTensor(SuperQuadric &sq, const TensorEigen& eigen) const{
	sq.SetAlpha(powf(clamp(1-eigen.GetCp()),sq.GetGama()));
	sq.SetBeta(powf(clamp(1-eigen.GetCl()),sq.GetGama()));
}

DisplayBase::DisplayBase(){
	shapeRenderInterval = 4;
	m_highLightTracedTraces = false;
	m_showLesionBox = false;
}


DisplayBase::~DisplayBase(){
	deleteShapeDisplayLists();
}

// int range 0~7
int faToInt(float fa){
	float hfa = (fa-0.2f)/0.8f;
	if(hfa<0){
		hfa=0;
	}
	if(hfa>1){
		hfa = 0.99;
	}
	hfa*=0.8;
	return hfa/0.1;
}

int DisplayBase::GenerateTrailDisplayList(const TrialInfoPtr trialInfoPtr,const TrialDataPtr trialDataPtr) const{
	// blank trial
	if(trialInfoPtr->IsEmpty()) return -1;

	// return main part
	else if(trialInfoPtr->GetShape() == TUBE){
		return generateTubesDisplayList(trialInfoPtr,trialDataPtr);
	}
	else if(trialInfoPtr->GetShape() == SUPERQUADRIC){
		return generateSuperQuadricsDisplayList(trialInfoPtr,trialDataPtr);
	}
	else if(trialInfoPtr->GetShape() == RIBBON){
		return generateRibbonDisplayList(trialInfoPtr,trialDataPtr);
	}
	else{
		return -1;
	}
}

void DisplayBase::renderText(TrialDataPtr trialDataPtr){
	
	vector<float> boxes;
	trialDataPtr->GetBoxes(boxes);
	int numBoxes = boxes.size()/6;

	// lesion task, don't show 
	// box idx icon
	if(numBoxes ==1){
		return;
	}

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	GLdouble modelview[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	GLdouble projection[16];
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	GLdouble winX, winY, winZ;
	vector<vec3> wins;

	// draw text icon
	for(int i = 0;i<numBoxes;i++){
		vector<float> tmp;
		assign(tmp,boxes,i*6,i*6+2);
		glPushMatrix();{
			gluProject( tmp[0], tmp[1], tmp[2],
				modelview, projection, viewport, 
				&winX, &winY, &winZ);
			wins.push_back(vec3(winX,winY,winZ));
		} glPopMatrix();
	}
	
	glLineWidth(10);
	glDisable(GL_LIGHTING);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();{
		glLoadIdentity();
		glOrtho(0,viewport[2],0,viewport[3],-5,5);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();{
			for(unsigned int i = 0;i<wins.size();i++){
				glLoadIdentity();

				// fixed color
				rgba color = OrdinalColorScheme::GetColor(2);
				glColor3f(color.r,color.g,color.b);
				glTranslatef(wins[i].x,wins[i].y,0);
				glScalef(0.6,0.6,0.6);
				glutStrokeCharacter(GLUT_STROKE_ROMAN,'1'+i);
			}
		}glPopMatrix();
		glMatrixMode(GL_PROJECTION);
	}glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	
	wins.clear();
	for(int i = 0;i<trialDataPtr->GetNumText();i++){
		vec3 tpos = trialDataPtr->GetTextPosition(i);
		glPushMatrix();{
			gluProject( tpos.x, tpos.y, tpos.z,
				modelview, projection, viewport, 
				&winX, &winY, &winZ);
			wins.push_back(vec3(winX,winY,winZ));
		} glPopMatrix();
	}

	for(int i = 0;i<wins.size();i++){
		string tt = trialDataPtr->GetText(i);
		rgba tcolor = trialDataPtr->GetTextColor(i);
		glColor3f(tcolor.r,tcolor.g,tcolor.b);
		myGLPrintString(wins[i].x,wins[i].y,tt,1,30);
	}

	glEnable(GL_LIGHTING);
}

void DisplayBase::DirectDraw(const TrialInfoPtr trialInfoPtr,const TrialDataPtr trialDataPtr){
	if(trialInfoPtr->GetShape() == RIBBON){
		Ribbon dummy;
		if(trialInfoPtr->GetRetinalChannel() == ORIENTATION){
			directDrawRibbonsAlwaysFacingYou(trialInfoPtr,trialDataPtr);
		}
	}
	renderText(trialDataPtr);
}

void DisplayBase::LoadTracesFromFile(const char* fileName, bool reserveZ){
	m_traces.SetZReverse(reserveZ);
	m_traces.BuildFromTensorFile(fileName);
}

void DisplayBase::LoadTrainingTracesFromFile(const char* fileName){
	m_trainingTraces.BuildFromColorFile(fileName);
}
void DisplayBase::BuildDisplayBase(){
	generateShapeDisplayLists();
	makeTubeTexture();
	makeSuperQuadricTexture();
	textureMaps.BuildTextures();
	textureMaps.BuildScaleTexture();
	axesLegend.SetColorTextureMap(&textureMaps);
	axesLegend.BuildDisplayList();
}

void DisplayBase::GetColorSquence(vector<rgba> &colors,const TrialInfoPtr trialInfoPtr){
	if(trialInfoPtr->GetRetinalChannel() == COLOR){
		if(trialInfoPtr->GetMriTask() == FA){
			for(int i = 0; i < COLORDISCRETE; i++){
				// value is a fraction of the color scheme. So, if there are 8
				// colors, color #3 is 3/8 of the way into the color scheme.
				float value = (float)i/(COLORDISCRETE-1);
				rgba color = CoolWarmColorScheme::GetColorDiscrete(value,COLORDISCRETE);
				colors.push_back(color);
			}
		}
	}
	else if(trialInfoPtr->GetRetinalChannel() == SATURATION){
		for(int i = 0;i<COLORCONTINOUS;i++){
			float value = (float)i/(COLORCONTINOUS-1);
			rgba color = SaturationColorScheme::GetColorDiscrete(value,8);
			colors.push_back(color);
		}
	}
}
unsigned int DisplayBase::GetTexture(const int idx) {
	return textureMaps.GetTextureNameByInt(idx);
};

const Traces& DisplayBase::GetTraces() const{
	return m_traces;
}

const Traces& DisplayBase::GetTrainingTraces() const{
	return m_trainingTraces;
}
void DisplayBase::LoadLegend(const char* fileName){
	axesLegend.LoadFromFile(fileName);
}

void DisplayBase::SetupDirectDrawContext(const TrialInfoPtr trialInfoPtr,const TrialDataPtr trialDataPtr) {
	if(trialInfoPtr->GetShape() != RIBBON || trialInfoPtr->GetRetinalChannel() != ORIENTATION){
		// only ribbon always facing you requires
		// direct draw and thus context
		return;
	}
	int numFibers = m_traces.GetNumFibers();
	m_colors.resize(numFibers);
	m_sizes.resize(numFibers);
	m_orientationValue.resize(numFibers);
	for(int t = 0;t<numFibers;t++){
		int numSegs = m_traces.GetNumSegs(t);
		m_colors[t].resize(numSegs);
		m_sizes[t].resize(numSegs);
		m_orientationValue[t].resize(numSegs);
		for(int s = 0;s<numSegs;s++){
			m_colors[t][s] = getColor(trialInfoPtr,trialDataPtr,t,s);
			m_sizes[t][s] = getSize(trialInfoPtr,trialDataPtr,t,s).y;
			if(trialInfoPtr->GetMriTask() == FA){
				m_orientationValue[t][s] = (faToInt(m_traces.GetEigen(t,s).GetFA())+1.f)/8.f;
			}
			else{
				const vec3& pos1 = m_traces.GetPositions()[t].front();
				const vec3& pos2 = m_traces.GetPositions()[t].back();
				vec3 dir = pos1-pos2;
				rgba color = axesLegend.GetColorByDirection(dir);
				int texIdx = ColorConverter::getIdxByBoysColor(color);
				m_orientationValue[t][s] = texIdx/9.f;
			}
		}
	}
}

int DisplayBase::generateTubesDisplayList(const TrialInfoPtr trialInfoPtr,const TrialDataPtr trialDataPtr) const{
	assert(trialInfoPtr->GetShape() == TUBE);
	int gl_displayList = glGenLists(1);
	glNewList(gl_displayList,GL_COMPILE);{
		glPushAttrib(GL_LIGHTING_BIT | GL_TEXTURE_BIT);{
			glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glDisable(GL_LIGHTING);
			drawBoxes(trialDataPtr);
			glPushMatrix();{
				vec3 center = trialDataPtr->GetFiberCenter();
				glTranslatef(-center.x,-center.y,-center.z);
			
				drawSpheres(trialDataPtr);
				glEnable(GL_LIGHTING);
				drawTubes(trialInfoPtr,trialDataPtr);
			}glPopMatrix();
			glDisable(GL_CULL_FACE);


			// draw halo
			glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
			glLineWidth(HALOSIZE);
			glDisable(GL_LIGHTING);
			glPushMatrix();{
				vec3 center = trialDataPtr->GetFiberCenter();
				glTranslatef(-center.x,-center.y,-center.z);
				drawTubes(trialInfoPtr,trialDataPtr,true);
				glEnable(GL_LIGHTING);
			}glPopMatrix();
			glDisable(GL_CULL_FACE);
		
		}glPopAttrib();
	}glEndList();
	return gl_displayList;
}

int DisplayBase::generateSuperQuadricsDisplayList(const TrialInfoPtr trialInfoPtr,const TrialDataPtr trialDataPtr) const{
	int gl_displayList = glGenLists(1);
	glNewList(gl_displayList,GL_COMPILE);{
		glPushAttrib(GL_LIGHTING_BIT | GL_TEXTURE_BIT);{
			glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
			glDisable(GL_LIGHTING);
			drawBoxes(trialDataPtr);
			glPushMatrix();{
				vec3 center = trialDataPtr->GetFiberCenter();
				glTranslatef(-center.x,-center.y,-center.z);
				drawSpheres(trialDataPtr);
				//drawHighlightLines(trialInfoPtr,trialDataPtr);//keqin
				glEnable(GL_LIGHTING);
				drawSuperQuadrics(trialInfoPtr,trialDataPtr);
				glDisable(GL_LIGHTING);
				drawUnHighlightLines(trialInfoPtr,trialDataPtr);
			}glPopMatrix();
			glDisable(GL_CULL_FACE);

		
			// now the halo
			glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glLineWidth(HALOSIZE);
			glDisable(GL_LIGHTING);
			glPushMatrix();{
				vec3 center = trialDataPtr->GetFiberCenter();
				glTranslatef(-center.x,-center.y,-center.z);
				drawSuperQuadrics(trialInfoPtr,trialDataPtr,true);
			}glPopMatrix();
			glDisable(GL_CULL_FACE);
		}
	}glEndList();
	return gl_displayList;
}

int DisplayBase::generateRibbonDisplayList(const TrialInfoPtr trialInfoPtr,const TrialDataPtr trialDataPtr) const{
	int gl_displayList = glGenLists(1);
	glNewList(gl_displayList,GL_COMPILE);{
		glPushAttrib(GL_LIGHTING_BIT | GL_TEXTURE_BIT);{
			glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
			glDisable(GL_CULL_FACE);
			glDisable(GL_LIGHTING);
			drawBoxes(trialDataPtr);
			glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
			glPushMatrix();{
				vec3 center = trialDataPtr->GetFiberCenter();
				
			//	glRotatef(270,0,0,1);
				//glRotatef(-90,0,1,0); //keqin
				glTranslatef(-center.x,-center.y,-center.z);
				//drawSpheres(trialDataPtr);
				//glEnable(GL_LIGHTING);
			
				drawRibbons(trialInfoPtr,trialDataPtr);
			
				//glDisable(GL_LIGHTING);
			}glPopMatrix();
			glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
		}
	}glEndList();
	return gl_displayList;
}

void DisplayBase::drawHighlightLines(const TrialInfoPtr trialInfoPtr,const TrialDataPtr trialDataPtr) const{
	glPushAttrib(GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT);{
		glDisable(GL_LIGHTING);
		glLineWidth(LINEWIDTH);
		for(int ft = 0;ft<trialDataPtr->GetNumFibers();ft++){
			int t = trialDataPtr->GetFibers(ft);

			if(!trialDataPtr->HighlightFiber(t)){
				continue;
			}
			
			int numSegs = trialDataPtr->GetTraces().GetNumSegs(t);
			glBegin(GL_LINES);{
				for(int s = 0;s<numSegs-1;s++){
					vec3 p1 = trialDataPtr->GetTraces().GetPosition(t,s);
					vec3 p2 = trialDataPtr->GetTraces().GetPosition(t,s+1);
					if(trialInfoPtr->GetMriTask() == LESION){
						if(trialDataPtr->InBox(p1,0)) {
							continue;
						}
						if(trialDataPtr->InBox(p2,0)) {
							continue;
						}
					}
					rgba color1, color2;
					// ensure line color is the same as the texture
					if((trialInfoPtr->GetMriTask() == BUNDLE_TRACE 
						|| (trialInfoPtr->GetMriTask() == BUNDLE_SAME && trialDataPtr->HighlightFiber(t))
						|| (trialInfoPtr->GetMriTask() == BUNDLE_NAME && trialDataPtr->HighlightFiber(t))
						|| (trialInfoPtr->GetMriTask() == FA && trialInfoPtr->textype == 2)
						|| trialInfoPtr->GetMriTask() == LESION)
						&& trialInfoPtr->GetRetinalChannel() == TEXTURE){
						color1 = textureBackColor;
						color2 = textureBackColor;
					}
					else{
						color1 = getColor(trialInfoPtr,trialDataPtr,t,s);
						color2 = getColor(trialInfoPtr,trialDataPtr,t,s+1);
					}
					
					glColor3f(color1.r,color1.g,color1.b);
					glVertex3f(p1.x,p1.y,p1.z);
					glColor3f(color2.r,color2.g,color2.b);
					glVertex3f(p2.x,p2.y,p2.z);
				}
			}glEnd();
			
		}
		
		glLineWidth(LINEHALOWIDTH);
		for(int ft = 0;ft<trialDataPtr->GetNumFibers();ft++){
			int t = trialDataPtr->GetFibers(ft);

			if(!trialDataPtr->HighlightFiber(t)){
				continue;
			}
			
			rgba color = FiberColorScheme::GetHighlightColor();
			glColor3f(color.r,color.g,color.b);
			int numSegs = trialDataPtr->GetTraces().GetNumSegs(t);
			glBegin(GL_LINES);{
				for(int s = 0;s<numSegs-1;s++){
					vec3 p1 = trialDataPtr->GetTraces().GetPosition(t,s);
					vec3 p2 = trialDataPtr->GetTraces().GetPosition(t,s+1);

					if(trialInfoPtr->GetMriTask() == LESION){
						if(trialDataPtr->InBox(p1,0)) {
							continue;
						}
						if(trialDataPtr->InBox(p2,0)) {
							continue;
						}
					}
					
					glVertex3f(p1.x,p1.y,p1.z);
					glVertex3f(p2.x,p2.y,p2.z);
				}
			}glEnd();
		}
	}glPopAttrib();
}

void DisplayBase::drawUnHighlightLines(const TrialInfoPtr trialInfoPtr,const TrialDataPtr trialDataPtr) const{
	glPushAttrib(GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT);{
		if(trialInfoPtr->GetMriTask() == BUNDLE_SAME){
			glEnable(GL_ALPHA_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		}
		glDisable(GL_LIGHTING);
		

		glLineWidth(LINEWIDTH);
		for(int ft = 0;ft<trialDataPtr->GetNumFibers();ft++){
			int t = trialDataPtr->GetFibers(ft);

			/*if(trialDataPtr->HighlightFiber(t)){
				continue;
			}*///keqin

			int numSegs = trialDataPtr->GetTraces().GetNumSegs(t);

			glBegin(GL_LINES);{
				for(int s = 0;s<numSegs-1;s++){
					vec3 p1 = trialDataPtr->GetTraces().GetPosition(t,s);
					vec3 p2 = trialDataPtr->GetTraces().GetPosition(t,s+1);
					if(trialInfoPtr->GetMriTask() == LESION){
						if(trialDataPtr->InBox(p1,0)) {
							continue;
						}
						if(trialDataPtr->InBox(p2,0)) {
							continue;
						}
					}
					rgba color1, color2;
					// ensure line color is the same as the texture
					if((trialInfoPtr->GetMriTask() == BUNDLE_TRACE 
						|| (trialInfoPtr->GetMriTask() == BUNDLE_SAME && trialDataPtr->HighlightFiber(t))
						|| (trialInfoPtr->GetMriTask() == BUNDLE_NAME && trialDataPtr->HighlightFiber(t))
						|| (trialInfoPtr->GetMriTask() == FA && trialInfoPtr->textype == 2)
						|| trialInfoPtr->GetMriTask() == LESION)
						&& trialInfoPtr->GetRetinalChannel() == TEXTURE){
						color1 = textureBackColor;
						color2 = textureBackColor;
					}
					else{
						color1 = getColor(trialInfoPtr,trialDataPtr,t,s);
						color2 = getColor(trialInfoPtr,trialDataPtr,t,s+1);
					}
					
					glColor3f(color1.r,color1.g,color1.b);
					glVertex3f(p1.x,p1.y,p1.z);
					glColor3f(color2.r,color2.g,color2.b);
					glVertex3f(p2.x,p2.y,p2.z);
				}
			}glEnd();

		}

		glLineWidth(LINEHALOWIDTH);
		//rgba color = FiberColorScheme::GetHaloColor();
		rgba color = rgba(0,0,0);
		glColor3f(color.r,color.g,color.b);
		for(int ft = 0;ft<trialDataPtr->GetNumFibers();ft++){
			int t = trialDataPtr->GetFibers(ft);
			if(trialDataPtr->HighlightFiber(t)){
				continue;
			}

			int numSegs = trialDataPtr->GetTraces().GetNumSegs(t);
			glBegin(GL_LINES);{
				for(int s = 0;s<numSegs-1;s++){
					vec3 p1 = trialDataPtr->GetTraces().GetPosition(t,s);
					vec3 p2 = trialDataPtr->GetTraces().GetPosition(t,s+1);
					
					if(trialInfoPtr->GetMriTask() == LESION){
						if(trialDataPtr->InBox(p1,0)) {
							continue;
						}
						if(trialDataPtr->InBox(p2,0)) {
							continue;
						}
					}
					glVertex3f(p1.x,p1.y,p1.z);
					glVertex3f(p2.x,p2.y,p2.z);
				}
			}glEnd();
		}
	}glPopAttrib();
}
void DisplayBase::directDrawRibbonsAlwaysFacingYou(const TrialInfoPtr trialInfoPtr,const TrialDataPtr trialDataPtr) const{
	Ribbon dummy;
	
	
	glPushAttrib(GL_LIGHTING_BIT | GL_TEXTURE_BIT);
	glDisable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glPushMatrix();{
		vec3 center = trialDataPtr->GetFiberCenter();
		glTranslatef(-center.x,-center.y,-center.z);
		
		// ribbons always face you
		float modelViewMat[4][4];
		float modelViewTranspose[4][4];
		glGetFloatv(GL_MODELVIEW_MATRIX,(GLfloat*)modelViewMat);
		myGLexp::MatrixTranspose4x4(modelViewMat,modelViewTranspose);

		// no fiber is longer than 1000 samples!!!
		vector<vec3> normals(1000);
		for(int tt=0;tt<trialDataPtr->GetNumFibers();tt++){
			int t = trialDataPtr->GetFibers(tt);
			dummy.SetDrawhalo(true);
			if(trialDataPtr->HighlightFiber(t)){
				glLineWidth(HIGHLIGHTHALOSIZE);
				dummy.SetHaloColor(FiberColorScheme::GetHighlightColor());
			}
			else{
				glLineWidth(HALOSIZE);
				dummy.SetHaloColor(FiberColorScheme::GetHaloColor());
			}

			dummy.SetPositions(&(m_traces.GetPositions()[t]));
			dummy.SetColors(&(m_colors[t]));
			dummy.SetSizes(&m_sizes[t]);
			int numSegs = m_traces.GetNumSegs(t);
			for(int s = 0;s<numSegs;s++){
				vec3 pos = m_traces.GetPosition(t,s);;
				float objPos[4] = {pos.x,pos.y,pos.z,1};
				float eyePos[4];
				float norm[4];

				// in eye space, pos == norm
				myGLexp::esMultiply4x4(modelViewMat,objPos,eyePos);
				eyePos[3]=0;

				// now add the rotating part
				float orientationValue = m_orientationValue[t][s];
				float rotateMat[3][3];
				vec3 objDir = m_traces.GetDirection(t,s);
				float objdir[4] = {objDir.x,objDir.y,objDir.z,0};
				float eyeDir[4];
				myGLexp::esMultiply4x4(modelViewMat,objdir,eyeDir);
				float up[3];
				float right[3];
				myGLexp::crossProduct(eyePos,eyeDir,up);
				myGLexp::crossProduct(up,eyePos,right);
				myGLexp::rotateMatrix3x3(rotateMat,(1-orientationValue)*myGLexp::PI/2,right[0],right[1],right[2]);
				myGLexp::esMultiply3x3(rotateMat,eyePos,eyePos);
				// end of rotation

				// go back to obj sapce
				myGLexp::esMultiply4x4(modelViewTranspose,eyePos,norm);
				normals[s].set(norm[0],norm[1],norm[2]);

				normals[s].normalize();
			}
			int cIdx = trialDataPtr->GetFiberCutIndex(t);
			if(cIdx>=0){
				const vector<int>& lefts = trialDataPtr->GetLeftSegmentIndices(cIdx);
				dummy.DrawGeometry(normals,lefts);
			}
			else{
				vector<int> all;
				all.clear();
				all.push_back(0);
				all.push_back(numSegs-1);
				dummy.DrawGeometry(normals,all);
			}
		}
	}glPopMatrix();
	glPopAttrib();
}

void DisplayBase::handleTextureOverall(const TrialInfoPtr trialInfoPtr,const TrialDataPtr trialDataPtr) const{
	if(trialInfoPtr->GetRetinalChannel() == TEXTURE){
		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE  );
		switch(trialInfoPtr->GetShape()){
			case RIBBON:
			case TUBE:
				glBindTexture(GL_TEXTURE_2D, tubeTexture);
				break;
			case SUPERQUADRIC:
				glBindTexture(GL_TEXTURE_2D, superQuadricTexture);
				break;
			default:
				break;
		}
		if(trialInfoPtr->textype == 2 && trialInfoPtr->GetMriTask()==FA ){
			GLuint texName = textureMaps.GetScaleTextureName();
			glBindTexture(GL_TEXTURE_2D, texName);
		}


	}
}

void DisplayBase::handleTextureByFiber(const TrialInfoPtr trialInfoPtr,const TrialDataPtr trialDataPtr, const int traceIdx) const{
	int numSegs = trialDataPtr->GetTraces().GetNumSegs(traceIdx);
	if(//trialDataPtr->HighlightFiber(t) 
		(trialInfoPtr->GetMriTask()==BUNDLE_SAME 
		||trialInfoPtr->GetMriTask()==BUNDLE_NAME)
		&& trialInfoPtr->GetRetinalChannel() == TEXTURE){
		glEnable(GL_TEXTURE_2D);
		rgba boyColor = axesLegend.GetColorByDirection(trialDataPtr->GetTraces().GetPosition(traceIdx,0)
			-trialDataPtr->GetTraces().GetPosition(traceIdx,numSegs-1));
		GLuint texName = textureMaps.GetTextureNameByColor(boyColor);
		glBindTexture(GL_TEXTURE_2D, texName);
		//if(trialInfoPtr->GetShape() != SUPERQUADRIC){
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		//}
		//else{
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//}
	}
	else if((trialInfoPtr->GetMriTask()==BUNDLE_TRACE ||trialInfoPtr->GetMriTask()==LESION)
		&& trialInfoPtr->GetRetinalChannel() == TEXTURE){
		glEnable(GL_TEXTURE_2D);
		rgba boyColor = axesLegend.GetColorByDirection(trialDataPtr->GetTraces().GetPosition(traceIdx,0)
			-trialDataPtr->GetTraces().GetPosition(traceIdx,numSegs-1));
		GLuint texName = textureMaps.GetTextureNameByColor(boyColor);
		glBindTexture(GL_TEXTURE_2D, texName);
		//if(trialInfoPtr->GetShape() != SUPERQUADRIC){
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		//}
		//else{
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//}
	}
	else if(trialInfoPtr->GetMriTask()==FA 
		&& trialInfoPtr->GetRetinalChannel() == TEXTURE){
		//glEnable(GL_TEXTURE_2D);
	}
	else{
		glDisable(GL_TEXTURE_2D);
	}
}

//void DisplayBase::handleTextureBySegment(const TrialInfoPtr trialInfoPtr,const TrialDataPtr trialDataPtr, const int traceIdx, const int segIdx) const{
//	if(trialInfoPtr->GetMriTask() == FA 
//		&& trialInfoPtr->GetRetinalChannel() == TEXTURE
//		&& trialInfoPtr->textype == 2){
//		GLuint texName = textureMaps.GetScaleTextureName();
//		glBindTexture(GL_TEXTURE_2D,texName);
//	}
//}

void DisplayBase::generateShapeDisplayLists(){
	deleteShapeDisplayLists();
	//m_superQuadrics_fixed.resize(m_traces.GetNumFibers());

	m_sqs.resize(m_traces.GetNumFibers());
	for(int i = 0;i<m_traces.GetNumFibers();i++){
		int numSegs = m_traces.GetNumSegs(i);
		m_sqs[i].resize(numSegs);
		//m_superQuadrics_fixed[i].resize(numSegs,-1);
		for(int j = 0;j<numSegs;j+=shapeRenderInterval){
			SuperQuadric &sq = m_sqs[i][j];
			sq.SetGama(3);
			encodeTensor(sq,m_traces.GetEigen(i,j));

			sq.SetPosition(m_traces.GetPosition(i,j));
			sq.SetDirection(m_traces.GetDirection(i,j).normalized());
			sq.SetDetail(6);

			// fixed
			sq.SetSize(vec3(DEFAULTSIZE,DEFAULTSIZE,DEFAULTSIZE));
			sq.SetFAForTexureCoord(1);
			int fixedDL = glGenLists(1);
			//glNewList(fixedDL,GL_COMPILE);{
			//	sq.Draw();
			//}glEndList();
			//m_superQuadrics_fixed[i][j]=fixedDL;
			sq.Build();

		}

		if (i%50==0)std::cerr << "\rDisplayBase: Building Display list: "<< (float)i/(m_traces.GetNumFibers()-1)*100 <<"%   ";
	}
	std::cerr <<  "\rDisplayBase: Building Display list: "<< "100%         "<< endl;
}

void DisplayBase::drawBoxes(TrialDataPtr trialDataPtr) const{
	if(trialDataPtr->GetNumBoxes()==1){
		// lesion task
		if(!m_showLesionBox){
			return;
		}
	}
	AlignedBoundingBox box;
	vector<float> boxes;
	trialDataPtr->GetBoxes(boxes);
	int numBoxes = boxes.size()/6;
	for(int i = 0;i<numBoxes;i++){
		vector<float> tmp;
		assign(tmp,boxes,i*6,i*6+5);
		box.build(tmp);
		box.Render();
	}

}

void DisplayBase::drawSpheres(TrialDataPtr trialDataPtr) const{
	vector<int> spheres;
	
	trialDataPtr->GetSpheres(spheres);
	int numSpheres = spheres.size()/2;
	glColor3f(1,1,0);
	for(int i = 0;i<numSpheres;i++){
		vec3 location;
		int idx = spheres[i*2];
		int tail = spheres[i*2+1];
		int numSegs = trialDataPtr->GetTraces().GetNumSegs(idx);
		if(tail == 0){
			// head
			float fa = trialDataPtr->GetTraces().GetEigen(idx,0).GetFA();
			location = trialDataPtr->GetTraces().GetPosition(idx,0);
			location += -trialDataPtr->GetTraces().GetDirection(idx,0)*fa;
		}
		else{
			// tail
			float fa = trialDataPtr->GetTraces().GetEigen(idx,numSegs-1).GetFA();
			location = trialDataPtr->GetTraces().GetPosition(idx,numSegs-1);
			location += trialDataPtr->GetTraces().GetDirection(idx,numSegs-1)*fa;
		}
		glPushMatrix();{
			glTranslatef(location.x,location.y,location.z);
			glutSolidSphere(0.4,10,10);
		}glPopMatrix();
	}
}

void DisplayBase::drawTubeSegment(int tubeIdx, int segIdx, const TrialInfoPtr trialInfoPtr,const TrialDataPtr trialDataPtr,bool isHalo) const{
	static TubeSegment dummyts;
	// set color
	vec3 pos1 = trialDataPtr->GetTraces().GetPosition(tubeIdx,segIdx);
	vec3 pos2 = trialDataPtr->GetTraces().GetPosition(tubeIdx,segIdx+1);

	if(trialInfoPtr->GetMriTask() == LESION){
		if(trialDataPtr->InBox(pos1,0)) {
			return;
		}
		if(trialDataPtr->InBox(pos2,0)) {
			return;
		}
	}
	int numSegsPerTube = 6;
	int numSegs = trialDataPtr->GetTraces().GetNumSegs(tubeIdx);
	vec3 dir1 = trialDataPtr->GetTraces().GetDirection(tubeIdx,segIdx);
	vec3 dir2 = trialDataPtr->GetTraces().GetDirection(tubeIdx,segIdx+1);
	vec3 size1 = getSize(trialInfoPtr,trialDataPtr,tubeIdx,segIdx);
	vec3 size2 = getSize(trialInfoPtr,trialDataPtr,tubeIdx,segIdx+1);
	rgba color1,color2;
	if(!isHalo){
		color1 = getColor(trialInfoPtr,trialDataPtr,tubeIdx,segIdx);
		color2 = getColor(trialInfoPtr,trialDataPtr,tubeIdx,segIdx+1);
	}
	else{
		if(trialDataPtr->HighlightFiber(tubeIdx)){
			color1 = FiberColorScheme::GetHighlightColor();
			color2 = FiberColorScheme::GetHighlightColor();
		}
		else{
			color1 = FiberColorScheme::GetHaloColor();
			color2 = FiberColorScheme::GetHaloColor();
		}
	}

	dummyts.SetSegments(numSegsPerTube);
	dummyts.SetDirections(dir1,dir2);
	dummyts.SetSizes(size1.x,size2.x);
	dummyts.SetColors(color1,color2);
	dummyts.SetPositions(pos1,pos2);

	if(segIdx==0){
		dummyts.SetCaps(true,false);
	}
	else if(segIdx==numSegs-2 ){
		dummyts.SetCaps(false,true);
	}
	else{
		dummyts.SetCaps(false,false);
	}

	if(isHalo){
		dummyts.SetCaps(false,false);
	}
	
	if(trialInfoPtr->GetRetinalChannel() == TEXTURE){
		if(trialInfoPtr->GetMriTask() == FA){
			if(trialInfoPtr->textype == 1){
				float fa = trialDataPtr->GetTraces().GetEigen(tubeIdx,segIdx).GetFA();
				float maxU = 1/fa;
				if(maxU>10) maxU=10;
				if(maxU<1) maxU=1;
				dummyts.SetU(0,maxU);
			}
			else{
				float fa = trialDataPtr->GetTraces().GetEigen(tubeIdx,segIdx).GetFA();
				float offset = faToInt(fa);
				dummyts.SetU(offset/8.f,(offset+1)/8.f);
			}
		}
		else{
			dummyts.SetU(0,1);
		}
	}
	else{
		dummyts.SetU(0,1);
	}
	dummyts.DrawGeometry();
}


void DisplayBase::drawTubeSegment(int tubeIdx, int segIdx, const TrialInfoPtr trialInfoPtr,const TrialDataPtr trialDataPtr,
	vector<float> &vertices, vector<float> &texCoords,
		vector<unsigned char> &colors, vector<float> &normals,
		vector<float> &capvertices, vector<float> &captexCoords,
		vector<unsigned char> &capcolors, vector<float> &capnormals) const{
	static TubeSegment dummyts;
	// set color
	int numSegsPerTube = 6;
	int numSegs = trialDataPtr->GetTraces().GetNumSegs(tubeIdx);
	vec3 dir1 = trialDataPtr->GetTraces().GetDirection(tubeIdx,segIdx);
	vec3 dir2 = trialDataPtr->GetTraces().GetDirection(tubeIdx,segIdx+1);
	rgba color1 = getColor(trialInfoPtr,trialDataPtr,tubeIdx,segIdx);
	rgba color2 = getColor(trialInfoPtr,trialDataPtr,tubeIdx,segIdx+1);
	vec3 size1 = getSize(trialInfoPtr,trialDataPtr,tubeIdx,segIdx);
	vec3 size2 = getSize(trialInfoPtr,trialDataPtr,tubeIdx,segIdx+1);
	vec3 pos1 = trialDataPtr->GetTraces().GetPosition(tubeIdx,segIdx);
	vec3 pos2 = trialDataPtr->GetTraces().GetPosition(tubeIdx,segIdx+1);

	dummyts.SetSegments(numSegsPerTube);
	dummyts.SetDirections(dir1,dir2);
	dummyts.SetSizes(size1.x,size2.x);
	dummyts.SetColors(color1,color2);
	dummyts.SetPositions(pos1,pos2);

	if(segIdx==0){
		dummyts.SetCaps(true,false);
	}
	else if(segIdx==numSegs-2){
		dummyts.SetCaps(false,true);
	}
				
	if(trialInfoPtr->GetRetinalChannel() == TEXTURE && trialInfoPtr->GetMriTask() == FA){
		float fa = trialDataPtr->GetTraces().GetEigen(tubeIdx,segIdx).GetFA();
		float maxU = 1/fa;
		if(maxU>10) maxU=10;
		if(maxU<1) maxU=1;
		dummyts.SetU(0,maxU);
	}
	else{
		dummyts.SetU(0,1);
	}
	dummyts.DrawHere(vertices, texCoords,
		colors, normals,
		capvertices, captexCoords,
		capcolors, capnormals);
}

void DisplayBase::drawTubes(const TrialInfoPtr trialInfoPtr,const TrialDataPtr trialDataPtr, bool isHalo) const{
	glPushAttrib(GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT);{
		handleTextureOverall(trialInfoPtr,trialDataPtr);
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glScalef(1,6,1);
		glMatrixMode(GL_MODELVIEW);

		// sort highlight first
		vector<int> sortedFibers;
		if(trialInfoPtr->GetMriTask() == BUNDLE_SAME){
			//glEnable(GL_ALPHA_TEST);
			//glEnable(GL_BLEND);
			//glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			// need to sort

			sortedFibers.clear();
			vector<int> transparentIdx;
			for(int i = 0;i<trialDataPtr->GetNumFibers();i++){
				int t = trialDataPtr->GetFibers(i);
				if(trialDataPtr->HighlightFiber(t)){
					sortedFibers.push_back(t);
				}
				else{
					transparentIdx.push_back(t);
				}
			}
			for(int i = 0;i<transparentIdx.size();i++){
				sortedFibers.push_back(transparentIdx[i]);
			}
		}
		else if(trialInfoPtr->GetQuestIndex() == TRAININGBUNDLEQUEST){
			glEnable(GL_ALPHA_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			// need to sort

			sortedFibers.clear();
			vector<int> transparentIdx;
			for(int i = 0;i<trialDataPtr->GetNumFibers();i++){
				int t = trialDataPtr->GetFibers(i);
				if(trialDataPtr->GetTraces().GetColor(t,0).isSimiliar(rgba(1,1,1),0.1)){
					// transparent
					transparentIdx.push_back(t);
				}
				else{
					sortedFibers.push_back(t);
				}
			}
			for(int i = 0;i<transparentIdx.size() && !isHalo;i++){
				sortedFibers.push_back(transparentIdx[i]);
			}
		}
		else {
			trialDataPtr->GetAllFibers(sortedFibers);
		}

		for(int ft = 0;ft<sortedFibers.size();ft++){
			int t = sortedFibers[ft];
			int numSegs = trialDataPtr->GetTraces().GetNumSegs(t);
			
			handleTextureByFiber(trialInfoPtr,trialDataPtr,t);

			if(isHalo){
				if(trialDataPtr->HighlightFiber(t)){
					glLineWidth(HIGHLIGHTHALOSIZE);
				}
				else{
					glLineWidth(HALOSIZE);
				}
			}
			for(int s = 0;s<numSegs-1;s++){

				drawTubeSegment(t,s,trialInfoPtr,trialDataPtr,isHalo);
			}
		}
	}glPopAttrib();
}

void DisplayBase::drawRibbons(const TrialInfoPtr trialInfoPtr,const TrialDataPtr trialDataPtr) const{
	Ribbon dummy;

	dummy.SetDrawhalo(true);
	
	// no fibers has more than 1000 segments!!!
	vector<rgba> colors(1000);
	vector<float> sizes(1000);
	glPushAttrib(GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT);{
		
		handleTextureOverall(trialInfoPtr,trialDataPtr);
		if(trialInfoPtr->GetRetinalChannel() == TEXTURE){
			if(trialInfoPtr->GetMriTask() != FA){
				glMatrixMode(GL_TEXTURE);
				glLoadIdentity();
				glMatrixMode(GL_MODELVIEW);
			}
		}
		
		if(trialInfoPtr->GetRetinalChannel() != ORIENTATION){
			for(int tt=0;tt<trialDataPtr->GetNumFibers();tt++){

				int t = trialDataPtr->GetFibers(tt);

				
				handleTextureByFiber(trialInfoPtr,trialDataPtr,t);
				
				if(trialDataPtr->HighlightFiber(t)){
					dummy.SetHaloColor(FiberColorScheme::GetHighlightColor());
					glLineWidth(HALOSIZE);
				}
				else{
					dummy.SetHaloColor(FiberColorScheme::GetHaloColor());
					glLineWidth(HALOSIZE);
				}

				dummy.SetPositions(&(m_traces.GetPositions()[t]));
				int numSegs = m_traces.GetNumSegs(t);

				vector<float> us(numSegs*2,1.f);
				for(int i = 0;i<numSegs;i++){
					us[i*2]=0;
				}
				if(trialInfoPtr->GetMriTask()==FA && trialInfoPtr->GetRetinalChannel() == TEXTURE) {
					if(trialInfoPtr->textype == 1){
						for(int s = 0;s<numSegs;s++){
							float fa = trialDataPtr->GetTraces().GetEigen(t,s).GetFA();
							float intFA = faToInt(fa);
							float maxU = 1/intFA;
							if(maxU>10) maxU=10;
							if(maxU<1) maxU=1;
							us[s*2+1] = maxU;
							us[s*2] = 0;
						}
					}
					else{						
						for(int s = 0;s<numSegs;s++){
							float fa = trialDataPtr->GetTraces().GetEigen(t,s).GetFA();
							float intFA = faToInt(fa);
							us[s*2+1] = (intFA+1)/8.f;
							us[s*2] = intFA/8.f;
						}
					}
				}
				dummy.SetUs(&us);

				for(int s = 0;s<numSegs;s++){
					colors[s] = getColor(trialInfoPtr,trialDataPtr,t,s);
					sizes[s] = getSize(trialInfoPtr,trialDataPtr,t,s).y;
				}
				dummy.SetColors(&colors);
				dummy.SetSizes(&sizes);
				vec3 pos1 = m_traces.GetPosition(t,0);
				vec3 pos2 = m_traces.GetPositions()[t].back();
				vec3 pos3 = m_traces.GetPositions()[t][m_traces.GetNumSegs(t)/2];
				vec3 norm = (pos2-pos1)^(pos3-pos1);
				norm.normalize();
				int cIdx = trialDataPtr->GetFiberCutIndex(t);
				if(cIdx>=0){
					const vector<int>& lefts = trialDataPtr->GetLeftSegmentIndices(cIdx);
					dummy.DrawGeometry(norm,lefts);
				}
				else{
					vector<int> all;
					all.clear();
					all.push_back(0);
					all.push_back(numSegs-1);
					dummy.DrawGeometry(norm,all);
				}
			}
		}
	}glPopAttrib();
}

void DisplayBase::drawHighlightTubeGeometry(const TrialInfoPtr trialInfoPtr,const TrialDataPtr trialDataPtr) const{
	glPushAttrib(GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT);{

		for(int ft = 0;ft<trialDataPtr->GetNumFibers();ft++){
			int t = trialDataPtr->GetFibers(ft);
			if(trialDataPtr->HighlightFiber(t)){
				int numSegs = trialDataPtr->GetTraces().GetNumSegs(t);

				for(int s = 0;s<numSegs-1;s++){
					drawTubeSegment(t,s,trialInfoPtr,trialDataPtr,true);
					//drawTubeSegment(t,s,trialInfoPtr,trialDataPtr,
					//	vertices,texCoords,colors,normals,
					//	capvertices,captexCoords,capcolors,capnormals);
				}
			}
		}
	}glPopAttrib();
}

// problem:
// client has to set texture for each
// individual fiber by orientation
void DisplayBase::drawSuperQuadrics(const TrialInfoPtr trialInfoPtr,const TrialDataPtr trialDataPtr, bool isHalo) const{
	glPushAttrib(GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT);{
		
		handleTextureOverall(trialInfoPtr,trialDataPtr);
		if(trialInfoPtr->GetRetinalChannel() == TEXTURE){
			if(trialInfoPtr->GetMriTask() != FA){
				glMatrixMode(GL_TEXTURE);
				glLoadIdentity();
				if(trialInfoPtr->textype == 1){
					glScalef(5,6,1);
					glTranslatef(-0.4,0,0);
				}
				else{
					//glScalef(1,6,1);
				}
				glMatrixMode(GL_MODELVIEW);
			}
		}
		// sort highlight first
		vector<int> sortedFibers;
		if(trialInfoPtr->GetMriTask() == BUNDLE_SAME){
			//glEnable(GL_ALPHA_TEST);
			//glEnable(GL_BLEND);
			//glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			// need to sort
			sortedFibers.clear();
			vector<int> transparentIdx;
			for(int i = 0;i<trialDataPtr->GetNumFibers();i++){
				int t = trialDataPtr->GetFibers(i);
				if(!trialDataPtr->HighlightFiber(t)){
					// transparent
					transparentIdx.push_back(t);
				}
				else{
					sortedFibers.push_back(t);
				}
			}
			for(int i = 0;i<transparentIdx.size();i++){
				sortedFibers.push_back(transparentIdx[i]);
			}
		}
		else trialDataPtr->GetAllFibers(sortedFibers);

		
		for(int ft = 0;ft<trialDataPtr->GetNumFibers();ft++){
			int t = sortedFibers[ft];
			int numSegs = trialDataPtr->GetTraces().GetNumSegs(t);

			// set up client data
			vector<float> normals;
			vector<float> texCoords;
			vector<float> vertices;
			vector<unsigned char> colors;
			vector<unsigned int> indices;
			int indicesOffset = 0;
			const int pointsPerShape = 66*2;
			int numSamplesOnThisFiber = trialDataPtr->GetTraces().GetNumSegs(t)/shapeRenderInterval+1;
			normals.reserve(numSamplesOnThisFiber*pointsPerShape*3);
			texCoords.reserve(numSamplesOnThisFiber*pointsPerShape*2);
			vertices.reserve(numSamplesOnThisFiber*pointsPerShape*3);
			colors.reserve(numSamplesOnThisFiber*pointsPerShape*4);
			//indices.reserve(numSamplesOnThisFiber*pointsPerShape*2);

			// set up texture
			handleTextureByFiber(trialInfoPtr,trialDataPtr,t);

			// start drawing
			for(int s = 0;s<numSegs;s+=shapeRenderInterval){
				
				vec3 pos = trialDataPtr->GetTraces().GetPosition(t,s);

				// do not draw lesion area
				if(trialInfoPtr->GetMriTask() == LESION){
					if(trialDataPtr->InBox(pos,0)) {
						continue;
					}
				}

				// set color
				// and line width if halo
				if(isHalo){
					rgba color;
					if(trialDataPtr->HighlightFiber(t)){
						color=FiberColorScheme::GetHighlightColor();
						glLineWidth(HIGHLIGHTHALOSIZE);
					}
					else{
						color=FiberColorScheme::GetHaloColor();
						glLineWidth(HALOSIZE);
					}

					for(int i = 0;i<pointsPerShape;i++){
						colors.push_back(color.r*255);
						colors.push_back(color.g*255);
						colors.push_back(color.b*255);
						colors.push_back(color.a*255);
					}
				}
				else{
					rgba color=getColor(trialInfoPtr,trialDataPtr,t,s);
					for(int i = 0;i<pointsPerShape;i++){
						colors.push_back(color.r*255);
						colors.push_back(color.g*255);
						colors.push_back(color.b*255);
						colors.push_back(color.a*255);
					}
				}


				// get geometry by list
				float scaleSize = 1;
				//int thisSuperQuadricList = m_superQuadrics_fixed[t][s];
				if(!isHalo){
					glEnable(GL_LIGHTING);
				}
				if(trialInfoPtr->GetRetinalChannel() == SIZE){
					if(trialInfoPtr->GetMriTask() == FA){
						float fa = trialDataPtr->GetTraces().GetEigen(t,s).GetFA();
						vec3 pos = trialDataPtr->GetTraces().GetPosition(t,s);
							int startIdx = vertices.size();
							m_sqs[t][s].AppendShape(vertices,texCoords,normals);
							//m_sqs[t][s].AppendShape(vertices,texCoords,normals,indices,indicesOffset);
							for(int i = startIdx;i<vertices.size();i+=3){
								vertices[i] *= fa*2*scaleSize;
								vertices[i+1] *= fa*2*scaleSize;
								vertices[i+2] *= fa*2*scaleSize;
								vertices[i] += pos.x;
								vertices[i+1] += pos.y;
								vertices[i+2] += pos.z;
							}
						//}
						//glPopMatrix();
					}
					else{
						// from 0.05~0.35
						float idxf = (float)s/(numSegs-1)*(6.f/7)+(1.f/7);
						//glPushMatrix();{
							//glTranslatef(pos.x,pos.y,pos.z);
							//glScalef(idxf*scaleSize,idxf*scaleSize,idxf*scaleSize);
							//glTranslatef(-pos.x,-pos.y,-pos.z);
							//glCallList(thisSuperQuadricList);
							//m_sqs[t][s].DrawAtOrigin();
							int startIdx = vertices.size();
							m_sqs[t][s].AppendShape(vertices,texCoords,normals);
							//m_sqs[t][s].AppendShape(vertices,texCoords,normals,indices,indicesOffset);
							for(int i = startIdx;i<vertices.size();i+=3){
								vertices[i] *= idxf*scaleSize;
								vertices[i+1] *= idxf*scaleSize;
								vertices[i+2] *= idxf*scaleSize;
								vertices[i] += pos.x;
								vertices[i+1] += pos.y;
								vertices[i+2] += pos.z;
							}
						//}glPopMatrix();
					}
				}
				else if(trialInfoPtr->GetRetinalChannel() == TEXTURE){
					if(trialInfoPtr->GetMriTask() == FA){
						vec3 pos = trialDataPtr->GetTraces().GetPosition(t,s);
						int startIdx = vertices.size();
						int texStartIdx = texCoords.size();
						m_sqs[t][s].AppendShape(vertices,texCoords,normals);
						//m_sqs[t][s].AppendShape(vertices,texCoords,normals,indices,indicesOffset);
						for(int i = startIdx;i<vertices.size();i+=3){
							vertices[i] *= scaleSize;
							vertices[i+1] *= scaleSize;
							vertices[i+2] *= scaleSize;
							vertices[i] += pos.x;
							vertices[i+1] += pos.y;
							vertices[i+2] += pos.z;
						}
						float fa = trialDataPtr->GetTraces().GetEigen(t,s).GetFA();
						if(trialInfoPtr->textype == 1){
							float rfa = 1.f/fa;
							for(int i = texStartIdx;i<texCoords.size();i+=2){
								texCoords[i] = (texCoords[i]-0.4f)*rfa+0.4;
							}
						}
						else{
							float offset = faToInt(fa);
							for(int i = texStartIdx;i<texCoords.size();i+=2){
								if(texCoords[i]<0.39){
									texCoords[i] = -1000;
								}
								else if(texCoords[i]>0.61){
									texCoords[i] = 1000;
								}
								else{
									texCoords[i] = (texCoords[i]-0.4f)*5/8.f+offset/8.f;
									texCoords[i+1] *=6;
								}
							}
						}
					}
					else{
						vec3 pos = trialDataPtr->GetTraces().GetPosition(t,s);
						int startIdx = vertices.size();
						int texStartIdx = texCoords.size();
						m_sqs[t][s].AppendShape(vertices,texCoords,normals);
						//m_sqs[t][s].AppendShape(vertices,texCoords,normals,indices,indicesOffset);
						for(int i = startIdx;i<vertices.size();i+=3){
							vertices[i] *= scaleSize;
							vertices[i+1] *= scaleSize;
							vertices[i+2] *= scaleSize;
							vertices[i] += pos.x;
							vertices[i+1] += pos.y;
							vertices[i+2] += pos.z;
						}
						for(int i = texStartIdx;i<texCoords.size();i+=2){
							if(texCoords[i]<=0.39){
								texCoords[i] = -1000;
							}
							else if(texCoords[i]>=0.61){
								texCoords[i] = 1000;
							}
							else{
								texCoords[i] = (texCoords[i]-0.4f)*5;
								texCoords[i+1] *=6;
							}
						}
					}
				}
				else {
					assert(trialInfoPtr->GetRetinalChannel() == COLOR || trialInfoPtr->GetRetinalChannel() == SATURATION);
					vec3 pos = trialDataPtr->GetTraces().GetPosition(t,s);
					int startIdx = vertices.size();
					m_sqs[t][s].AppendShape(vertices,texCoords,normals);
					//m_sqs[t][s].AppendShape(vertices,texCoords,normals,indices,indicesOffset);
					for(int i = startIdx;i<vertices.size();i+=3){
						vertices[i] *= scaleSize;
						vertices[i+1] *= scaleSize;
						vertices[i+2] *= scaleSize;
						vertices[i] += pos.x;
						vertices[i+1] += pos.y;
						vertices[i+2] += pos.z;
					}
				}
			}
			
			int numVertices = vertices.size()/3;
			glEnableClientState(GL_NORMAL_ARRAY);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_COLOR_ARRAY);

			glColorPointer(4,GL_UNSIGNED_BYTE,0,&colors[0]);
			glNormalPointer(GL_FLOAT, 0, &normals[0]);
			glTexCoordPointer(2, GL_FLOAT, 0, &texCoords[0]);
			glVertexPointer(3, GL_FLOAT, 0, &vertices[0]);
			glDrawArrays(GL_QUAD_STRIP, 0, numVertices);

			glDisableClientState(GL_COLOR_ARRAY);
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glDisableClientState(GL_NORMAL_ARRAY);
		}
		
	}glPopAttrib();
}

void DisplayBase::makeTubeTexture(){
	GLubyte texData[TEXHEIGHT][TEXWIDTH][4];
	for(int h=0;h<TEXHEIGHT;h++){
		for(int w = 0;w<TEXWIDTH;w++){
			int i;
			
			int rep = 1;
			int hp = (h*rep)%TEXHEIGHT;
			float boarder = 0.125f;
			if(hp+(0.5-boarder)*w<TEXHEIGHT*(1-boarder) && hp-(0.5-boarder)*w>boarder*TEXHEIGHT){
				i= 0;
			}
			else{
				i = 255;
			}
			if(w >=  TEXWIDTH-1){
				// make sure it does not touch the edge
				i=255;
			}
			//i = PerlinNoise2D((float)w/TEXWIDTH,(float)h/TEXHEIGHT,1.5,1,5)*255;
			texData[h][w][0]=i;
			texData[h][w][1]=i;
			texData[h][w][2]=i;
			texData[h][w][3]=255;
		}
	}

	// make texutre
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	if(glIsTexture(tubeTexture) == GL_TRUE){
		glDeleteTextures(1,&tubeTexture);
	}
	glGenTextures(1, &tubeTexture);
	glBindTexture(GL_TEXTURE_2D, tubeTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
					GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
					GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEXWIDTH, 
				TEXHEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, 
				texData);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void DisplayBase::makeSuperQuadricTexture(){
	GLubyte texData[TEXHEIGHT][TEXWIDTH][4];
	for(int h=0;h<TEXHEIGHT;h++){
		for(int w = 0;w<TEXWIDTH;w++){
			int i;
			int rep = 4;
			int hp = (h*rep)%TEXHEIGHT;
			float boarder = 0.2;
			//if(w>0.4*TEXWIDTH && w <0.6*TEXWIDTH && hp+0.83*w<TEXHEIGHT && hp-0.83*w>0)
			if(w>0.4*TEXWIDTH && w <0.6*TEXWIDTH && hp+1.5*w<1.4*TEXHEIGHT && hp-1.5*w>-0.4*TEXHEIGHT){
				i= 0;
			}
			else{
				i = 255;
			}
			texData[h][w][0]=i;
			texData[h][w][1]=i;
			texData[h][w][2]=i;
			texData[h][w][3]=255;
		}
	}

	// make texutre
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	if(glIsTexture(superQuadricTexture) == GL_TRUE){
		glDeleteTextures(1,&superQuadricTexture);
	}
	glGenTextures(1, &superQuadricTexture);
	glBindTexture(GL_TEXTURE_2D, superQuadricTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
					GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
					GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEXWIDTH, 
				TEXHEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, 
				texData);
	glBindTexture(GL_TEXTURE_2D, 0);
}
void DisplayBase::deleteShapeDisplayLists(){
	for(int i = 0;i<m_sqs.size();i++){
		m_sqs[i].clear();
	}
	m_sqs.clear();
}

void DisplayBase::deleteDisplayLists(vector<int>& displayLists){
	for(unsigned int i = 0;i<displayLists.size();i++){
		glDeleteLists(displayLists[i],1);
	}
}

int DisplayBase::mapDirectionToInt(const vec3& dir) const{
	float dx = fabs(dir.x);
	float dy = fabs(dir.y);
	float dz = fabs(dir.z);
	if(dx>=dy && dx>=dz){
		if(dir.x>=0){
			return 1;
		}
		else{
			return 2;
		}
	}
	else if(dy>=dx && dy>=dz){
		if(dir.y>=0){
			return 3;
		}
		else{
			return 4;
		}
	}
	else {
		if(dir.z>=0){
			return 5;
		}
		else{
			return 6;
		}
	}
}

float DisplayBase::mapDirectionToFloat(const vec3& dir) const{
	return (mapDirectionToInt(dir)-1)/5.f;
}

rgba DisplayBase::mapDirectionToSaturation(const vec3& dir) const{
	// use boys surface
	rgba color = axesLegend.GetColorByDirection(dir);
	int texIdx = ColorConverter::getIdxByBoysColor(color);
	color.fromHSV(10,texIdx/9.f,1);
	return color;

	// use new simple
	//rgba dcolor;
	//float v = mapDirectionToFloat(dir);
	//dcolor.fromHSV(10,v,1);
	//return dcolor;
}

vec3 DisplayBase::mapDirectionToSize(const vec3& dir) const{
	float s = mapDirectionToFloat(dir);
	return vec3(s,s,s);
}

rgba DisplayBase::getColor(const TrialInfoPtr trialInfoPtr,const TrialDataPtr trialDataPtr,int traceIdx, int segIdx) const{
	rgba color(0.9,0.9,0.9,1);

	int numSegs = trialDataPtr->GetTraces().GetNumSegs(traceIdx);
	// highlight comes first
	// only bundle_same
	// has highlighted fibers

	// special case
	if(trialInfoPtr->GetQuestIndex() == TRAININGBUNDLEQUEST){
		color = trialDataPtr->GetTraces().GetColor(traceIdx,segIdx);
		if(color.isSimiliar(rgba(1,1,1),0.1f)){
			color.a = TRAINING_BUNDLE_OPACITY;
		}
	}
	else{
		// normal cases

		// same bundle task
		if(trialDataPtr->HighlightFiber(traceIdx)){
			if(trialInfoPtr->GetRetinalChannel() == SATURATION){
				//float value = (float)segIdx/(numSegs-1);
				//color = SaturationColorScheme::GetColorDiscrete(value,COLORDISCRETE);
				color = mapDirectionToSaturation(trialDataPtr->GetSegmentOrientation(traceIdx,segIdx));

			}
			else if(trialInfoPtr->GetRetinalChannel() == COLOR){
				color = BoysSurfaceColorScheme::GetColor(traceIdx,segIdx);
			}
			else if(trialInfoPtr->GetRetinalChannel() == SIZE){
				if(trialDataPtr->HighlightFiber(traceIdx)){
					//color = FiberColorScheme::GetHighlightColor();
				}
			}
		}
		else{
			if(trialInfoPtr ->GetMriTask() == BUNDLE_SAME || trialInfoPtr ->GetMriTask() == BUNDLE_NAME){

				
				// newly added
				if(trialInfoPtr->GetRetinalChannel() == SATURATION){
					//float value = (float)segIdx/(numSegs-1);
					//color = SaturationColorScheme::GetColorDiscrete(value,COLORDISCRETE);
					color = mapDirectionToSaturation(trialDataPtr->GetSegmentOrientation(traceIdx,segIdx));
				}
				else if(trialInfoPtr->GetRetinalChannel() == COLOR){
					color = BoysSurfaceColorScheme::GetColor(traceIdx,segIdx);
				}
				else if(trialInfoPtr->GetRetinalChannel() == SIZE){
					if(trialDataPtr->HighlightFiber(traceIdx)){
						//color = FiberColorScheme::GetHighlightColor();
					}
				}
				// newly added end
				

				// transparent
				color.a = OPACITY;
			}
			// bundle_trace task
			else if(trialInfoPtr ->GetMriTask() == BUNDLE_TRACE){
				if(m_highLightTracedTraces && trialDataPtr->IsFiberTraced(traceIdx)){
					color = FiberColorScheme::GetHighlightColor();
				}
				// boyssurface color
				else if(trialInfoPtr->GetRetinalChannel() == SATURATION){
					//float value = (float)segIdx/(numSegs-1);
					//color = SaturationColorScheme::GetColorDiscrete(value,COLORDISCRETE);
					color = mapDirectionToSaturation(trialDataPtr->GetSegmentOrientation(traceIdx,segIdx));
				}
				else if(trialInfoPtr->GetRetinalChannel() == COLOR){
					float value = trialDataPtr->GetTraces().GetEigen(traceIdx,segIdx).GetFA();
					color = BoysSurfaceColorScheme::GetColor(traceIdx,segIdx);
				}
			}
			else if(trialInfoPtr ->GetMriTask() == LESION){

				// boyssurface color
				if(trialInfoPtr->GetRetinalChannel() == SATURATION){
					//float value = trialDataPtr->GetSegmentSubFiberIndexf(traceIdx,segIdx);
					//color = SaturationColorScheme::GetColorDiscrete(value,COLORDISCRETE);
					color = mapDirectionToSaturation(trialDataPtr->GetSegmentOrientation(traceIdx,segIdx));
				}
				else if(trialInfoPtr->GetRetinalChannel() == COLOR){
					color = axesLegend.GetColorByDirection(trialDataPtr->GetSegmentOrientation(traceIdx,segIdx));
				}
			}
			else{
				// fa task
				assert(trialInfoPtr ->GetMriTask() == FA);
				float value = trialDataPtr->GetTraces().GetEigen(traceIdx,segIdx).GetFA();
				value-=0.2;
				value/=0.8;
				if(trialInfoPtr->GetRetinalChannel() == SATURATION){
					color = SaturationColorScheme::GetColorDiscrete(value,COLORDISCRETE);
				}
				else if(trialInfoPtr->GetRetinalChannel() == COLOR){
					color = CoolWarmColorScheme::GetColorDiscrete(value,COLORDISCRETE);
				}
			}
		}
	}
	return color;
}

vec3 DisplayBase::getSize(const TrialInfoPtr trialInfoPtr,const TrialDataPtr trialDataPtr,int traceIdx, int segIdx) const{
	vec3 sz(DEFAULTSIZE,DEFAULTSIZE,DEFAULTSIZE);
				
	if(trialInfoPtr->GetRetinalChannel() == SIZE){
		if(trialInfoPtr->GetMriTask() == FA){
			float value = trialDataPtr->GetTraces().GetEigen(traceIdx,segIdx).GetFA()*2;
			//value = cos((1-value/2)*3.1415926/2);
			sz*=value;
		}
		else{
			//assert(trialInfoPtr->GetMriTask() == BUNDLE_TRACE || trialInfoPtr->GetMriTask() == BUNDLE_SAME);
			//int numSegs = trialDataPtr->GetTraces().GetNumSegs(traceIdx);
			//float value = (float)segIdx/numSegs*((float)6.f/7)+1.f/7;
			//// from 0.05~0.35
			//sz.scale(vec3(value,value,value));
			vec3 ss = mapDirectionToSize(trialDataPtr->GetSegmentOrientation(traceIdx,segIdx));
			float value = ss.x;
			value = value*6.f/7+1.f/7;
			sz*=value;
		}
	}
	return sz;
}