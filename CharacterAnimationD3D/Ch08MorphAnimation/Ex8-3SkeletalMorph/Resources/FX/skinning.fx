//////////////////////////////////////////////////////////////////////////
//                  Character Animation with Direct3D                   //
//                         Author: C. Granberg                          //
//                             2008 - 2009                              //
//////////////////////////////////////////////////////////////////////////

//Transformation Matrices
matrix matW;
matrix matVP;

//World Light Position
float3 lightPos;

//Texture
texture texHuman;
texture texWolf;

//Samplers
sampler HumanSampler = sampler_state
{
    Texture = (texHuman);
    MinFilter = Linear;   MagFilter = Linear;   MipFilter = Linear;
    AddressU  = Wrap;     AddressV  = Wrap;     AddressW  = Wrap;
    MaxAnisotropy = 16;
};

sampler WolfSampler = sampler_state
{
    Texture = (texWolf);
    MinFilter = Linear;   MagFilter = Linear;   MipFilter = Linear;
    AddressU  = Wrap;     AddressV  = Wrap;     AddressW  = Wrap;
    MaxAnisotropy = 16;
};

////////////////////////////////////////////////////////////////////////////

extern float4x4 FinalTransforms[35];
extern int NumVertInfluences = 2; // <--- Normally set dynamically.

//Morph Weight
float shapeShift;

//Vertex Input
struct VS_INPUT_SKIN
{
    float4 position    : POSITION0;
    float3 normal      : NORMAL0;
    float2 tex0        : TEXCOORD0;
    float4 weights     : BLENDWEIGHT0;
    int4   boneIndices : BLENDINDICES0;

    float4 position2 : POSITION1;
    float3 normal2   : NORMAL1;
};

//Vertex Output / Pixel Shader Input
struct VS_OUTPUT
{
    float4 position : POSITION0;
    float2 tex0     : TEXCOORD0;
    float  shade    : TEXCOORD1;
};

VS_OUTPUT vs_Skinning(VS_INPUT_SKIN IN)
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;

    float4 position = IN.position + (IN.position2 - IN.position) * shapeShift;
    float4 p = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float3 norm = float3(0.0f, 0.0f, 0.0f);
    float lastWeight = 0.0f;
    int n = NumVertInfluences-1;

    IN.normal = normalize(IN.normal);
    for(int i = 0; i < n; ++i)
    {
        lastWeight += IN.weights[i];
        p += IN.weights[i] * mul(position, FinalTransforms[IN.boneIndices[i]]);
        norm += IN.weights[i] * mul(IN.normal, FinalTransforms[IN.boneIndices[i]]);
    }
    lastWeight = 1.0f - lastWeight;

    p += lastWeight * mul(position, FinalTransforms[IN.boneIndices[n]]);
    norm += lastWeight * mul(IN.normal, FinalTransforms[IN.boneIndices[n]]);

    p.w = 1.0f;
    float4 posWorld = mul(p, matW);
    OUT.position = mul(posWorld, matVP);
    OUT.tex0 = IN.tex0;

    //Calculate Lighting
    norm = normalize(norm);
    norm = mul(norm, matW);
    OUT.shade = max(dot(norm, normalize(lightPos - posWorld)), 0.2f);

    return OUT;
}

//Pixel Shader
float4 ps_lighting(VS_OUTPUT IN) : COLOR0
{
    //Sample human texture
    float4 colorHuman = tex2D(HumanSampler, IN.tex0);

    //sample wolf texture
    float4 colorWolf = tex2D(WolfSampler, IN.tex0);

    //Blend the result based on the shapeShift variable
    return (colorHuman * (1.0f - shapeShift) + colorWolf * shapeShift) * IN.shade;
}

technique Skinning
{
    pass P0
    {
        Lighting = false;

        VertexShader = compile vs_2_0 vs_Skinning();
        PixelShader  = compile ps_2_0 ps_lighting();
    }
}
