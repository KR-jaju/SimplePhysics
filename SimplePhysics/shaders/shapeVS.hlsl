
cbuffer PerRenderer: register(b0) {
    row_major float4x4 vp;
    float3  worldCameraPosition;
}

cbuffer PerRenderable : register(b1) {
    row_major float4x4 trs;
}

struct VSInput
{
    float3 position : POSITION;   // 정점 위치
    //float3 normal   : NORMAL;     // 정점 노멀 (조명용, 선택)
};

struct PSInput
{
    float4 position : SV_POSITION; // 클립 공간 위치
    noperspective float3 worldPosition : TEXCOORD0;
    //float3 normal   : NORMAL;      // 월드 노멀 (조명 계산용)
};

PSInput main(VSInput input)
{
    PSInput output;
    float4 worldPosition = mul(float4(input.position, 1.0f), trs);
    float4 clipPosition = mul(worldPosition, vp);

    //output.position = worldPosition;
    output.worldPosition = worldPosition;
    output.position = clipPosition;
	return output;
}