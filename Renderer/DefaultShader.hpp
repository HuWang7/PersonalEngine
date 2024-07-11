#pragma once

const char* shaderSource = R"(

cbuffer CameraConstants : register(b2)
{
	float4x4 ViewMatrix;
	float4x4 ProjectionMatrix;
};

cbuffer ModelConstants : register(b3)
{
    matrix ModelMatrix;
    float4 TintColor;
};

float Interpolate(float start, float end, float fractionTowardEnd) {
	return start + fractionTowardEnd * (end - start);
}

float GetFractionWithinRange(float value, float rangeStart, float rangeEnd) {
	if (rangeEnd - rangeStart == 0) return 0; 
	return (value - rangeStart) / (rangeEnd - rangeStart);
}

float RangeMap(float inValue, float inStart, float inEnd, float outStart, float outEnd) {
	float fraction = GetFractionWithinRange(inValue, inStart, inEnd);
	return Interpolate(outStart, outEnd, fraction);
}

struct vs_input_t
{
	float3 localPosition : POSITION;
	float4 color : COLOR;
	float2 uv : TEXCOORD;
};

struct v2p_t
{
	float4 position : SV_Position;
	float4 color : COLOR;
	float2 uv : TEXCOORD;
};

Texture2D diffuseTexture : register(t0);
SamplerState diffuseSampler : register(s0);

v2p_t VertexMain(vs_input_t input)
{
	float4 worldPosition = mul(ModelMatrix, float4(input.localPosition, 1.0f));
	
	float4 clipPosition = mul(ProjectionMatrix, mul(ViewMatrix, worldPosition));
	
	v2p_t v2p;
	v2p.position = clipPosition;
	v2p.color = input.color;
	v2p.uv = input.uv;
	return v2p;
};

float4 PixelMain(v2p_t input) : SV_Target0
{
	float4 textureColor = diffuseTexture.Sample(diffuseSampler, input.uv);
	float4 color = textureColor * input.color * TintColor;
	clip(color.a - 0.01f);
	return color;
};
)";
