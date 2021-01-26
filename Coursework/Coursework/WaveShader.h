#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class WaveShader : public BaseShader
{
private:
	struct LightBufferType
	{
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT4 position;
		XMFLOAT4 specularColour;
		XMFLOAT4 specularPower;
	};

	struct CameraBufferType {
		XMFLOAT3 cameraPosition;
		float padding;
	};

	struct AttenuationBufferType
	{
		float constantFactor;
		float linearFactor;
		float quadraticFactor;
	};

	struct TimeBufferType
	{
		float time;
		float amplitude;
		float frequency;
		float speed;
		XMFLOAT4 DirectionalVector;
	};

public:
	WaveShader(ID3D11Device* device, HWND hwnd);
	~WaveShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, Light* light, float time_, float amplitude_, float frequency_, float speed_, float Dx, float Dz, Camera* camera);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* timeBuffer;
	ID3D11Buffer* cameraBuffer;
	ID3D11Buffer* attenuationBuffer;
	ID3D11BlendState* blendState;
	D3D11_BLEND_DESC blendDesc;
	ID3D11Device* device_;
};
