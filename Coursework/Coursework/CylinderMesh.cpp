#include "CylinderMesh.h"

// Store shape resolution (default is 20), initialise buffers and load texture.
CylinderMesh::CylinderMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int lresolution, int stacks, int slices)
{
	resolution_ = lresolution;
	stacks_ = stacks;
	slices_ = slices;
	initBuffers(device);
}

// Release resources.
CylinderMesh::~CylinderMesh()
{
	// Run parent deconstructor
	BaseMesh::~BaseMesh();
}

// Generate sphere. Generates a cube based on resolution provided. Then normalises vertex positions to create sphere.
// Shape has texture coordinates and normals.
void CylinderMesh::initBuffers(ID3D11Device* device)
{

	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	vertexCount = (stacks_ * slices_) * 6;
	indexCount = vertexCount;

	vertices = new VertexType[vertexCount];
	indices = new unsigned long[indexCount];

	float interval = (2 * 3.14159) / (slices_ * 2);
	float angle = 0;
	float x = 0;
	float y = 0;
	float z = 0;

	//Counters
	int v = 0;	// vertex counter
	int ind = 0;	// index counter

	for (float i = 0; i < stacks_; i++) {

		for (float j = 0; j < slices_; j++) {

			x = cosf(angle); //number 1
			y = 1 - i;
			z = sinf(angle);
			// Load the vertex array with data.
			vertices[v].position = XMFLOAT3(x, y, z);
			vertices[v].texture = XMFLOAT2((1.0f / slices_) * j, (1.0f / stacks_) * i);
			vertices[v].normal = XMFLOAT3(x, y, z);
			angle += interval;
			indices[ind] = ind;
			v++;
			ind++;


			x = cosf(angle - interval); //number 2
			y = 0 - i;
			z = sinf(angle - interval);
			// Load the vertex array with data.
			vertices[v].position = XMFLOAT3(x, y, z);
			vertices[v].texture = XMFLOAT2((1.0f / slices_) * j, (1.0f / stacks_) * (i + 1));
			vertices[v].normal = XMFLOAT3(x, y, z);
			angle += interval;
			indices[ind] = ind;
			v++;
			ind++;

			x = cosf(angle); //number 4
			y = 1 - i;
			z = sinf(angle);
			vertices[v].position = XMFLOAT3(x, y, z);
			vertices[v].texture = XMFLOAT2((1.0f / slices_) * (j + 1), (1.0f / stacks_) * i);
			vertices[v].normal = XMFLOAT3(x, y, z);
			indices[ind] = ind;
			v++;
			ind++;

			x = cosf(angle - (interval * 2)); //number 2
			y = 0 - i;
			z = sinf(angle - (interval * 2));
			// Load the vertex array with data.
			vertices[v].position = XMFLOAT3(x, y, z);
			vertices[v].texture = XMFLOAT2((1.0f / slices_) * j, (1.0f / stacks_) * (i + 1));
			vertices[v].normal = XMFLOAT3(x, y, z);
			indices[ind] = ind;
			v++;
			ind++;

			x = cosf(angle); //number 3
			y = 0 - i;
			z = sinf(angle);
			vertices[v].position = XMFLOAT3(x, y, z);
			vertices[v].texture = XMFLOAT2((1.0f / slices_) * (j + 1), (1.0f / stacks_) * (i + 1));
			vertices[v].normal = XMFLOAT3(x, y, z);

			indices[ind] = ind;
			v++;
			ind++;


			x = cosf(angle); //number 4
			y = 1 - i;
			z = sinf(angle);
			vertices[v].position = XMFLOAT3(x, y, z);
			vertices[v].texture = XMFLOAT2((1.0f / slices_) * (j + 1), (1.0f / stacks_) * i);
			vertices[v].normal = XMFLOAT3(x, y, z);
			indices[ind] = ind;
			v++;
			ind++;

		}
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	// Now create the vertex buffer.
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	// Create the index buffer.
	device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;
	delete[] indices;
	indices = 0;

}