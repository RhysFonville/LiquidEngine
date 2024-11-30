// https://www.3dgep.com/learning-directx-12-4/

#define GenerateMips_RootSignature \
	"RootFlags(0), " \
	"RootConstants(b0, num32BitConstants = 6), " \
	"DescriptorTable( SRV(t0, numDescriptors = 1) )," \
	"DescriptorTable( UAV(u0, numDescriptors = 4) )," \
	"StaticSampler(s0," \
		"addressU = TEXTURE_ADDRESS_CLAMP," \
		"addressV = TEXTURE_ADDRESS_CLAMP," \
		"addressW = TEXTURE_ADDRESS_CLAMP," \
		"filter = FILTER_MIN_MAG_MIP_LINEAR)"

#define GROUP_SIZE 8

#define WIDTH_HEIGHT_EVEN 0     // Both the width and the height of the texture are even.
#define WIDTH_ODD_HEIGHT_EVEN 1 // The texture width is odd and the height is even.
#define WIDTH_EVEN_HEIGHT_ODD 2 // The texture width is even and the height is odd.
#define WIDTH_HEIGHT_ODD 3      // Both the width and height of the texture are odd.

struct ComputeShaderInput
{
	uint3 group_id : SV_GroupID; // 3D index of the thread group in the dispatch.
	uint3 group_thread_id : SV_GroupThreadID; // 3D index of local thread ID in a thread group.
	uint3 dispatch_thread_id : SV_DispatchThreadID; // 3D index of global thread ID in the dispatch.
	uint group_index : SV_GroupIndex; // Flattened local index of the thread within a thread group.
};

cbuffer GenerateMipsCB : register(b0)
{
	uint src_mip_level; // Texture level of source mip
	uint num_mip_levels; // Number of OutMips to write: [1-4]
	uint src_dimension; // Width and height of the source texture are even or odd.
	bool is_srgb; // Must apply gamma correction to sRGB textures.
	float2 texel_size; // 1.0 / OutMip1.Dimensions
}

// Source mip map.
Texture2D<float4> src_mip : register(t0);

// Write up to 4 mip map levels.
RWTexture2D<float4> out_mip1 : register(u0);
RWTexture2D<float4> out_mip2 : register(u1);
RWTexture2D<float4> out_mip3 : register(u2);
RWTexture2D<float4> out_mip4 : register(u3);

// Linear clamp sampler.
SamplerState linear_clamp_sampler : register(s0);

// The reason for separating channels is to reduce bank conflicts in the
// local data memory controller.  A large stride will cause more threads
// to collide on the same memory bank.
groupshared float gs_R[64];
groupshared float gs_G[64];
groupshared float gs_B[64];
groupshared float gs_A[64];

void store_color(uint index, float4 color) {
	gs_R[index] = color.r;
	gs_G[index] = color.g;
	gs_B[index] = color.b;
	gs_A[index] = color.a;
}

float4 load_color(uint index) {
	return float4(gs_R[index], gs_G[index], gs_B[index], gs_A[index]);
}

// https://en.wikipedia.org/wiki/SRGB#The_reverse_transformation
float3 srgb_to_linear(float3 x) {
	return x < 0.04045f ? x / 12.92 : pow((x + 0.055) / 1.055, 2.4);
}

// https://en.wikipedia.org/wiki/SRGB#The_forward_transformation_(CIE_XYZ_to_sRGB)
float3 linear_to_srgb(float3 x) {
	return x < 0.0031308 ? 12.92 * x : 1.055 * pow(abs(x), 1.0 / 2.4) - 0.055;
}

// Convert linear color to sRGB before storing if the original source is 
// an sRGB texture.
float4 pack_color(float4 x) {
	if (is_srgb) {
		return float4(linear_to_srgb(x.rgb), x.a);
	} else {
		return x;
	}
}

//[RootSignature(GenerateMips_RootSignature)]
[numthreads(GROUP_SIZE, GROUP_SIZE, 1)]
void main(ComputeShaderInput cs_in) {
	float4 src1 = (float4)0;
	
	// One bilinear sample is insufficient when scaling down by more than 2x.
	// You will slightly undersample in the case where the source dimension
	// is odd.  This is why it's a really good idea to only generate mips on
	// power-of-two sized textures.  Trying to handle the undersampling case
	// will force this shader to be slower and more complicated as it will
	// have to take more source texture samples.
	
	// Determine the path to use based on the dimension of the 
	// source texture.
	switch (src_dimension) {
		case WIDTH_HEIGHT_EVEN: {
			float2 uv = texel_size * (cs_in.dispatch_thread_id.xy + 0.5);
			src1 = src_mip.SampleLevel(linear_clamp_sampler, uv, src_mip_level);
		} break;
		case WIDTH_ODD_HEIGHT_EVEN: {
			// > 2:1 in X dimension
			// Use 2 bilinear samples to guarantee we don't undersample when downsizing by more than 2x
			// horizontally.
			float2 uv1 = texel_size * (cs_in.dispatch_thread_id.xy + float2(0.25, 0.5));
			float2 offset = texel_size * float2(0.5, 0.0);

			src1 = 0.5 * (src_mip.SampleLevel(linear_clamp_sampler, uv1, src_mip_level) +
						src_mip.SampleLevel(linear_clamp_sampler, uv1 + offset, src_mip_level));
		} break;
		case WIDTH_EVEN_HEIGHT_ODD: {
			// > 2:1 in Y dimension
			// Use 2 bilinear samples to guarantee we don't undersample when downsizing by more than 2x
			// vertically.
			float2 uv1 = texel_size * (cs_in.dispatch_thread_id.xy + float2(0.5, 0.25));
			float2 offset = texel_size * float2(0.0, 0.5);

			src1 = 0.5 * (src_mip.SampleLevel(linear_clamp_sampler, uv1, src_mip_level) +
						src_mip.SampleLevel(linear_clamp_sampler, uv1 + offset, src_mip_level));
		} break;
		case WIDTH_HEIGHT_ODD: {
			// > 2:1 in in both dimensions
			// Use 4 bilinear samples to guarantee we don't undersample when downsizing by more than 2x
			// in both directions.
			float2 uv1 = texel_size * (cs_in.dispatch_thread_id.xy + float2(0.25, 0.25));
			float2 offset = texel_size * 0.5;

			src1 = src_mip.SampleLevel(linear_clamp_sampler, uv1, src_mip_level);
			src1 += src_mip.SampleLevel(linear_clamp_sampler, uv1 + float2(offset.x, 0.0), src_mip_level);
			src1 += src_mip.SampleLevel(linear_clamp_sampler, uv1 + float2(0.0, offset.y), src_mip_level);
			src1 += src_mip.SampleLevel(linear_clamp_sampler, uv1 + float2(offset.x, offset.y), src_mip_level);
			src1 *= 0.25;
		} break;
		
		out_mip1[cs_in.dispatch_thread_id.xy] = pack_color(src1);
		
		// A scalar (constant) branch can exit all threads coherently.
		if (num_mip_levels == 1)
			return;
 
		// Without lane swizzle operations, the only way to share data with other
		// threads is through LDS.
		store_color(cs_in.group_index, src1);
 
		// This guarantees all LDS writes are complete and that all threads have
		// executed all instructions so far (and therefore have issued their LDS
		// write instructions.)
		GroupMemoryBarrierWithGroupSync();
		
		// With low three bits for X and high three bits for Y, this bit mask
		// (binary: 001001) checks that X and Y are even.
		if ((cs_in.group_index & 0x9) == 0) {
			float4 src2 = load_color(cs_in.group_index + 0x01);
			float4 src3 = load_color(cs_in.group_index + 0x08);
			float4 src4 = load_color(cs_in.group_index + 0x09);
			src1 = 0.25 * (src1 + src2 + src3 + src4);

			out_mip2[cs_in.dispatch_thread_id.xy / 2] = pack_color(src1);
			store_color(cs_in.group_index, src1);
		}
		
		if (num_mip_levels == 2)
			return;

		GroupMemoryBarrierWithGroupSync();
		
		// This bit mask (binary: 011011) checks that X and Y are multiples of four.
		if ((cs_in.group_index & 0x1B) == 0) {
			float4 src2 = load_color(cs_in.group_index + 0x02);
			float4 src3 = load_color(cs_in.group_index + 0x10);
			float4 src4 = load_color(cs_in.group_index + 0x12);
			src1 = 0.25 * (src1 + src2 + src3 + src4);

			out_mip3[cs_in.dispatch_thread_id.xy / 4] = pack_color(src1);
			store_color(cs_in.group_index, src1);
		}
		
		if (num_mip_levels == 3)
			return;

		GroupMemoryBarrierWithGroupSync();
		
		// This bit mask would be 111111 (X & Y multiples of 8), but only one
		// thread fits that criteria.
		if (cs_in.group_index == 0) {
			float4 src2 = load_color(cs_in.group_index + 0x04);
			float4 src3 = load_color(cs_in.group_index + 0x20);
			float4 src4 = load_color(cs_in.group_index + 0x24);
			src1 = 0.25 * (src1 + src2 + src3 + src4);

			out_mip4[cs_in.dispatch_thread_id.xy / 8] = pack_color(src1);
		}
	}
}