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

//Morph Weights
float4 weights;

//Vertex Input
struct VS_INPUT
{
    float4 basePos     : POSITION0;
    float3 baseNorm    : NORMAL0;
    float2 baseUV      : TEXCOORD0;

    float4 targetPos1  : POSITION1;
    float3 targetNorm1 : NORMAL1;

    float4 targetPos2  : POSITION2;
    float3 targetNorm2 : NORMAL2;

    float4 targetPos3  : POSITION3;
    float3 targetNorm3 : NORMAL3;

    float4 targetPos4  : POSITION4;
    float3 targetNorm4 : NORMAL4;
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

    float4 pos  = IN.basePos;
    float3 norm = IN.baseNorm;

    //Blend Position
    pos += (IN.targetPos1 - IN.basePos) * weights.r;
    pos += (IN.targetPos2 - IN.basePos) * weights.g;
    pos += (IN.targetPos3 - IN.basePos) * weights.b;
    pos += (IN.targetPos4 - IN.basePos) * weights.a;

    //Blend Normal
    norm += (IN.targetNorm1 - IN.baseNorm) * weights.r;
    norm += (IN.targetNorm2 - IN.baseNorm) * weights.g;
    norm += (IN.targetNorm3 - IN.baseNorm) * weights.b;
    norm += (IN.targetNorm4 - IN.baseNorm) * weights.a;

    //getting the position of the vertex in the world
    float4 posWorld = mul(pos, matW);
    float4 normal = normalize(mul(norm, matW));

    //getting to position to object space
    OUT.position = mul(posWorld, matVP);

    OUT.shade = max(dot(normal, normalize(lightPos - posWorld)), 0.2f);

    OUT.tex0 = IN.baseUV;

    return OUT;
}

//Pixel Shader
float4 ps_lighting(VS_OUTPUT IN) : COLOR0
{
    return float4(0.8f, 0.8f, 0.8f, 1.0f) * IN.shade;
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
