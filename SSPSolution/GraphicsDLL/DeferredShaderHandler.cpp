#include "DeferredShaderHandler.h"

DeferredShaderHandler::DeferredShaderHandler() : ShaderHandler()
{
	this->m_samplerState = nullptr;

	for (int i = 0; i < BUFFER_COUNT; i++) {
		this->m_deferredRenderTargetTextures[i] = nullptr;
		this->m_deferredRenderTargetViews[i] = nullptr;
		this->m_deferredShaderResources[i] = nullptr;
	}
}


DeferredShaderHandler::~DeferredShaderHandler()
{
}

int DeferredShaderHandler::Initialize(ID3D11Device * device, HWND * windowHandle, DirectX::XMFLOAT2 resolution)
{
	HRESULT hResult;
	ID3D10Blob* vertexShaderBuffer[4] = { nullptr };
	ID3D10Blob* geoShaderBuffer = nullptr;
	ID3D10Blob* pixelShaderBuffer = nullptr;

	//Insert shader path here
	WCHAR* vsFilename = L"../GraphicsDLL/DeferredVertexShader.hlsl";
	WCHAR* gsFilename = L"../GraphicsDLL/DeferredGeometryShader.hlsl";
	WCHAR* psFilename = L"../GraphicsDLL/DeferredPixelShader.hlsl";

	// Compile the shaders \\

	hResult = D3DCompileFromFile(vsFilename, NULL, NULL, "main", "vs_5_0", D3D10_SHADER_DEBUG, 0, &vertexShaderBuffer[0], NULL);
	if (FAILED(hResult)) 
	{
		return 1;
	}
	hResult = D3DCompileFromFile(gsFilename, NULL, NULL, "main", "vs_5_0", D3D10_SHADER_DEBUG, 0, &geoShaderBuffer, NULL);
	if (FAILED(hResult)) 
	{
		return 1;
	}
	hResult = D3DCompileFromFile(psFilename, NULL, NULL, "main", "vs_5_0", D3D10_SHADER_DEBUG, 0, &pixelShaderBuffer, NULL);
	if (FAILED(hResult)) 
	{
		return 1;
	}

	// Create the shaders \\

	hResult = device->CreateVertexShader(vertexShaderBuffer[0]->GetBufferPointer(), vertexShaderBuffer[0]->GetBufferSize(), NULL, &this->m_vertexShader[0]);
	if (FAILED(hResult)) 
	{
		return 1;
	}
	hResult = device->CreateGeometryShader(geoShaderBuffer->GetBufferPointer(), geoShaderBuffer->GetBufferSize(), NULL, &this->m_geoShader);
	if (FAILED(hResult)) {
		return 1;
	}
	hResult = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &this->m_pixelShader);
	if (FAILED(hResult)) {
		return 1;
	}

	// Create the input layout \\

	D3D11_INPUT_ELEMENT_DESC polygonLayout[4];
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "NORMAL";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "TANGENT";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	polygonLayout[3].SemanticName = "TEXCOORD";
	polygonLayout[3].SemanticIndex = 0;
	polygonLayout[3].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[3].InputSlot = 0;
	polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[3].InstanceDataStepRate = 0;

	unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);
	//Create the vertex input layout.
	hResult = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer[0]->GetBufferPointer(), vertexShaderBuffer[0]->GetBufferSize(), &this->m_layout);
	if (FAILED(hResult)) {
		return 1;
	}

	//Release and nullptr the buffers as they are no longer needed
	vertexShaderBuffer[0]->Release();
	vertexShaderBuffer[0] = nullptr;
	geoShaderBuffer->Release();
	geoShaderBuffer = nullptr;
	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;

	// Create the matrix buffer \\

	D3D11_BUFFER_DESC matrixBufferDesc;
	ZeroMemory(&matrixBufferDesc, sizeof(matrixBufferDesc));
	//Fill the description of the dynamic matrix constant buffer that is in the vertex shader
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(ShaderLib::DeferredConstantBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	hResult = device->CreateBuffer(&matrixBufferDesc, NULL, &this->m_matrixBuffer);
	if (FAILED(hResult)) {
		return 1;
	}

	// Create the sampler \\

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));

	//Fill the texture sampler state description
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//Create the texture sampler state
	hResult = device->CreateSamplerState(&samplerDesc, &this->m_samplerState);
	if (FAILED(hResult))
	{
		return 1;
	}

	// Create the render target and shader resource views \\

	D3D11_TEXTURE2D_DESC renderTextureDesc;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	//Initialize the render target texture description
	ZeroMemory(&renderTextureDesc, sizeof(renderTextureDesc));

	//Setup the render target texture description
	renderTextureDesc.Width = resolution.x;
	renderTextureDesc.Height = resolution.y;
	renderTextureDesc.MipLevels = 1;
	renderTextureDesc.ArraySize = 1;
	renderTextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	renderTextureDesc.SampleDesc.Count = 1; //No MSAA
	renderTextureDesc.SampleDesc.Quality = 0;

	renderTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	renderTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	renderTextureDesc.CPUAccessFlags = 0;
	renderTextureDesc.MiscFlags = 0;

	//Create the render target textures
	for (int i = 0; i < BUFFER_COUNT; i++) {
		hResult = device->CreateTexture2D(&renderTextureDesc, NULL, &this->m_deferredRenderTargetTextures[i]);
		if (FAILED(hResult)) {
			return 1;
		}
	}

	//Setup the description of the render target views
	renderTargetViewDesc.Format = renderTextureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	//Create the render target views
	for (int i = 0; i<BUFFER_COUNT; i++) {
		hResult = device->CreateRenderTargetView(this->m_deferredRenderTargetTextures[i], &renderTargetViewDesc, &this->m_deferredRenderTargetViews[i]);
		if (FAILED(hResult)) {
			return 1;
		}
	}

	//Setup the description of the shader resource view
	shaderResourceViewDesc.Format = renderTextureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	//Create the shader resource views
	for (int i = 0; i < BUFFER_COUNT; i++) {
		hResult = device->CreateShaderResourceView(this->m_deferredRenderTargetTextures[i], &shaderResourceViewDesc, &this->m_deferredShaderResources[i]);
		if (FAILED(hResult)) {
			return 1;
		}
	}

	return 0;
}

int DeferredShaderHandler::SetActive(ID3D11DeviceContext * deviceContext, ShaderLib::ShaderType shaderType)
{
	return 0;
}

void DeferredShaderHandler::Shutdown()
{
}

ID3D11ShaderResourceView ** DeferredShaderHandler::GetShaderResourceViews()
{
	return nullptr;
}
