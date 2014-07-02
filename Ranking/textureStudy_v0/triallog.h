#pragma once
#include "trial.h"
#include <time.h>
#include <fstream>
using namespace std;

class TrialLog
{
public:
	TrialLog(void);
	~TrialLog(void);

	void StartParticipant(int idx, bool isTraining);

	// end participant
	void EndParticipant();

	// only for indication
	// won't start timing
	void NextTrial(const TrialInfoPtr trialInfoPtr,const TrialDataPtr trialDataPtr);

	//
	bool IsStarted() const {return m_trialStarted;};


	// start timing
	// won't cause second start
	// unless trial changed
	void StartTrial();

	// set the ending time
	// can be renewed
	void EndTimeing();

	// end timing
	// if answer<0, use answerJudge as correct or not
	void EndTrial(int answer, bool answerJudge = false);

	// debugUse
	void SetDebugOut(const bool debugOut){m_debugOut=debugOut;};

protected:
	int m_participantIndex;
	bool m_trialStarted;
	TrialInfoPtr m_trialInfoPtr;
	TrialDataPtr m_trialDataPtr;

	bool m_debugOut;

private:
	time_t m_trialStartTime;
	time_t m_trialEndTime;

	int m_trialIndex;
	ofstream m_outfile;
	ofstream m_analysisFile;
};