#pragma once
#ifndef GRAPHICSDLL_LIGHTHSTRUCTS_H
#define GRAPHICSDLL_LIGHTHSTRUCTS_H
#include <DirectXMath.h>

#ifdef GRAPHICSDLL_EXPORTS
#define GRAPHICSDLL_API __declspec(dllexport)
#else
#define GRAPHICSDLL_API __declspec(dllimport)
#endif
namespace LIGHT
{
	enum GRAPHICSDLL_API LIGHT_TYPE
	{
		LT_POINT,
		LT_DIRECTIONAL,
		LT_AREA,
		LT_SPOT,

		NUM_LT
	};

	struct LightColor 
	{
		float r, g, b;
	};
	struct LightFalloff
	{
		float constant, linear, quadratic;
	};

	struct GRAPHICSDLL_API Light
	{
		LIGHT_TYPE LT;
		LightColor color;
		float intensity;
	};

	struct GRAPHICSDLL_API Point : Light
	{
		DirectX::XMVECTOR position;
		float radius;
		LightFalloff falloff;
	};

	struct GRAPHICSDLL_API Area : Light
	{
		DirectX::XMVECTOR position;
		DirectX::XMVECTOR direction;
		DirectX::XMVECTOR up;
		DirectX::XMVECTOR right;

		LightFalloff falloff;

		float width;
		float height;
	};

	struct GRAPHICSDLL_API Directional : Light
	{
		DirectX::XMVECTOR direction;
	};

	struct GRAPHICSDLL_API Spot : Light
	{
		DirectX::XMVECTOR position;
		DirectX::XMVECTOR direction;
		LightFalloff falloff;
		//cone stuff here
		float range, cone, pad;
	};

}

#endif