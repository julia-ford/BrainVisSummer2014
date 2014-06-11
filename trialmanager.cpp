#include "trialmanager.h"
#include "mystdexp.h"
using namespace mystdexp;
#include <iostream>
#include <fstream>

//===========================================================================//
// CHANGE LOG: file name changed by Julia on 6/11/2014.                      //
// Original File Name: "trailmanager.cpp"                                    //
//===========================================================================//

/*
	if trial data is changed,
	please also modify the following functions:
	GetTrailDataPtr()
*/

// this affect both training and 
// study, so change it whenever
// switch between training/study

#define DATARESOLUTION 3

// affect only training
#define TRAININGRES 4

#define NUMPARTICIPANTS 15
#define TRAININGBUNDLEQUEST 5
//> E1-cc  E2-cst  E3-ilf  E4-ifo
//> E3-ifo  E4-ilf   E1-cst E2-cc
//> E4-cst E3-cc  E2-ifo   E1-ilf
//> E2-ilf   E1-ifo  E4-cc  E3-cst 

RetinalChannel LTEbb[4][4]={
	//// block 1
     // E1-cc  E2-cst  E3-ilf  E4-ifo
	{COLOR, SATURATION, SIZE, TEXTURE},

	//// block 2
     //> E3-ifo  E4-ilf   E1-cst E2-cc
    {SIZE, TEXTURE, COLOR, SATURATION},

	//// block 3
    //> E4-cst E3-cc  E2-ifo   E1-ilf
    {TEXTURE, SIZE, SATURATION, COLOR},
	
	//// block 4
    //> E2-ilf   E1-ifo  E4-cc  E3-cst 
    {SATURATION, COLOR, TEXTURE, SIZE},
};

/*Bundle         LTBbb[4][4]={
	//// block 1
     // E1-cc  E2-cst  E3-ilf  E4-ifo
	{CC, CST, ILF, IFO},

	//// block 2
    //> E3-ifo  E4-ilf   E1-cst E2-cc
	{IFO, ILF, CST, CC},

	//// block 3
    //> E4-cst E3-cc  E2-ifo   E1-ilf
	{CST, CC, IFO, ILF},

	//// block 4
    //> E2-ilf   E1-ifo  E4-cc  E3-cst 
	{ILF, IFO, CC, CST}
};*/

//keqin_1
Bundle LTBbb[4][4]={
	//// block 1
     // E1-cc  E2-IFO  E3-IFO  E4-IFO
	{IFO, IFO, IFO, IFO},

	//// block 2
    //> E3-IFO  E4-IFO   E1-IFO E2-cc
	{IFO, IFO, IFO, IFO},

	//// block 3
    //> E4-IFO E3-cc  E2-IFO   E1-IFO
	{IFO, IFO, IFO, IFO},

	//// block 4
    //> E2-IFO   E1-IFO  E4-cc  E3-IFO 
	{IFO, IFO, IFO, IFO}
};

	//// block 1
/*
Aa	Bd	Cb	De	Ec
Db	Ee	Ac	Ba	Cd
Bc	Ca	Dd	Eb	Ae
Ed	Ab	Be	Cc	Da
Ce	Dc	Ea	Ad	Bb
*/

/*
CC	CG	CST	IFO ILF
IFO ILF	CC	CG	CST
CG	CST	IFO	ILF	CC
ILF	CC	CG	CST	IFO
CST	IFO	ILF	CC	CG 
*/

/*
COLOR,	TEXTURE,	SATURATION,	ORIENTATION,	SIZE,
SATURATION,	ORIENTATION,	SIZE,	COLOR,	TEXTURE,
SIZE,	COLOR,	TEXTURE,	SATURATION,	ORIENTATION,
TEXTURE,	SATURATION,	ORIENTATION,	SIZE,	COLOR,
ORIENTATION,	SIZE,	COLOR,	TEXTURE,	SATURATION,
*/


RetinalChannel LTE[5][5]={
	{COLOR,	TEXTURE,	SATURATION,	ORIENTATION,	SIZE},
	{SATURATION,	ORIENTATION,	SIZE,	COLOR,	TEXTURE},
	{SIZE,	COLOR,	TEXTURE,	SATURATION,	ORIENTATION},
	{TEXTURE,	SATURATION,	ORIENTATION,	SIZE,	COLOR},
	{ORIENTATION,	SIZE,	COLOR,	TEXTURE,	SATURATION}
};


//keqin_1
/*RetinalChannel LTE[5][5]={
	{SIZE,	SIZE,	SIZE,	SIZE,	SIZE},
	{SIZE,	SIZE,	SIZE,	SIZE,	SIZE},
	{SIZE,	SIZE,	SIZE,	SIZE,	SIZE},
	{SIZE,	SIZE,	SIZE,	SIZE,	SIZE},
	{SIZE,	SIZE,	SIZE,	SIZE,	SIZE}
};*/

/*Bundle         LTB[5][5]={
	{CC,	CG,		CST,	IFO,	ILF},
	{IFO,	ILF,	CC,		CG,		CST},
	{CG,	CST,	IFO,	ILF,	CC},
	{ILF,	CC,		CG,		CST,	IFO},
	{CST,	IFO,	ILF,	CC,		CG} 
};*/

#define IFO ILF
//keqin_1
Bundle         LTB[5][5]={
	{IFO,	IFO,		IFO,	   IFO,	IFO},
	{IFO,	IFO,	    IFO,	   IFO,	IFO},
	{IFO,	IFO,	    IFO,	   IFO,	IFO},
	{IFO,	IFO,		IFO,	   IFO,	IFO},
	{IFO,	IFO,	    IFO,	   IFO,	IFO} 
};
#undef IFO

//keqin_2
/*Bundle         LTB[5][5]={
	{IFO,	IFO,		IFO,	   IFO,	IFO},
	{IFO,	IFO,	    IFO,	   IFO,	IFO},
	{IFO,	IFO,	    IFO,	   IFO,	IFO},
	{IFO,	IFO,		IFO,	   IFO,	IFO},
	{IFO,	IFO,	    IFO,	   IFO,	IFO} 
};*/

/*FiberCover     D[4][4]={
	{BUNDLE, WHOLE},

	{BUNDLE, WHOLE},

	{BUNDLE, WHOLE},

	{BUNDLE, WHOLE},
};*/

//keqin_2
/*FiberCover     D[4][4]={
	{BUNDLE, BUNDLE},

	{BUNDLE, BUNDLE},

	{BUNDLE, BUNDLE},

	{BUNDLE, BUNDLE},
};*/

#define WHOLE BUNDLE
//keqin_1
FiberCover     D[4][4]={
	{WHOLE, WHOLE},

	{WHOLE, WHOLE},

	{WHOLE, WHOLE},

	{WHOLE, WHOLE},
};
#undef WHOLE

/*
	R-1,T-2,S-3
	S-2,R-3,T-1
	T-3,S-1,R-2
*/

/*Shape LTS[3][3]={
	{RIBBON,		TUBE,			SUPERQUADRIC},
	{SUPERQUADRIC,	RIBBON,			TUBE	},
	{TUBE,			SUPERQUADRIC,	RIBBON},
};*/

#define RIBBON RIBBON
Shape LTS[3][3]={
	{RIBBON,	RIBBON,		RIBBON},
	{RIBBON,	RIBBON,		RIBBON	},
	{RIBBON,	RIBBON,  	RIBBON},
};
#undef RIBBON

int texType = 2;

MriTask tasks[4] = {
	FA, BUNDLE_TRACE,BUNDLE_SAME,BUNDLE_NAME
};


// do not change
 Bundle bundleSquence[5]={
    CC,CST,ILF,IFO,CG
};

FiberCover fiberCovers[2] = {
	BUNDLE,WHOLE
};

TrialManager::TrialManager(void){
	m_hasTrainingTraces = false;
};

TrialManager::~TrialManager(void){
	deleteTrialInfos();
	deleteTrialData();
	m_hasTrainingTraces = false;
}

int TrialManager::GetNumTrials() const{
	return m_trialInfoPtrs.size();
}

// each participant does same amount of jobs
int TrialManager::GetNumTrialsPerParticipant() const{
	return m_trialInfoPtrs.size()/NUMPARTICIPANTS+m_trainingInfoPtrs.size();
}

// get methods
TrialInfoPtr TrialManager::GetTrailInfoPtr(int participant, int trialIdx) const{
	if(trialIdx<m_trainingInfoPtrs.size()) return m_trainingInfoPtrs[trialIdx];
	return m_trialInfoPtrs[participant*GetNumTrialsPerParticipant()+trialIdx-m_trainingInfoPtrs.size()];
}


void TrialManager::reWriteLatinSquareTrailInfos(){

} 


// shuffle methods
void TrialManager::GenerateLatinSquareTrailInfos(){
	deleteTrialInfos();

	//vector<TrialInfoPtr> infos;
	//for(int iP = 0;iP<NUMPARTICIPANTS;iP++){
	//	for(int iT = 0;iT<4;iT++){
	//		for(int iB=0;iB<4;iB++){
	//			for(int iD = 0;iD<2;iD++){
	//				for(int iQ=0;iQ<2;iQ++){
	//					TrialInfoPtr info = new TrialInfo(ORIENTATION,tasks[iT],B[iB],RIBBON,D[iD],quests[iQ],
	//						3,texTypes[1]);
	//					m_trialInfoPtrs.push_back(info);
	//				}
	//			}
	//		}
	//	}
	//}

	ofstream outfile("Latin.txt");
	if(!outfile.is_open()){
		cout << "Latin file can't open!!!" << endl;
	}
	for(int ip=0;ip<NUMPARTICIPANTS;ip++){
		int latin1row = ip%3;
		int latin2row = (ip/3)%3;
		outfile << endl << "paricipant "<< ip << endl;
		//outfile << "\t" << TrialInfo::GetTableHeader("\t") << endl;

		for(int it=0;it<4;it++){
			MriTask task = tasks[it];
			TrialInfoPtr info = new TrialInfo(task);
			m_trialInfoPtrs.push_back(info);
			int thisTaskStartIdx = m_trialInfoPtrs.size();
			int outIdx = 0;
			bool isOrientationAddedWhole = false;
			bool isOrientationAddedBundle = false;

			vector<vector<int>> questOrder(10);

			for(int iqo = 0;iqo<questOrder.size();iqo++){
				questOrder[iqo].clear();
				questOrder[iqo].push_back(1);
				questOrder[iqo].push_back(2);
				questOrder[iqo].push_back(3);
				permute(questOrder[iqo]);
			}

			for(int i=0;i<30;i++){
				int latin1column = (i/2)%5;
				int ibd = i%2;
				int ishape = i/10;
				int quest = questOrder[i%10][i/10];
				FiberCover fc = D[latin1row][ibd];
				RetinalChannel rc = LTE[latin1row][latin1column];
				Shape shape= LTS[latin2row][ishape];
				Bundle bd = LTB[latin1row][latin1column];
				if(rc == ORIENTATION){
					if(fc == WHOLE){
						if(isOrientationAddedWhole){
							continue;
						}
						isOrientationAddedWhole = true;
					}
					if(fc == BUNDLE){
						if(isOrientationAddedBundle){
							continue;
						}
						isOrientationAddedBundle = true;
					}
					quest = 3;
					shape = RIBBON;
				}
				TrialInfoPtr info = new TrialInfo(rc,task,
					bd,shape,
					fc,quest,
					DATARESOLUTION,texType);
				m_trialInfoPtrs.push_back(info);

				//outfile << it*24+i << "\t" << info->GetTrialInfoString("\t") << endl;
				if(it==3){
					outfile << outIdx << "\t"  <<toString(shape) << "--"
						<<toString(fc) << "--"
						<<toString(bd) << "--"
						<< toString(rc) << "--"
						<<quest << "\t\t\t";
					outfile << endl;
					outIdx ++;
				}
			}

			// shuffle naming task
			if(task == BUNDLE_NAME){
				permute(m_trialInfoPtrs,m_trialInfoPtrs.size()-1,thisTaskStartIdx);
			}
		}
		outfile << endl;
	}

	outfile.close();

}

void TrialManager::GenerateTrainingInfos(){
	// add training session here
	deleteTrainingInfos();
	// remember empty screen between tasks
	m_trainingInfoPtrs.reserve(20);
	const int numTrainings = 21;
	MriTask trainingTasks[numTrainings]
								= {	FA,		FA,		FA,		FA,		FA,		FA,
									BUNDLE_TRACE,	BUNDLE_TRACE,	BUNDLE_TRACE,	BUNDLE_TRACE,	BUNDLE_TRACE,
									BUNDLE_SAME,	BUNDLE_SAME,	BUNDLE_SAME,	BUNDLE_SAME,	BUNDLE_SAME,
									BUNDLE_NAME,	BUNDLE_NAME,	BUNDLE_NAME,	BUNDLE_NAME,	BUNDLE_NAME};

	RetinalChannel trainingRcs[numTrainings]
								={	COLOR,			SATURATION,		SIZE,			TEXTURE,	ORIENTATION,	TEXTURE,
									ORIENTATION,	TEXTURE,		COLOR,			SATURATION,		SIZE,
									SIZE,			ORIENTATION,	TEXTURE,		COLOR,			SATURATION,
									SATURATION,		SIZE,			ORIENTATION,	TEXTURE,		COLOR,
								};

	Shape trainingShapes[numTrainings]
								= {	TUBE,			TUBE,			TUBE,			TUBE,		RIBBON,	SUPERQUADRIC,
									RIBBON,			TUBE,			RIBBON,			SUPERQUADRIC,	SUPERQUADRIC,
									TUBE,			RIBBON,			RIBBON,			SUPERQUADRIC,	RIBBON,
									TUBE,			RIBBON,			RIBBON,			SUPERQUADRIC,	TUBE};

	Bundle trainingBundles[numTrainings]
								= {	CC,				ILF,			IFO,			CST,	CC,		CG,
									IFO,			ILF,			CST,			CC,		CG,
									CST,			CC,				IFO,			CG,		ILF,
									ILF,			CG,				IFO,			CC,				CST};

	FiberCover trainingCovers[numTrainings]
								={	BUNDLE,			WHOLE,			BUNDLE,			WHOLE,	WHOLE,	BUNDLE,
									BUNDLE,			WHOLE,			BUNDLE,			WHOLE,	WHOLE,
									BUNDLE,			WHOLE,			BUNDLE,			WHOLE,	BUNDLE,
									BUNDLE,			WHOLE,			BUNDLE,			WHOLE,	WHOLE};

	int trainingQuests[numTrainings]
								={	1,				1,				1,				2,		2,		1,
									1,				2,				2,				1,		2,
									2,				1,				1,				2,		1,
									2,				1,				2,				1,		2};


	TrialInfoPtr trainingTrialInfo;
	// first come the training traces
	TrialInfoPtr emptyTrialInfo = new TrialInfo(COLOR,FA,CC,TUBE,WHOLE,TRAININGBUNDLEQUEST,true);
	m_trainingInfoPtrs.push_back(emptyTrialInfo);
	
	ofstream outfile("Training.txt");
	if(!outfile.is_open()){
		cout << "Latin file can't open!!!" << endl;
	}

	// any not FA
	MriTask task = LESION;
	for(int i = 0;i<numTrainings;i++){
		if(task!=trainingTasks[i]){
			TrialInfoPtr emptyTrialInfo = new TrialInfo(trainingTasks[i]);
			m_trainingInfoPtrs.push_back(emptyTrialInfo);
			task = trainingTasks[i];
			
			outfile << toString(task) << endl;  

		}
		
		TrialInfoPtr trainingTrialInfo = new TrialInfo(
			trainingRcs[i],task,trainingBundles[i],
			trainingShapes[i],trainingCovers[i],trainingQuests[i],TRAININGRES,texType,true);

		m_trainingInfoPtrs.push_back(trainingTrialInfo);

		outfile << i << "\t"  
			<<toString(trainingRcs[i]) << "--"
			<<toString(trainingBundles[i]) << "--"
			<<toString(trainingShapes[i]) << "--"
			<<trainingQuests[i] << "\t\t\t";
		outfile << endl;

	}

	outfile.close();
	/*
	for(int it=0;it<4;it++){
		TrialInfoPtr emptyTrialInfo = new TrialInfo(tasks[it]);
		m_trainingInfoPtrs.push_back(emptyTrialInfo);
		for(int i = 0;i<4;i++){
			TrialInfoPtr trainingTrialInfo = new TrialInfo(
				trainingRcs[i],tasks[it],trainingBundles[i],
				trainingShapes[i],trainingCovers[i],trainingQuests[i],TRAININGRES,texType,true);

			m_trainingInfoPtrs.push_back(trainingTrialInfo);
		}
	}
	*/
}

// load other affiliate info
void TrialManager::LoadTrialData(const char* dataRootDir,const Traces& traces){
	m_trialDataPtrs.clear();
	m_trialDataPtrs.reserve(97);

	// read all rendering unrelated data from file
	// if order is changed, please also modify GetTrailDataPtr()
	// bundle loop
	for(unsigned int bdi = 0;bdi<5;bdi++){
		// quest loop
		for(unsigned int qsti=0;qsti<3;qsti++){
			// cover loop
			for(unsigned int covi=0;covi<2;covi++){
				// task loop
				for(unsigned int tski=0;tski<4;tski++){
					TrialInfo trialInfo(TEXTURE,
					tasks[tski],bundleSquence[bdi],TUBE,
						fiberCovers[covi],qsti+1,DATARESOLUTION);
					//m_trialInfoPtrs.push_back(trialInfo);
					TrialDataPtr trialDataPtr = new TrialData(traces);
					trialDataPtr -> LoadFromFiles(dataRootDir,&trialInfo);
					m_trialDataPtrs.push_back(trialDataPtr);
					std::cout << "\rTrialManager: Loading Trial Data: " 
						<< (float)(bdi*24+qsti*8+covi*4+tski+1)/120*100<<"%        ";
				}
			}
		}
	}

	// plus an empty data
	TrialDataPtr emptyDataPtr = new TrialData(traces);
	m_trialDataPtrs.push_back(emptyDataPtr);
	std::cout << endl;
}

void TrialManager::LoadTrianingTracesData(const Traces& trainingTraces){
	TrialDataPtr trainingTracesDataPtr = new TrialData(trainingTraces);
	trainingTracesDataPtr->SetAllToShow();
	m_trialDataPtrs.push_back(trainingTracesDataPtr);
	std::cout << endl;
	m_hasTrainingTraces = true;
}

TrialDataPtr TrialManager::GetTrailDataPtr(const TrialInfoPtr trialInfoPtr) const{
	// empty trial
	if(trialInfoPtr->IsEmpty()){
		int numTrailData = m_trialDataPtrs.size();
		if(m_hasTrainingTraces){
			return m_trialDataPtrs[numTrailData-2];
		}
		else{
			return m_trialDataPtrs[numTrailData-1];
		}
	}

	// training trial
	if(trialInfoPtr->GetQuestIndex() == TRAININGBUNDLEQUEST){
		return m_trialDataPtrs.back();
	}

	int idx = 0;
	int weight = 1;
	switch(trialInfoPtr->GetMriTask()){
	case FA:
		idx += 0*weight;
		break;
	case BUNDLE_TRACE:
		idx += 1*weight;
		break;
	case BUNDLE_SAME:
		idx += 2*weight;
		break;
	case BUNDLE_NAME:
		idx += 3*weight;
		break;
	default:
		break;
	}
	weight *= 4;
	switch(trialInfoPtr->GetFiberConver()){
	case BUNDLE:
		idx += 0*weight;
		break;
	case WHOLE:
		idx += 1*weight;
		break;
	default:
		break;
	}
	weight *=2;
	switch(trialInfoPtr->GetQuestIndex()){
	case 1:
		idx += 0*weight;
		break;
	case 2:
		idx += 1*weight;
		break;
	case 3:
		idx += 2*weight;
		break;
	default:
		break;
	}
	weight *=3;

	// follow the first participant's order
	switch(trialInfoPtr->GetBundle()){
	case CC:
		idx += 0*weight;
		break;
	case CST:
		idx += 1*weight;
		break;
	case ILF:
		idx += 2*weight;
		break;
	case IFO:
		idx += 3*weight;
		break;
	case CG:
		idx += 4*weight;
		break;
	default:
		break;
	}
	return m_trialDataPtrs[idx];
}

void TrialManager::deleteTrialInfos(){
	freePointerVector(m_trialInfoPtrs);
	m_trialInfoPtrs.clear();
}

void TrialManager::deleteTrialData(){
	freePointerVector(m_trialDataPtrs);
	m_trialInfoPtrs.clear();
	m_hasTrainingTraces = false;
}
void TrialManager::deleteTrainingInfos(){
	freePointerVector(m_trainingInfoPtrs);
	m_trainingInfoPtrs.clear();
}