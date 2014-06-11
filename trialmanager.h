#pragma once

#include "trial.h"
#include <vector>
using namespace std;

class TrialManager
{
public:
	TrialManager(void);
	~TrialManager(void);

	// total number of trials
	int GetNumTrials() const;

	// number trianing trials
	int GetNumTraining() const{return m_trainingInfoPtrs.size();};

	// each participant does same amount of jobs
	int GetNumTrialsPerParticipant() const;

	// get methods
	TrialInfoPtr GetTrailInfoPtr(int participant, int trialIdx) const;
	TrialDataPtr GetTrailDataPtr(const TrialInfoPtr trialInfoPtr) const;

	// shuffle methods
	void GenerateLatinSquareTrailInfos();

	//keqin overwrite part of GenerateLatinSquareTrailInfos()'s output
	void reWriteLatinSquareTrailInfos();

	// same traing for every one
	void GenerateTrainingInfos();

	// load other affiliate info
	void LoadTrialData(const char* dataRootDir,const Traces& traces);

	void LoadTrianingTracesData(const Traces& traces);

protected:
	bool m_hasTrainingTraces;
	void deleteTrialInfos();
	void deleteTrialData();
	void deleteTrainingInfos();
	vector<TrialInfoPtr> m_trialInfoPtrs;
	vector<TrialInfoPtr> m_trainingInfoPtrs;
	vector<TrialDataPtr> m_trialDataPtrs;
};

