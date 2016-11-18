#include "ScreenQuad.h"

ScreenQuad::ScreenQuad()
{
}

ScreenQuad::~ScreenQuad()
{
}

int ScreenQuad::Initialize(ID3D11Device * device, int screenWidth, int screenHeight)
{
	DirectX::XMFLOAT3 vertices[6];
	unsigned long indices[6];
	int sizeVertices = 6;
	int sizeIndices = 6;
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;
	D3D11_SUBRESOURCE_DATA indexData;
	float left, right, top, bottom;
	HRESULT hresult;
	bool result;

	//Calculate the screen coordinates of the left side of the window
	left = (float)((screenWidth / 2) * -1);

	//Calculate the screen coordinates of the right side of the window
	right = left + (float)screenWidth;

	//Calculate the screen coordinates of the top of the window
	top = (float)(screenHeight / 2);

	//Calculate the screen coordinates of the bottom of the window
	bottom = top - (float)screenHeight;

	//Load the vertex array with data
	//First triangle
	vertices[0] = DirectX::XMFLOAT3(left, top, 0.0f);  //Top left

	vertices[1] = DirectX::XMFLOAT3(right, bottom, 0.0f);  //Bottom right

	vertices[2] = DirectX::XMFLOAT3(left, bottom, 0.0f);  //Bottom left

	//Second triangle
	vertices[3] = DirectX::XMFLOAT3(left, top, 0.0f);  //Top left

	vertices[4] = DirectX::XMFLOAT3(right, top, 0.0f);  //Top right

	vertices[5] = DirectX::XMFLOAT3(right, bottom, 0.0f);  //Bottom right

	//Load the index array with data
	for (int i = 0; i < sizeIndices; i++)
	{
		indices[i] = i;
	}

	//Set the description of the static vertex buffer
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(DirectX::XMFLOAT3) * sizeVertices;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//Give the subresource structure a pointer to the vertex data
	ZeroMemory(&vertexData, sizeof(vertexData));
	vertexData.pSysMem = &vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//Create the vertex buffer
	hresult = device->CreateBuffer(&vertexBufferDesc, &vertexData, &this->m_vertexBuffer);
	if (FAILED(hresult)) {
		return false;
	}

	//Set up the description of the static index buffer
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * sizeIndices;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	ZeroMemory(&indexData, sizeof(indexData));
	indexData.pSysMem = &indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	//Create the index buffer
	hresult = device->CreateBuffer(&indexBufferDesc, &indexData, &this->m_indexBuffer);
	if (FAILED(hresult)) {
		return false;
	}

	return 0;
}

void ScreenQuad::SetBuffers(ID3D11DeviceContext * deviceContext)
{
	unsigned int stride;
	unsigned offset;

	//Set vertex buffer stride and offset
	stride = sizeof(DirectX::XMFLOAT3);
	offset = 0;

	//Set the vertex buffer to active in the input assembly so it can rendered
	deviceContext->IASetVertexBuffers(0, 1, &this->m_vertexBuffer, &stride, &offset);

	//Set the index buffer to active in the input assembler so it can be rendered
	deviceContext->IASetIndexBuffer(this->m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//Set the type od primitiv that should be rendered from this vertex buffer, in this case triangles
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

void ScreenQuad::Shutdown()
{
	//Release the index buffer
	if (this->m_indexBuffer) {
		this->m_indexBuffer->Release();
		this->m_indexBuffer = nullptr;
	}

	//Release the vertex buffer
	if (this->m_vertexBuffer) {
		this->m_vertexBuffer->Release();
		this->m_vertexBuffer = nullptr;
	}
}
