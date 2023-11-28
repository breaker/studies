//////////////////////////////////////////////////////////////////////////
//                  Character Animation with Direct3D                   //
//                         Author: C. Granberg                          //
//                             2008 - 2009                              //
//////////////////////////////////////////////////////////////////////////

//Transformation Matrices
matrix matW;
matrix matVP;

//Control hair table
extern float3 ControlHairTable[20];

int numPointsPerHair = 5;


//Texture
texture texDiffuse;

//Samplers
sampler DiffuseSampler = sampler_state
{
    Texture = (texDiffuse);
    MinFilter = Linear;   MagFilter = Linear;   MipFilter = Linear;
    AddressU  = Wrap;     AddressV  = Wrap;     AddressW  = Wrap;
    MaxAnisotropy = 16;
};

//Hair Vertex
struct VS_INPUT_HAIR
{
    float4 position    : POSITION0;
    float3 normal      : NORMAL;
    float2 tex0        : TEXCOORD0;
    int4   hairIndices : BLENDINDICES0;
};

//Vertex Output / Pixel Shader Input
struct VS_OUTPUT
{
    float4 position : POSITION0;
    float2 tex0     : TEXCOORD0;
};

float3 GetHairPos(int hair, int index1, int index2, float prc)
{
    //Calculate index 0 & 3
    int index0 = max(index1 - 1, 0);
    int index3 = min(index2 + 1, numPointsPerHair - 1);

    //Offset index to correct hair in ControlHairTable
    index0 += hair * numPointsPerHair;
    index1 += hair * numPointsPerHair;
    index2 += hair * numPointsPerHair;
    index3 += hair * numPointsPerHair;

    //Perform cubic interpolation
    float3 P = (ControlHairTable[index3] - ControlHairTable[index2]) - (ControlHairTable[index0] - ControlHairTable[index1]);
    float3 Q = (ControlHairTable[index0] - ControlHairTable[index1]) - P;
    float3 R =  ControlHairTable[index2] - ControlHairTable[index0];
    float3 S =  ControlHairTable[index1];

    return (P * prc * prc * prc) +
           (Q * prc * prc) +
           (R * prc) +
            S;
}

//Hair Vertex Shader
VS_OUTPUT vs_hair(VS_INPUT_HAIR IN)
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;

    //Get position from the four control hairs
    float3 ch1 = GetHairPos(0, IN.hairIndices[0], IN.hairIndices[1], IN.position.z);
    float3 ch2 = GetHairPos(1, IN.hairIndices[0], IN.hairIndices[1], IN.position.z);
    float3 ch3 = GetHairPos(2, IN.hairIndices[0], IN.hairIndices[1], IN.position.z);
    float3 ch4 = GetHairPos(3, IN.hairIndices[0], IN.hairIndices[1], IN.position.z);

    //Blend linearly in 2D
    float3 px1 = ch2 * IN.position.x + ch1 * (1.0f - IN.position.x);
    float3 px2 = ch3 * IN.position.x + ch4 * (1.0f - IN.position.x);

    float3 pos = px2 * IN.position.y + px1 * (1.0f - IN.position.y);

    //Transform to world coordinates
    float4 posWorld = mul(float4(pos.xyz, 1), matW);
    OUT.position = mul(posWorld, matVP);

    //Copy texture coordinates
    OUT.tex0 = IN.tex0;

    return OUT;
}


//Pixel Shader
float4 ps_hair(VS_OUTPUT IN) : COLOR0
{
    float4 color = tex2D(DiffuseSampler, IN.tex0);
    return color;
}

//Lighting Technique
technique Hair
{
    pass P0
    {
        //FillMode = Wireframe;
        AlphaBlendEnable = true;
        SrcBlend = SRCALPHA;
        DestBlend = INVSRCALPHA;
        CullMode = None;
        AlphaOp[0] = ADD;
        ColorOp[0] = ADD;
        ZWriteEnable = false;
        VertexShader = compile vs_2_0 vs_hair();
        PixelShader  = compile ps_2_0 ps_hair();
    }
}

//////////////////////////////////////////////////////
//                  Bounding Sphere                 //
//////////////////////////////////////////////////////

//World Light Position
float3 lightPos = float3(300.0f, 500.0f, -150.0f);

//Vertex Input
struct VS_BS_INPUT
{
    float4 position : POSITION0;
    float3 normal   : NORMAL;
};

//Vertex Output / Pixel Shader Input
struct VS_BS_OUTPUT
{
    float4 position : POSITION0;
    float  shade    : TEXCOORD0;
};

//Vertex Shader
VS_BS_OUTPUT vs_bs_lighting(VS_BS_INPUT IN)
{
    VS_BS_OUTPUT OUT = (VS_BS_OUTPUT)0;

    //getting the position of the vertex in the world
    float4 posWorld = mul(IN.position, matW);
    float4 normal = normalize(mul(IN.normal, matW));

    //getting to position to object space
    OUT.position = mul(posWorld, matVP);

    OUT.shade = dot(normal, normalize(lightPos - posWorld)) * 0.7f + 0.3f;

    return OUT;
}

//Pixel Shader
float4 ps_bs_lighting(VS_BS_OUTPUT IN) : COLOR0
{
    return float4(0.8f, 1.0f, 0.8, 1.0f) * IN.shade;
}

//Lighting Technique
technique BSLighting
{
    pass P0
    {
        Lighting = false;

        VertexShader = compile vs_2_0 vs_bs_lighting();
        PixelShader  = compile ps_2_0 ps_bs_lighting();
    }
}
