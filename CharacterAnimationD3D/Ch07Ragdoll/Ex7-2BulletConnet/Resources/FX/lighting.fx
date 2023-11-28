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
