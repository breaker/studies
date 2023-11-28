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
    float3 normal : NORMAL;
    float2 tex0 : TEXCOORD0;
};

//Vertex Output / Pixel Shader Input
struct VS_OUTPUT {
    float4 position : POSITION0;
    float2 tex0 : TEXCOORD0;
    float  shade : TEXCOORD1;
};

//Vertex Shader
VS_OUTPUT vs_lighting(VS_INPUT IN) {
    VS_OUTPUT OUT = (VS_OUTPUT) 0;

    //getting the position of the vertex in the world
    float4 posWorld = mul(IN.position, matW);
    float4 normal = normalize(mul(IN.normal, matW));

    //getting to position to object space
    OUT.position = mul(posWorld, matVP);

    // Light Intensity ·¶Î§ 0.2 ~ 1.0
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
        Lighting = FALSE;

        VertexShader = compile vs_2_0 vs_lighting();
        PixelShader  = compile ps_2_0 ps_lighting();
    }
}

//Pixel Shader
float4 ps_shadow(VS_OUTPUT IN) : COLOR0 {
    return float4(0.1f, 0.1f, 0.1f, 1.0f);
}

//Shadow Technique
technique Shadow {
    pass P0 {
        Lighting = FALSE;

        VertexShader = compile vs_2_0 vs_lighting();
        PixelShader  = compile ps_2_0 ps_shadow();
    }
}
