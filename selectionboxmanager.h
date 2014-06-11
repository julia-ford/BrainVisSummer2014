#pragma once
#include "pushbutton.h"
#include "vec3.h"
#include <vector>
#include <string>
using namespace std;

class SelectionBoxManager
{
public:
	SelectionBoxManager(void);
	~SelectionBoxManager(void);

	void Render();

	// number of choice
	// not including confirm button
	void SetNumBoxes(unsigned int numBoxes);
	void SetString(const string& str, int idx);
	bool NeedRepaint() const {return m_needRepaint;};
	bool IsIn(int x, int y) const;

	// pixel pos in GL screen
	void SetPosition(const vec3& bottomLeft, const vec3& range);
	void MousePress(int button, int state, int x, int y);
	void MouseMove(int button, int state, int x,int y);

	bool HasAnswered() const;
	int GetFinnalAnswer() const;
	int GetCurrentSelection() const{return m_selectedIdx;};
	int GetNumBoxes() const{return m_buttons.size();};

protected:

	void changed(){m_needRepaint = true;};
	void updated(){m_needRepaint = false;};
	vector<PushButton> m_buttons;

	PushButton m_confirmButton;
	vec3 m_bottomLeft;
	vec3 m_range;

	bool m_answer;
	bool m_needRepaint;
	
	int m_selectedIdx;
};

