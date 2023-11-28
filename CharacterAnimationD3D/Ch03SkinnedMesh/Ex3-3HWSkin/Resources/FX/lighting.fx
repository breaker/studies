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
texture texDiffuse;

//Sampler
sampler DiffuseSampler = sampler_state {
    Texture = (texDiffuse);
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU  = Wrap;
    AddressV  = Wrap;
    AddressW  = Wrap;
    MaxAnisotropy = 16;
};

//Vertex Input
struct VS_INPUT {
    float4 position : POSITION0;
    float3 normal   : NORMAL;
    float2 tex0     : TEXCOORD0;
};

//Vertex Output / Pixel Shader Input
struct VS_OUTPUT {
    float4 position : POSITION0;
    float2 tex0     : TEXCOORD0;
    float  shade    : TEXCOORD1;
};

//Vertex Shader
VS_OUTPUT vs_lighting(VS_INPUT IN) {
    VS_OUTPUT OUT = (VS_OUTPUT) 0;

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
float4 ps_lighting(VS_OUTPUT IN) : COLOR0 {
    float4 color = tex2D(DiffuseSampler, IN.tex0);
    return color * IN.shade;
}

//Lighting Technique
technique Lighting {
    pass P0 {
        Lighting = false;

        VertexShader = compile vs_2_0 vs_lighting();
        PixelShader  = compile ps_2_0 ps_lighting();
    }
}

//Pixel Shader
float4 ps_shadow(VS_OUTPUT IN) : COLOR0 {
    return float4(0.3f, 0.3f, 0.3f, 1.0f);
}

//Shadow Technique
technique Shadow {
    pass P0 {
        Lighting = false;

        VertexShader = compile vs_2_0 vs_lighting();
        PixelShader  = compile ps_2_0 ps_shadow();
    }
}

////////////////////////////////////////////////////////////////////////////

extern float4x4 MatrixPalette[35];
extern int numBoneInfluences = 2;

//Vertex Input
struct VS_INPUT_SKIN {
    float4 position    : POSITION0;
    float3 normal      : NORMAL;
    float2 tex0        : TEXCOORD0;
    float4 weights     : BLENDWEIGHT0;  // weights 和 boneIndices 两个成员是由 ConvertToIndexedBlendedMesh 转换普通 vertex 产生的
    int4   boneIndices : BLENDINDICES0; // 这个 vertex 称为 index blended vertex
};

VS_OUTPUT vs_Skinning(VS_INPUT_SKIN IN) {
    VS_OUTPUT OUT = (VS_OUTPUT) 0;

    float4 p = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float3 norm = float3(0.0f, 0.0f, 0.0f);
    float lastWeight = 0.0f;
    int n = numBoneInfluences - 1;
    IN.normal = normalize(IN.normal);

    //Blend vertex position & normal
    for (int i = 0; i < n; ++i) {
        lastWeight += IN.weights[i];
        p += IN.weights[i] * mul(IN.position, MatrixPalette[IN.boneIndices[i]]);
        norm += IN.weights[i] * mul(IN.normal, MatrixPalette[IN.boneIndices[i]]);
    }
    lastWeight = 1.0f - lastWeight;

    p += lastWeight * mul(IN.position, MatrixPalette[IN.boneIndices[n]]);
    norm += lastWeight * mul(IN.normal, MatrixPalette[IN.boneIndices[n]]);
    p.w = 1.0f;

    //Transform vertex to world space
    float4 posWorld = mul(p, matW);

    //... then to screen space
    OUT.position = mul(posWorld, matVP);

    //Copy UV coordinate
    OUT.tex0 = IN.tex0;

    //Calculate Lighting
    norm = normalize(norm);
    norm = mul(norm, matW);
    OUT.shade = max(dot(norm, normalize(lightPos - posWorld)), 0.2f);

    return OUT;
}

technique Skinning {
    pass P0 {
        Lighting = false;

        VertexShader = compile vs_2_0 vs_Skinning();
        PixelShader  = compile ps_2_0 ps_lighting();
    }
}
