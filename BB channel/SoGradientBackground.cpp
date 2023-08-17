
#ifdef WIN32
#include <windows.h>
#endif // WIN32

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else // __APPLE__
#include <GL/gl.h>
#endif // __APPLE__

#include <Inventor/nodes/SoSubNode.h>

#include "SoGradientBackground.h"

SO_NODE_SOURCE(SoGradientBackground)

SoGradientBackground::SoGradientBackground() :
	SoNode(),
	color0(),
	color1()
{
	SO_NODE_CONSTRUCTOR(SoGradientBackground);
	
	SO_NODE_ADD_FIELD(color0, (1.0f, 1.0f, 1.0f));
	SO_NODE_ADD_FIELD(color1, (0.0f, 0.0f, 0.0f));
}

SoGradientBackground::~SoGradientBackground()
{
}

SbBool
SoGradientBackground::affectsState() const
{
	return false;
}

void
SoGradientBackground::exitClass()
{
}

void
SoGradientBackground::GLRender(SoGLRenderAction* action)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	
	glBegin(GL_QUADS);
	
	glColor3f(this->color0.getValue()[0], this->color0.getValue()[1], this->color0.getValue()[2]);
	glVertex2f(1.0, 1.0);
	glVertex2f(-1.0, 1.0);
	
	glColor3f(this->color1.getValue()[0], this->color1.getValue()[1], this->color1.getValue()[2]);
	glVertex2f(-1.0, -1.0);
	glVertex2f(1.0, -1.0);
	
	glEnd();
	
	glPopAttrib();
	
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void
SoGradientBackground::initClass()
{
	SO_NODE_INIT_CLASS(SoGradientBackground, SoNode, "Node");
}
