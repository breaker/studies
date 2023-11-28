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

//Morphing weights
float4 weights;

//Texture
texture texDiffuse;

//Sampler
sampler DiffuseSampler = sampler_state
{
    Texture = (texDiffuse);
    MinFilter = Linear;   MagFilter = Linear;   MipFilter = Linear;
    AddressU  = Wrap;     AddressV  = Wrap;     AddressW  = Wrap;
    MaxAnisotropy = 16;
};

//Vertex Input
struct VS_INPUT
{
    float4 position : POSITION0;
    float3 normal   : NORMAL;
    float2 tex0     : TEXCOORD0;
};

//Vertex Output / Pixel Shader Input
struct VS_OUTPUT
{
    float4 position : POSITION0;
    float2 tex0     : TEXCOORD0;
    float  shade    : TEXCOORD1;
};

//Vertex Shader
VS_OUTPUT vs_lighting(VS_INPUT IN)
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;

    //getting the position of the vertex in the world
    float4 posWorld = mul(IN.position, matW);
    float4 normal = normalize(mul(IN.normal, matW));

    //getting to position to object space
    OUT.position = mul(posWorld, matVP);

    OUT.shade = dot(normal, normalize(lightPos - posWorld)) * 0.8f + 0.1f;

    OUT.tex0 = IN.tex0;

    return OUT;
}

//Pixel Shader
float4 ps_lighting(VS_OUTPUT IN) : COLOR0
{
    float4 color = tex2D(DiffuseSampler, IN.tex0);
    return color * IN.shade;
}

//Lighting Technique
technique Lighting
{
    pass P0
    {
        Lighting = false;
        VertexShader = compile vs_2_0 vs_lighting();
        PixelShader  = compile ps_2_0 ps_lighting();
    }
}


////////////////////////////////////////////////////////////////////////////

extern float4x4 FinalTransforms[35];
extern int NumVertInfluences = 2; // <--- Normally set dynamically.

//Vertex Input
struct VS_INPUT_SKIN
{
    float4 position : POSITION0;
    float3 normal   : NORMAL;
    float2 tex0     : TEXCOORD0;
    float4 weights  : BLENDWEIGHT0;
    int4   boneIndices : BLENDINDICES0;
};

VS_OUTPUT vs_Skinning(VS_INPUT_SKIN IN)
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;

    float4 p = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float3 norm = float3(0.0f, 0.0f, 0.0f);
    float lastWeight = 0.0f;
    int n = NumVertInfluences-1;

    IN.normal = normalize(IN.normal);

    for(int i = 0; i < n; ++i)
    {
        lastWeight += IN.weights[i];
        p += IN.weights[i] * mul(IN.position, FinalTransforms[IN.boneIndices[i]]);
        norm += IN.weights[i] * mul(IN.normal, FinalTransforms[IN.boneIndices[i]]);
    }
    lastWeight = 1.0f - lastWeight;

    p += lastWeight * mul(IN.position, FinalTransforms[IN.boneIndices[n]]);
    norm += lastWeight * mul(IN.normal, FinalTransforms[IN.boneIndices[n]]);

    p.w = 1.0f;
    float4 posWorld = mul(p, matW);
    OUT.position = mul(posWorld, matVP);

    //Calculate Lighting
    norm = normalize(norm);
    norm = mul(norm, matW);
    OUT.shade = max(dot(norm, normalize(lightPos - posWorld)), 0.2f);

    OUT.tex0 = IN.tex0;

    return OUT;
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


///////////////////////////////////////////////////////////////////////////////////////////

struct VS_MORPH_INPUT
{
    float4 pos0  : POSITION0;
    float3 norm0 : NORMAL0;
    float2 tex0  : TEXCOORD0;

    float4 pos1  : POSITION01;
    float3 norm1 : NORMAL1;

    float4 pos2  : POSITION2;
    float3 norm2 : NORMAL2;

    float4 pos3  : POSITION3;
    float3 norm3 : NORMAL3;

    float4 pos4  : POSITION4;
    float3 norm4 : NORMAL4;
};

VS_OUTPUT vsFaceMorph(VS_MORPH_INPUT IN)
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;

    float4 position  = IN.pos0;
    float3 normal = IN.norm0;

    //Blend Position
    position += (IN.pos1 - IN.pos0) * weights.r;
    position += (IN.pos2 - IN.pos0) * weights.g;
    position += (IN.pos3 - IN.pos0) * weights.b;
    position += (IN.pos4 - IN.pos0) * weights.a;

    //Blend Normal
    normal += (IN.norm1 - IN.norm0) * weights.r;
    normal += (IN.norm2 - IN.norm0) * weights.g;
    normal += (IN.norm3 - IN.norm0) * weights.b;
    normal += (IN.norm4 - IN.norm0) * weights.a;

    //getting the position of the vertex in the world
    float4 worldPos = mul(position, matW);
    normal = normalize(mul(normal, matW));

    //getting to position to object space

    OUT.position = mul(worldPos, matVP);

    OUT.shade = max(dot(normal, normalize(lightPos - worldPos)), 0.2f);

    OUT.tex0 = IN.tex0;

    return OUT;
}

technique FaceMorph
{
    pass P0
    {
        VertexShader = compile vs_2_0 vsFaceMorph();
        PixelShader  = compile ps_2_0 ps_lighting();
        Lighting = false;
    }
}


///////////////////////////////////////////////////////////////////////////////////////////

struct VS_BONE_MORPH_INPUT
{
    float4 pos0  : POSITION0;
    float3 norm0 : NORMAL0;
    float2 tex0  : TEXCOORD0;

    float4 pos1  : POSITION01;
    float3 norm1 : NORMAL1;

    float4 pos2  : POSITION2;
    float3 norm2 : NORMAL2;

    float4 pos3  : POSITION3;
    float3 norm3 : NORMAL3;

    float4 pos4  : POSITION4;
    float3 norm4 : NORMAL4;

    float4 weights  : BLENDWEIGHT5;
    int4   boneIndices : BLENDINDICES5;
};

VS_OUTPUT vsFaceBoneMorph(VS_BONE_MORPH_INPUT IN)
{
    //First Morph the mesh, then apply skinning!

    VS_OUTPUT OUT = (VS_OUTPUT)0;

    float4 position  = IN.pos0;
    float3 normal = IN.norm0;

    //Blend Position
    position += (IN.pos1 - IN.pos0) * weights.r;
    position += (IN.pos2 - IN.pos0) * weights.g;
    position += (IN.pos3 - IN.pos0) * weights.b;
    position += (IN.pos4 - IN.pos0) * weights.a;

    //Blend Normal
    normal += (IN.norm1 - IN.norm0) * weights.r;
    normal += (IN.norm2 - IN.norm0) * weights.g;
    normal += (IN.norm3 - IN.norm0) * weights.b;
    normal += (IN.norm4 - IN.norm0) * weights.a;

    //getting the position of the vertex in the world
    float4 posWorld = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float3 normWorld = float3(0.0f, 0.0f, 0.0f);
    float lastWeight = 0.0f;
    int n = NumVertInfluences-1;
    normal = normalize(normal);

    for(int i = 0; i < n; ++i)
    {
        lastWeight += IN.weights[i];
        posWorld += IN.weights[i] * mul(position, FinalTransforms[IN.boneIndices[i]]);
        normWorld += IN.weights[i] * mul(normal, FinalTransforms[IN.boneIndices[i]]);
    }
    lastWeight = 1.0f - lastWeight;

    posWorld += lastWeight * mul(position, FinalTransforms[IN.boneIndices[n]]);
    normWorld += lastWeight * mul(normal, FinalTransforms[IN.boneIndices[n]]);
    posWorld.w = 1.0f;

    OUT.position = mul(posWorld, matVP);

    OUT.shade = max(dot(normWorld, normalize(lightPos - posWorld)), 0.2f);

    OUT.tex0 = IN.tex0;

    return OUT;
}

technique FaceBoneMorph
{
    pass P0
    {
        VertexShader = compile vs_2_0 vsFaceBoneMorph();
        PixelShader  = compile ps_2_0 ps_lighting();
        Lighting = false;
    }
}
