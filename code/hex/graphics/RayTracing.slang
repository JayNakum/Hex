#include "FramePass.slangh"

struct RayTracingParams
{
};

ConstantBuffer<PassConstants> passConstants;
ConstantBuffer<RayTracingParams> rayTracingParams;
RWTexture2D<float4> outputImage;

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
    if (dtid.x >= frameConstants.width || dtid.y >= frameConstants.height)
    {
        return;
    }

    const float2 uv = float2(dtid.xy) / float2(frameConstants.width, frameConstants.height);
    outputImage[dtid.xy] = float4(uv, 0.0f, 1.0f);
}
