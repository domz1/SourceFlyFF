// Blur Pixel Shader
//
// (c)2003 Virtools

//--- Additional Automatic Parameters

texture tex0 : TEXTURE0;
texture tex1 : TEXTURE1;
texture tex2 : TEXTURE2;

//--- Textures Samplers
sampler texSampler0 = sampler_state
{
    texture = <tex0>;
//    MipFilter = LINEAR;
    Minfilter = LINEAR;
    Magfilter = LINEAR;
    AddressU = CLAMP;
    AddressV = CLAMP;
};
sampler texSampler1 = sampler_state
{
    texture = <tex1>;
    Minfilter = LINEAR;
    Magfilter = LINEAR;
    AddressU = CLAMP;
    AddressV = CLAMP;
};
sampler texSampler2 = sampler_state
{
    texture = <tex2>;
    Minfilter = LINEAR;
    Magfilter = LINEAR;
    AddressU = CLAMP;
    AddressV = CLAMP;
};


technique BlurTech
{

    pass pX
    {
		Sampler[0] = (texSampler0);
		Sampler[1] = (texSampler1);
		Sampler[2] = (texSampler2);

		ColorOp[0] = SELECTARG1;
		ColorArg1[0] = TEXTURE;
		ColorArg2[0] = DIFFUSE;
		AlphaOp[0] = DISABLE;

		ColorOp[1] = SELECTARG1;
		ColorArg1[1] = TEXTURE;
		ColorArg2[1] = DIFFUSE;
		AlphaOp[1] = DISABLE;

		ColorOp[2] = SELECTARG1;
		ColorArg1[2] = TEXTURE;
		ColorArg2[2] = DIFFUSE;
		AlphaOp[2] = DISABLE;

        VertexShader = 
		asm
        {
            vs_1_1
//			def c0, -0.0009765625, 0, 0, 0		// offset
			def c0,  0.25, 0.25, 0, 0		// offset
			def c1,  0, 0, 0, 0
			def c2,  0.0009765625, 0, 0, 0
            
			dcl_position v0
            dcl_texcoord v1

//            mov oPos, v0
			mov oPos, c0
//			mov oD0, c0

//            add oT0, v1, c0
//			add oT1, v1, c1
//			add oT2, v1, c2
        };

        PixelShader = 
		asm
		{
			ps_1_3
			def c0, 0.25, 0.25, 0.25, 0.25	// -1 offset weight
			def c1, 0.50, 0.50, 0.50, 0.50  //  0 offset weight
			def c2, 0.25, 0.25, 0.25, 0.25	//  1 offset weight
			tex t0
			tex t1
			tex t2

//			mul r0, t0, c1	// left 
			mov r0, t0

//			mul r0, t0, c0	// left 
//			mad r0, t1, c1, r0
//			mad r0, t2, c2, r0
		};
    }
}
