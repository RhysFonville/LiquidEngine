#pragma once

#include <d3d12.h>

//static bool operator==(const D3D12_RECT &lhs, const D3D12_RECT &rhs) noexcept {
//	return (lhs.left == rhs.left &&
//		lhs.right == rhs.right &&
//		lhs.top == rhs.top &&
//		lhs.bottom == rhs.bottom);
//}

static bool operator==(const D3D12_VERTEX_BUFFER_VIEW& lhs, const D3D12_VERTEX_BUFFER_VIEW& rhs) noexcept {
	return (
		lhs.BufferLocation == rhs.BufferLocation &&
		lhs.SizeInBytes == rhs.SizeInBytes &&
		lhs.StrideInBytes && rhs.StrideInBytes
	);
}

static bool operator==(const D3D12_STREAM_OUTPUT_DESC& lhs, const D3D12_STREAM_OUTPUT_DESC& rhs) noexcept {
	return (
		lhs.NumEntries == rhs.NumEntries &&
		lhs.NumStrides == rhs.NumStrides &&
		lhs.pBufferStrides == rhs.pBufferStrides &&
		lhs.pSODeclaration == rhs.pSODeclaration &&
		lhs.RasterizedStream == rhs.RasterizedStream
	);
}

static bool operator==(const D3D12_RASTERIZER_DESC& lhs, const D3D12_RASTERIZER_DESC& rhs) noexcept {
	return (
		lhs.AntialiasedLineEnable == rhs.AntialiasedLineEnable &&
		lhs.ConservativeRaster == rhs.ConservativeRaster &&
		lhs.CullMode == rhs.CullMode &&
		lhs.DepthBias == rhs.DepthBias &&
		lhs.DepthBiasClamp == rhs.DepthBiasClamp &&
		lhs.DepthClipEnable == rhs.DepthClipEnable &&
		lhs.FillMode == rhs.FillMode &&
		lhs.ForcedSampleCount == rhs.ForcedSampleCount &&
		lhs.FrontCounterClockwise == rhs.FrontCounterClockwise &&
		lhs.MultisampleEnable == rhs.MultisampleEnable &&
		lhs.SlopeScaledDepthBias == rhs.SlopeScaledDepthBias
	);
}

static bool operator==(const D3D12_ROOT_SIGNATURE_DESC& lhs, const D3D12_ROOT_SIGNATURE_DESC& rhs) noexcept {
	return (
		lhs.Flags == rhs.Flags &&
		lhs.NumParameters == rhs.NumParameters &&
		lhs.NumStaticSamplers == rhs.NumStaticSamplers &&
		lhs.pParameters == rhs.pParameters &&
		lhs.pStaticSamplers == rhs.pStaticSamplers
	);
}

static bool operator==(const D3D12_VERSIONED_ROOT_SIGNATURE_DESC& lhs, const D3D12_VERSIONED_ROOT_SIGNATURE_DESC& rhs) noexcept {
	if (lhs.Version == rhs.Version) {
		if (lhs.Version == D3D_ROOT_SIGNATURE_VERSION_1_0) {
			return (
				lhs.Desc_1_0.Flags ==				rhs.Desc_1_0.Flags &&
				lhs.Desc_1_0.NumParameters ==		rhs.Desc_1_0.NumParameters &&
				lhs.Desc_1_0.NumStaticSamplers ==	rhs.Desc_1_0.NumStaticSamplers &&
				lhs.Desc_1_0.pParameters ==			rhs.Desc_1_0.pParameters &&
				lhs.Desc_1_0.pStaticSamplers ==		rhs.Desc_1_0.pStaticSamplers
			);
		} else {
			return (
				lhs.Desc_1_1.Flags ==				rhs.Desc_1_1.Flags &&
				lhs.Desc_1_1.NumParameters ==		rhs.Desc_1_1.NumParameters &&
				lhs.Desc_1_1.NumStaticSamplers ==	rhs.Desc_1_1.NumStaticSamplers &&
				lhs.Desc_1_1.pParameters ==			rhs.Desc_1_1.pParameters &&
				lhs.Desc_1_1.pStaticSamplers ==		rhs.Desc_1_1.pStaticSamplers
			);
		}
	} else {
		return false;
	}
}

static bool operator==(const D3D12_DESCRIPTOR_RANGE& lhs, const D3D12_DESCRIPTOR_RANGE& rhs) noexcept {
	return (
		lhs.BaseShaderRegister == rhs.BaseShaderRegister &&
		lhs.NumDescriptors == rhs.NumDescriptors &&
		lhs.OffsetInDescriptorsFromTableStart == rhs.OffsetInDescriptorsFromTableStart &&
		lhs.RangeType == rhs.RangeType &&
		lhs.RegisterSpace == rhs.RegisterSpace
	);
}

static bool operator==(const D3D12_DESCRIPTOR_RANGE1& lhs, const D3D12_DESCRIPTOR_RANGE1& rhs) noexcept {
	return (
		lhs.BaseShaderRegister == rhs.BaseShaderRegister &&
		lhs.NumDescriptors == rhs.NumDescriptors &&
		lhs.OffsetInDescriptorsFromTableStart == rhs.OffsetInDescriptorsFromTableStart &&
		lhs.RangeType == rhs.RangeType &&
		lhs.RegisterSpace == rhs.RegisterSpace &&
		lhs.Flags == rhs.Flags
	);
}


static bool operator==(const D3D12_ROOT_DESCRIPTOR_TABLE& lhs, const D3D12_ROOT_DESCRIPTOR_TABLE& rhs) noexcept {
	return (lhs.NumDescriptorRanges == rhs.NumDescriptorRanges &&
		lhs.pDescriptorRanges == rhs.pDescriptorRanges);
}

static bool operator==(const D3D12_ROOT_DESCRIPTOR_TABLE1& lhs, const D3D12_ROOT_DESCRIPTOR_TABLE1& rhs) noexcept {
	return (lhs.NumDescriptorRanges == rhs.NumDescriptorRanges &&
		lhs.pDescriptorRanges == rhs.pDescriptorRanges);
}
