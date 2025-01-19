#include "Includes/BaseShader.hlsli"

cbuffer TimeStruct : register(b5) {
    float time;
};

float4 main(PS_INPUT ps_in) : SV_TARGET {
    return float4(0.0f, 1.0f, 1.0f, 1.0f);
    //return calculate_lit(ps_in);
}
