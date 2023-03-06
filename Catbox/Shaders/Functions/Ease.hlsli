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
    const float c3= c1+1;

    return c3 * x * x * x - c1 * x * x;
}

/////////////////////////////////////////////////////////////
//--- Outputs ease out Back of x
float easeOutBack(float x)
{
    // 10% bounce, is this better -> 1.7015401988668 ?
    const float c1 = 1.70158;
    const float c3= c1+1;

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
           : pow(2, -10 * x) *sin((x * 10 - 0.75) * c4) + 1;
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