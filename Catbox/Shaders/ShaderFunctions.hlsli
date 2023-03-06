#pragma once
//#include "Noise/Noise.hlsli"
//#include "Functions/Ease.hlsli"
#define tau 6.28318530718
#define pi 3.14159265359

/////////////////////////////////////////////////////////////
//--- Outputs ease in Sine of x
float easeInSine(float x)
{
    return 1 - cos((x * pi) / 2);
}

/////////////////////////////////////////////////////////////
//--- Outputs ease out Sine of x
float easeOutSine(float x)
{
    return sin((x * pi) / 2);
}

/////////////////////////////////////////////////////////////
//--- Outputs ease in and out Sine of x
float easeInOutSine(float x)
{
    return -(cos(pi * x) - 1) / 2;
}

/////////////////////////////////////////////////////////////
//--- Outputs ease in Quad of x
float easeInQuad(float x)
{
    return x * x;
}

/////////////////////////////////////////////////////////////
//--- Outputs ease out Quad of x
float easeOutQuad(float x)
{
    return 1 - (1 - x) * (1 - x);
}

/////////////////////////////////////////////////////////////
//--- Outputs ease in and out Quad of x
float easeInOutQuad(float x)
{
    return x < 0.5 ? 2 * x * x : 1 - pow(-2 * x + 2, 2) / 2;
}

/////////////////////////////////////////////////////////////
//--- Outputs ease in Cubic of x
float easeInCubic(float x)
{
    return x * x * x;
}

/////////////////////////////////////////////////////////////
//--- Outputs ease out cubic of x
float easeOutCubic(float x)
{
    return 1 - pow(1 - x, 3);
}

/////////////////////////////////////////////////////////////
//--- Outputs ease in and out cubic of x
float easeInOutCubic(float x)
{
    return x < 0.5 ? 4 * x * x * x : 1 - pow(-2 * x + 2, 3) / 2;
}

/////////////////////////////////////////////////////////////
//--- Outputs ease in Quart of x
float easeInQuart(float x)
{
    return x * x * x * x;
}

/////////////////////////////////////////////////////////////
//--- Outputs ease out Quart of x
float easeOutQuart(float x)
{
    return 1 - pow(1 - x, 4);
}

/////////////////////////////////////////////////////////////
//--- Outputs ease in and out Quart of x
float easeInOutQuart(float x)
{
    return x < 0.5 ? 8 * x * x * x * x : 1 - pow(-2 * x + 2, 4) / 2;
}

/////////////////////////////////////////////////////////////
//--- Outputs ease in Quint of x
float easeInQuint(float x)
{
    return x * x * x * x * x;
}

/////////////////////////////////////////////////////////////
//--- Outputs ease out Quint of x
float easeOutQuint(float x)
{
    return 1 - pow(1 - x, 5);
}

/////////////////////////////////////////////////////////////
//--- Outputs ease in and out Quint of x
float easeInOutQuint(float x)
{
    return x < 0.5 ? 16 * x * x * x * x * x : 1 - pow(-2 * x + 2, 5) / 2;
}

/////////////////////////////////////////////////////////////
//--- Outputs ease in Expo of x
float easeInExpo(float x)
{
    return x == 0 ? 0 : pow(2, 10 * x - 10);
}

/////////////////////////////////////////////////////////////
//--- Outputs ease out Expo of x
float easeOutExpo(float x)
{
    return x == 1 ? 1 : 1 - pow(2, -10 * x);
}

/////////////////////////////////////////////////////////////
//--- Outputs ease in and out Expo of x
float easeInOutExpo(float x)
{
    return x == 0
           ? 0
           : x == 1
           ? 1
           : x < 0.5 ? pow(2, 20 * x - 10) / 2
           : (2 - pow(2, -20 * x + 10)) / 2;
}

/////////////////////////////////////////////////////////////
//--- Outputs ease in Circ of x
float easeInCirc(float x)
{
    return 1 - sqrt(1 - x * x);
}

/////////////////////////////////////////////////////////////
//--- Outputs ease out Circ of x
float easeOutCirc(float x)
{
    return sqrt(1 - pow(x - 1, 2));
}

/////////////////////////////////////////////////////////////
//--- Outputs ease in and out Circ of x
float easeInOutCirc(float x)
{
    return x < 0.5
           ? (1 - sqrt(1 - pow(2 * x, 2))) / 2
           : (sqrt(1 - pow(-2 * x + 2, 2)) + 1) / 2;
}

/////////////////////////////////////////////////////////////
//--- Outputs ease in Back of x
float easeInBack(float x)
{
    // 10% bounce, is this better -> 1.7015401988668 ?
    const float c1 = 1.70158;
    const float c3 = c1 + 1;

    return c3 * x * x * x - c1 * x * x;
}

/////////////////////////////////////////////////////////////
//--- Outputs ease out Back of x
float easeOutBack(float x)
{
    // 10% bounce, is this better -> 1.7015401988668 ?
    const float c1 = 1.70158;
    const float c3 = c1 + 1;

    return 1 + c3 * pow(x - 1, 3) + c1 * pow(x - 1, 2);
}

/////////////////////////////////////////////////////////////
//--- Outputs ease in and out Back of x
float easeInOutBack(float x)
{
    // 10% bounce, is this better -> 1.7015401988668 ?
    const float c1 = 1.70158;
    const float c2 = c1 * 1.525;

    return x < 0.5
           ? (pow(2 * x, 2) * ((c2 + 1) * 2 * x - c2)) / 2
           : (pow(2 * x - 2, 2) * ((c2 + 1) * (x * 2 - 2) + c2) + 2) / 2;
}

/////////////////////////////////////////////////////////////
//--- Outputs ease in Elastic of x
float easeInElastic(float x)
{
    const float c4 = (2 * pi) / 3;
    return x == 0
           ? 0
           : x == 1
           ? 1
           : -pow(2, 10 * x - 10) * sin((x * 10 - 10.75) * c4);
}

/////////////////////////////////////////////////////////////
//--- Outputs ease out Elastic of x
float easeOutElastic(float x)
{
    const float c4 = (2 * pi) / 3;

    return x == 0
           ? 0
           : x == 1
           ? 1
           : pow(2, -10 * x) * sin((x * 10 - 0.75) * c4) + 1;
}

/////////////////////////////////////////////////////////////
//--- Outputs ease in and out Elastic of x
float easeInOutElastic(float x)
{
    const float c5 = (2 * pi) / 4.5;

    return x == 0
           ? 0
           : x == 1
           ? 1
           : x < 0.5
           ? -(pow(2, 20 * x - 10) * sin((20 * x - 11.125) * c5)) / 2
           : (pow(2, -20 * x + 10) * sin((20 * x - 11.125) * c5)) / 2 + 1;
}

/////////////////////////////////////////////////////////////
//--- Outputs ease out Bounce of x
float easeOutBounce(float x)
{
    //Magic numbers i know :/
    const float n1 = 7.5625;
    const float d1 = 2.75;

    if (x < 1 / d1)
    {
        return n1 * x * x;
    }
    else if (x < 2 / d1)
    {
        return n1 * (x -= 1.5 / d1) * x + 0.75;
    }
    else if (x < 2.5 / d1)
    {
        return n1 * (x -= 2.25 / d1) * x + 0.9375;
    }
    else
    {
        return n1 * (x -= 2.625 / d1) * x + 0.984375;
    }
}

/////////////////////////////////////////////////////////////
//--- Outputs ease in Bounce of x
float easeInBounce(float x)
{
    return 1 - easeOutBounce(1 - x);
}

/////////////////////////////////////////////////////////////
//--- Outputs ease in and out Bounce of x
float easeInOutBounce(float x)
{
    return x < 0.5
           ? (1 - easeOutBounce(1 - 2 * x)) / 2
           : (1 + easeOutBounce(2 * x - 1)) / 2;
}

///////////////////////////////////
//--- Special Crafted Combinations

/////////////////////////////////////////////////////////////
//--- Outputs ease out quint with inverted ease out quad to x
float fjongInEaseOut(float x)
{
    float a = 1 - pow(1 - x, 4);
    return sqrt(a) * (1 - x) * (1 - x) * 2;
}
/////////////////////////////////////
//--- New stuff

float Arch(float t)
{
    return (t * (1.f - t)) * 4; //Gångra med 4 för att normalisera mellan 0 och 1
}
float BellCurve(float t)
{
    return ((1.f - t) * t) * ((1.f - t) * t) * 16; //Gångra med 16 för att normalisera mellan 0 och 1
}
float Bezier(float b, float c, float t)
{
    float s = 1.f - t;
    float t2 = t * t;
    float s2 = s * s;
    float t3 = t2 * t;
    return (3.f * b * s2 * t) + (3.f * c * s * t2) + (t3);
}

float3 Hash23(float2 input)
{
    float3 output;
    
    float3 first = float3(input.r, input.g, input.r);
    float3 second = float3(input.g, input.r, input.r);
    float3 third = float3(input.r, input.g, input.g);
    
    output = float3(dot(first, float3(127.1, 311.7, 74.7)), dot(second, float3(269.5, 183.3, 246.1)), dot(third, float3(113.5, 271.9, 124.6)));
    
    output = frac(sin(output) * float3(43758.5453123, 43758.5453123, 43758.5453123));
    
    return output;
}


float2 SimplexHash(float2 p) // replace this by something better
{
    //p = float2(dot(p, float2(127.1, 311.7)), dot(p, float2(269.5, 183.3)));
    //return -1.0 + 2.0 * frac(sin(p) * 43758.5453123);
    
    return -1.0 + 2.0 * Hash23(p);

}

float SimplexNoise(float2 p)
{
    const float K1 = 0.366025404; // (sqrt(3)-1)/2;
    const float K2 = 0.211324865; // (3-sqrt(3))/6;

    float2 i = floor(p + (p.x + p.y) * K1);
    float2 a = p - i + (i.x + i.y) * K2;
    float m = step(a.y, a.x);
    float2 o = float2(m, 1.0 - m);
    float2 b = a - o + K2;
    float2 c = a - 1.0 + 2.0 * K2;
    float3 h = max(0.5 - float3(dot(a, a), dot(b, b), dot(c, c)), 0.0);
    float3 n = h * h * h * h * float3(dot(a, SimplexHash(i + 0.0)), dot(b, SimplexHash(i + o)), dot(c, SimplexHash(i + 1.0)));
    return dot(n, float3(70.0, 70.0, 70.0));
}


/////
//-- 3d implementation
float noise3D(float3 p)
{
    return frac(sin(dot(p, float3(12.9898, 78.233, 128.852))) * 43758.5453) * 2.0 - 1.0;
}

const float f3 = 0.33333333333333333333333333333333; //-> 1.0 / 3.0
const float g3 = 0.16666666666666666666666666666667; //-> 1.0 / 6.0
float SimplexNoise(float3 p)
{
    float s = (p.x + p.y + p.z) * f3;
    int i = int(floor(p.x + s));
    int j = int(floor(p.y + s));
    int k = int(floor(p.z + s));
	
    float t = float((i + j + k)) * g3;
    float x0 = float(i) - t;
    float y0 = float(j) - t;
    float z0 = float(k) - t;
    x0 = p.x - x0;
    y0 = p.y - y0;
    z0 = p.z - z0;

	
    int i1 = (x0 >= y0) ? ((y0 >= z0) ? 1 : ((x0 >= z0) ? 1 : 0)) : ((y0 < z0) ? 0 : ((x0 < z0) ? 0 : 0));
    int j1 = (x0 >= y0) ? ((y0 >= z0) ? 0 : ((x0 >= z0) ? 0 : 0)) : ((y0 < z0) ? 0 : ((x0 < z0) ? 1 : 1));
    int k1 = (x0 >= y0) ? ((y0 >= z0) ? 0 : ((x0 >= z0) ? 0 : 1)) : ((y0 < z0) ? 1 : ((x0 < z0) ? 0 : 0));
    int i2 = (x0 >= y0) ? ((y0 >= z0) ? 1 : ((x0 >= z0) ? 1 : 1)) : ((y0 < z0) ? 0 : ((x0 < z0) ? 0 : 1));
    int j2 = (x0 >= y0) ? ((y0 >= z0) ? 1 : ((x0 >= z0) ? 0 : 0)) : ((y0 < z0) ? 1 : ((x0 < z0) ? 1 : 1));
    int k2 = (x0 >= y0) ? ((y0 >= z0) ? 0 : ((x0 >= z0) ? 1 : 1)) : ((y0 < z0) ? 1 : ((x0 < z0) ? 1 : 0));
	
    float x1 = x0 - float(i1) + g3;
    float y1 = y0 - float(j1) + g3;
    float z1 = z0 - float(k1) + g3;
    float x2 = x0 - float(i2) + 2.0 * g3;
    float y2 = y0 - float(j2) + 2.0 * g3;
    float z2 = z0 - float(k2) + 2.0 * g3;
    float x3 = x0 - 1.0 + 3.0 * g3;
    float y3 = y0 - 1.0 + 3.0 * g3;
    float z3 = z0 - 1.0 + 3.0 * g3;
				 
    float3 ijk0 = float3(i, j, k);
    float3 ijk1 = float3(i + i1, j + j1, k + k1);
    float3 ijk2 = float3(i + i2, j + j2, k + k2);
    float3 ijk3 = float3(i + 1, j + 1, k + 1);
            
    float3 gr0 = normalize(float3(noise3D(ijk0), noise3D(ijk0 * 2.01), noise3D(ijk0 * 2.02)));
    float3 gr1 = normalize(float3(noise3D(ijk1), noise3D(ijk1 * 2.01), noise3D(ijk1 * 2.02)));
    float3 gr2 = normalize(float3(noise3D(ijk2), noise3D(ijk2 * 2.01), noise3D(ijk2 * 2.02)));
    float3 gr3 = normalize(float3(noise3D(ijk3), noise3D(ijk3 * 2.01), noise3D(ijk3 * 2.02)));
	
    float n0 = 0.0;
    float n1 = 0.0;
    float n2 = 0.0;
    float n3 = 0.0;

    float t0 = max(0.0, 0.5 - x0 * x0 - y0 * y0 - z0 * z0);
    t0 *= t0;
    n0 = t0 * t0 * dot(gr0, float3(x0, y0, z0));

    float t1 = max(0.0, 0.5 - x1 * x1 - y1 * y1 - z1 * z1);
    t1 *= t1;
    n1 = t1 * t1 * dot(gr1, float3(x1, y1, z1));

    float t2 = max(0.0, 0.5 - x2 * x2 - y2 * y2 - z2 * z2);
    t2 *= t2;
    n2 = t2 * t2 * dot(gr2, float3(x2, y2, z2));

    float t3 = max(0.0, 0.5 - x3 * x3 - y3 * y3 - z3 * z3);
    t3 *= t3;
    n3 = t3 * t3 * dot(gr3, float3(x3, y3, z3));
    return 96.0 * (n0 + n1 + n2 + n3);
}

float fbm(float3 p)
{
    float f;
    f = 0.50000 * SimplexNoise(p);
    p = p * 2.01;
    f += 0.25000 * SimplexNoise(p);
    p = p * 2.02; //from iq
    f += 0.12500 * SimplexNoise(p);
    p = p * 2.03;
    f += 0.06250 * SimplexNoise(p);
    p = p * 2.04;
    f += 0.03125 * SimplexNoise(p);
    return f;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

float StartDelay(float startDelay, float time)
{
    return max(startDelay, time) - startDelay;
}

float StopDelay(float stopDelay, float time)
{
    return min(stopDelay, time);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//--- Timer starts counting after <startDelay> of <time> and stops after <stopDelay> of <time> ---
float TimerSS(float startDelay, float stopDelay, float time)
{

    return StopDelay(stopDelay, StartDelay(startDelay, time));
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//--- Input UV and does an atlas walk to output flipbook animnated UV's according to parameters ---
float2 FlipBook(float2 uvCoord, int numRows, int numColumns, float frameRate, float time, bool loop=false)
{ 
    if (time > (numRows * numColumns) / frameRate && !loop) discard;
    //--- Stepped HZ or something ---
    float stepHorizontal = floor(time * frameRate);
    float stepVertical = floor((time * frameRate) / numRows);
    float2 timeStep = float2(stepHorizontal, stepVertical);
    
    //--- Should point at one frame ---
    float2 singleFrame = 1.0f / float2(numRows, numColumns);
    float2 frameCoord = uvCoord + timeStep;
    singleFrame *= frameCoord;
  
    //--- Always return values between 0..1 ---
    singleFrame = frac(singleFrame);
    return singleFrame;
}
////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
//--- FLIPBOOK UVCOMPUTATION (LINEAR BLENDING) ---
struct FlipBookBlendData
{
    float2 CurrentImageTexCoord;
    float2 NextImageTexCoord;
    float Ratio;
};

FlipBookBlendData FlipBookUVBlend(float2 TexCoord, int NumU, int NumV, float FrameRate, float Time)
{

    FlipBookBlendData o;

    float2 NumUV = float2((float) NumU, (float) NumV);
    float T = (Time * FrameRate) % (NumUV.x * NumUV.y);
    float CF = floor(T);
    float NF = ceil(T);
    o.Ratio = frac(T);
    o.CurrentImageTexCoord = (TexCoord + float2(CF % NumU, NumV - floor(CF / NumU))) / NumUV;
    o.NextImageTexCoord = (TexCoord + float2(NF % NumU, NumV - floor(NF / NumU))) / NumUV;
    return o;
}

///////////////////////////////////////////////////////////////
//--- Input UV and rotates coords with <radian>
float2 RotateUV(float2 uv_coords, float radian)
{
    float2 outout = uv_coords - 0.5;
    
    float rotCos = cos(radian);
    float rotSin = sin(radian);

    float rotCosX = rotCos * outout.x;
    float rotCosY = rotCos * outout.y;

    float rotSinX = rotSin * outout.x;
    float rotSinY = rotSin * outout.y;

    outout = float2(rotCosX - rotSinY, rotSinX + rotCosY) + 0.5;
    return outout;
}


///////////////////////////////////////////////////////////////
//--- Input UV and rotates coords with <radian>
float2 RotateScaleUV(float2 uv_coords, 
                     float2 pivot = 0.5, 
                     float rotations = 0, 
                     float2 scale = 1.0)
{    
    float2 outout = uv_coords - pivot;
    
    float rotCos = cos(rotations);
    float rotSin = sin(rotations);

    float rotCosX = rotCos * outout.x;
    float rotCosY = rotCos * outout.y;

    float rotSinX = rotSin * outout.x;
    float rotSinY = rotSin * outout.y;

    outout = float2(rotCosX - rotSinY, rotSinX + rotCosY);
    outout = float2(outout.x *= scale.x, outout.y *= scale.y);
    outout += pivot;
    return outout;
}


///////////////////////////////////////////////////////////////
//--- Input UV and scrolls coords with <vScroll> and <uScroll>
float2 OffsetUV(float2 uv_coords, float2 offset)
{
    return float2(uv_coords.x += offset.x, uv_coords.y += offset.y);
}


///////////////////////////////////////////////////////////////
//--- Input UV and scale coords with <vScale> and <uScale>
float2 ScaleUV(float2 uv_coords, float2 scale)
{
    return float2(uv_coords.x *= scale.x, uv_coords.y *= scale.y);
}


///////////////////////////////////////////////////////////////
//--- Input UV and scale coords with <vScale> and <uScale>
float2 TransformUV(float2 uv_coords, 
                   float2 pivot = 0.5, 
                   float rotations = 0, 
                   float2 offset = 0.5, 
                   float2 scale = 1.0)
{
    float2 output = RotateScaleUV(uv_coords, pivot, rotations, scale);
    output = OffsetUV(output, offset);
        
    return output;
}


/////////////////////////////////////////////////////////////////////////////////
//--- Input UV and scale coords with <vScale> and <uScale> randomly from <seed>
float2 RandomTransformUV(float2 uv_coords,
                         float2 seed,
                         float  rotateMinMax = float2(-180.0, 180.0), 
                         float2 moveMinMax   = float2(0.8, 1.2), 
                         float2 scaleMinMax  = float2(0.8, 1.2))
{
    float2 output;
    float3 ran = Hash23(seed);
    
 // OBS! this is broken atm...
    
    float2 ranUv = TransformUV(uv_coords,
                               0.5,
                               lerp(rotateMinMax.x, rotateMinMax.r, ran.x),
                               lerp(rotateMinMax.x, rotateMinMax.r, frac(16.0 * ran.x)),
                               ran.xy);
        
    return ranUv;
}


///////////////////////////////////////////////////////////////
//--- Input UV and scale coords with <vScale> and <uScale>
float2 SwirlUV(float2 uv_coords, float rotate = 0.0, float radius = 1.0, float strength = 1.0)
{
    float2 uv = uv_coords;
    float2 st = float2(atan2(uv.x, uv.y), length(uv));
    
    return float2(st.x / tau + .5 + rotate+st.y*sin(rotate*10), st.y);
}
///////////////////////////////////////////////////////////////
//--- Input two albedo-textures and lerp between them
float4 MultiplyAlbedos(float4 albedo1, float4 albedo2)
{
    float4 albedosCombined = lerp(albedo1, albedo2, 0.5);
    float4 Output = albedosCombined;
    
    return Output;
}

/////////////////////////////////////////////////////////////
//--- Input two ssao-textures and lerp between them
float MultiplySsao(float ssao1, float ssao2)
{
    float Combined = lerp(ssao1, ssao2, 0.5);
    float Output = Combined;
    
    return Output;
}

/////////////////////////////////////////////////////////////
//--- Input two material-textures and lerp between them
float4 MultiplyMaterials(float4 material1, float4 material2)
{
    float4 Combined = lerp(material1, material2, 0.5);
    float4 Output = Combined;
    
    return Output;
}
/////////////////////////////////////////////////////////////
//--- Input two normal-textures and lerp between them
float3 MultiplyNormals(float3 normal1, float3 normal2)
{
    float2 normalsCombined = lerp(normal1.xy, normal2.xy, 0.5);
    float3 Output = float3(normalsCombined, normal1.z);
    
    return Output;
}
/////////////////////////////////////////////////////////////
//--- Fade in the UV
float UVFadeIn(float UV, float GradientSharpness)
{
    float Output = lerp(1, 0, pow(smoothstep(0.25, 0.6, UV), GradientSharpness));
   
    return Output;
}
/////////////////////////////////////////////////////////////
//--- Fade out the UV
float UVFadeOut(float UV, float GradientSharpness)
{
    float Output = lerp(0, 1, pow(smoothstep(0.0, 0.9, UV), GradientSharpness));
    
    return Output;
}
/////////////////////////////////////////////////////////////
//--- Fade in and out the UV
float UVFadeInAndOut(float UV, float GradientSharpness)
{
         
    float UVFadeIn = lerp(1, 0, pow(smoothstep(0.25, 0.6, UV), GradientSharpness));
    float UVFadeOut = lerp(0, 1, pow(smoothstep(0.0, 1, UV), GradientSharpness));
    float Output = UVFadeIn + UVFadeOut;
    
    return Output;
    
}


/////////////////////////////////////////////////////////////
//--- Outputs a gradient
float Gradient(float t, float start, float end)
{
    return smoothstep(start - end, start + end, t);
}

/////////////////////////////////////////////////////////////
//--- Outputs a band with adjustable edge blur
float Band(float t, float start, float end, float blur)
{
    float step1 = smoothstep(start - blur, start + blur, t);
    float step2 = smoothstep(end + blur, end - blur, t);
    return step1 * step2;
}

/////////////////////////////////////////////////////////////
//--- Outputs a rectangle with adjustable edge blur
float Rect(float2 uv, float left, float right, float bottom, float top, float blur)
{
    float band1 = Band(uv.x, left, right, blur);
    float band2 = Band(uv.y, bottom, top, blur);
    return band1 * band2;
}


/////////////////////////////////////////////////////////////////////
//--- Outputs remapped t from range xMin - xMax to range yMin - yMax
//--- non clamped output
float Remap(float xMin, float xMax, float yMin, float yMax, float t)
{   
    return yMin + (yMax - yMin) * ((t - xMin) / (xMax - xMin));
}


/////////////////////////////////////////////////////////////////////
//--- Outputs remapped t from range xMin - xMax to range yMin - yMax
//--- Clamped output with xMin and xMax
float RemapClamp(float xMin, float xMax, float yMin, float yMax, float t)
{
    float value = yMin + (yMax - yMin) * ((t - xMin) / (xMax - xMin));
    return max(yMin, min(yMax, value));
}


/////////////////////////////////////////////////////////////////
//--- Outputs pixel data dependent on higtMap and driver, 0..1
//--- Cunstruction of gradient border color:
//--- 
//---                                         Lead color            Traíl color
//--- borderColorGradient = float2x3(float3(1.0, 0.5, 0.1), float3(0.2, 0.4, 1.0))
//---                                                                            
float3 AlphaGradientErode(float heightMap, float3 background, float3 foreground, float driver,
                          float borderThikness = 0.0, float2x3 borderColorGradient = float2x3(float3(1.0, 0.5, 0.1), 
                                                                                      float3(0.2, 0.4, 1.0)))
{
    float3 erosion = smoothstep(driver - borderThikness, driver, heightMap);
    
    float3 border = smoothstep(0., .1, erosion) - smoothstep(.1, 1., erosion);
    
    float3 erosionBlend = (1. - erosion) * foreground + erosion * background;

    float3 leadcol = borderColorGradient._11_12_13;
    float3 trailcol = borderColorGradient._21_22_23;
    float3 borderGradient = lerp(leadcol, trailcol, smoothstep(0.8, 1., border)) * 2.;
    
    return erosionBlend += border * borderGradient;
}

/////////////////////////////////////////////////////////////////
//--- Outputs pixel data dependent on higtMap and driver, 0..1
//---                                                                            
//float3 AlphaErode(float heightMap, float3 background, float3 foreground, float driver)
//{
//    float3 erosion = smoothstep(driver - borderThikness, driver, heightMap);
    
//    float3 border = smoothstep(0., .1, erosion) - smoothstep(.1, 1., erosion);
    
//    float3 erosionBlend = (1. - erosion) * foreground + erosion * background;

//    float3 leadcol = borderColorGradient._11_12_13;
//    float3 trailcol = borderColorGradient._21_22_23;
//    float3 borderGradient = lerp(leadcol, trailcol, smoothstep(0.8, 1., border)) * 2.;
    
//    return erosionBlend += border * borderGradient;
//}