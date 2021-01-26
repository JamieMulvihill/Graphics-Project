#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class LightShader : public BaseShader
{
private:
	struct DirectionalLightBufferType
	{
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT4 direction;
	};

	struct LightBufferType
	{
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT4 position;
		XMFLOAT4 specularPower;
		XMFLOAT4 specularColour;
	};

	struct CameraBufferType {
		XMFLOAT3 camerPosition;
		float padding;
	};

	struct DisplacementBufferType {
		float displacement;
		XMFLOAT3 padding;
	};

	struct SpotLightBufferType{
		XMFLOAT4 ambientColour;
		XMFLOAT4 diffuseColour;
		XMFLOAT3 spotPosition;
		float angleRad;
		XMFLOAT3 direction;
		int light_Setting;
	};

	struct AttenuationBufferType{
		float constant_Factor;
		float linear_Factor;
		float quadratic_Factor;
		float pad;
	};

public:
	LightShader(ID3D11Device* device, HWND hwnd);
	~LightShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* textureHeight, Light* light, Light* pointLight, Light* spotLight, Camera* camera, float displacement, XMFLOAT3 direction, int lightSetting, float constantFactor, float linearFactor, float quadraticFactor);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* spotLightBuffer;
	ID3D11Buffer* directionaLlightBuffer;
	ID3D11Buffer* cameraBuffer;
	ID3D11Buffer* displacementBuffer;
	ID3D11Buffer* attenuationBuffer;
};

