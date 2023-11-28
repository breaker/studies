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

//Vertex Input
struct VS_INPUT
{
    float4 position : POSITION0;
    float3 normal   : NORMAL;
};

//Vertex Output / Pixel Shader Input
struct VS_OUTPUT
{
    float4 position : POSITION0;
    float  shade    : TEXCOORD0;
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

    return OUT;
}

//Pixel Shader
float4 ps_lighting(VS_OUTPUT IN) : COLOR0
{
    return float4(0.8f, 1.0f, 0.8, 1.0f) * IN.shade;
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
