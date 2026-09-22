Texture2D<float4> srcTexture;

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

[shader("fragment")]
float4 fragMain(VertexOutput input) : SV_Target
{
    int2 pixelCoord = int2(input.sv_position.xy);
    return srcTexture.Load(int3(pixelCoord, 0));
}
