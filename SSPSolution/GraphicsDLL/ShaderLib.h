#ifndef GRAPHICSDLL_SHADERLIB
#define GRAPHICSDLL_SHADERLIB

namespace ShaderLib
{
	enum ShaderType
	{
		Normal = 0,
		Instanced = 1,
		Animated = 2,
		InstancedAnimated = 3
	};

	struct DeferredConstantBuffer
	{
		DirectX::XMMATRIX worldMatrix;
		DirectX::XMMATRIX viewMatrix;
		DirectX::XMMATRIX projectionMatrix;

		DirectX::XMFLOAT4 diffColor;

		DirectX::XMFLOAT4 camPos;
	};
	struct LightConstantBuffer
	{
		DirectX::XMMATRIX viewMatrix;
		DirectX::XMMATRIX projectionMatrix;

		ID3D11ShaderResourceView** gBuffers;

		DirectX::XMFLOAT2 resolution;

		DirectX::XMFLOAT4 camPos;
	};
}

#endif