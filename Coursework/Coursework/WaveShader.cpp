#include "WaveShader.h"

WaveShader::WaveShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	device_ = device;
	initShader(L"wave_vs.cso", L"wave_ps.cso");
}


WaveShader::~WaveShader()
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
	if (timeBuffer)
	{
		timeBuffer->Release();
		timeBuffer = 0;
	}
	if (cameraBuffer)
	{
		cameraBuffer->Release();
		cameraBuffer = 0;
	}
	if (attenuationBuffer)
	{
		attenuationBuffer->Release();
		attenuationBuffer = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}

void WaveShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC timeBufferDesc;
	D3D11_BUFFER_DESC attenuationBufferDesc;
	D3D11_BUFFER_DESC CameraBufferDesc;
	ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));

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

	timeBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	timeBufferDesc.ByteWidth = sizeof(TimeBufferType);
	timeBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	timeBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	timeBufferDesc.MiscFlags = 0;
	timeBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&timeBufferDesc, NULL, &timeBuffer);

	attenuationBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	attenuationBufferDesc.ByteWidth = sizeof(AttenuationBufferType);
	attenuationBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	attenuationBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	attenuationBufferDesc.MiscFlags = 0;
	attenuationBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&lightBufferDesc, NULL, &attenuationBuffer);

	CameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	CameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	CameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	CameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	CameraBufferDesc.MiscFlags = 0;
	CameraBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&CameraBufferDesc, NULL, &cameraBuffer);

	// alpha blending
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	device_->CreateBlendState(&blendDesc, &blendState);
}


void WaveShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, Light* light, float time_, float amplitude_, float frequency_, float speed_, float Dx, float Dz, Camera* camera)
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
	// Send time buffer vaues
	TimeBufferType* timePtr;
	deviceContext->Map(timeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	timePtr = (TimeBufferType*)mappedResource.pData;
	timePtr->time = time_;
	timePtr->amplitude = amplitude_;
	timePtr->frequency = frequency_;
	timePtr->speed = speed_;
	timePtr->DirectionalVector = XMFLOAT4(Dx, Dz, 0, 0);
	deviceContext->Unmap(timeBuffer, 0);
	deviceContext->VSSetConstantBuffers(1, 1, &timeBuffer);

	//Additional
	// Send light data to pixel shader
	LightBufferType* lightPtr;
	deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	lightPtr = (LightBufferType*)mappedResource.pData;
	lightPtr->ambient = light->getAmbientColour();
	lightPtr->diffuse = light->getDiffuseColour();
	lightPtr->position = XMFLOAT4(light->getPosition().x, light->getPosition().y, light->getPosition().z, 1);
	lightPtr->specularColour = light->getSpecularColour();
	lightPtr->specularPower = XMFLOAT4(light->getSpecularPower(), 0, 0, 0);
	deviceContext->Unmap(lightBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &lightBuffer);

	//camera buffer
	CameraBufferType* camPtr;
	deviceContext->Map(cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	camPtr = (CameraBufferType*)mappedResource.pData;
	camPtr->cameraPosition = camera->getPosition();
	deviceContext->Unmap(cameraBuffer, 0);
	deviceContext->VSSetConstantBuffers(2, 1, &cameraBuffer);

	//attenuation bufer
	AttenuationBufferType* attenuationPtr;
	deviceContext->Map(attenuationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	attenuationPtr = (AttenuationBufferType*)mappedResource.pData;
	attenuationPtr->constantFactor = 0.5;
	attenuationPtr->linearFactor =  0.125;
	attenuationPtr->quadraticFactor =  0.0;
	deviceContext->Unmap(attenuationBuffer, 0);
	deviceContext->PSSetConstantBuffers(1, 1, &attenuationBuffer);


	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetSamplers(0, 1, &sampleState);

	deviceContext->OMSetBlendState(blendState, 0, 0xffffffff);
}
