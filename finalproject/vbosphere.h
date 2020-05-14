#pragma once

class VBOSphere 
{
private: 
	unsigned int vaoHandle;
public:
	VBOSphere();
	~VBOSphere();

	void render();
};

