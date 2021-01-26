// Light shader.h
// Basic single light shader setup
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;


class TessellationShader : public BaseShader
{

public:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX lightView;
		XMMATRIX lightProjection;
		XMMATRIX lightView2;
		XMMATRIX lightProjection2;
	};

	struct TessellationBufferType {

		XMFLOAT4 cameraPosition;
	};

	struct LightBufferType
	{
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT4 direction;
		float shadow_bias;
		int light_Setting;
		XMFLOAT2 pading;
	};

	struct PointLightBufferType
	{
		XMFLOAT4 ambientColor;
		XMFLOAT4 diffuseColor;
		XMFLOAT4 position;
		float specularPower;
		XMFLOAT3 pad;
		XMFLOAT4 specularColor;
	};

	struct SpotLightBufferType
	{
		XMFLOAT4 ambientColour;
		XMFLOAT4 diffuseColour;
		XMFLOAT4 spotPosition;
		float angleRad;
		XMFLOAT3 spotDirection;
	};

	struct AttenuationBufferType {
		float constant_Factor;
		float linear_Factor;
		float quadratic_Factor;
		float pad;
	};

	TessellationShader(ID3D11Device* device, HWND hwnd);
	~TessellationShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, XMFLOAT4 cameraPos, float time, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* textureHeight, Light* light, Light* pointLight, Light* spotLight, float x, float y, float z, ID3D11ShaderResourceView* depthMap, ID3D11ShaderResourceView* spotdepthMap, float shadowBias, int lightSetting, float constantFactor, float linearFactor, float quadraticFactor);

private:
	void initShader(const wchar_t* vsFilename, const wchar_t* psFilename);
	void initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* pointlightBuffer;
	ID3D11Buffer* spotlightBuffer;
	ID3D11Buffer* tessellationBuffer;
	ID3D11Buffer* attenuationBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateShadow;
};
