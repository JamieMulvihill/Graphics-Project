#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class DepthShader : public BaseShader
{

public:

	struct TessellationBufferType {
		XMFLOAT4 cameraPosition;
	};

	DepthShader(ID3D11Device* device, HWND hwnd);
	~DepthShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* textureHeight, XMFLOAT4 cameraPos);

private:
	void initShader(const wchar_t* vsFilename, const wchar_t* psFilename);
	void initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* tessellationBuffer;
	ID3D11SamplerState* sampleState;
};
