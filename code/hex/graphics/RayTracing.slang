#include "FramePass.slangh"

#include "utility.slangh"
#include "Ray.slangh"
#include "RayCollision.slangh"

struct RayTracingParams
{
    float3 cameraCenter;
    const uint _pad0;

    float3 pixel00Loc;
    const uint _pad1;

    float3 pixelDeltaU;
    const uint _pad2;

    float3 pixelDeltaV;
    const uint _pad3;

    uint32_t randomSeed;
    uint32_t samplesPerPixel;

    float quadZ;

    const uint _pad5;
};

ConstantBuffer<PassConstants> passConstants;
ConstantBuffer<RayTracingParams> rayTracingParams;
RWTexture2D<float4> outputImage;

static const uint SPHERE_COUNT = 2;
static const Sphere SCENE_SPHERES[SPHERE_COUNT] = {
    { float3(0.0f, 0.0f, -1.0f), 0.5f },
    { float3(0.0f, -100.5f, -1.0f), 100.0f }
};
bool hit(in const Ray ray, in Interval rayT, out HitRecord hitRecord, inout float2 hitUV)
{
    HitRecord tempRec;
    bool hitAnything = false;

    Quad checkerQuad = make_quad(
        float3(-0.5f, -0.5f, -rayTracingParams.quadZ),
        float3(1.0f, 0.0f, 0.0f),
        float3(0.0f, 1.0f, 0.0f)
    );

    return hit(checkerQuad, ray, rayT, hitRecord, hitUV);
}

float3 checkerboard_8x8(in const float2 uv)
{
    const int2 tile = int2(floor(uv * 8.0f));
    const bool isEven = ((tile.x + tile.y) & 1) == 0;
    return isEven ? float3(1.0f, 1.0f, 1.0f) : float3(0.0f, 0.0f, 0.0f);
}

float2 sample_square(inout uint randomNumberState)
{
    return float2(
        random_float(randomNumberState) - 0.5f,
        random_float(randomNumberState) - 0.5f
    );
}

Ray ray_get(in const uint2 pixel, inout uint randomNumberState)
{
    const float2 offset = (rayTracingParams.samplesPerPixel <= 1)
                              ? float2(0.0f, 0.0f)
                              : sample_square(randomNumberState);

    const float3 pixelSample = rayTracingParams.pixel00Loc.xyz 
                             + (float(pixel.x) + offset.x) * rayTracingParams.pixelDeltaU.xyz 
                             + (float(pixel.y) + offset.y) * rayTracingParams.pixelDeltaV.xyz;

    Ray r;
    r.origin = rayTracingParams.cameraCenter.xyz;
    r.direction = pixelSample - r.origin;
    return r;
}

float3 ray_color(in const Ray ray)
{
    HitRecord hitRecord;
    float2 uv;
    const bool didHit = hit(ray, { 0.0f, INFINITY }, hitRecord, uv);
    if (didHit)
    {
        return checkerboard_8x8(uv);
    }

    const float3 unit_direction = normalize(ray.direction);
    const float a = 0.5f * (unit_direction.y + 1.0f);
    return (1.0f - a) * float3(1.0f, 1.0f, 1.0f) + a * float3(0.5f, 0.7f, 1.0f);
}

[shader("compute")]
[numthreads(32, 32, 1)]
void computeMain(
    uint3 dtid : SV_DispatchThreadID,
    uint3 gtid : SV_GroupThreadID,
    uint3 gid : SV_GroupID,
    uint idx : SV_GroupIndex,
    uniform FrameConstants frameConstants
)
{
    if (dtid.x >= frameConstants.size.x || dtid.y >= frameConstants.size.y)
    {
        return;
    }

    uint randomNumberState = dtid.x + dtid.y * 65536u;
    randomNumberState ^= rayTracingParams.randomSeed * 2654435761u; // Knuth's multiplicative hash
    pcg_hash(randomNumberState);

    float3 pixelColor = float3(0.0f);
    [loop]
    for (uint sample = 0; sample < rayTracingParams.samplesPerPixel; ++sample)
    {
        Ray ray = ray_get(dtid.xy, randomNumberState);
        pixelColor += ray_color(ray);
    }

    pixelColor /= max(1.0f, float(rayTracingParams.samplesPerPixel));
    outputImage[dtid.xy] = float4(saturate(pixelColor), 1.0f);
}
