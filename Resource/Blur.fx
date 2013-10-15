// Blur Pixel Shader
//
// (c)2003 Virtools

//--- Additional Automatic Parameters
string XFile = "plane.x";   // model
int    BCLR = 0xffffffff;   // background

texture tex0 : TEXTURE0;
//texture tex < string name = "flyff256.bmp"; >;
//float2 texsize <bool isTexelSize=true;>;
float2 texsize = 0.00097;

// transformations
float4x4 World      : WORLD;
float4x4 View       : VIEW;
float4x4 Projection : PROJECTION;

//--- Manual Parameters
float blurFactor = 0.2;
float blurAngle = 0.7;
float blurBurnFactor = 0.25;

//--- Some static pre-computation
static float2 texSizeFactor = texsize * blurFactor * 10;
static float2 UVOffset0 = float2( cos(blurAngle), -sin(blurAngle) )*texSizeFactor;
static float2 UVOffset1 = float2(-sin(blurAngle), -cos(blurAngle) )*texSizeFactor;
static float2 UVOffset2 = float2(-cos(blurAngle),  sin(blurAngle) )*texSizeFactor;
static float2 UVOffset3 = float2( sin(blurAngle),  cos(blurAngle) )*texSizeFactor;

//--- VS Output Structure

struct    VSOUT 
{
    float4 pos : POSITION;
    float2 tex0 : TEXCOORD0;
//    float2 tex1 : TEXCOORD1;
//    float2 tex2 : TEXCOORD2;
//	float2 tex3 : TEXCOORD3;
//	float2 tex4 : TEXCOORD4;
//	float2 tex5 : TEXCOORD5;
//	float2 tex6 : TEXCOORD6;
//	float2 tex7 : TEXCOORD7;
};

//float gWeight[7] = { 0.1, 0.358, 0.773, 1.0, 0.773, 0.358, 0.1 };

//--- Vertex Shader Blur
VSOUT BlurVS( 
    float4 Pos : POSITION,
    float2 Tex0 : TEXCOORD0 )
{
    VSOUT vsout = (VSOUT)0;

	vsout.pos = Pos;
/*
    float4x4 WorldView = mul(World, View);

    float3 P = mul(Pos, (float4x3)WorldView);  // position (view space)
    vsout.pos  = mul(float4(P, 1), Projection);             // position (projected)
*/

    vsout.tex0 = Tex0;

//    vsout.tex0 = Tex0.x + 0.0009765625;		// offset
  //  vsout.tex1 = Tex0;
    //vsout.tex2 = Tex0.x - 0.0009765625;
//	vsout.tex3 = Tex0 + gWeight[3];
//	vsout.tex4 = Tex0 + gWeight[4];
//	vsout.tex5 = Tex0 + gWeight[5];
//	vsout.tex6 = Tex0 + gWeight[6];

    return vsout;
}

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

float4 BlurPS3_0H( float2 Tex : TEXCOORD0 ) : COLOR
{
    float4 Color = 0;

    Color = tex2D( texSampler0, Tex );
	Color /= 2;
    
    return Color;
}
/*
float4 BlurPS_X( float2 Tex : TEXCOORD0 ) : COLOR
{
    float Weight[7] = { 0.1, 0.358, 0.773, 1.0, 0.773, 0.358, 0.1 };
    float BlurN = 6;
    float4 Color = 0;

    Color += Weight[0] * ( tex2D( texSampler0, Tex + float2( 0, -3.0 / MAP_CX ) ) );    // 
    Color += Weight[1] * ( tex2D( texSampler0, Tex + float2( 0, -2.0 / MAP_CX ) ) );    // 
    Color += Weight[2] * ( tex2D( texSampler0, Tex + float2( 0, -1.0 / MAP_CX ) ) );    // 
    Color += Weight[3] * ( tex2D( texSampler0, Tex + float2( 0,  0.0 / MAP_CX ) ) );    // 
    Color += Weight[4] * ( tex2D( texSampler0, Tex + float2( 0,  1.0 / MAP_CX ) ) );    // 
    Color += Weight[5] * ( tex2D( texSampler0, Tex + float2( 0,  2.0 / MAP_CX ) ) );    // 
    Color += Weight[6] * ( tex2D( texSampler0, Tex + float2( 0,  3.0 / MAP_CX ) ) );    // 
    
    return Color / BlurN;
}

float4 BlurPS_Y( float2 Tex : TEXCOORD0 ) : COLOR
{
    float Weight[7] = { 0.1, 0.358, 0.773, 1.0, 0.773, 0.358, 0.1 };
    float BlurN = 6;
    float4 Color = 0;

    Color += Weight[0] * ( tex2D( texSampler0, Tex + float2( 0, -3.0 / MAP_CY ) ) );    // 
    Color += Weight[1] * ( tex2D( texSampler0, Tex + float2( 0, -2.0 / MAP_CY ) ) );    // 
    Color += Weight[2] * ( tex2D( texSampler0, Tex + float2( 0, -1.0 / MAP_CY ) ) );    // 
    Color += Weight[3] * ( tex2D( texSampler0, Tex + float2( 0,  0.0 / MAP_CY ) ) );    // 
    Color += Weight[4] * ( tex2D( texSampler0, Tex + float2( 0,  1.0 / MAP_CY ) ) );    // 
    Color += Weight[5] * ( tex2D( texSampler0, Tex + float2( 0,  2.0 / MAP_CY ) ) );    // 
    Color += Weight[6] * ( tex2D( texSampler0, Tex + float2( 0,  3.0 / MAP_CY ) ) );    // 
    
    return Color / BlurN;
}
*/


technique BlurTech
{
/*
    pass pX
    {
        VertexShader = compile vs_2_0 BlurVS();
        PixelShader = compile ps_2_0 BlurPS_X2();
    }
    pass pY
    {
        VertexShader = compile vs_1_1 BlurVS();
        PixelShader = compile ps_2_0 BlurPS_Y();
    }
*/
    pass p
    {
        VertexShader = compile vs_1_1 BlurVS();
//        PixelShader = compile ps_1_1 BlurPS3_0H();
		PixelShader =
		asm
		{
			ps_1_1
			def c0, 0.910f, 0.910f, 0.910f, 1.0	
			tex t0

			mul r0, t0, c0	// * 0.5
		};
    }
/*
    pass pM
    {
        VertexShader = compile vs_1_1 BlurVS();
        PixelShader = compile ps_1_1 BlurPS3_0M();
    }
    pass pL
    {
        VertexShader = compile vs_1_1 BlurVS();
        PixelShader = compile ps_1_1 BlurPS3_0L();
    }
*/
}
