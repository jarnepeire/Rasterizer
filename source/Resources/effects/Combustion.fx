//-----------------------------------
// Global Variables
//-----------------------------------
float4x4 gWorldViewProj : WorldViewProjection;
float4x4 gWorld : WORLD;
float4x4 gViewInverse : VIEWINVERSE; //Last column contains camera pos!

Texture2D gDiffuseMap : DiffuseMap;

//-----------------------------------
// Sampler States
//-----------------------------------
SamplerState gSamplerState
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Wrap;
	AddressV = Wrap;
	BorderColor = float4(0.0f, 0.0f, 1.0f, 1.0f);
};

//-----------------------------------
// Rasterizer States
//-----------------------------------
RasterizerState gRasterizerState
{
	CullMode = none;
	FrontCounterClockwise = true;
};

//-----------------------------------
// Blend States
//-----------------------------------
BlendState gBlendState
{
	BlendEnable[0] = true;
	SrcBlend = src_alpha;
	DestBlend = inv_src_alpha;
	BlendOp = add;
	SrcBlendAlpha = zero;
	DestBlendAlpha = zero;
	BlendOpAlpha = add;
	RenderTargetWriteMask[0] = 0x0F;
};

DepthStencilState gDepthStencilState
{
	DepthEnable = true;
	DepthWriteMask = zero;
	DepthFunc = less;
	StencilEnable = false;
};

//-----------------------------------
// Input/Output Structs
//-----------------------------------

struct VS_INPUT
{
	float3 Position : POSITION;
	float3 Color : COLOR;
	float2 TexCoord : TEXCOORD;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float4 WorldPosition : COLOR;
	//float3 Color : COLOR;
	float2 TexCoord : TEXCOORD;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
};


//-----------------------------------
// Vertex Shader
//-----------------------------------

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Position = mul(float4(input.Position, 1.0f), gWorldViewProj);
	output.WorldPosition = mul(float4(input.Position, 1.0f), gWorld);
	//output.Color = input.Color;
	output.TexCoord = input.TexCoord;
	output.Normal = mul(normalize(input.Normal), (float3x3)gWorld);
	output.Tangent = mul(normalize(input.Tangent), (float3x3)gWorld);
	return output;
}


//-----------------------------------
// Pixel Shader
//-----------------------------------

float4 ShadePixel(VS_OUTPUT input, SamplerState sampleState)
{
	return gDiffuseMap.Sample(sampleState, input.TexCoord);
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
	float4 shade = ShadePixel(input, gSamplerState);
	return shade;
}


//-----------------------------------
// Technique
//-----------------------------------
technique11 DefaultTechnique
{
	pass P0
	{
		SetRasterizerState(gRasterizerState);
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_5_0, PS() ) );
	}
}