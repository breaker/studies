////////////////////////////////////////////////////////////////////////////
// 
// File: light_tex.fx
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Effect file that handles device states for lighting and texturing
//       a 3D model.
//          
////////////////////////////////////////////////////////////////////////////

//
// Globals
//

matrix WorldMatrix;
matrix ViewMatrix;
matrix ProjMatrix;

texture Tex;


//
// Sampler
//

sampler S0 = sampler_state
{
    Texture   = (Tex);
    MinFilter = LINEAR; 
    MagFilter = LINEAR; 
    MipFilter = LINEAR;
};


// 
// Effect
//

technique LightAndTexture
{
    pass P0
    {
        //
        // Set Misc render states.

        pixelshader      = null;
        vertexshader     = null;
        fvf              = XYZ | Normal | Tex1;
        Lighting         = true;
        NormalizeNormals = true;
        SpecularEnable   = false;


        //
        // Set Transformation States

        WorldTransform[0]   = (WorldMatrix);
        ViewTransform       = (ViewMatrix);
        ProjectionTransform = (ProjMatrix);


        //
        // Set a light source at light index 0.  We fill out all the 
        // components for light[0] because  The Direct3D
        // documentation recommends us to fill out all components
        // for best performance.

        LightType[0]         = Directional;
        LightAmbient[0]      = {0.2f,  0.2f, 0.2f, 1.0f};
        LightDiffuse[0]      = {1.0f,  1.0f, 1.0f, 1.0f};
        LightSpecular[0]     = {0.0f,  0.0f, 0.0f, 1.0f};
        LightDirection[0]    = {1.0f, -1.0f, 1.0f, 0.0f};
        LightPosition[0]     = {0.0f,  0.0f, 0.0f, 0.0f};
        LightFalloff[0]      = 0.0f;
        LightRange[0]        = 0.0f;
        LightTheta[0]        = 0.0f;
        LightPhi[0]          = 0.0f;
        LightAttenuation0[0] = 1.0f;
        LightAttenuation1[0] = 0.0f;
        LightAttenuation2[0] = 0.0f;

        // Finally, enable the light:

        LightEnable[0] = true;



        //
        // Set Material components.  This is like calling
        // IDirect3DDevice9::SetMaterial.

        MaterialAmbient  = {1.0f, 1.0f, 1.0f, 1.0f};
        MaterialDiffuse  = {1.0f, 1.0f, 1.0f, 1.0f};
        MaterialEmissive = {0.0f, 0.0f, 0.0f, 0.0f};
        MaterialPower    = 1.0f;
        MaterialSpecular = {1.0f, 1.0f, 1.0f, 1.0f};


        //
        // Hook up the sampler object to sampler stage 0,
        // which is given by Sampler[0].

        Sampler[0] = (S0);

    }
}
