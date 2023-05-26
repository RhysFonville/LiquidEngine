#include "BaseShader.hlsl"

cbuffer PerFrameVSCB : register(b0) {
	matrix WVP;
}

//cbuffer PerObjectVSCB : register(b1) {
//	matrix transform;
//}
