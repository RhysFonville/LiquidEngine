#include "BaseLitPS.hlsli"

cbuffer TimeStruct : register(b5) {
    float time;
};

float4 main(PS_INPUT ps_in) : SV_TARGET {
    //return calculate_lit(ps_in);
    return float4(float1(1.0f).xxxx);
}
