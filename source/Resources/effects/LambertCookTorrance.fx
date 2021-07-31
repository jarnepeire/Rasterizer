//-----------------------------------
// Global Variables
//-----------------------------------

//-----Matrices-----
float4x4 gWorldViewProj : WorldViewProjection;
float4x4 gWorld : WORLD;
float4x4 gViewInverse : VIEWINVERSE; //Last column contains camera pos!

//-----Diffuse-----
bool gUseDiffuseMap = false;
Texture2D gDiffuseMap : DiffuseMap;

//-----Normal-----
bool gUseNormalMap = false;
Texture2D gNormalMap : NormalMap;

//-----Specular-----
float gShininess = 25.0f;
bool gUseSpecularMap = false;
Texture2D gSpecularMap : SpecularMap;

//-----Glossiness-----
bool gUseGlossinessMap = false;
Texture2D gGlossinessMap : GlossinessMap;

//-----Metalness-----
bool gUseMetalMap = false;
Texture2D gMetalnessMap : MetalnessMap;

//-----Roughness-----
bool gUseRoughnessMap = false;
Texture2D gRoughnessMap : RoughnessMap;

//-----LightManager-----
//Note that this value should also be known in the C++ code
//In the LightManager there's a max lights int defined, keep this in mind!
static const int gMaxLights = 5;

//Note, a LightMatrix is defined as a column based matrix with the following:
float4x4 gLights[gMaxLights];

//Column 0: Light Color (x,y,z == r,g,b)
int gLightColorIndex = 0;

//Column 1: Light Dir (x,y,z)
int gLightDirIndex = 1;

//Column 2: Light Intensity (same for all components)
int gLightIntensityIndex = 2;

//-----Other-----
float gPi = 3.14159265f;

float3 gLightDirection = float3(0.577f, -0.577f, 0.577f);
float gLightIntensity = 7.0f;
float3 gDefaultColor = float3(1.0f, 1.0f, 1.0f);



//-----------------------------------
// Sampler States
//-----------------------------------
SamplerState gSamplerState
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Wrap;
	AddressV = Wrap;
	//AddressU = Border;
	//AddressV = Clamp;
	BorderColor = float4(0.0f, 0.0f, 1.0f, 1.0f);
};

//-----------------------------------
// Rasterizer States
//-----------------------------------
RasterizerState gRasterizerState
{
	CullMode = back;
	FrontCounterClockwise = true;
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
// Blend States
//-----------------------------------
BlendState gBlendState
{
	BlendEnable[0] = false;
};

DepthStencilState gDepthStencilState
{
	DepthEnable = true;
	DepthWriteMask = all;
	StencilEnable = true;
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
float3 ShadePixelLCT(VS_OUTPUT input, SamplerState sampleState)
{
	//------ Diffuse ------
	float3 diffuse = gDefaultColor;
	if (gUseDiffuseMap)
	{
		diffuse = gDiffuseMap.Sample(sampleState, input.TexCoord);
	}
	
	//------ Normal ------
	float3 normal = input.Normal;
	if (gUseNormalMap)
	{
		//We define our tangent space
		float3 binormal = cross(input.Normal, input.Tangent);
		float3x3 localTangentSpace = float3x3(input.Tangent, binormal, input.Normal);

		//Sample normal map
		float3 normalSample = gNormalMap.Sample(sampleState, input.TexCoord);

		//An RBG Color goes from [0, 255] range, while we will need this in [-1, 1]
		//Sampled value is already returned in range [0, 1]
		normalSample = (normalSample * 2.0f) - float3(1.0f, 1.0f, 1.0f);

		//Transform to tangent space
		normal = normalize(mul(normalSample, localTangentSpace));
	}
	
	//------ Metalness -----
	int metalness = 0;
	if (gUseMetalMap)
	{
		float metalSample = gMetalnessMap.Sample(sampleState, input.TexCoord).r;
		if (metalSample > 0.5f)
		{
			metalness = 1;
		}
		else if (metalSample < 0.5f)
		{ 
			metalness = 0;
		}
	}
	
	//------ Roughness -----
	float roughness = 0.5f;
	if (gUseRoughnessMap)
	{
		roughness = gRoughnessMap.Sample(sampleState, input.TexCoord).r;
	}
	
	//------ Lambert Cook Torrance -----
	float3 finalColor = (float3)0;
	for (int i = 0; i < 2; i++)
	{
		//Common Variables
		float4x4 currentLightMatrix = gLights[i];
		float3 lightCol = currentLightMatrix[gLightColorIndex].xyz;
		float3 lightDir = currentLightMatrix[gLightDirIndex].xyz;
		float lightIntensity = currentLightMatrix[gLightIntensityIndex].x;

		//Calculate irradiance for this light
		float observedArea = saturate(dot(normal, -lightDir));
		float3 irradiance = lightCol * lightIntensity * observedArea;
		
		//Roughness sqrd
		float alpha = roughness * roughness;
		
		//Determine F0 value (metalness ?) 
		float3 F0 = float3(0.04f, 0.04f, 0.04f);
		if (metalness == 0)
		{
			F0 = diffuse;
		}
		
		//Halfvector
		float3 viewDirection = normalize(input.WorldPosition.xyz - gViewInverse[3].xyz);
		float3 halfVector = normalize(viewDirection + lightDir);
		
		//Dot products
		float dotNL = saturate(dot(normal, lightDir));
		float dotNV = saturate(dot(normal, viewDirection));
		float dotNH = saturate(dot(normal, halfVector));
		float dotVH = saturate(dot(viewDirection, halfVector));
	
		//Fresnel
		float3 fresnel = F0 + (float3(1,1,1) - F0) * pow(1 - dotVH, 5);
		
		//Normal Distribution
		float a = alpha * alpha;
		float denom = pow(pow(dotNH, 2) * (a-1) + 1, 2);
		float normalDistribution = a / (gPi * denom);
		
		//Geometry
		float kDirect = pow(alpha + 1, 2) / 8.0f;
		float geometryMask = dotNV / (dotNV * (1.0f - kDirect) + kDirect);
		float geometryShadow = dotNL / (dotNL * (1.0f - kDirect) + kDirect);
		float geometry = geometryMask * geometryShadow;
		
			
		//Specular
		float dfg = fresnel * normalDistribution * geometry;
		float specular = dfg / 4.0f * dotNV * dotNL;
		
		//Lambert Cook Torrance BRDF
		float3 brdf = (diffuse / gPi) + specular;
		finalColor += irradiance * brdf;
	}
	
	return finalColor;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
	float3 shade = ShadePixelLCT(input, gSamplerState);
	return float4(shade, 1.0f);
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
