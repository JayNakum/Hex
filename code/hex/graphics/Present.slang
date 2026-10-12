#include "FramePass.slangh"

struct BlitParams
{
    float sourceWidth;
    float sourceHeight;
    float targetWidth;
    float targetHeight;
};

ConstantBuffer<PassConstants> passConstants;
Texture2D<float4> srcTexture;
SamplerState srcSampler;
ConstantBuffer<BlitParams> blitParams;

struct VertexOutput
{
    float4 sv_position : SV_Position;
};

[shader("vertex")]
VertexOutput vertMain(uint vid : SV_VertexID)
{
    VertexOutput output;
    float2 pos = float2((vid << 1) & 2, vid & 2) * 2.0 - 1.0;
    output.sv_position = float4(pos, 0.0, 1.0);
    return output;
}

float3 srgb_decode(const float3 color)
{
    const float3 lo = color / 12.92f;
    const float3 hi = pow((color + 0.055f) / 1.055f, 2.4f);
    return select(color <= 0.04045f, lo, hi);
}

[shader("fragment")]
float4 fragMain(VertexOutput input, uniform FrameConstants frameConstants) : SV_Target
{
    const float2 targetSize = float2(blitParams.targetWidth, blitParams.targetHeight);
    const float2 sourceSize = float2(blitParams.sourceWidth, blitParams.sourceHeight);

    const float2 screenUV = input.sv_position.xy / targetSize;

    const float sourceAspect = sourceSize.x / sourceSize.y;
    const float targetAspect = targetSize.x / targetSize.y;

    const float2 scale = (targetAspect > sourceAspect)
        ? float2(sourceAspect / targetAspect, 1.0)
        : float2(1.0, targetAspect / sourceAspect);

    const float2 uv = (screenUV - 0.5) / scale + 0.5;

    if (any(uv < 0.0) || any(uv > 1.0))
    {
        return float4(0.0, 0.0, 0.0, 1.0);
    }

    const float4 color = srcTexture.Sample(srcSampler, uv);
    return float4(srgb_decode(color.rgb), color.a);
}
