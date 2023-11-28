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
float3 cameraPos;

//Morphing weights
float4 weights;

//Textures
texture texDiffuse;
texture texNormalMap;
texture texSpecularMap;

//Samplers
sampler DiffuseSampler = sampler_state
{
    Texture = (texDiffuse);
    MinFilter = Linear;   MagFilter = Linear;   MipFilter = Linear;
    AddressU  = Wrap;     AddressV  = Wrap;     AddressW  = Wrap;
    MaxAnisotropy = 16;
};

sampler NormalSampler = sampler_state
{
    Texture = (texNormalMap);
    MinFilter = Linear;   MagFilter = Linear;   MipFilter = Linear;
    AddressU  = Wrap;     AddressV  = Wrap;     AddressW  = Wrap;
    MaxAnisotropy = 16;
};

sampler SpecularSampler = sampler_state
{
    Texture = (texSpecularMap);
    MinFilter = Linear;   MagFilter = Linear;   MipFilter = Linear;
    AddressU  = Wrap;     AddressV  = Wrap;     AddressW  = Wrap;
    MaxAnisotropy = 16;
};

//Vertex Input
struct VS_INPUT
{
    float4 pos0  : POSITION0;
    float3 norm0 : NORMAL0;
    float2 tex0  : TEXCOORD0;
    float3 tangent  : TANGENT0;
    float3 binormal : BINORMAL0;

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
    float4 position     : POSITION0;
    float2 tex0         : TEXCOORD0;
    float3 lightVec     : TEXCOORD1;
    float3 lightHalf    : TEXCOORD2;
};

//Vertex Shader
VS_OUTPUT morphNormalMapVS(VS_INPUT IN)
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
    float4 posWorld = mul(position, matW);
    OUT.position = mul(posWorld, matVP);

    normal = normalize(mul(normal, matW));
    float3 tangent = normalize(mul(IN.tangent, matW));
    float3 binormal = normalize(mul(IN.binormal, matW));

    //getting light-to-vertex direction
    float3 lightDir = normalize(lightPos - posWorld);

    //get camera-to-vertex direction
    float3 viewDir = normalize(cameraPos - posWorld);

    //Calculate the Half vector
    float3 vHalf = normalize(lightDir + viewDir);

    //calculating the binormal and setting the Tangent Binormal and Normal matrix
    float3x3 TBNMatrix = float3x3(tangent, binormal, normal);

    //setting the lightVector
    OUT.lightVec = mul(TBNMatrix, lightDir);
    OUT.lightHalf = mul(TBNMatrix, vHalf);

    OUT.tex0 = IN.tex0;

    return OUT;
}

//Pixel Shader
float4 morphNormalMapPS(VS_OUTPUT IN) : COLOR0
{
    //calculate the color and the normal
    float4 color = tex2D(DiffuseSampler, IN.tex0);

    //this is how you uncompress a normal map
    float3 normal = 2.0f * tex2D(NormalSampler, IN.tex0).rgb - 1.0f;

    //get specular
    float4 specularColor = tex2D(SpecularSampler, IN.tex0);

    //set the output color
    float diffuse = max(saturate(dot(normal, normalize(IN.lightVec))), 0.2f);

    float specular = max(saturate(dot(normal, normalize(IN.lightHalf))), 0.0f);
    specular = pow(specular, 85.0f) * 0.4f;

    return color * diffuse + specularColor * specular;
}

//Face Morph Technique
technique FaceMorphNormalMap
{
    pass P0
    {
        Lighting = false;

        VertexShader = compile vs_2_0 morphNormalMapVS();
        PixelShader  = compile ps_2_0 morphNormalMapPS();
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

//Vertex Output 2
struct VS_OUTPUT2
{
    float4 position   : POSITION0;
    float2 tex0       : TEXCOORD0;
    float shade       : TEXCOORD1;
    float3 normal     : TEXCOORD2;
    float3 lightHalf  : TEXCOORD3;
};

//Vertex Shader
VS_OUTPUT2 vs_lighting(VS_INPUT2 IN)
{
    VS_OUTPUT2 OUT = (VS_OUTPUT2)0;

    //getting the position of the vertex in the world
    float4 posWorld = mul(IN.position, matW);
    float4 normal = normalize(mul(IN.normal, matW));

    //getting to position to object space
    OUT.position = mul(posWorld, matVP);

    //Half vector
    float3 viewDir = cameraPos - posWorld;
    float3 vHalf = normalize(normalize(lightPos) + normalize(viewDir));

    OUT.shade = max(dot(normal, normalize(lightPos - posWorld)), 0.2f);

    OUT.normal = normal;
    OUT.lightHalf = normalize(mul(vHalf, matW));

    OUT.tex0 = IN.tex0;

    return OUT;
}

//Pixel Shader
float4 ps_lighting(VS_OUTPUT2 IN) : COLOR0
{
    float specular = max(saturate(dot(normalize(IN.normal), normalize(IN.lightHalf))), 0.0f);
    float power = pow(specular, 500.0f);

    return tex2D(DiffuseSampler, IN.tex0) * IN.shade + float4(1,1,1,1) * power;
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
