#pragma once

#include "displaybase.h"
#include "trialmanager.h"
#include "legend.h"

class DisplayManager
{
public:
	DisplayManager();
	~DisplayManager();

	void Render();
	void RenderAxes();
	void RenderLegend();

	int GetCurrentTrialIndex() const{return m_trailIndex;};
	int GetNumTrialsPerparticipant() const{
		return m_trialManager.GetNumTrialsPerParticipant();
	};

	void SetDebugMode(bool debug);
	void SetTraining(bool hasTraining){m_hasTraining = hasTraining;};
	void SetStudy(bool hasStudy){m_hasStudy = hasStudy;};
	void SetParticipantIndex(int idx);
	void SetTrialIndex(int idx);
	void ResetDisplay();
	void NextDisplay();
	void PreviousDisplay();
	void Update();
	bool IsLastDisplay() const;
	string GetProgressString() const;

	void LoadData(const char* dataDir, const char* traceFile,const char* trainingTraceFile);
	void LoadLegend(const char* fileName);
	void BuildDataBase();

	string GetQuestionString() const;
	void PrintTrialInfo() const;
	const TrialInfoPtr GetCurrentTrialInfoPtr() const;
	const TrialDataPtr GetCurrentTrialDataPtr() const;

protected:
	void buildCurrentDisplayList();
	void deleteCurrentDisplayList();
	void changed();

	DisplayBase  m_displayBase;  /**< reference to the DisplayBase instance */
	TrialManager m_trialManager; /**< reference to the TrialManager instance */

	BarLegend barLegend;

	int m_participantIndex;
	int m_trailIndex;
	int m_currentDisplayList;

	bool m_hasTraining; /**< indicates if the program is in training mode */
	bool m_hasStudy;    /**< indicates if the program is in study mode */
	bool m_isDebug;     /**< indicates if the program is in debug mode */

private:
	bool m_needRedraw;

	// data debug use
	string m_dataDir;
};

