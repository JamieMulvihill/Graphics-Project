#include "LightShader.h"

LightShader::LightShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"light_vs.cso", L"light_ps.cso");
}


LightShader::~LightShader()
{
	// Release the sampler state.
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}

	// Release the matrix constant buffer.
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}

	// Release the layout.
	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	// Release the light constant buffer.
	if (lightBuffer)
	{
		lightBuffer->Release();
		lightBuffer = 0;
	}

	if (spotLightBuffer)
	{
		spotLightBuffer->Release();
		spotLightBuffer = 0;
	}

	if (directionaLlightBuffer)
	{
		directionaLlightBuffer->Release();
		directionaLlightBuffer = 0;
	}
	
	if (cameraBuffer)
	{
		cameraBuffer->Release();
		cameraBuffer = 0;
	}
	if (displacementBuffer)
	{
		displacementBuffer->Release();
		displacementBuffer = 0;
	}
	if (attenuationBuffer)
	{
		attenuationBuffer->Release();
		attenuationBuffer = 0;
	}
	


	//Release base shader components
	BaseShader::~BaseShader();
}

void LightShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC spotLightBufferDesc;
	D3D11_BUFFER_DESC directionalLightBufferDesc;
	D3D11_BUFFER_DESC cameraBufferDesc;
	D3D11_BUFFER_DESC displacementBufferDesc;
	D3D11_BUFFER_DESC attenuationBufferDesc;

	

	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
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
	renderer->CreateSamplerState(&samplerDesc, &sampleState);

	// Setup light buffer
	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);

	directionalLightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	directionalLightBufferDesc.ByteWidth = sizeof(DirectionalLightBufferType);
	directionalLightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	directionalLightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	directionalLightBufferDesc.MiscFlags = 0;
	directionalLightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&directionalLightBufferDesc, NULL, &directionaLlightBuffer);

	spotLightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	spotLightBufferDesc.ByteWidth = sizeof(SpotLightBufferType);
	spotLightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	spotLightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	spotLightBufferDesc.MiscFlags = 0;
	spotLightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&spotLightBufferDesc, NULL, &spotLightBuffer);

	//camera buffer
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(LightBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&cameraBufferDesc, NULL, &cameraBuffer);

	displacementBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	displacementBufferDesc.ByteWidth = sizeof(DisplacementBufferType);
	displacementBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	displacementBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	displacementBufferDesc.MiscFlags = 0;
	displacementBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&displacementBufferDesc, NULL, &displacementBuffer);

	
	attenuationBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	attenuationBufferDesc.ByteWidth = sizeof(AttenuationBufferType);
	attenuationBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	attenuationBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	attenuationBufferDesc.MiscFlags = 0;
	attenuationBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&attenuationBufferDesc, NULL, &attenuationBuffer);
}


void LightShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* textureHeight, Light* light, Light* pointLight, Light* spotLight, Camera* camera, float displacement,  XMFLOAT3 direction, int lightSetting, float constantFactor, float linearFactor, float quadraticFactor)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;

	XMMATRIX tworld, tview, tproj;


	// Transpose the matrices to prepare them for the shader.
	tworld = XMMatrixTranspose(worldMatrix);
	tview = XMMatrixTranspose(viewMatrix);
	tproj = XMMatrixTranspose(projectionMatrix);
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);

	//Additional
	// Send light data to pixel shader
	
	DirectionalLightBufferType* directLightPtr;
	deviceContext->Map(directionaLlightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	directLightPtr = (DirectionalLightBufferType*)mappedResource.pData;
	directLightPtr->ambient = light->getAmbientColour();
	directLightPtr->diffuse = light->getDiffuseColour();
	directLightPtr->direction = XMFLOAT4(light->getDirection().x, light->getDirection().y, light->getDirection().z, 1);
	deviceContext->Unmap(directionaLlightBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &directionaLlightBuffer);
	
	LightBufferType* lightPtr;
	deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	lightPtr = (LightBufferType*)mappedResource.pData;
	lightPtr->ambient = pointLight->getAmbientColour();
	lightPtr->diffuse = pointLight->getDiffuseColour();
	lightPtr->position = XMFLOAT4(pointLight->getPosition().x, pointLight->getPosition().y, pointLight->getPosition().z, 1);
	lightPtr->specularPower = XMFLOAT4(pointLight->getSpecularPower(), 0, 0, 1);
	lightPtr->specularColour = pointLight->getSpecularColour();
	deviceContext->Unmap(lightBuffer, 0);
	deviceContext->PSSetConstantBuffers(1, 1, &lightBuffer);

	SpotLightBufferType* spotPtr;
	deviceContext->Map(spotLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	spotPtr = (SpotLightBufferType*)mappedResource.pData;
	spotPtr->ambientColour = spotLight->getAmbientColour();
	spotPtr->diffuseColour = spotLight->getDiffuseColour();
	spotPtr->spotPosition = spotLight->getPosition();
	spotPtr->angleRad = 3.14159 / 8;
	spotPtr->direction = spotLight->getDirection();
	spotPtr->light_Setting = lightSetting;
	deviceContext->Unmap(spotLightBuffer, 0);
	deviceContext->PSSetConstantBuffers(2, 1, &spotLightBuffer);

	AttenuationBufferType* attenuationPtr;
	deviceContext->Map(attenuationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	attenuationPtr = (AttenuationBufferType*)mappedResource.pData;
	attenuationPtr->constant_Factor = constantFactor;
	attenuationPtr->linear_Factor = linearFactor;
	attenuationPtr->quadratic_Factor = quadraticFactor;
	attenuationPtr->pad = 0.f;
	deviceContext->Unmap(attenuationBuffer, 0);
	deviceContext->PSSetConstantBuffers(3, 1, &attenuationBuffer);

	CameraBufferType* camPtr;
	deviceContext->Map(cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	camPtr = (CameraBufferType*)mappedResource.pData;
	camPtr->camerPosition = camera->getPosition();
	camPtr->padding = 0.0f;
	deviceContext->Unmap(cameraBuffer, 0);
	deviceContext->VSSetConstantBuffers(1, 1, &cameraBuffer);

	DisplacementBufferType* displacePtr;
	deviceContext->Map(displacementBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	displacePtr = (DisplacementBufferType*)mappedResource.pData;
	displacePtr->displacement = displacement;
	displacePtr->padding = XMFLOAT3(0, 0, 0);
	deviceContext->Unmap(displacementBuffer, 0);
	deviceContext->VSSetConstantBuffers(2, 1, &displacementBuffer);

	

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetSamplers(0, 1, &sampleState);

	deviceContext->VSSetShaderResources(0, 1, &textureHeight);
	deviceContext->VSSetSamplers(0, 1, &sampleState);
}