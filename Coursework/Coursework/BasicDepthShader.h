#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;


class BasicDepthShader : public BaseShader
{
	

public:
	struct DisplacementBufferType {
		float displacement;
		XMFLOAT3 padding;
	};

	BasicDepthShader(ID3D11Device* device, HWND hwnd);
	~BasicDepthShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, float displacement);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* displacementBuffer;
};
