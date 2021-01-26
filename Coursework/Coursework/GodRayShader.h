#pragma once

#include "BaseShader.h"
#include "DXF.h"

using namespace std;
using namespace DirectX;

class GodRayShader : public BaseShader
{
public:
	GodRayShader(ID3D11Device* device, HWND hwnd);
	~GodRayShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* texture1, Light* light, XMFLOAT2 screenSize, const XMMATRIX& view2, const XMMATRIX& proj2, float decay, float density, float exposure, float weight);
private:
	struct GodRayBufferType
	{
		XMFLOAT4 lightPosition;
		XMFLOAT2 screenSize;
		XMFLOAT2 padding;
		XMMATRIX viewMatrix;
		XMMATRIX projectionMatrix;
	};
	struct VariableBufferType
	{
		float decay;
		float exposure;
		float density;
		float weight;
	};


private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* godRayBuffer;
	ID3D11Buffer* variableBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateRay;
};

