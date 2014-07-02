#pragma once

#include <string>
#include "vec3.h"
#include "rgba.h"
using namespace std;

class PushButton
{
public:
	PushButton(void);
	~PushButton(void);

	void Draw();
	bool IsIn(const vec3 &pos) const;
	bool IsIn(float x, float y) const;

	bool IsSelected() const {return m_selected;};
	bool IsHovered() const {return m_hovered;};
	void SetHovered(bool b){m_hovered = b;};
	void SetSelected(bool b){m_selected = b;};
	void SetPosition(const vec3 &pos,const vec3 &range);
	void SetPosition(const float x, const float y,const float width, const float height);
	void SetColor(const rgba &color){m_color = color;};
	void SetContent(const string &str){m_content=str;};

protected:
	string m_content;
	bool m_hovered;
	bool m_selected;

	vec3 m_bottomLeft;
	vec3 m_range;

	rgba m_color;
};

