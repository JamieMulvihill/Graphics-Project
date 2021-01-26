#pragma once

#include "BaseMesh.h"
#include "DXF.h"

class SkyBox : public BaseMesh
{
public:
	SkyBox(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int resolution = 20);
	~SkyBox();

protected:
	void initBuffers(ID3D11Device* device);
	int resolution;
};



