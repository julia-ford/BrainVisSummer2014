#include "selectionboxmanager.h"
#include "myGL.h"
#include "mystdexp.h"
using namespace mystdexp;

SelectionBoxManager::SelectionBoxManager(void){
	m_selectedIdx = -1;
	m_answer = false;
	m_confirmButton.SetContent("Next");
	changed();
}


SelectionBoxManager::~SelectionBoxManager(void){
	m_buttons.clear();
}


void SelectionBoxManager::Render(){
	for(unsigned int i = 0;i<m_buttons.size();i++){
		m_buttons[i].Draw();
	} 
	m_confirmButton.Draw();
	updated();
}

void SelectionBoxManager::SetNumBoxes(unsigned int numBoxes){
	m_buttons.clear();
	m_selectedIdx = -1;
	m_answer = false;

	// set up boxes
	// line up vertically

	float bwidth = m_range.x;
	float bheight = m_range.y/(numBoxes+1)/2;
	// better not be too tall
	bheight = (bheight > 0.618*bwidth?(0.618*bwidth):bheight);

	float tx = m_bottomLeft.x;
	m_buttons.resize(numBoxes);
	for(unsigned int i = 0;i<numBoxes;i++){
		float ty = m_bottomLeft.y + (numBoxes-i)*bheight*2;
		m_buttons[i].SetPosition(tx,ty,bwidth,bheight);
	} 
	m_confirmButton.SetPosition(tx,m_bottomLeft.y,bwidth,bheight);
	m_confirmButton.SetSelected(false);
	m_confirmButton.SetHovered(false);
	// fill in numbers
	for(unsigned int i = 0;i<m_buttons.size();i++){
		m_buttons[i].SetContent(convertInt(i+1));
	}
	changed();
}

void SelectionBoxManager::SetString(const string& str, int idx){
	if(idx>=0 && idx < m_buttons.size()){
		m_buttons[idx].SetContent(str);
	}
	changed();
}

bool SelectionBoxManager::IsIn(int x, int y) const{
	for(int i = 0;i<m_buttons.size();i++){
		if(m_buttons[i].IsIn(x,y)){
			return true;
		}
	}
	if(m_confirmButton.IsIn(x,y)){
		return true;
	}
	return false;
}
	// pixel pos in GL screen
void SelectionBoxManager::SetPosition(const vec3& bottomLeft, const vec3& range){
	m_bottomLeft = bottomLeft;
	m_range = range;
	int numBoxes = m_buttons.size();
	float cellHeight = m_range.y/(numBoxes+1);
	float bwidth = m_range.x;
	float bheight = cellHeight*0.7;
	// better not be too tall
	bheight = (bheight > 0.618*bwidth?(0.618*bwidth):bheight);

	float tx = m_bottomLeft.x;
	for(unsigned int i = 0;i<numBoxes;i++){
		float ty = m_bottomLeft.y + (numBoxes-i)*cellHeight;
		m_buttons[i].SetPosition(tx,ty,bwidth,bheight);
	} 

	m_confirmButton.SetPosition(tx,m_bottomLeft.y,bwidth,bheight);
	changed();
}

void SelectionBoxManager::MousePress(int button, int state, int x, int y){
	if(button==GLUT_MIDDLE_BUTTON || button==GLUT_RIGHT_BUTTON ) {
		return;
	}

	if (state == GLUT_DOWN) {
		for(int i = 0;i<m_buttons.size();i++){
			if(m_buttons[i].IsIn((float)x,(float)y)){
				if(m_buttons[i].IsSelected()){
					//m_buttons[i].SetSelected(false);
					//m_selectedIdx = -1;
				}
				else{
					m_buttons[i].SetSelected(true);
					m_selectedIdx = i;
					for(int k = 0;k<m_buttons.size();k++){
						if(k != i){
							m_buttons[k].SetSelected(false);
						}
					}
					changed();
				}
				break;
			}
		}

  		if(m_confirmButton.IsIn((float)x,(float)y)){
			if(!m_confirmButton.IsSelected()){
				m_confirmButton.SetSelected(true);
				// only has an answer
				// or no answer needed
				// shall we confirm
				m_answer = (m_selectedIdx >= 0 || m_buttons.size() == 0);
				changed();
			}
		}
		else{			
			if(m_confirmButton.IsSelected()){
				m_confirmButton.SetSelected(false);
				m_answer = false;
				changed();
			}
		}
	}
	/*
	else if (state == GLUT_UP){
		for(int i = 0;i<m_buttons.size();i++){
			if(m_buttons[i].IsIn((float)x,(float)y)){
				if(m_selectedIdx == i){
					m_buttons[i].SetSelected(true);
					m_selectedIdx = i;
				}
			}
			else{
				m_buttons[i].SetSelected(false);
			}
		}

  		if(m_confirmButton.IsIn((float)x,(float)y)){
			m_confirmButton.SetSelected(true);
			m_answer = true;
		}
		else{
			m_confirmButton.SetSelected(false);
			m_answer = false;
		}
	} 
	*/
	
}

void SelectionBoxManager::MouseMove(int button, int state, int x,int y){
	
	if(button==GLUT_MIDDLE_BUTTON || button==GLUT_RIGHT_BUTTON ) {
		return;
	}

	if (state == GLUT_DOWN) {
		return;
	}

	for(int i = 0;i<m_buttons.size();i++){
		if(m_buttons[i].IsIn((float)x,(float)y)){
			if(!m_buttons[i].IsHovered()){
				m_buttons[i].SetHovered(true);
				changed();
			}
		}
		else{
			if(m_buttons[i].IsHovered()){
				m_buttons[i].SetHovered(false);
				changed();
			}
		}
	}

  	if(m_confirmButton.IsIn((float)x,(float)y)){
		if(!m_confirmButton.IsHovered()){
			m_confirmButton.SetHovered(true);
			changed();
		}
	}
	else{
		if(m_confirmButton.IsHovered()){
			m_confirmButton.SetHovered(false);
			changed();
		}
	}
}

bool SelectionBoxManager::HasAnswered() const{
	return m_answer;
}

int SelectionBoxManager::GetFinnalAnswer() const{
	if(HasAnswered()){
		return m_selectedIdx;
	}
	else return -1;
}