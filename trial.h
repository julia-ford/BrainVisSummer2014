#ifndef trail_h
#define trail_h

#include "mrienum.h"
#include <vector>
#include <string>
#include "traces.h"
#include "vec3.h"
#include "rgba.h"
using namespace mriEnum;
using namespace std;

class TrialInfo;
class TrialData;
typedef TrialInfo* TrialInfoPtr;
typedef TrialData* TrialDataPtr;
typedef TrialInfo*const TrialInfoPtrConst;
typedef TrialData*const TrialDataPtrConst;

class TrialInfo{
public:
	TrialInfo(MriTask task):m_task(task),m_isEmpty(true),textype(2){};

	TrialInfo(RetinalChannel rc,MriTask tsk,Bundle 
		bd,Shape shp,FiberCover fbc, int qst, 
		int res, int texType = 1, bool isTraining = false, bool isEmpty = false)
		:m_channel(rc),m_task(tsk),m_bundle(bd),
		m_shape(shp),m_cover(fbc),m_quest(qst),
		m_resolution(res),m_isTraining(isTraining),
		m_isEmpty(isEmpty),textype(texType){};

	RetinalChannel GetRetinalChannel() const {return m_channel;};
	MriTask GetMriTask() const {return m_task;};
	Bundle GetBundle() const {return m_bundle;};
	Shape GetShape() const {return m_shape;};
	FiberCover GetFiberConver() const {return m_cover;};
	int GetQuestIndex() const {return m_quest;};
	int GetResolution() const {return m_resolution;};
	bool IsTraining() const {return m_isTraining;};
	bool IsEmpty() const {return m_isEmpty;};

	string GetTrialInfoString(const string separator = " ") const;
	static string GetTableHeader(const string separator = " ");
	string GetQuestionString() const;

	int textype;

	RetinalChannel m_channel;
	Bundle m_bundle;
	Shape m_shape;
	// allfb, partial or whole
	FiberCover m_cover;

protected:
	
	MriTask m_task;
	// each has two different
	// quests related data
	// 1 or 2
	int m_quest;

	// resolution
	// 1~5
	// high~low
	int m_resolution;

	// training session
	bool m_isTraining;

	// empty
	bool m_isEmpty;
};

class TrialData{
public:
	TrialData(const Traces& traces);

	bool ContainsFiber(int idx) const;
	bool HighlightFiber(int idx) const;
	bool IsFiberTraced(int idx) const;
	bool InBox(const vec3& pos, int boxIdx) const;
	
	int GetFiberCutIndex(const int traceIdx) const;
	int GetNumFibers() const;
	int GetFibers(int idx) const;
	int GetNumBoxes() const{return boundingBoxes.size()/6;};
	int GetCorrectAnswer() const {return correctAnswer;};
	int GetNumChoices() const;
	int GetNumSubFiberSegments(int traceIdx, int segIdx) const;
	float GetDifficulty() const {return m_difficulty;};
	float GetSegmentSubFiberIndexf(int traceIdx, int segIdx) const;
	void GetAllFibers(vector<int>& fibers) const{fibers = fiberIndices;};
	vec3 GetFiberCenter() const{return m_fiberCenter;};
	vec3 GetBoxCenter(int idx) const;
	vec3 GetCorrectAnswerMinLocation() const{return correctLocationMin;};
	vec3 GetCorrectAnswerMaxLocation() const{return correctLocationMax;};
	vec3 GetSegmentOrientation(int traceIdx, int segIdx) const;
	void GetBoxes(vector<float> &boxes)const{boxes=boundingBoxes;};
	void GetSpheres(vector<int> &sspheres)const{sspheres=spheres;};
	void LoadFromFiles(const char* fileRootDir,TrialInfoPtr trialInfoPtr);
	const Traces& GetTraces() const{return m_traces;};
	string GetDataPath() const{return m_dataPath;};
	string GetCorrectAnswerString() const;

	int GetNumText() const{return m_texts.size();};
	string GetText(int idx){return m_texts[idx];};
	rgba GetTextColor(int idx){return m_textColors[idx];};
	vec3 GetTextPosition(int idx){return m_textPositions[idx];};
	const vector<int>& GetLeftSegmentIndices(int fiberCutIdx) const{return leftSegments[fiberCutIdx];};
	
	void SetAllToShow();

protected:
	void swapBoxes(int a, int b);
	void permuteBoxes();
	void clearAll();
	
	// refine the data
	// and fill in right answer
	// and difficulty
	void modify();

	// fibers to show
	vector<int> fiberIndices;

	// fiber to highlight
	vector<int> highlightFiberIndices;

	// each box is indicated by 6 floats
	vector<float> boundingBoxes;

	// each sphere is indicated by 2 integers
	// first number is fiber index
	// and followed by 0: head, 1: tail
	vector<int> spheres;

	// the correct answer
	int correctAnswer;

	// correctLocation
	vec3 correctLocationMin;
	vec3 correctLocationMax;

	// difficulty
	// task specified
	float m_difficulty;

	// int resolution
	int m_resolution;

	// text to show and locs
	vector<vec3> m_textPositions;
	vector<string> m_texts;
	vector<rgba> m_textColors;

	const Traces& m_traces;

private:
	string m_dataPath;
	vec3 m_fiberCenter;

	// for lesion task
	vector<int> cutFiberIndices;
	// indicate by start & end indices pairs
	// for each cutFiberIndices;
	vector<vector<bool>> reverseHeadTail;
	vector<vector<int>> leftSegments;

	// have to use it to differenciate
	// bundle same/name
	MriTask m_task;
};

#endif