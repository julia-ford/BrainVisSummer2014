#include "keymouselog.h"
#include <iostream>
#include "mystdexp.h"
#include <iostream>
#include <time.h>
#include "myGL.h"
using namespace mystdexp;
using namespace std;


string timeString(const time_t &ctime){
	struct tm * timeinfo = localtime(&ctime);
	char timeStr[80];
	strftime (timeStr,80,"[ Time: %m/%d/%y %H:%M:%S ]",timeinfo);
	return timeStr;
}

string KeyEventRecord::GetString() const{
	return "[ KeyEvent: "+ timeString(m_time)+" "+convertChar(m_key)+" "+convertInt(m_state)+" ]";
}

string MouseEventRecord::GetString() const{
	return "[ MouseEvent: "+timeString(m_time)+" "+convertInt(m_button)+" "+convertInt(m_state)+" "
		+convertInt(m_x)+" "+convertInt(m_y)+" ]";
}

KeyMouseLog::KeyMouseLog(void){
	m_outSteamBuf = NULL;
	m_debugOut = false;
	m_key = 0;
	m_mouseButton = -1;
	m_mouseState = GLUT_UP;
	m_mouseX = -1;
	m_mouseY = -1;
}


KeyMouseLog::~KeyMouseLog(void){
	if(m_outFile.is_open()){
		m_outFile.close();
	}
}

void KeyMouseLog::SetOutput(const string& filename){
	m_outFile.open(filename.c_str());
	if(!m_outFile.is_open()){
		cout << "can't open event log file, user log will be lost!!!" << endl;
	}
	m_outSteamBuf = m_outFile.rdbuf();
}

void KeyMouseLog::SetOutput(std::streambuf* sb){
	m_outSteamBuf = sb;
}

void KeyMouseLog::SetDebugOut(bool debugOut){
	m_debugOut = debugOut;
}

void KeyMouseLog::KeyPressed(unsigned char key, int x, int y){
	m_key = key;
	m_mouseX = x;
	m_mouseY = y;
	m_keyState = GLUT_DOWN;
	keyRecord();
}

void KeyMouseLog::KeyReleased(unsigned char key, int x, int y){
	m_key = key;
	m_mouseX = x;
	m_mouseY = y;
	m_keyState = GLUT_UP;
	keyRecord();
}

void KeyMouseLog::InsertString(const string& str){
	time_t ctime;
	time(&ctime);
	ostream out(m_outSteamBuf);
	out << "[ Outside: "+ timeString(ctime)+ str +" ]"<<endl;
	if(m_debugOut) cout << str;
}

void KeyMouseLog::MouseDrag(int x,int y){
	m_mouseX = x;
	m_mouseY = y;
	mouseRecord();
}

void KeyMouseLog::MouseMove(int x, int y){
	m_mouseX = x;
	m_mouseY = y;
	mouseRecord();
}
void KeyMouseLog::MousePressed(int button, int state,int x, int y){
	m_mouseState = state;
	m_mouseX = x;
	m_mouseY = y;
	m_mouseButton = button;
	mouseRecord();
}
void KeyMouseLog::mouseRecord(){
	time_t ctime;
	time(&ctime);
	MouseEventRecord mouseRec(m_mouseButton,m_mouseState,m_mouseX,m_mouseY,ctime);
	string rec = mouseRec.GetString();
	ostream out(m_outSteamBuf);
	out << rec<<endl;
	if(m_debugOut) cout << rec<<endl;

}
void KeyMouseLog::keyRecord(){
	time_t ctime;
	time(&ctime);
	KeyEventRecord keyRec(m_key,m_keyState,ctime);
	string rec = keyRec.GetString();
	ostream out(m_outSteamBuf);
	out << rec<<endl;
	if(m_debugOut) cout << rec<<endl;
}