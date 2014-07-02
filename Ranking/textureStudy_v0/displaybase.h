#pragma once

#include "trial.h"
#include "traces.h"
#include <vector>
#include "alignedboundingbox.h"
#include "superquadric.h"
#include "colortotexturemap.h"
#include "legend.h"
using namespace std;

class DisplayBase
{
public:
	DisplayBase(void);
	~DisplayBase(void);

	void DirectDraw(const TrialInfoPtr trialInfoPtr,const TrialDataPtr trialDataPtr);
	int GenerateTrailDisplayList(const TrialInfoPtr trialInfoPtr,const TrialDataPtr trialDataPtr) const;

	void LoadTracesFromFile(const char* fileName, bool reserveZ = false);
	void LoadTrainingTracesFromFile(const char* fileName);
	void BuildDisplayBase();
	void GetColorSquence(vector<rgba> &colors,const TrialInfoPtr trialInfoPtr);
	unsigned int GetTexture(const int idx);
	unsigned int GetTexture(){return tubeTexture;};
	const Traces& GetTraces() const;
	const Traces& GetTrainingTraces() const;
	void SetHighlightTracedFibers(bool h) {m_highLightTracedTraces = h;};
	void SetShowLesionBox(bool h) {m_showLesionBox = h;};
	void SetupDirectDrawContext(const TrialInfoPtr trialInfoPtr,const TrialDataPtr trialDataPtr) ;
	
	void LoadLegend(const char* fileName);
	
	
	BallLegend axesLegend;

protected:
	void renderText(TrialDataPtr trialDataPtr);
	int generateTubesDisplayList(const TrialInfoPtr trialInfoPtr,const TrialDataPtr trialDataPtr) const;
	int generateSuperQuadricsDisplayList(const TrialInfoPtr trialInfoPtr,const TrialDataPtr trialDataPtr) const;
	int generateRibbonDisplayList(const TrialInfoPtr trialInfoPtr,const TrialDataPtr trialDataPtr) const;
	void drawBoxes(TrialDataPtr trialDataPtr) const;
	void drawSpheres(TrialDataPtr trialDataPtr) const;
	void drawTubeSegment(int tubeIdx, int segIdx, const TrialInfoPtr trialInfoPtr,const TrialDataPtr trialDataPtr, bool isHalo = false) const;
	void drawTubeSegment(int tubeIdx, int segIdx, const TrialInfoPtr trialInfoPtr,const TrialDataPtr trialDataPtr,
		vector<float> &vertices, vector<float> &texCoords,
		vector<unsigned char> &colors, vector<float> &normals,
		vector<float> &capvertices, vector<float> &captexCoords,
		vector<unsigned char> &capcolors, vector<float> &capnormals) const;
	void drawTubes(const TrialInfoPtr trialInfoPtr,const TrialDataPtr trialDataPtr, bool isHalo = false) const;
	void drawRibbons(const TrialInfoPtr trialInfoPtr,const TrialDataPtr trialDataPtr) const;
	void drawHighlightTubeGeometry(const TrialInfoPtr trialInfoPtr,const TrialDataPtr trialDataPtr) const;
	void drawSuperQuadrics(const TrialInfoPtr trialInfoPtr,const TrialDataPtr trialDataPtr, bool isHalo = false) const;
	void drawHighlightLines(const TrialInfoPtr trialInfoPtr,const TrialDataPtr trialDataPtr) const;
	void drawUnHighlightLines(const TrialInfoPtr trialInfoPtr,const TrialDataPtr trialDataPtr) const;
	void directDrawRibbonsAlwaysFacingYou(const TrialInfoPtr trialInfoPtr,const TrialDataPtr trialDataPtr) const;
	void makeTubeTexture();
	void makeSuperQuadricTexture();

	void handleTextureOverall(const TrialInfoPtr trialInfoPtr,const TrialDataPtr trialDataPtr) const;
	void handleTextureByFiber(const TrialInfoPtr trialInfoPtr,const TrialDataPtr trialDataPtr, const int traceIdx) const;
	//void handleTextureBySegment(const TrialInfoPtr trialInfoPtr,const TrialDataPtr trialDataPtr, const int traceIdx, const int segIdx) const;
	void generateShapeDisplayLists();
	void deleteShapeDisplayLists();

	rgba getColor(const TrialInfoPtr trialInfoPtr,const TrialDataPtr trialDataPtr,int traceIdx, int segIdx) const;
	vec3 getSize(const TrialInfoPtr trialInfoPtr,const TrialDataPtr trialDataPtr,int traceIdx, int segIdx) const;

	rgba mapDirectionToSaturation(const vec3& dir) const;
	vec3 mapDirectionToSize(const vec3& dir) const;
	float mapDirectionToFloat(const vec3& dir) const;
	int mapDirectionToInt(const vec3& dir) const;
	Traces m_traces;
	
	Traces m_trainingTraces;
	vector<vec3> m_bundlePostions;

	unsigned int tubeTexture;
	unsigned int superQuadricTexture;

	ColorToTextureMap textureMaps;

	//vector<vector<int>> m_superQuadrics_fixed;
	vector<vector<SuperQuadric>> m_sqs;

	bool m_showLesionBox;
	bool m_highLightTracedTraces;

	// direct draw context
	vector<vector<rgba>> m_colors;
	vector<vector<float>> m_orientationValue;
	vector<vector<float>> m_sizes;

private:
	void deleteDisplayLists(vector<int>& displayLists);
	float clamp(float v, float min=0,float max=1) const;
	void encodeTensor(SuperQuadric &sq, const TensorEigen& eigen) const;
	int shapeRenderInterval;

};

