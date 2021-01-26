// tessellation shader.cpp
#include "tessellationshader.h"


TessellationShader::TessellationShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"tessellation_vs.cso", L"tessellation_hs.cso", L"tessellation_ds.cso", L"tessellation_ps.cso");
}


TessellationShader::~TessellationShader()
{
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}
	if (layout)
	{
		layout->Release();
		layout = 0;
	}
	if (lightBuffer)
	{
		lightBuffer->Release();
		lightBuffer = 0;
	}

	if (pointlightBuffer)
	{
		pointlightBuffer->Release();
		pointlightBuffer = 0;
	}
	if (spotlightBuffer)
	{
		spotlightBuffer->Release();
		spotlightBuffer = 0;
	}
	if (tessellationBuffer)
	{
		tessellationBuffer->Release();
		tessellationBuffer = 0;
	}
	if (attenuationBuffer)
	{
		attenuationBuffer->Release();
		attenuationBuffer = 0;
	}
	
	//Release base shader components
	BaseShader::~BaseShader();
}

void TessellationShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_BUFFER_DESC tessellationBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC pointlightBufferDesc;
	D3D11_BUFFER_DESC spotlightBufferDesc;
	D3D11_BUFFER_DESC attenuationBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);


	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	renderer->CreateSamplerState(&samplerDesc, &sampleState);

	// Sampler for shadow map sampling.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 1.0f;
	samplerDesc.BorderColor[2] = 1.0f;
	samplerDesc.BorderColor[3] = 1.0f;
	renderer->CreateSamplerState(&samplerDesc, &sampleStateShadow);


	tessellationBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	tessellationBufferDesc.ByteWidth = sizeof(TessellationBufferType);
	tessellationBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tessellationBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tessellationBufferDesc.MiscFlags = 0;
	tessellationBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&tessellationBufferDesc, NULL, &tessellationBuffer);

	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);

	pointlightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	pointlightBufferDesc.ByteWidth = sizeof(PointLightBufferType);
	pointlightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	pointlightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pointlightBufferDesc.MiscFlags = 0;
	pointlightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&pointlightBufferDesc, NULL, &pointlightBuffer);

	spotlightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	spotlightBufferDesc.ByteWidth = sizeof(SpotLightBufferType);
	spotlightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	spotlightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	spotlightBufferDesc.MiscFlags = 0;
	spotlightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&spotlightBufferDesc, NULL, &spotlightBuffer);

	attenuationBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	attenuationBufferDesc.ByteWidth = sizeof(AttenuationBufferType);
	attenuationBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	attenuationBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	attenuationBufferDesc.MiscFlags = 0;
	attenuationBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&attenuationBufferDesc, NULL, &attenuationBuffer);
	
}

void TessellationShader::initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename)
{
	// InitShader must be overwritten and it will load both vertex and pixel shaders + setup buffers
	initShader(vsFilename, psFilename);

	// Load other required shaders.
	loadHullShader(hsFilename);
	loadDomainShader(dsFilename);
}


void TessellationShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, XMFLOAT4 cameraPos, float time, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* textureHeight, Light* light, Light* pointLight, Light* spotLight, float x, float y, float z, ID3D11ShaderResourceView* depthMap, ID3D11ShaderResourceView* spotdepthMap, float shadowBias, int lightSetting, float constantFactor, float linearFactor, float quadraticFactor)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Transpose the matrices to prepare them for the shader.
	XMMATRIX tworld = XMMatrixTranspose(worldMatrix);
	XMMATRIX tview = XMMatrixTranspose(viewMatrix);
	XMMATRIX tproj = XMMatrixTranspose(projectionMatrix);
	XMMATRIX tLightViewMatrix;
	XMMATRIX tLightProjectionMatrix;
	tLightViewMatrix = XMMatrixTranspose(light->getViewMatrix());
	tLightProjectionMatrix = XMMatrixTranspose(light->getOrthoMatrix());
	XMMATRIX tLightViewMatrix2;
	XMMATRIX tLightProjectionMatrix2;
	tLightViewMatrix2 = XMMatrixTranspose(spotLight->getViewMatrix());
	tLightProjectionMatrix2 = XMMatrixTranspose(spotLight->getProjectionMatrix());


	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	dataPtr->lightView = tLightViewMatrix;
	dataPtr->lightProjection = tLightProjectionMatrix;
	dataPtr->lightView2 = tLightViewMatrix2;
	dataPtr->lightProjection2 = tLightProjectionMatrix2;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->DSSetConstantBuffers(0, 1, &matrixBuffer);

	TessellationBufferType* tessPtr;
	deviceContext->Map(tessellationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	tessPtr = (TessellationBufferType*)mappedResource.pData;
	tessPtr->cameraPosition = cameraPos;
	deviceContext->Unmap(tessellationBuffer, 0);
	deviceContext->HSSetConstantBuffers(0, 1, &tessellationBuffer);
	deviceContext->DSSetConstantBuffers(1, 1, &tessellationBuffer);

	LightBufferType* lightptr;
	deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	lightptr = (LightBufferType*)mappedResource.pData;
	lightptr->ambient = light->getAmbientColour();
	lightptr->diffuse = light->getDiffuseColour();
	lightptr->direction = XMFLOAT4(x, y, z, 1);
	lightptr->shadow_bias = shadowBias;
	lightptr->light_Setting = lightSetting;
	lightptr->pading = XMFLOAT2(0, 0);
	deviceContext->Unmap(lightBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &lightBuffer);

	PointLightBufferType* pointLightptr;
	deviceContext->Map(pointlightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	pointLightptr = (PointLightBufferType*)mappedResource.pData;
	pointLightptr->ambientColor = pointLight->getAmbientColour();
	pointLightptr->diffuseColor = pointLight->getDiffuseColour();
	pointLightptr->position = XMFLOAT4(pointLight->getPosition().x, pointLight->getPosition().y, pointLight->getPosition().z, 1);
	pointLightptr->specularPower = pointLight->getSpecularPower();
	pointLightptr->pad = XMFLOAT3(0, 0, 0);
	pointLightptr->specularColor = pointLight->getSpecularColour();
	deviceContext->Unmap(pointlightBuffer, 0);
	deviceContext->PSSetConstantBuffers(1, 1, &pointlightBuffer);

	SpotLightBufferType* spotlightptr;
	deviceContext->Map(spotlightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	spotlightptr = (SpotLightBufferType*)mappedResource.pData;
	spotlightptr->ambientColour = spotLight->getAmbientColour();
	spotlightptr->diffuseColour = spotLight->getDiffuseColour();
	spotlightptr->spotPosition = XMFLOAT4(spotLight->getPosition().x, spotLight->getPosition().y, spotLight->getPosition().z, 1) ;
	spotlightptr->angleRad = 3.14159f / 8.f;
	spotlightptr->spotDirection = spotLight->getDirection();
	deviceContext->Unmap(spotlightBuffer, 0);
	deviceContext->PSSetConstantBuffers(2, 1, &spotlightBuffer);

	AttenuationBufferType* attenuationPtr;
	deviceContext->Map(attenuationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	attenuationPtr = (AttenuationBufferType*)mappedResource.pData;
	attenuationPtr->constant_Factor = constantFactor;
	attenuationPtr->linear_Factor = linearFactor;
	attenuationPtr->quadratic_Factor = quadraticFactor;
	attenuationPtr->pad = 0.f;
	deviceContext->Unmap(attenuationBuffer, 0);
	deviceContext->PSSetConstantBuffers(3, 1, &attenuationBuffer);

	deviceContext->DSSetShaderResources(0, 1, &textureHeight);
	deviceContext->DSSetSamplers(0, 1, &sampleState);

	// Set shader texture and sampler resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetShaderResources(1, 1, &depthMap);
	deviceContext->PSSetShaderResources(2, 1, &spotdepthMap);
	deviceContext->PSSetSamplers(0, 1, &sampleState);
	deviceContext->PSSetSamplers(1, 1, &sampleStateShadow);

}

