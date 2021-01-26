#pragma once

#include "BaseMesh.h"

class TessellationPlane : public BaseMesh
{

public:

	TessellationPlane(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int resolution);
	~TessellationPlane();

	void sendData(ID3D11DeviceContext*);

protected:

	void initBuffers(ID3D11Device* device);

	int resolution_;
};
