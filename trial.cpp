#include "trial.h"
#include "mystdexp.h"
#include <string>
#include <fstream>
#include <cassert>
#include "vec3.h"
using namespace std;
using namespace mystdexp;

#define MINLOCATION -999999.f
#define MAXLOCATION 999999.f
#define TRAININGBUNDLEQUEST 5

string getCoverFolderName(TrialInfoPtr trialInfoPtr){
	switch(trialInfoPtr->GetFiberConver()){
	case BUNDLE:
		return string("normal_allfb");
		break;
	case PARTIAL:
		return string("normal_partial");
		break;
	case WHOLE:
		return string("normal_whole");
		break;
	default:
		return string("[WRONG FIBER COVER!!!]");
		break;
	}
}

int getTaskIndex(TrialInfoPtr trialInfoPtr){
	switch(trialInfoPtr->GetMriTask()){
	case FA:
		return 1;
		break;
	case BUNDLE_TRACE:
		return 2;
		break;
	case BUNDLE_NAME:
		return 3;
		break;
	case LESION:
		return 4;
		break;
	case BUNDLE_SAME:
		return 5;
		break;
	default:
		return -1;
		break;
	}
}

string getTaskFolderName(TrialInfoPtr trialInfoPtr){
	switch(trialInfoPtr->GetMriTask()){
	case FA:
		return string("region_for_task1");
		break;
	case BUNDLE_TRACE:
		return string("region_for_task2");
		break;
	case BUNDLE_NAME:
		return string("region_for_task3");
		break;
	case LESION:
		return string("region_for_task4");
		break;
	case BUNDLE_SAME:
		return string("region_for_task5");
		break;
	default:
		return string("[WRONG TASK NAME!!!]");
		break;
	}
}

// fix resolution
string getResolutionFolderName(TrialInfoPtr trialInfoPtr){
	return string("s")+convertInt(trialInfoPtr->GetResolution());
}

string getBundleFolderName(TrialInfoPtr trialInfoPtr){
	switch(trialInfoPtr->GetBundle()){
	case CC:
		return string("cc");
		break;
	case CST:
		return string("cst");
		break;
	case IFO:
		return string("ilf");
		break;
	case ILF:
		return string("ifo");
		break;
	case CG:
		return string("cg");
		break;
	default:
		return string("[WRONG BUNDLE NAME!!!]");
		break;
	}
}

string getQuestFolderName(TrialInfoPtr trialInfoPtr){
	switch(trialInfoPtr->GetQuestIndex()){
	case 1:
		return string(".");
		break;
	case 2:
		return string("pos1");
		break;
	case 3:
		return string("pos2");
		break;
	case 4:
		return string("pos3");
		break;
	default:
		return string("[WRONG QUEST NAME!!!]");
		break;
	}
}

// fix resolution
string getTomorBoxFileName(TrialInfoPtr trialInfoPtr, int idx){
	return string("tumorbox_")+convertInt(idx)+string("_region_")+getResolutionFolderName(trialInfoPtr)+".data";
}

string getHighlightFiberIndexFileName(TrialInfoPtr trialInfoPtr){
	int taskIdx = getTaskIndex(trialInfoPtr);
	if(trialInfoPtr->GetMriTask() == BUNDLE_SAME){
		if(trialInfoPtr->GetQuestIndex()==1){
			// let 1 be yes case
			return string("yes/fiberidx_200000")+convertInt(taskIdx)+string("_region_")+getResolutionFolderName(trialInfoPtr)+".data";
		}
		else if(trialInfoPtr->GetQuestIndex()==2){
			// and 2 be no case
			return string("no/fiberidx_compound_region_")+getResolutionFolderName(trialInfoPtr)+".data";
		}
		else if(trialInfoPtr->GetQuestIndex()==3){
			// and 3 be complex
			Bundle bd = trialInfoPtr->GetBundle();
			FiberCover fb = trialInfoPtr->GetFiberConver();
			if( (bd == CC && fb == WHOLE)
				||(bd == CST && fb == BUNDLE)
				||(bd == CG && fb == WHOLE)
				||(bd == IFO && fb == WHOLE)
				||(bd == ILF && fb == BUNDLE)){
				return string("no/fiberidx_compound_region_")+getResolutionFolderName(trialInfoPtr)+".data";
			}
			else{
				return string("yes/fiberidx_200000")+convertInt(taskIdx)+string("_region_")+getResolutionFolderName(trialInfoPtr)+".data";
			}
		}
	}
	//else if (trialInfoPtr->GetMriTask() == BUNDLE_NAME){
	//}
	else{
		return string("fiberidx_200000")+convertInt(taskIdx)+string("_region_")+getResolutionFolderName(trialInfoPtr)+".data";
	}
}


string TrialInfo::GetTrialInfoString(const string separator) const{
	return "[ " + toString(m_task) + separator
		+toString(m_channel)+separator
		+toString(m_bundle)+separator
		+toString(m_shape)+separator
		+toString(m_cover)+separator
		+convertInt(m_quest)+separator
		+convertInt(textype)+separator
		+convertInt(m_resolution)+separator
		+(m_isTraining?"TRAINING":"STUDY")+separator
		+(m_isEmpty?"EMPTY":"NOTEMPTY")+" ]";
}

string TrialInfo::GetTableHeader(const string separator){
	return "[ " + MriTaskString()+separator
		+RentinalChannelString()+separator
		+BundleString()+separator
		+ShapeString()+separator
		+FiberCoverString()+separator
		+"qustSet"+separator
		+"txtType"+separator
		+"resltn"+separator
		+"proc"+separator
		+"isEmpty ]";

}

string TrialInfo::GetQuestionString() const{
	string str;
	// TODO: Why is this code commented out?
	//if(m_isTraining){
	//	str="[TRAINING] ";
	//}
	if(m_quest == TRAININGBUNDLEQUEST){
		return "";
	}
	switch(m_task){
	case FA:
		str += "Task 1: Which box, 1 or 2, has higher average FA value?";
		break;
	case BUNDLE_TRACE:
		// CHANGE LOG: text changed by Julia on 6/11/2014. 
		// Original Message:
		// "Task 2: The tubes originated from the yellow spheres end in the box 1, 2 or 3?"
		str += "Task 2: Do the tubes originating from the yellow spheres end in box 1, 2 or 3?";
		break;
	case BUNDLE_SAME:
		str += "Task 3: Do the fibers with golden halos belong to the same bundle? ";
		break;
	case LESION:
		str += "Task 4: Click the region where there is lesion.";
		break;
	case BUNDLE_NAME:
		str += "Task 4: Decide the name of the bundle.";
		break;
	default:
		str = "[ WRONG TEXT ]";
		break;
	}
	return str;
}

TrialData::TrialData(const Traces& traces)
	:m_traces(traces){
	correctAnswer = -1;
	m_difficulty = 0;
}

int TrialData::GetNumChoices() const{
	if(!boundingBoxes.empty()){
		return boundingBoxes.size()/6;
	}
	return 2;
}

int TrialData::GetNumSubFiberSegments(int traceIdx, int segIdx) const{
	int idx = indexOfFirst(cutFiberIndices,traceIdx);
	int startIdx = 0;
	int endIdx=m_traces.GetNumSegs(traceIdx)-1;
	if(idx>=0){
		int numIdx = leftSegments[idx].size();
		for(int ss=0;ss<numIdx;ss+=2){
			if(leftSegments[idx][ss]<=segIdx && segIdx <= leftSegments[idx][ss+1]){
				startIdx = leftSegments[idx][ss];
				endIdx = leftSegments[idx][ss+1];
				break;
			}
		}
	}
	return endIdx-startIdx+1;
}
float TrialData::GetSegmentSubFiberIndexf(int traceIdx, int segIdx) const{
	int idx = indexOfFirst(cutFiberIndices,traceIdx);
	int startIdx = 0;
	int endIdx=m_traces.GetNumSegs(traceIdx)-1;
	int ss;
	if(idx>=0){
		int numIdx = leftSegments[idx].size();
		for(ss=0;ss<numIdx;ss+=2){
			if(leftSegments[idx][ss]<=segIdx && segIdx <= leftSegments[idx][ss+1]){
				startIdx = leftSegments[idx][ss];
				endIdx = leftSegments[idx][ss+1];
				break;
			}
		}
		if(reverseHeadTail[idx][ss/2]){
			return 1-(segIdx-startIdx)/(float)(endIdx-startIdx);
		}
	}
	return (segIdx-startIdx)/(float)(endIdx-startIdx);
}

vec3 TrialData::GetBoxCenter(int idx) const{
	float x = (boundingBoxes[idx*6+0]+boundingBoxes[idx*+3])/2;
	float y = (boundingBoxes[idx*6+1]+boundingBoxes[idx*+4])/2;
	float z = (boundingBoxes[idx*6+2]+boundingBoxes[idx*+5])/2;
	return vec3(x,y,z);
}

vec3 TrialData::GetSegmentOrientation(int traceIdx, int segIdx) const{
	int idx = indexOfFirst(cutFiberIndices,traceIdx);
	int startIdx = 0;
	int endIdx=m_traces.GetNumSegs(traceIdx)-1;
	if(idx>=0){
		int numIdx = leftSegments[idx].size();
		for(int ss=0;ss<numIdx;ss+=2){
			if(leftSegments[idx][ss]<=segIdx && segIdx <= leftSegments[idx][ss+1]){
				startIdx = leftSegments[idx][ss];
				endIdx = leftSegments[idx][ss+1];
				break;
			}
		}
	}
	return m_traces.GetPosition(traceIdx,startIdx)-m_traces.GetPosition(traceIdx,endIdx);
}

bool TrialData::IsFiberTraced(int idx) const{
	for(int i = 0;i<spheres.size()/2;i++){
		if(spheres[i*2] == idx){
			return true;
		}
	}
	return false;
}

bool TrialData::InBox(const vec3& pos, int boxIdx) const{
	vec3 pp = pos-m_fiberCenter;
	if(pp.x < boundingBoxes[boxIdx*6+0] || pp.x > boundingBoxes[boxIdx*6+3] ) return false;
	if(pp.y < boundingBoxes[boxIdx*6+1] || pp.y > boundingBoxes[boxIdx*6+4] ) return false;
	if(pp.z < boundingBoxes[boxIdx*6+2] || pp.z > boundingBoxes[boxIdx*6+5] ) return false;
	return true;
}

bool TrialData::ContainsFiber(int idx) const{
	return contains(fiberIndices,idx);
}

bool TrialData::HighlightFiber(int idx) const{
	return contains(highlightFiberIndices,idx);
	//return false; //keqin
}

int TrialData::GetNumFibers() const{
	return fiberIndices.size();
}

int TrialData::GetFiberCutIndex(const int traceIdx) const{
	return indexOfFirst(cutFiberIndices,traceIdx);
}

int TrialData::GetFibers(int idx) const{
	return fiberIndices[idx];
}

void TrialData::LoadFromFiles(const char* fileRootDir,TrialInfoPtr trialInfoPtr){
	string slash("/");
	string dataPath = string(fileRootDir)+slash
		+getCoverFolderName(trialInfoPtr)+slash
		+getTaskFolderName(trialInfoPtr)+slash
		+getResolutionFolderName(trialInfoPtr)+slash
		+getBundleFolderName(trialInfoPtr)+slash;

	string dataPath2 = dataPath;
	if(trialInfoPtr->GetQuestIndex() >= 2){
			dataPath2 += getQuestFolderName(trialInfoPtr)+slash;
	}
	m_dataPath = dataPath2;
	m_task = trialInfoPtr->GetMriTask();
	clearAll();
	
	// read fibers to show
	{
		string fiberIdxFileName;
		fiberIdxFileName = dataPath2+"match.data";
		ifstream infile(fiberIdxFileName.c_str());
		if(infile.is_open()){
			int numFibers;
			infile >> numFibers;
			fiberIndices.reserve(numFibers);
			for(unsigned int i=0;i<numFibers;i++){
				int fiberIdx;
				infile >> fiberIdx;
				fiberIndices.push_back(fiberIdx);
			}
			infile.close();
		}
		else{
			// just show all
			fiberIndices.reserve(m_traces.GetNumFibers());
			for (int i = 0;i<m_traces.GetNumFibers();i++){
				fiberIndices.push_back(i);
			}
		}
	}
	m_fiberCenter = m_traces.GetCenter(fiberIndices);
	// read highlight/spheres
	if (m_task == BUNDLE_TRACE
		|| m_task == BUNDLE_SAME
		|| m_task == BUNDLE_NAME){
		string highlightFileName = dataPath2+getHighlightFiberIndexFileName(trialInfoPtr);
		ifstream infile(highlightFileName.c_str());
		if(infile.is_open()){
			unsigned int numIdxs;
			if(m_task == BUNDLE_SAME){
				// first line is the bundle number
				infile >> numIdxs;

				// bundle_same task: answer depend on questset
				// 1 is yes, 2 is no
				if(trialInfoPtr->GetQuestIndex()==1){
					// let 1 be yes case
					correctAnswer = 1;
				}
				else if(trialInfoPtr->GetQuestIndex()==2){
					// and 2 be no case
					correctAnswer = 0;
				}
				else if(trialInfoPtr->GetQuestIndex()==3){
					// and 3 be complex
				Bundle bd = trialInfoPtr->GetBundle();
				FiberCover fb = trialInfoPtr->GetFiberConver();
				if( (bd == CC && fb == WHOLE)
					||(bd == CST && fb == BUNDLE)
					||(bd == CG && fb == WHOLE)
					||(bd == IFO && fb == WHOLE)
					||(bd == ILF && fb == BUNDLE)){
						correctAnswer = 0;
					}
					else{
						correctAnswer = 1;
					}
				}
			}
			infile >> numIdxs;
			if(m_task == BUNDLE_TRACE){
				spheres.reserve(numIdxs*2);
				for(unsigned int i = 0;i<numIdxs;i++){
					int tmpIdx;
					infile >> tmpIdx;
					int idx_inWhole = fiberIndices[tmpIdx];
					spheres.push_back(idx_inWhole);

					// let spheres be put at head
					spheres.push_back(0);
				}
			}
			else {
				// bundle same task
				// or bundle name task
				highlightFiberIndices.reserve(numIdxs);
				for(unsigned int i = 0;i<numIdxs;i++){
					int tmpIdx;
					infile >> tmpIdx;
					assert(tmpIdx < fiberIndices.size());
					int highlightFiberIdx_inWhole = fiberIndices[tmpIdx];
					highlightFiberIndices.push_back(highlightFiberIdx_inWhole);
				}

				// correct answer for bundle name
				if(m_task == BUNDLE_NAME){
					switch(trialInfoPtr->GetBundle()){
					case CC:
						correctAnswer = 0;
						break;
					case CST:
						correctAnswer = 1;
						break;
					case ILF:
						correctAnswer = 2;
						break;
					case IFO:
						correctAnswer = 3;
						break;
					case CG:
						correctAnswer = 4;
						break;
					}
				}
			}
			infile.close();
		}
	}

	// read bounding box
	if (trialInfoPtr->GetMriTask() == BUNDLE_TRACE
		|| trialInfoPtr->GetMriTask() == FA
		|| trialInfoPtr->GetMriTask() == LESION){
		int numBoxes = 0;
		if(trialInfoPtr->GetMriTask() == BUNDLE_TRACE){
			numBoxes = 3;
		}
		else if( trialInfoPtr->GetMriTask() == FA) {
			numBoxes = 2;
		}
		else{
			numBoxes = 1;
		}
		if(numBoxes>0){
			boundingBoxes.reserve(6*numBoxes);
			for(unsigned int ibox=0;ibox<numBoxes;ibox++){

				string boxFileName = dataPath2+getTomorBoxFileName(trialInfoPtr,ibox);
				ifstream infile(boxFileName.c_str());
				if (infile.is_open()){
					char tmp[200];
					infile.getline(tmp,200,'\n');
					for(unsigned int i = 0;i<6;i++){
						float xx;
						infile >> xx;
						boundingBoxes.push_back(xx);
					}
					infile.close();
				}
			}
			permuteBoxes();
		}
	}

	m_resolution = trialInfoPtr->GetResolution();
	modify();
}

string TrialData::GetCorrectAnswerString() const{
	
	if(m_task == FA){
		// fa task
		return convertInt(correctAnswer+1);
	}
	else if(m_task == BUNDLE_TRACE){
		// bundle trace task
		return convertInt(correctAnswer+1);
	}
	// same bundle task
	else if(m_task == BUNDLE_SAME){
		if(correctAnswer == 0) return "NO";
		else return "YES";
	}
	else if(m_task == BUNDLE_NAME){
		// bundle name task
		switch(correctAnswer){
		case 0:
			return "CC";
			break;
		case 1:
			return "CST";
			break;
		case 2:
			return "ILF";
			break;
		case 3:
			return "IFO";
			break;
		case 4:
			return "CG";
			break;
		default:
			return "[WRONG NAME!!!]";
			break;
		}
	}
	else{
		// lesion task
		return "";
	}
}

void TrialData::swapBoxes(int a, int b){
	for(int i = 0;i<6;i++){
		swap(boundingBoxes,a*6+i,b*6+i);
	}
}
void TrialData::permuteBoxes(){
	for(int i = 1;i<boundingBoxes.size()/6;i++){
		int toSwap = rand()%(i+1);
		swapBoxes(i,toSwap);
	}
}
void TrialData::clearAll(){
	fiberIndices.clear();
	highlightFiberIndices.clear();
	boundingBoxes.clear();
	//spheres.clear();
	correctAnswer = -1;
}

void TrialData::modify(){

	if(m_task == FA){
		// fa task
		//assert(boundingBoxes.size() == 12);
		vec3 botlef0,toprgt0;
		vec3 botlef1,toprgt1;
		botlef0.set(boundingBoxes[0],boundingBoxes[1],boundingBoxes[2]);
		toprgt0.set(boundingBoxes[3],boundingBoxes[4],boundingBoxes[5]);
		botlef1.set(boundingBoxes[6],boundingBoxes[7],boundingBoxes[8]);
		toprgt1.set(boundingBoxes[9],boundingBoxes[10],boundingBoxes[11]);
		botlef0 += m_fiberCenter;
		toprgt0 += m_fiberCenter;
		botlef1 += m_fiberCenter;
		toprgt1 += m_fiberCenter;

		float fa0 = m_traces.GetAverageFA(botlef0,toprgt0);
		float fa1 = m_traces.GetAverageFA(botlef1,toprgt1);

		/*
		// test whether low sample rate
		// will affect the final answer
		float fa10 = m_traces.GetAverageFA(botlef0,toprgt0,3);
		float fa11 = m_traces.GetAverageFA(botlef1,toprgt1,3);
		if((fa0-fa1)*(fa10-fa11)>0) {
			assert(0);
		}
		*/

		float similiarFA = 0.08f;
		if(fa0>fa1+similiarFA) correctAnswer = 0;
		else if(fa1>fa0+similiarFA) correctAnswer = 1;
		else correctAnswer = 2;
		m_difficulty = fa0-fa1;
	}
	else if(m_task == BUNDLE_TRACE){
		// bundle trace task
		//assert(!spheres.empty());
		int numBoxes = boundingBoxes.size()/6;
		vector<vec3> botlft(numBoxes);
		vector<vec3> toprgt(numBoxes);
		for(int i = 0;i<numBoxes;i++){
			botlft[i].set(boundingBoxes[i*6],boundingBoxes[i*6+1],boundingBoxes[i*6+2]);
			toprgt[i].set(boundingBoxes[i*6+3],boundingBoxes[i*6+4],boundingBoxes[i*6+5]);
			botlft[i] += m_fiberCenter;
			toprgt[i] += m_fiberCenter;
		}
		for(int i = 0;i<spheres.size()/2;i++){
			int t = spheres[i*2];
			int numSegs = m_traces.GetNumSegs(t);
			vec3 head = m_traces.GetPosition(t,0);
			vec3 tail = m_traces.GetPosition(t,numSegs-1);
			for(int j = 0;j<numBoxes;j++){
				if(botlft[j] <= head && head <= toprgt[j] ){
					correctAnswer = j;

					// change the spheres to tail
					spheres[2*i+1] = 1;
				}
				else if(botlft[j] <= tail && tail <= toprgt[j] ){
					correctAnswer = j;

					// change the spheres to tail
					spheres[2*i+1] = 0;
				}
			}
		}
		//assert(correctAnswer >= 0);
		m_difficulty = spheres.size()/2;
		// push to further side
		if(correctAnswer>=0){
			vec3 correctCenter = (botlft[correctAnswer]+toprgt[correctAnswer])/2;
			if(correctAnswer >= 0 && numBoxes>0){
				for(unsigned int i = 0;i<spheres.size()/2;i++){
					int fibidx = spheres[i*2];
					vec3 head = m_traces.GetPosition(fibidx,0);
					vec3 tail = m_traces.GetPosition(fibidx,m_traces.GetNumSegs(fibidx)-1);
					if((correctCenter - head).norm() < (correctCenter-tail).norm()){
						spheres[i*2+1] = 1;
					}
				}
				//assert(!spheres.empty());
			}
		}
	}
	else if(m_task == LESION){
		// lesion task
		assert(boundingBoxes.size()==6);
		// get the actual bounding box of cut-off segments
		correctAnswer = -1;
		vec3 minLoc(MAXLOCATION,MAXLOCATION,MAXLOCATION),maxLoc(MINLOCATION,MINLOCATION,MINLOCATION);
		for(int tt = 0;tt<fiberIndices.size();tt++){
			int t = fiberIndices[tt];
			int numSegs = m_traces.GetNumSegs(t);
			for(int s = 0;s<numSegs;s++){
				vec3 pp = m_traces.GetPosition(t,s);
				if(InBox(pp,0)){
					minLoc.boundAsMin(pp);
					maxLoc.boundAsMax(pp);
				}
			}
		}
		correctLocationMin = minLoc - m_fiberCenter;
		correctLocationMax = maxLoc - m_fiberCenter;

		// first find fibers that requires cutting tag
		cutFiberIndices.clear();
		for(int tt = 0;tt<fiberIndices.size();tt++){
			int t = fiberIndices[tt];
			bool isFiberCut = false;
			int numSegs = m_traces.GetNumSegs(t);
			for(int s = 0;s<numSegs && !isFiberCut;s++){
				vec3 pp = m_traces.GetPosition(t,s);
				if(InBox(pp,0)){
					// add to this to the specials
					cutFiberIndices.push_back(t);
					isFiberCut = true;
				}
			}
		}

		// then tag those left over segments
		leftSegments.clear();
		for(int tt =0; tt<cutFiberIndices.size();tt++){
			int t = cutFiberIndices[tt];
			int numSegs = m_traces.GetNumSegs(t);
			bool isSegmentInBox = true;
			vector<int> tVec;
			for(int s = 0;s<numSegs;s++){
				vec3 pp = m_traces.GetPosition(t,s);
				if(InBox(pp,0)){
					if(!isSegmentInBox){
						// push back the last segment's end index
						tVec.push_back(s-1);
					}
					isSegmentInBox = true;
				}
				else{
					if(isSegmentInBox){
						// push back the next segment's start index
						tVec.push_back(s);
					}
					isSegmentInBox = false;

					if(s == numSegs-1){
						tVec.push_back(s);
					}
				}

			}
			assert(tVec.size()%2==0);
			leftSegments.push_back(tVec);
		}

		// last decide if the subFiber need to reverse head/tail
		reverseHeadTail.clear();
		for(int tt =0; tt<cutFiberIndices.size();tt++){
			int t = cutFiberIndices[tt];
			vector<bool> tVec;
			for(int i = 0;i<leftSegments[tt].size();i+=2){
				vec3 head = m_traces.GetPosition(t,leftSegments[tt][i]);
				vec3 tail = m_traces.GetPosition(t,leftSegments[tt][i+1]);
				float dx = fabs(head.x-tail.x);
				float dy = fabs(head.y-tail.y);
				float dz = fabs(head.z-tail.z);
				if(dx>=dy && dx>=dz){
					if(head.x<tail.x){
						tVec.push_back(true);
					}
					else{
						tVec.push_back(false);
					}
				}
				else if(dy>=dx && dy>=dz){
					if(head.y<tail.y){
						tVec.push_back(true);
					}
					else{
						tVec.push_back(false);
					}
				}
				else {
					if(head.z<tail.z){
						tVec.push_back(true);
					}
					else{
						tVec.push_back(false);
					}
				}
			}
			reverseHeadTail.push_back(tVec);
		}
	}
	else if(m_task == BUNDLE_SAME){
		// same bundle task
		if(boundingBoxes.empty()){
			// answer is defined by the quest set
			// so no more modify here
			// quest 1 is yes, 2 is no
			m_difficulty = highlightFiberIndices.size();
		}
	}
	else if(m_task == BUNDLE_NAME){
		m_difficulty = highlightFiberIndices.size();
	}
	assert(correctAnswer>=0 || m_task == LESION);
}
void TrialData::SetAllToShow(){
	fiberIndices.clear();
	int num = m_traces.GetNumFibers();
	fiberIndices.reserve(num);
	for(int i = 0;i<num;i++){
		//if(!m_traces.GetColor(i,0).isSimiliar(rgba(1,1,1),0.1)){
			fiberIndices.push_back(i);
		//}
	}
	m_fiberCenter = m_traces.GetCenter();

	m_textColors.clear();
	//m_textColors.push_back(rgba(1,1,1));
	m_textColors.push_back(rgba(1,0,0));
	m_textColors.push_back(rgba(1,1,0));
	m_textColors.push_back(rgba(0,1,0));
	m_textColors.push_back(rgba(0,0,1));
	m_textColors.push_back(rgba(1,0,1));

	m_textPositions.resize(m_textColors.size());
	for(int c = 0;c<m_textColors.size();c++){
		for(int t = 0;t<m_traces.GetNumFibers();t++){
			if(m_traces.GetColor(t,0).isSimiliar(m_textColors[c],0.1)){
				m_textPositions[c] = m_traces.GetPosition(t,0) - m_fiberCenter;
				break;
			}
		}
	}

	m_texts.resize(m_textColors.size());
	m_texts[0] = "CG";
	m_texts[1] = "CC";
	m_texts[2] = "ILF";
	m_texts[3] = "CST";
	m_texts[4] = "IFO";
}