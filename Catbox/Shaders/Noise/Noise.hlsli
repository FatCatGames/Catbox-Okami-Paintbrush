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