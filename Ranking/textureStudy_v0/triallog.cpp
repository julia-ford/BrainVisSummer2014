#include "triallog.h"
#include "mystdexp.h"
#include <iostream>
#include <fstream>

using namespace mriEnum;
using namespace mystdexp;

TrialLog::TrialLog(void)
{
	m_debugOut = false;
}


TrialLog::~TrialLog(void)
{
}

void TrialLog::StartParticipant(int idx, bool isTraining){
	m_participantIndex = idx;
	if(!m_outfile.is_open()){
		if(isTraining){
			m_outfile.open(("userTrainingLog_"+convertInt(idx)+".log").c_str());
			m_analysisFile.open(("userTrainingAnalysis_"+convertInt(idx)+".log").c_str());
		}
		else {
			m_outfile.open(("userLog_"+convertInt(idx)+".log").c_str());
			m_analysisFile.open(("userAnalysis_"+convertInt(idx)+".log").c_str());
		}
		if(!m_outfile.is_open() || !m_analysisFile.is_open() ){
			cout << "can't open trial log file, user log will be lost!!!" << endl;
		}
		else{
			m_analysisFile << "trialIndex" << " " << "participantIndex" << " " << "task"
				 << " " << "encodingChannel" << " " << "shape" << " " 
				 << "bundle" << " " << "fiberCover" << " " << "textype" << " " << "timeUsed" << " " << "correctAnswer" << " "
				 << "userAnswer" << " " << "difference" << " " << "numFibers" << " " << "isCorrect" << endl;
		}
		m_trialIndex = -1;
	}
}

// end participant
void TrialLog::EndParticipant(){
	if(m_outfile.is_open()){
		m_outfile.close();
	}
	if(m_analysisFile.is_open()){
		m_analysisFile.close();
	}
}

	// only for indication
	// won't start timing
void TrialLog::NextTrial(const TrialInfoPtr trialInfoPtr,const TrialDataPtr trialDataPtr){
	m_trialInfoPtr = trialInfoPtr;
	m_trialDataPtr = trialDataPtr;
	m_trialStarted = false;
	m_trialIndex ++;
}

	// start timing
void TrialLog::StartTrial(){
	if(!m_trialStarted){
		time(&m_trialStartTime);
	}
	m_trialStarted = true;
}
void TrialLog::EndTimeing(){
	time(&m_trialEndTime);
}
	// end timing
void TrialLog::EndTrial(int answer, bool answerJudge){
	if(m_trialStarted){
		// cout << "This is written to the file";
		
		// participant index
		m_outfile << "participant_idx: [ " << m_participantIndex <<" ]" <<endl;
		if(m_debugOut) cout << "participant_idx: [ " << m_participantIndex <<" ]" <<endl;

		// trial idx
		m_outfile << "trial_idx: [ " << m_trialIndex <<" ]" <<endl;
		if(m_debugOut) cout << "trial_idx: [ " << m_trialIndex <<" ]" <<endl;

		// start time
		struct tm * startTimeinfo = localtime(&m_trialStartTime);
		char startTimeStr[80];
		strftime (startTimeStr,80,"[ %m/%d/%y %H:%M:%S ]",startTimeinfo);
		m_outfile << "start_time: " << startTimeStr << endl;
		if(m_debugOut) cout << "start_time: " << startTimeStr << endl;

		// end time
		struct tm * endTimeinfo = localtime(&m_trialEndTime);
		char endTimeStr[80];
		strftime (endTimeStr,80,"[ %m/%d/%y %H:%M:%S ]",endTimeinfo);
		m_outfile << "end_time: " << endTimeStr << endl;
		if(m_debugOut) cout << "end_time: " << endTimeStr << endl;

		// trial info & data
		m_outfile << "trial_info: " << m_trialInfoPtr->GetTrialInfoString().c_str() << endl;
		if(m_debugOut) cout << "trial_info: " << m_trialInfoPtr->GetTrialInfoString().c_str() << endl;

		m_outfile << "trial_data: [ " << m_trialDataPtr->GetDataPath().c_str() << " ]" << endl;
		if(m_debugOut) cout << "trial_data: [ " << m_trialDataPtr->GetDataPath().c_str() << " ]" << endl;

		// user selection
		m_outfile << "selection: [ " << answer << " ]" << endl;
		if(m_debugOut) cout << "selection: [ " << answer << " ]" << endl;

		// write correct answer
		int rightAnswer = m_trialDataPtr->GetCorrectAnswer();
		m_outfile << "correct_answer: [ " << rightAnswer << " ]" << endl;
		if(m_debugOut) cout << "correct_answer: [ " << rightAnswer << " ]" << endl;


		// difficulty
		float difficulty = m_trialDataPtr->GetDifficulty();
		m_outfile << "difficulty: [ " << difficulty << " ]" << endl;
		if(m_debugOut) cout << "difficulty: [ " << difficulty << " ]" << endl;

		// check if is right
		// write analysis file
		MriTask task = m_trialInfoPtr->GetMriTask();
		RetinalChannel rc = m_trialInfoPtr->GetRetinalChannel();
		Shape sp = m_trialInfoPtr->GetShape();
		Bundle bd = m_trialInfoPtr->GetBundle();
		FiberCover cov = m_trialInfoPtr->GetFiberConver();
		float diff = m_trialDataPtr->GetDifficulty();
		if(task != FA){
			diff = 0;
		}
		int correctAnswer = m_trialDataPtr ->GetCorrectAnswer();
		int numFiber = m_trialDataPtr->GetNumFibers();
		int isCorrect = 0;
		if(correctAnswer == answer){
			isCorrect = 1;
		}
		else if(task == FA){
			if(diff > 0 && answer == 0){
				isCorrect = 1;
			}
			else if(diff < 0 && answer == 1){
				isCorrect = 1;
			}
		}

		if(answer<0){
			isCorrect = answerJudge?1:0;
		}

		// write correct answer
		// 1: correct, 0: wrong
		m_outfile << "is_correct: [ " << isCorrect << " ]" << endl << endl;
		if(m_debugOut) cout << "is_correct: [ " << isCorrect << " ]" << endl << endl;

		if(!m_trialInfoPtr->IsEmpty()){
			int second = difftime(m_trialEndTime,m_trialStartTime);
			m_analysisFile << m_trialIndex << " " << m_participantIndex << " " << toString(task).c_str()
				 << " " << toString(rc).c_str() << " " << toString(sp).c_str() << " " 
				 << toString(bd).c_str() << " " << toString(cov).c_str() << " " 
				 << convertInt(m_trialInfoPtr->textype) << " " << second << " " << correctAnswer << " "
				 << answer << " " << diff << " " << numFiber << " " << isCorrect << endl;
		}
	}
	else{
		m_outfile << "can't end an unstart trial!!!" << endl;
	}
}