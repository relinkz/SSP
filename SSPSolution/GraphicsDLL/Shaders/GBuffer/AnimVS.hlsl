
cbuffer worldMatrix : register(b0)
{
    float4x4 worldMatrix;
}

cbuffer camera : register(b1)
{
    float4x4 viewMatrix;
    float4x4 projectionMatrix;

    float4 camPos;
}
cbuffer skeleton : register(b5)
{
	float4x4 joints[32];
}


struct VS_IN
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT0;
    float2 UV : TEXCOORD0;

    float4 weights : WEIGHTS;
    int4 influences : INFLUENCE; //currently a 32bit uinteger, does it need to be 32bit?
};

struct VS_OUT
{
    float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT0;
    float2 UV : TEXCOORD0;

    float4 wPos : WORLDPOSITION;
};


VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT) 0;
	
	float3 skinnedPos = float3(0.f, 0.f, 0.f);
	float3 skinnedNormal = float3(0.f, 0.f, 0.f);
	float3 skinnedTan = float3(0.f, 0.f, 0.f);
    float weight;
    int influences;
	/*Vertex blending is performed here. With the following: weights, influences and the matrix of each joint.*/
	for (int i = 0; i < 4; i++)
	{
        weight = input.weights[i];
        influences = input.influences[i];
        if (influences > 0)
        {          
            skinnedPos += mul(mul(float4(input.Pos, 1.0f), joints[influences]), weight);
            skinnedNormal += mul(mul(float4(input.Normal, 1.0f), joints[influences]), weight);
            skinnedTan += mul(mul(float4(input.Tangent, 1.0f), joints[influences]), weight);
        }
    }

	matrix WV = mul(viewMatrix, projectionMatrix);
	matrix WVP = mul(worldMatrix, WV);

    output.wPos = mul(float4(skinnedPos, 1), worldMatrix);
    output.Pos = mul(float4(skinnedPos, 1), WVP);

    output.Normal = mul(float4(skinnedNormal, 1), worldMatrix).rgb;
    output.Tangent = mul(float4(skinnedTan, 1), worldMatrix).rgb;

	output.UV = input.UV;

   return output;
}