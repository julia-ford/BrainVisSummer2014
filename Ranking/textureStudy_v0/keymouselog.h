#pragma once
#include <ostream>
#include <string>
#include <time.h>
#include <fstream>
using namespace std;

class KeyEventRecord{
public:
	KeyEventRecord(unsigned char key, int state, const time_t &ctime)
	: m_key(key),m_state(state),m_time(ctime){};

	string GetString() const;
	friend ostream& operator<<(ostream &out, const KeyEventRecord &keyRec){
		out << keyRec.GetString();
	};

protected:
	unsigned char m_key;
	int m_state;
	time_t m_time;
};

class MouseEventRecord{
public:
	MouseEventRecord(int button, int state, int x, int y, const time_t &ctime)
		:m_button(button),m_state(state),m_x(x),m_y(y),m_time(ctime){};

	string GetString() const;
	friend ostream& operator<<(ostream &out, const MouseEventRecord &mouseRec){
		out << mouseRec.GetString();
	};

protected:
	int m_button;
	int m_state;
	int m_x;
	int m_y;
	time_t m_time;
};

class KeyMouseLog
{
public:
	KeyMouseLog(void);
	~KeyMouseLog(void);

	void SetOutput(const string& filename);
	void SetOutput(streambuf* sb);
	void SetDebugOut(bool debugOut);
	void KeyPressed(unsigned char key, int x, int y);
	void KeyReleased(unsigned char key, int x, int y);
	void InsertString(const string& str);

	void MouseDrag(int x,int y);
	void MouseMove(int x, int y);
	void MousePressed(int button, int state,int x, int y);

protected:
	void mouseRecord();
	void keyRecord();
	streambuf* m_outSteamBuf;
	ofstream m_outFile;
	bool m_debugOut;
	
	unsigned char m_key;
	int m_keyState;
	int m_mouseButton;
	int m_mouseState;
	int m_mouseX;
	int m_mouseY;
};

