#pragma once
#include "DXF.h"

class TreeSegmentMesh : public BaseMesh
{


public:
	TreeSegmentMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int resolution = 20);
	~TreeSegmentMesh();

protected:
	void initBuffers(ID3D11Device* device);
	int resolution;
};

