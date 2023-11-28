////////////////////////////////////////////////////////////////////////////
// 
// File: fog.fx
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Effect file that handles device states for linear vertex fog.
//          
////////////////////////////////////////////////////////////////////////////

// 
// Effect
//

technique Fog
{
    pass P0
    {
        //
        // Set Misc render states.

        pixelshader      = null;
        vertexshader     = null;
        fvf              = XYZ | Normal;
        Lighting         = true;
        NormalizeNormals = true;
        SpecularEnable   = false;

        //
        // Fog States

        FogVertexMode = LINEAR; // linear fog function
        FogStart      = 50.0f;  // fog starts 50 units away from viewpoint
        FogEnd        = 300.0f; // fog ends 300 units away from viewpoint

        FogColor      = 0x00CCCCCC; // gray
        FogEnable     = true;       // enable
    }
}
