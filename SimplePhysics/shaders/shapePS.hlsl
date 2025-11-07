
cbuffer PerRenderer: register(b0) {
    row_major float4x4 vp;
    float3  worldCameraPosition;
}

struct PSInput
{
    float4 position : SV_POSITION; // 클립 공간 위치
    noperspective float3 worldPosition : TEXCOORD0;
};

/*float4 main(PSInput i) : SV_Target
{
    // 화면 x/y에 대한 월드좌표의 미분 → 접선벡터 2개
    float3 dpdx = ddx(i.worldPosition);
    float3 dpdy = ddy(i.worldPosition);

    // 기하 노멀 (면 노멀)
    // 교차곱 순서는 임의지만, 뒤집힘 방지를 위해 면 앞/뒤에 따라 보정
    float3 N = normalize(cross(dpdx, dpdy));
    float3 lightDir = normalize(float3(0, -1, 0));
    float3 L = normalize(-lightDir); // 표면→광원 (lightDir가 씬으로 향한다 가정)
    float3 V = normalize(worldCameraPosition - i.worldPosition);

    // 램버트 + 블린-폰 스펙룰러
    float  NdotL = saturate(dot(N, L));
    float3 H = normalize(L + V);
    float  spec = pow(saturate(dot(N, H)), 20) * smoothstep(0.001, 0.004, NdotL);

    //float3 color = gBaseColor.rgb * (gAmbient + NdotL) + spec.xxx;
    float3 color = float3(1.0, 1.0, 1.0) * (0.1 + NdotL) + spec.xxx;
    //float3 color = float3(1.0, 1.0, 1.0) * (0.1 + NdotL);
    return float4(color, 1.0);
}
*/




float3 fresnel(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float ggx(float3 N, float3 H, float alpha)
{
    float a2 = alpha * alpha;
    float NdotH = max(dot(N, H), 0.0);
    float d = (NdotH * NdotH) * (a2 - 1.0) + 1.0;
    return a2 / max(3.14159265 * d * d, 1e-6);
}

float gs(float3 N, float3 V, float3 L, float alpha)
{
    // Schlick-GGX
    float k = (alpha + 1.0);
    k = (k * k) * 0.125; // (alpha+1)^2 / 8
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float gv = NdotV / max(NdotV * (1.0 - k) + k, 1e-6);
    float gl = NdotL / max(NdotL * (1.0 - k) + k, 1e-6);
    return gv * gl;
}

float step_quantize(float x, float steps)
{
    // 0..1 을 steps 구간으로
    return floor(saturate(x) * steps) / max(steps - 1.0, 1.0);
}

float4 main(PSInput i) : SV_Target
{
    //float3   Albedo = float3(0.5, 0.5, 0.5);      // 0~1
    float3   Albedo = float3(1.0, 1.0, 1.0);      // 0~1
    float    Metalness = 0;   // 0~1
    float    Roughness = 0.8;   // 0~1 (GGX alpha^2로 변환)

    // 기본 벡터
    //float3 N = normalize(i.WorldN);
    //if (!isFrontFace) N = -N;
    float3 dpdx = ddx(i.worldPosition);
    float3 dpdy = ddy(i.worldPosition);
    float3 N = normalize(cross(dpdx, dpdy));

    float3 V = normalize(worldCameraPosition - i.worldPosition);
    float3 lightDir = normalize(float3(0, -1, 0));
    float3 L = normalize(-lightDir);      // 표면→광원
    float3 H = normalize(L + V);

    // PBR 파라미터
    float  alpha = max(Roughness * Roughness, 1e-3); // GGX용
    float3 F0 = lerp(float3(0.04,0.04,0.04), Albedo, Metalness);

    float  NdotL = max(dot(N,L), 0.0);
    float  NdotV = max(dot(N,V), 0.0);

    // BRDF
    float  D = ggx(N,H,alpha);
    float  G = gs(N,V,L,alpha);
    float3 F = fresnel(max(dot(H,V),0.0), F0);

    float3 spec = (D * G) * F / max(4.0 * NdotV * NdotL, 1e-6) * smoothstep(0.001, 0.004, NdotL);
    float3 kd = (1.0 - F) * (1.0 - Metalness);
    float3 diffuse = kd * Albedo / 3.14159265;

    float3 lit = (diffuse + spec) * NdotL;

    float3 color = (0.1 * Albedo + lit) * 3;


    return float4(saturate(color), 1.0);
}