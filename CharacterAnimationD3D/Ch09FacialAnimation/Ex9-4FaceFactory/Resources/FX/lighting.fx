//////////////////////////////////////////////////////////////////////////
//                  Character Animation with Direct3D                   //
//                         Author: C. Granberg                          //
//                             2008 - 2009                              //
//////////////////////////////////////////////////////////////////////////

//Transformation matrices
matrix matW;
matrix matVP;

//World Light Position
float3 lightPos;

//Texture
texture texDiffuse;

//Morphing weights
float4 weights;

//Sampler
sampler DiffuseSampler = sampler_state
{
    Texture = (texDiffuse);
    MinFilter = Linear;   MagFilter = Linear;   MipFilter = Linear;
    AddressU  = Wrap;     AddressV  = Wrap;     AddressW  = Wrap;
    MaxAnisotropy = 16;
};

struct VS_INPUT
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

//Vertex Output / Pixel Shader Input
struct VS_OUTPUT
{
    float4 position : POSITION0;
    float2 tex0     : TEXCOORD0;
    float  shade    : TEXCOORD1;
};

VS_OUTPUT vsFaceMorph(VS_INPUT IN)
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

float4 psFaceMorph(VS_OUTPUT IN) : COLOR0
{
    return tex2D(DiffuseSampler, IN.tex0) * IN.shade;
}

technique FaceMorph
{
    pass P0
    {
        VertexShader = compile vs_2_0 vsFaceMorph();
        PixelShader  = compile ps_2_0 psFaceMorph();
        Lighting = false;
    }
}

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

//Vertex Input 2
struct VS_INPUT2
{
    float4 position : POSITION0;
    float3 normal   : NORMAL;
    float2 tex0     : TEXCOORD0;
};

//Vertex Shader
VS_OUTPUT vs_lighting(VS_INPUT2 IN)
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;

    //getting the position of the vertex in the world
    float4 posWorld = mul(IN.position, matW);
    float4 normal = normalize(mul(IN.normal, matW));

    //getting to position to object space
    OUT.position = mul(posWorld, matVP);

    OUT.shade = max(dot(normal, normalize(lightPos - posWorld)), 0.2f);

    OUT.tex0 = IN.tex0;

    return OUT;
}

//Pixel Shader
float4 ps_lighting(VS_OUTPUT IN) : COLOR0
{
    return tex2D(DiffuseSampler, IN.tex0) * IN.shade;
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
