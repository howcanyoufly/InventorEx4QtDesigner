

#ifndef SOGRADIENTBACKGROUND_H
#define SOGRADIENTBACKGROUND_H

#include <Inventor/fields/SoSFColor.h>
#include <Inventor/nodes/SoNode.h>

class SoGradientBackground : public SoNode
{
	SO_NODE_HEADER(SoGradientBackground);
	
public:
	SoGradientBackground();
	
	virtual SbBool affectsState() const;
	
	static void exitClass();
	
	virtual void GLRender(SoGLRenderAction* action);
	
	static void initClass();
	
	SoSFColor color0;
	
	SoSFColor color1;
	
protected:
	virtual ~SoGradientBackground();
	
private:
	
};

#endif // SOGRADIENTBACKGROUND_H
