#pragma once

#include "DXF.h"

class CylinderMesh : public BaseMesh
{

public:
	CylinderMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int resolution, int stacks, int slices);
	~CylinderMesh();

protected:
	void initBuffers(ID3D11Device* device);

	int resolution_, stacks_, slices_;
};


