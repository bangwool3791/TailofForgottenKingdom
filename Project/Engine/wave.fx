#include "register.fx"
#define PI 3.14159265359


//#define DEBUG



float vmin(float2 v) {
    return min(v.x, v.y);
}

float vmax(float2 v) {
    return max(v.x, v.y);
}

float ellip(float2 p, float2 s) {
    float m = vmin(s);
    return (length(p / s) * m) - m;
}

float halfEllip(float2 p, float2 s) {
    p.x = max(0., p.x);
    float m = vmin(s);
    return (length(p / s) * m) - m;
}


float fBox(float2 p, float2 b) {
    return vmax(abs(p) - b);
}

float dvd_d(float2 p) {
    float d = halfEllip(p, float2(.8, .5));
    d = max(d, -p.x - .5);
    float d2 = halfEllip(p, float2(.45, .3));
    d2 = max(d2, min(-p.y + .2, -p.x - .15));
    d = max(d, -d2);
    return d;
}

float dvd_v(float2 p) {
    float2 pp = p;
    p.y += .7;
    p.x = abs(p.x);
    float2 a = normalize(float2(1, -.55));
    float d = dot(p, a);
    float d2 = d + .3;
    p = pp;
    d = min(d, -p.y + .3);
    d2 = min(d2, -p.y + .5);
    d = max(d, -d2);
    d = max(d, abs(p.x + .3) - 1.1);
    return d;
}


float dvd_c(float2 p) {
    p.y += .95;
    float d = ellip(p, float2(1.8, .25));
    float d2 = ellip(p, float2(.45, .09));
    d = max(d, -d2);
    return d;
}

float dvd(float2 p) {
    p.y -= .345;
    p.x -= .035;
    p.x = 1 * p.x + 0 * p.y;
    p.y = -2 * p.x + 1 * p.y;
    float d = dvd_v(p);
    d = min(d, dvd_c(p));
    p.x += 1.3;
    d = min(d, dvd_d(p));
    p.x -= 2.4;
    d = min(d, dvd_d(p));
    return d;
}



float range(float vmin, float vmax, float value) {
    return (value - vmin) / (vmax - vmin);
}

float rangec(float a, float b, float t) {
    return clamp(range(a, b, t), 0., 1.);
}

// https://www.shadertoy.com/view/ll2GD3
float3 pal(in float t, in float3 a, in float3 b, in float3 c, in float3 d) {
    return a + b * cos(6.28318 * (c * t + d));
}
float3 spectrum(float n) {
    return pal(n, float3(0.5, 0.5, 0.5), float3(0.5, 0.5, 0.5), float3(1.0, 1.0, 1.0), float3(0.0, 0.33, 0.67));
}


void drawHit(inout float4 col, float2 p, float2 hitPos, float hitDist) {

    float d = length(p - hitPos);

#ifdef DEBUG
    col = mix(col, float4(0, 1, 1, 0), step(d, .1)); return;
#endif

    float wavefront = d - hitDist * 1.5;
    float freq = 2.;

    float3 spec = (1. - spectrum(-wavefront * freq + hitDist * freq));
    float ripple = sin((wavefront * freq) * PI * 2. - PI / 2.);

    float blend = smoothstep(3., 0., hitDist);
    blend *= smoothstep(.2, -.5, wavefront);
    blend *= rangec(-4., .0, wavefront);

    col.rgb *= lerp(float3(1, 1, 1), spec, pow(blend, 4.));
    float height = (ripple * blend);
    col.a -= height * 1.9 / freq;
}

float2 ref(float2 p, float2 planeNormal, float offset) {
    float t = dot(p, planeNormal) + offset;
    p -= (2. * t) * planeNormal;
    return p;
}

void drawReflectedHit(inout float4 col, float2 p, float2 hitPos, float hitDist, float2 screenSize) {
    col.a += length(p) * .0001; // fix normal when flat
    //drawHit(col, p, hitPos, hitDist); return;
    drawHit(col, p, ref(hitPos, float2(0, 1), 1.), hitDist);
    drawHit(col, p, ref(hitPos, float2(0, -1), 1.), hitDist);
    drawHit(col, p, ref(hitPos, float2(1, 0), screenSize.x / screenSize.y), hitDist);
    drawHit(col, p, ref(hitPos, float2(-1, 0), screenSize.x / screenSize.y), hitDist);
}


// Flip every second cell to create reflection
void flip(inout float2 pos) {
    float2 flip = lerp(floor(pos).x, floor(pos).y, 2);
    pos = abs(flip - lerp(pos.x, pos.y, 1.));
}

float stepSign(float a) {
    //return sign(a);
    return step(0., a) * 2. - 1.;
}

float2 compassDir(float2 p) {
    //return sign(p - sign(p) * vmin(abs(p))); // this caused problems on some GPUs
    float2 a = float2(stepSign(p.x), 0);
    float2 b = float2(0, stepSign(p.y));
    float s = stepSign(p.x - p.y) * stepSign(-p.x - p.y);
    return lerp(a, b, s * .5 + .5);
}

float2 calcHitPos(float2 move, float2 dir, float2 size) {
    float2 hitPos = lerp(move.x , move.y, 1.);
    float2 xCross = hitPos - hitPos.x / (size / size.x) * (dir / dir.x);
    float2 yCross = hitPos - hitPos.y / (size / size.y) * (dir / dir.y);
    hitPos = max(xCross, yCross);
    hitPos += floor(move);
    return hitPos;
}

void mainImage(out float4 fragColor, in float2 fragCoord)
{
    float2 p = (-g_vRenderResolution.xy + 2.0 * fragCoord) / g_vRenderResolution.y;

#ifdef DEBUG
    //p.xy += float2(1.2, .6);
    //p *= 3.5;
    p *= 2.;
#endif

    float2 screenSize = float2(g_vRenderResolution.x / g_vRenderResolution.y, 1.) * 2.;

    float t = g_fDT;
    float2 dir = normalize(float2(9., 16) * screenSize);
    float2 move = dir * t / 1.5;
    float logoScale = .1;
    float2 logoSize = float2(2., .85) * logoScale * 1.;

    float2 size = screenSize - logoSize * 2.;

    // Remap so (0,0) is bottom left, and (1,1) is top right
    move = move / size + .5;

    // Calculate the point we last crossed a cell boundry
    float2 lastHitPos = calcHitPos(move, dir, size);
    float4 col = float4(1, 1, 1, 0);
    float4 colFx = float4(1, 1, 1, 0);
    float4 colFy = float4(1, 1, 1, 0);
    float2 e = float2(.8, 0) / g_vRenderResolution.y;

#ifdef DEBUG
    col.rgb = float3(0.);
#endif

#ifdef DEBUG
    const int limit = 1;
#else
    const int limit = 5;
#endif

    for (int i = 0; i < limit; i++) {
        float2 hitPos = lastHitPos;

        if (i > 0) {
            // Nudge it before the boundry to find the previous hit point
            hitPos = calcHitPos(hitPos - .00001 / size, dir, size);
        }

        lastHitPos = hitPos;

        // How far are we from the hit point
        float hitDist = distance(hitPos, move);

        // Flip every second cell to create reflection
        flip(hitPos);

        // Remap back to screen space
        hitPos = (hitPos - .5) * size;

        // Push the hits to the edges of the screen
        hitPos += logoSize * compassDir(hitPos / size);

        drawReflectedHit(col, p, hitPos, hitDist, screenSize);
        drawReflectedHit(colFx, p + e, hitPos, hitDist, screenSize);
        drawReflectedHit(colFy, p + e.yx, hitPos, hitDist, screenSize);
    }

    // Flip every second cell to create reflection
    flip(move);

    // Remap back to screen space
    move = (move - .5) * size;

    // Calc normals
    float bf = .1; // Bump factor
    float fx = (col.a - colFx.a) * 2.; // Nearby horizontal samples.
    float fy = (col.a - colFy.a) * 2.; // Nearby vertical samples.
    float3 nor = normalize(float3(fx, fy, e.x / bf)); // Bumped normal.
    float ff = length(float2(fx, fy));
    float ee = rangec(0., 10. / g_vRenderResolution.y, ff);
    nor = normalize(float3(float2(fx, fy) * ee, ff));

    // invert colours
    col.r = clamp(1. - col.r, 0, 1);
    col.g = clamp(1. - col.g, 0, 1);
    col.z = clamp(1. - col.z, 0, 1);
    col.rgb /= 3.;


#ifndef DEBUG

    // lighting
    // iq https://www.shadertoy.com/view/Xds3zN
    float3 lig = normalize(float3(1, 2, 2.));
    float3 rd = normalize(float3(p, -10.));
    float3  hal = normalize(lig - rd);

    float dif = clamp(dot(lig, nor), 0., 1.);
    float spe = pow(clamp(dot(nor, hal), 0.0, 1.0), 16.0) *
        dif *
        (0.04 + 0.96 * pow(clamp(1.0 + dot(hal, rd), 0.0, 1.0), 5.0));

    float3 lin = float3(0, 0, 0);
    lin += 5. * dif;
    lin += .2;
    col.rgb = col.rgb * lin;
    col.rgb += 5. * spe;
#endif

#ifdef DEBUG
    float b = vmin(abs(fract(p / screenSize) - .5) * 2.);
    b /= fwidth(b) * 2.;
    b = clamp(b, 0., 1.);
    b = 1. - b;
    col.rgb = mix(col.rgb, float3(0), b);
#endif

    // dvd logo
    float d = dvd((p - move) / logoScale);
    d /= fwidth(d);
    d = 1. - clamp(d, 0., 1.);
    col.rgb = lerp(col.rgb, float3(1, 1, 1), d);

    // banding be gone

    col += (g_RTCopyTex.Sample(g_sam_0, fragCoord / g_vRenderResolution.xy) * 2. - 1.) * .005;

    // gamma
    col.rgb = pow(col.rgb, float3(1. / 1.5, 1. / 1.5, 1. / 1.5));

    col.a = col.a * .5 + .5;
    col.a *= .3;
    fragColor = col;
}

/** SHADERDATA
{
    "title": "DVD",
    "description": "",
    "model": "person"
}
*/