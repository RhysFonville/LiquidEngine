#include "SkyComponent.h"

SkyComponent::SkyComponent() : GraphicsComponent{Type::SkyComponent} { }

void SkyComponent::compile() {
	pipeline.input_layout = {
		{ "POSITION",	0,	DXGI_FORMAT_R32G32B32_FLOAT,	0,	0,								D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0,	DXGI_FORMAT_R32G32_FLOAT,		0,	D3D12_APPEND_ALIGNED_ELEMENT,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	0 },
	};

	pipeline.depth_stencil_desc = D3D12_DEPTH_STENCIL_DESC{};

	auto rdesc = pipeline.rasterizer.get_desc();
	rdesc.CullMode = D3D12_CULL_MODE_FRONT;
	pipeline.rasterizer.set_desc(rdesc);

	pipeline.vs = shader_storage->add_and_compile_shader("vs_6_0", "Graphics/Shaders/SkyVS.hlsl");
	pipeline.ps = shader_storage->add_and_compile_shader("ps_6_0", "Graphics/Shaders/SkyPS.hlsl");

	if (has_texture()) {
		albedo_texture.compile();
	}

	pipeline.input_assembler.add_command(std::make_shared<GraphicsPipelineIAAddMeshCommand>(std::make_shared<Mesh>("Shapes/skybox.obj")));
	pipeline.input_assembler.add_command(std::make_shared<GraphicsPipelineIASetInstancesCommand>(std::vector<Transform>{Transform{}}));
}

void SkyComponent::clean_up() {
	pipeline.clean_up();
}

bool SkyComponent::has_texture() const noexcept {
	return albedo_texture.exists();
}

//Mesh SkyComponent::create_sphere(UINT LatLines, UINT LongLines) {
//	//NumSphereVertices = ((LatLines-2) * LongLines) + 2;
//	//NumSphereFaces  = ((LatLines-3)*(LongLines)*2) + (LongLines*2);
//
//	float sphereYaw = 0.0f;
//	float spherePitch = 0.0f;
//
//	Mesh ret;
//	auto vertices = ret.get_vertices();
//
//	XMVECTOR currVertPos = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
//
//	vertices[0].position.x = 0.0f;
//	vertices[0].position.y = 0.0f;
//	vertices[0].position.z = 1.0f;
//
//	for(UINT i = 0; i < LatLines-2; i++) {
//		spherePitch = (i+1) * (3.14/(LatLines-1));
//		Rotationx = XMMatrixRotationX(spherePitch);
//		for(DWORD j = 0; j < LongLines; ++j)
//		{
//			sphereYaw = j * (6.28/(LongLines));
//			Rotationy = XMMatrixRotationZ(sphereYaw);
//			currVertPos = XMVector3TransformNormal( XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), (Rotationx * Rotationy) );    
//			currVertPos = XMVector3Normalize( currVertPos );
//			vertices[i*LongLines+j+1].pos.x = XMVectorGetX(currVertPos);
//			vertices[i*LongLines+j+1].pos.y = XMVectorGetY(currVertPos);
//			vertices[i*LongLines+j+1].pos.z = XMVectorGetZ(currVertPos);
//		}
//	}
//
//	vertices[NumSphereVertices-1].pos.x =  0.0f;
//	vertices[NumSphereVertices-1].pos.y =  0.0f;
//	vertices[NumSphereVertices-1].pos.z = -1.0f;
//
//
//	D3D11_BUFFER_DESC vertexBufferDesc;
//	ZeroMemory( &vertexBufferDesc, sizeof(vertexBufferDesc) );
//
//	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
//	vertexBufferDesc.ByteWidth = sizeof( Vertex ) * NumSphereVertices;
//	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	vertexBufferDesc.CPUAccessFlags = 0;
//	vertexBufferDesc.MiscFlags = 0;
//
//	D3D11_SUBRESOURCE_DATA vertexBufferData; 
//
//	ZeroMemory( &vertexBufferData, sizeof(vertexBufferData) );
//	vertexBufferData.pSysMem = &vertices[0];
//	hr = d3d11Device->CreateBuffer( &vertexBufferDesc, &vertexBufferData, &sphereVertBuffer);
//
//
//	std::vector<DWORD> indices(NumSphereFaces * 3);
//
//	int k = 0;
//	for(DWORD l = 0; l < LongLines-1; ++l)
//	{
//		indices[k] = 0;
//		indices[k+1] = l+1;
//		indices[k+2] = l+2;
//		k += 3;
//	}
//
//	indices[k] = 0;
//	indices[k+1] = LongLines;
//	indices[k+2] = 1;
//	k += 3;
//
//	for(DWORD i = 0; i < LatLines-3; ++i)
//	{
//		for(DWORD j = 0; j < LongLines-1; ++j)
//		{
//			indices[k]   = i*LongLines+j+1;
//			indices[k+1] = i*LongLines+j+2;
//			indices[k+2] = (i+1)*LongLines+j+1;
//
//			indices[k+3] = (i+1)*LongLines+j+1;
//			indices[k+4] = i*LongLines+j+2;
//			indices[k+5] = (i+1)*LongLines+j+2;
//
//			k += 6; // next quad
//		}
//
//		indices[k]   = (i*LongLines)+LongLines;
//		indices[k+1] = (i*LongLines)+1;
//		indices[k+2] = ((i+1)*LongLines)+LongLines;
//
//		indices[k+3] = ((i+1)*LongLines)+LongLines;
//		indices[k+4] = (i*LongLines)+1;
//		indices[k+5] = ((i+1)*LongLines)+1;
//
//		k += 6;
//	}
//
//	for(DWORD l = 0; l < LongLines-1; ++l)
//	{
//		indices[k] = NumSphereVertices-1;
//		indices[k+1] = (NumSphereVertices-1)-(l+1);
//		indices[k+2] = (NumSphereVertices-1)-(l+2);
//		k += 3;
//	}
//
//	indices[k] = NumSphereVertices-1;
//	indices[k+1] = (NumSphereVertices-1)-LongLines;
//	indices[k+2] = NumSphereVertices-2;
//
//	D3D11_BUFFER_DESC indexBufferDesc;
//	ZeroMemory( &indexBufferDesc, sizeof(indexBufferDesc) );
//
//	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
//	indexBufferDesc.ByteWidth = sizeof(DWORD) * NumSphereFaces * 3;
//	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
//	indexBufferDesc.CPUAccessFlags = 0;
//	indexBufferDesc.MiscFlags = 0;
//
//	D3D11_SUBRESOURCE_DATA iinitData;
//
//	iinitData.pSysMem = &indices[0];
//	d3d11Device->CreateBuffer(&indexBufferDesc, &iinitData, &sphereIndexBuffer);
//
//}

void SkyComponent::render_editor_gui_section() {
	std::string albedo_texture{this->albedo_texture.get_file()};
	if (ImGui::InputText("Albedo texure", &albedo_texture, ImGuiInputTextFlags_EnterReturnsTrue)) {
		this->albedo_texture.set_texture(albedo_texture);
	}

	FVector4 alb{color_to_fvector(albedo)};
	float col[4]{alb.x, alb.y, alb.z, alb.w};
	if (ImGui::ColorEdit4("Albedo", col))
		set_albedo(Color{UCHAR(col[0]*255.0f), UCHAR(col[1]*255.0f), UCHAR(col[2]*255.0f), UCHAR(col[3]*255.0f)});
}
