#include "FramePass.slangh"

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

struct Ray
{
    float3 origin;
    float3 direction;
};

float3 ray_at(const Ray ray, const float t)
{
    return ray.origin + t * ray.direction;
}

float3 ray_color(const Ray ray)
{
    return float3(0.0f);
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
