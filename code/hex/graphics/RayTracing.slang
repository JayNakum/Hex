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
};

ConstantBuffer<PassConstants> passConstants;
ConstantBuffer<RayTracingParams> rayTracingParams;
RWTexture2D<float4> outputImage;

static const uint SPHERE_COUNT = 2;
static const Sphere SCENE_SPHERES[SPHERE_COUNT] = {
    { float3(0.0f, 0.0f, -1.0f), 0.5f },
    { float3(0.0f, -100.5f, -1.0f), 100.0f }
};
bool hit(in const Ray ray, in Interval rayT, out HitRecord hitRecord)
{
    HitRecord tempRec;
    bool hitAnything = false;

    float closestSoFar = rayT.max;
    [unroll]
    for (uint i = 0; i < SPHERE_COUNT; ++i)
    {
        if (hit(SCENE_SPHERES[i], ray, {rayT.min, closestSoFar}, tempRec))
        {
            hitAnything = true;
            closestSoFar = tempRec.t;
            hitRecord = tempRec;
        }
    }

    return hitAnything;
}

float3 ray_color(in const Ray ray)
{
    HitRecord hitRecord;
    const bool didHit = hit(ray, { 0.0f, INFINITY }, hitRecord);
    if (didHit)
    {
        return 0.5f * (hitRecord.normal + 1.0f);
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

    const float3 pixelCenter = rayTracingParams.pixel00Loc + float(dtid.x) * rayTracingParams.pixelDeltaU + float(dtid.y) * rayTracingParams.pixelDeltaV;

    Ray ray;
    ray.origin = rayTracingParams.cameraCenter;
    ray.direction = pixelCenter - rayTracingParams.cameraCenter;

    const float3 pixelColor = ray_color(ray);

    outputImage[dtid.xy] = float4(pixelColor, 1.0f);
}
