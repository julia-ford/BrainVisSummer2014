#include "displaymanager.h"
#include <iostream>
#include "myGL.h"
#include "mystdexp.h"
#include "myGLexp.h"
using namespace myGLexp;
#include <cmath>
#include "ribbon.h"
using namespace mystdexp;
using namespace std;


#define TRAININGBUNDLEQUEST 5

DisplayManager::DisplayManager(){
	m_hasTraining = true;
	m_hasStudy = true;
	changed();
}


DisplayManager::~DisplayManager(){
	deleteCurrentDisplayList();
}

void DisplayManager::Render(){
	glCallList(m_currentDisplayList);
	m_displayBase.DirectDraw(GetCurrentTrialInfoPtr(),GetCurrentTrialDataPtr());
}

void DisplayManager::RenderAxes(){
	TrialInfoPtr trialInfo = GetCurrentTrialInfoPtr();
	if((trialInfo->GetMriTask() == BUNDLE_SAME 
		|| trialInfo->GetMriTask() == BUNDLE_TRACE
		|| trialInfo->GetMriTask() == BUNDLE_NAME) 
		&& trialInfo->GetRetinalChannel() == COLOR){
		m_displayBase.axesLegend.SetDrawColorBall(true);
	}
	else {
		m_displayBase.axesLegend.SetDrawColorBall(false);
	}
	if((trialInfo->GetMriTask() == BUNDLE_SAME 
		|| trialInfo->GetMriTask() == BUNDLE_TRACE
		|| trialInfo->GetMriTask() == BUNDLE_NAME) 
		&& trialInfo->GetRetinalChannel() == TEXTURE){
		m_displayBase.axesLegend.SetDrawTextureBall(true);
	}
	else {
		m_displayBase.axesLegend.SetDrawTextureBall(false);
	}
	if((trialInfo->GetMriTask() == BUNDLE_SAME 
		|| trialInfo->GetMriTask() == BUNDLE_TRACE
		|| trialInfo->GetMriTask() == BUNDLE_NAME) 
		&& trialInfo->GetRetinalChannel() == SATURATION){
		m_displayBase.axesLegend.SetDrawSaturationBall(true);
	}
	else {
		m_displayBase.axesLegend.SetDrawSaturationBall(false);
	}
	m_displayBase.axesLegend.Render();
}

void DisplayManager::RenderLegend(){
	TrialInfoPtr trialInfo = GetCurrentTrialInfoPtr();
	if(trialInfo->GetQuestIndex() == TRAININGBUNDLEQUEST) return;
	vector<rgba> colors;
	colors.clear();
	m_displayBase.GetColorSquence(colors,trialInfo);
	barLegend.SetColors(colors);
	if(trialInfo->GetMriTask() == FA){
		barLegend.SetDiscrete(true);
		barLegend.SetTitle("FA value");
		barLegend.SetRange(0.2,1.0f);
		barLegend.Render();
	}
	else{
		//barLegend.SetDiscrete(true);
		//barLegend.SetTitle("Fiber");
		//barLegend.Render();
	}

	if(trialInfo->GetMriTask() == FA 
		&& trialInfo->GetRetinalChannel() == TEXTURE){
		TextureLegend texLegend;
		int tex[8];
		if(trialInfo->textype == 1){
			for(int i = 0;i<8;i++){
				tex[i] = m_displayBase.GetTexture();
			}
		}
		else{
			for(int i = 0;i<8;i++){
				tex[i] = m_displayBase.GetTexture(i);
			}
		}
		texLegend.SetTexture(tex);
		texLegend.SetRange(0.2f,1.f);
		texLegend.SetTitle("FA value");
		texLegend.Render(trialInfo->textype);
	}
	
	if(trialInfo->GetMriTask() == FA 
		&& trialInfo->GetRetinalChannel() == ORIENTATION
		&& trialInfo->GetShape() == RIBBON){
		Ribbon rib;
		rib.SetHaloColor(rgba(0,0,0));
		rib.SetDrawhalo(true);
		vector<vec3> points(9);
		vector<vec3> normals(9);
		vector<float> sizes(9,1);
		vector<rgba> colors(9);
		vector<int> lefts;
		lefts.push_back(0);
		lefts.push_back(8);
		for(int i = 0;i<9;i++){
			float x = i/8.f;
			float angle = (1-x)/2*3.1415926;
			points[i].set(0,x*2-1,0);
			normals[i].set(sin(angle),0,cos(angle));
			colors[i] = rgba(1,1,1);
		}
		rib.SetPositions(&points);
		rib.SetColors(&colors);
		rib.SetSizes(&sizes);
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		GLdouble modelview[16];
		glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
		GLdouble projection[16];
		glGetDoublev(GL_PROJECTION_MATRIX, projection);
		GLdouble winX, winY, winZ;

		//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		rib.DrawGeometry(normals,lefts);

		const int numTags = 9;
		vec3 loc[numTags];
		for(int i = 0;i<numTags;i++){
			loc[i].set(1,(float)i/(numTags-1)*2-1,0);
		}
		// draw text icon
		float range = 0.8;
		for(int i = 0;i<numTags;i++){
			glPushMatrix();{
				glColor3f(1,1,1);
				gluProject( loc[i].x, loc[i].y, loc[i].z,
					modelview, projection, viewport, 
					&winX, &winY, &winZ);
				myGLPrintString(winX+5,winY,convertFloat((float)i/(numTags-1)*range+0.2),0,12);
			} glPopMatrix();
		}
	
		// here is the title
		gluProject( 0,1.2, 0,
			modelview, projection, viewport, 
			&winX, &winY, &winZ);
		myGLPrintString(winX+15,winY,"FA value",1,16);
	}
}

void DisplayManager::SetParticipantIndex(int idx){
	if(m_participantIndex != idx){
		m_participantIndex = idx;
		changed();
	}
}

void DisplayManager::SetDebugMode(bool debug){
	m_isDebug = debug;
	if(m_isDebug){
		m_displayBase.SetHighlightTracedFibers(false);
		m_displayBase.SetShowLesionBox(true);
	}
	else{
		m_displayBase.SetHighlightTracedFibers(false);
		m_displayBase.SetShowLesionBox(false);
	}
};

void DisplayManager::SetTrialIndex(int idx){
	if(m_trailIndex != idx){
		m_trailIndex = idx;
		changed();
	}
}

void DisplayManager::ResetDisplay(){
	SetTrialIndex(0); 
}

void DisplayManager::NextDisplay(){
	if(m_trailIndex<m_trialManager.GetNumTrialsPerParticipant()-1){
		m_trailIndex ++;
		changed();
	}
}

void DisplayManager::PreviousDisplay(){
	if(m_trailIndex>0){
		m_trailIndex --;
		changed();
	}
}

void DisplayManager::Update(){
	if(m_needRedraw){
		buildCurrentDisplayList();
		m_needRedraw = false;
	}
	glCallList(m_currentDisplayList);
}

bool DisplayManager::IsLastDisplay() const{
	return m_trailIndex==m_trialManager.GetNumTrialsPerParticipant()-1;
}

string DisplayManager::GetProgressString() const{
	int numTrial = m_trialManager.GetNumTrialsPerParticipant();
	int currentStep = 0;
	int totalStep = 0;
	for(int i = 0;i<numTrial;i++){
		if(!m_trialManager.GetTrailInfoPtr(m_participantIndex,i)->IsEmpty() 
			&& m_trialManager.GetTrailInfoPtr(m_participantIndex,i)->GetQuestIndex() != TRAININGBUNDLEQUEST){
			totalStep++;
			if(i <= m_trailIndex){
				currentStep ++;
			}
		}
	}
	return convertInt(currentStep)+"/"+convertInt(totalStep);
}

void DisplayManager::LoadData(const char* dataDir, const char* traceFile,const char* trainingTraceFile){
	m_dataDir = dataDir;

	// training res 4, need reverse z
	m_displayBase.LoadTracesFromFile(traceFile,m_hasTraining);
	m_trialManager.LoadTrialData(dataDir,m_displayBase.GetTraces());
	if(m_hasTraining){
		m_displayBase.LoadTrainingTracesFromFile(trainingTraceFile);
		m_trialManager.LoadTrianingTracesData(m_displayBase.GetTrainingTraces());
	}
}

void DisplayManager::LoadLegend(const char* fileName){
	m_displayBase.LoadLegend(fileName);
}

void DisplayManager::BuildDataBase(){
	if(m_hasTraining) {
		m_trialManager.GenerateTrainingInfos();
	};
	if(m_hasStudy) {
		m_trialManager.GenerateLatinSquareTrailInfos();
	}
	m_displayBase.BuildDisplayBase();
}

string DisplayManager::GetQuestionString() const{
	return GetCurrentTrialInfoPtr()->GetQuestionString();
}

void DisplayManager::PrintTrialInfo() const{
	cout << (GetCurrentTrialInfoPtr()->GetTrialInfoString().c_str()) << endl;
}
const TrialInfoPtr DisplayManager::GetCurrentTrialInfoPtr() const{
	return m_trialManager.GetTrailInfoPtr(m_participantIndex,m_trailIndex);
}
const TrialDataPtr DisplayManager::GetCurrentTrialDataPtr() const{
	const TrialInfoPtr currentTrialInfoPtr = m_trialManager.GetTrailInfoPtr(m_participantIndex,m_trailIndex);
	return m_trialManager.GetTrailDataPtr(currentTrialInfoPtr);
}

void DisplayManager::buildCurrentDisplayList(){
	deleteCurrentDisplayList();
	m_currentDisplayList = m_displayBase.GenerateTrailDisplayList(GetCurrentTrialInfoPtr(),GetCurrentTrialDataPtr());
	m_displayBase.SetupDirectDrawContext(GetCurrentTrialInfoPtr(),GetCurrentTrialDataPtr());
}

void DisplayManager::deleteCurrentDisplayList(){
	glDeleteLists(m_currentDisplayList,1);
}

void DisplayManager::changed(){
	m_needRedraw = true;
}