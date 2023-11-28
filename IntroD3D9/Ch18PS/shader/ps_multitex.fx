////////////////////////////////////////////////////////////////////////////
// 
// File: ps_alphablend.fx
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Pixel shader that does multi texturing.
//          
////////////////////////////////////////////////////////////////////////////

//
// Globals
//

sampler BaseTex;
sampler SpotLightTex;
sampler StringTex;

//
// Structures
//


struct PS_INPUT
{
    float2 base      : TEXCOORD0;
    float2 spotlight : TEXCOORD1;
    float2 text      : TEXCOORD2;
};

struct PS_OUTPUT
{
    vector diffuse : COLOR0;
};


//
// Main
//

PS_OUTPUT Main(PS_INPUT input)
{
    // zero out members of output
    PS_OUTPUT output = (PS_OUTPUT)0;

    // sample appropriate textures
    vector b = tex2D(BaseTex,      input.base);
    vector s = tex2D(SpotLightTex, input.spotlight);
    vector t = tex2D(StringTex,    input.text);

    // combine texel colors
    vector c = b * s + t;

    // increase the intensity of the pixel slightly
    c += 0.1f;

    // save the resulting pixel color
    output.diffuse = c;

    return output;
}