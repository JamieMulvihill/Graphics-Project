#include "GodRayShader.h"

GodRayShader::GodRayShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"god_ray_vs.cso", L"god_ray_ps.cso");
}

GodRayShader::~GodRayShader()
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
	if (godRayBuffer)
	{
		godRayBuffer->Release();
		godRayBuffer = 0;
	}
	if (variableBuffer)
	{
		variableBuffer->Release();
		variableBuffer = 0;
	}



	// Release the layout.
	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}

void GodRayShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_SAMPLER_DESC samplerRayDesc;
	D3D11_BUFFER_DESC godraysBufferDesc;
	D3D11_BUFFER_DESC variableBufferDesc;

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

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	renderer->CreateSamplerState(&samplerDesc, &sampleState);

	// Create a texture sampler state description.
	samplerRayDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerRayDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerRayDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerRayDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerRayDesc.MipLODBias = 0.0f;
	samplerRayDesc.MaxAnisotropy = 1;
	samplerRayDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerRayDesc.MinLOD = 0;
	samplerRayDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	renderer->CreateSamplerState(&samplerRayDesc, &sampleStateRay);

	godraysBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	godraysBufferDesc.ByteWidth = sizeof(GodRayBufferType);
	godraysBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	godraysBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	godraysBufferDesc.MiscFlags = 0;
	godraysBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&godraysBufferDesc, NULL, &godRayBuffer);

	variableBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	variableBufferDesc.ByteWidth = sizeof(VariableBufferType);
	variableBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	variableBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	variableBufferDesc.MiscFlags = 0;
	variableBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&variableBufferDesc, NULL, &variableBuffer);
}

void GodRayShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* texture1, Light* light, XMFLOAT2 screenSize, const XMMATRIX& view2, const XMMATRIX& proj2, float decay, float density, float exposure, float weight)
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

	// Set shader texture and sampler resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetShaderResources(1, 1, &texture1);

	deviceContext->PSSetSamplers(0, 1, &sampleState);
	deviceContext->PSSetSamplers(1, 1, &sampleStateRay);

	GodRayBufferType* godPtr;
	deviceContext->Map(godRayBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	godPtr = (GodRayBufferType*)mappedResource.pData;
	godPtr->lightPosition = XMFLOAT4(light->getPosition().x, light->getPosition().y, light->getPosition().z, 1);
	godPtr->screenSize = screenSize;
	godPtr->padding = XMFLOAT2(0, 0);
	godPtr->viewMatrix = XMMatrixTranspose(view2);
	godPtr->projectionMatrix = XMMatrixTranspose(proj2);
	deviceContext->Unmap(godRayBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &godRayBuffer);

	VariableBufferType* variablePtr;
	deviceContext->Map(variableBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	variablePtr = (VariableBufferType*)mappedResource.pData;
	variablePtr->decay = decay;
	variablePtr->density = density;
	variablePtr->exposure = exposure;
	variablePtr->weight = weight;
	deviceContext->Unmap(variableBuffer, 0);
	deviceContext->PSSetConstantBuffers(1, 1, &variableBuffer);

}