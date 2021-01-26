#include "BasicDepthShader.h"

BasicDepthShader::BasicDepthShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"basic_depth_vs.cso", L"basic_depth_ps.cso");
}

BasicDepthShader::~BasicDepthShader()
{
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
	if (displacementBuffer)
	{
		displacementBuffer->Release();
		displacementBuffer = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}

void BasicDepthShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC displacementBufferDesc;

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

	displacementBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	displacementBufferDesc.ByteWidth = sizeof(DisplacementBufferType);
	displacementBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	displacementBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	displacementBufferDesc.MiscFlags = 0;
	displacementBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&displacementBufferDesc, NULL, &displacementBuffer);

}

void BasicDepthShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, float displacement)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;

	// Transpose the matrices to prepare them for the shader.
	XMMATRIX tworld = XMMatrixTranspose(worldMatrix);
	XMMATRIX tview = XMMatrixTranspose(viewMatrix);
	XMMATRIX tproj = XMMatrixTranspose(projectionMatrix);

	// Lock the constant buffer so it can be written to.
	deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);

	DisplacementBufferType* displacePtr;
	deviceContext->Map(displacementBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	displacePtr = (DisplacementBufferType*)mappedResource.pData;
	displacePtr->displacement = displacement;
	displacePtr->padding = XMFLOAT3(0, 0, 0);
	deviceContext->Unmap(displacementBuffer, 0);
	deviceContext->VSSetConstantBuffers(1, 1, &displacementBuffer);
}
