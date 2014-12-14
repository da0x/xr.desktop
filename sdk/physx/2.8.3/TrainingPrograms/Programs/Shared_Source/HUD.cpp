// ===============================================================================
//						  NVIDIA PHYSX SDK TRAINING PROGRAMS
//			                       HEADS-UP DISPLAY
//
//						    Written by Bob Schade, 5-1-06
// ===============================================================================

#include <GL/glut.h>

#include "GLFontRenderer.h"
#include "HUD.h"

void HUD::AddDisplayString(char* s, NxReal x, NxReal y)
{
	DisplayString ds;
	sprintf(ds.m_string, "%s", s);
	ds.m_xpos = x;
	ds.m_ypos = y;
	m_DisplayString.pushBack(ds);
}

void HUD::SetDisplayString(NxU32 i, char* s, NxReal x, NxReal y)
{
	sprintf(m_DisplayString[i].m_string, "%s", s);
	m_DisplayString[i].m_xpos = x;
	m_DisplayString[i].m_ypos = y;
}

void HUD::Clear()
{
	m_DisplayString.clear();
}

void HUD::Render()
{
	GLFontRenderer::setScreenResolution(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	for (unsigned int i = 0;  i < m_DisplayString.size(); ++i)
	{
		GLFontRenderer::print(m_DisplayString[i].m_xpos, m_DisplayString[i].m_ypos, 0.024f, m_DisplayString[i].m_string);
	}
}


