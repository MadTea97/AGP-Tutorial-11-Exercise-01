#include "Model.h"

struct MODEL_CONSTANT_BUFFER
{
	XMMATRIX WorldViewProjection; //64 bytes
	XMVECTOR directional_light_vector;	//16 bytes
	XMVECTOR directional_light_colour;	//16 bytes
	XMVECTOR ambient_light_colour;		//16 bytes
	//112 bytes
};

Model::Model(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pD3DDevice = pDevice;
	m_pImmediateContext = pContext;

	m_x = 0.0f;
	m_y = 0.0f;
	m_z = 0.0f;
	m_xangle = 0.0f;
	m_zangle = 0.0f;
	m_yangle = 0.0f;

	m_scale = 1.0f;
}
int Model::LoadObjModel(char* filename)
{
	m_pObject = new ObjFileModel(filename, m_pD3DDevice, m_pImmediateContext);
	AddTexture("assets/tex.jpg");

	if (m_pObject->filename == "FILE NOT LOADED")
		return S_FALSE;

	HRESULT hr = S_OK;
	ID3DBlob *VS, *PS, *error;


	hr = D3DX11CompileFromFile("model_shaders.hlsl", 0, 0, "ModelVS", "vs_4_0", 0, 0, 0, &VS, &error, 0);
	if (error != 0)
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr))
			return hr;
	}

	hr = D3DX11CompileFromFile("model_shaders.hlsl", 0, 0, "ModelPS", "ps_4_0", 0, 0, 0, &PS, &error, 0);
	if (error != 0)
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr))
			return hr;
	}

	hr = m_pD3DDevice->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &m_pVShader);
	if (FAILED(hr))
		return hr;

	hr = m_pD3DDevice->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &m_pPShader);
	if (FAILED(hr))
		return hr;

	//maybe set

	D3D11_INPUT_ELEMENT_DESC iedesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	hr = m_pD3DDevice->CreateInputLayout(iedesc, ARRAYSIZE(iedesc), VS->GetBufferPointer(), VS->GetBufferSize(), &m_pInputLayout);

	if (FAILED(hr))
	{
		return hr;
	}

	//create constant buffer size 64
	D3D11_BUFFER_DESC constant_buffer_desc;
	ZeroMemory(&constant_buffer_desc, sizeof(constant_buffer_desc));

	constant_buffer_desc.Usage = D3D11_USAGE_DEFAULT; //can use UpdateSubresource() to update
	constant_buffer_desc.ByteWidth = 112; //MUST be a multiple of 16, calculate from cb struct
	constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // use as a constant buffer

	hr = m_pD3DDevice->CreateBuffer(&constant_buffer_desc, NULL, &m_pConstantBuffer);

	if (FAILED(hr)) return hr;


}
void Model::Draw(XMMATRIX* view, XMMATRIX* projection)
{
	XMMATRIX world, transpose;

	g_directional_light_shines_from = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	g_directional_light_colour = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
	g_ambient_light_colour = XMVectorSet(0.1f, 0.1f, 0.1f, 1.0f);

	world = XMMatrixScaling(m_scale, m_scale, m_scale);
	world *= XMMatrixRotationY(m_yangle);
	//world *= XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_xangle), XMConvertToRadians(m_yangle), XMConvertToRadians(m_zangle));
	world *= XMMatrixTranslation(m_x, m_y, m_z);

	MODEL_CONSTANT_BUFFER model_cb_values;
	model_cb_values.WorldViewProjection = world*(*view)*(*projection);

	transpose = XMMatrixTranspose(world);

	model_cb_values.directional_light_colour = g_directional_light_colour;
	model_cb_values.ambient_light_colour = g_ambient_light_colour;
	model_cb_values.directional_light_vector = XMVector3Transform(g_directional_light_shines_from, transpose);
	model_cb_values.directional_light_vector = XMVector3Normalize(model_cb_values.directional_light_vector);

	if (m_pConstantBuffer)
	{


		m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, 0, &model_cb_values, 0, 0);
		m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);

		m_pImmediateContext->VSSetShader(m_pVShader, 0, 0);
		m_pImmediateContext->PSSetShader(m_pPShader, 0, 0);

		m_pImmediateContext->IASetInputLayout(m_pInputLayout);

		m_pImmediateContext->PSSetSamplers(0, 1, &g_pSampler0);
		m_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexture0);


		m_pObject->Draw();
	}
}
void Model::AddTexture(char * filename)
{
	D3DX11CreateShaderResourceViewFromFile(m_pD3DDevice, filename, NULL, NULL, &g_pTexture0, NULL);

	D3D11_SAMPLER_DESC sampler_desc;
	ZeroMemory(&sampler_desc, sizeof(sampler_desc));
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

	m_pD3DDevice->CreateSamplerState(&sampler_desc, &g_pSampler0);

}
void Model::LookAt_XZ(float x, float z)
{
	float dx;
	float dz;

	dx = sin(x * (XM_PI / 180));
	dz = cos(z * (XM_PI / 180));

	m_yangle = atan2(dx, dz) * (180.0f / XM_PI);
}
void Model::MoveForward(float distance)
{
	m_x += sin(m_yangle * (XM_PI / 180.0f)) * distance;
	m_z += cos(m_yangle * (XM_PI / 180.0f)) * distance;

}
float Model::GetXPos()
{
	return m_x;
}
float Model::GetYPos()
{
	return m_y;
}
float Model::GetZPos()
{
	return m_z;
}
float Model::GetXAngle()
{
	return m_xangle;
}
float Model::GetYAngle()
{
	return m_yangle;
}
float Model::GetZAngle()
{
	return m_zangle;
}
float Model::GetScale()
{
	return m_scale;
}
//setter
void Model::SetXPos(float xpos)
{
	m_x = xpos;
}
void Model::SetYPos(float ypos)
{
	m_y = ypos;
}
void Model::SetZPos(float zpos)
{
	m_z = zpos;
}
void Model::SetXAngle(float xangle)
{
	m_xangle = xangle;
}
void Model::SetYAngle(float yangle)
{
	m_yangle = yangle;
}
void Model::SetZAngle(float zangle)
{
	m_zangle = zangle;
}
void Model::SetScale(float scale)
{
	m_scale = scale;
}
Model::~Model()
{
	if (m_pConstantBuffer) m_pConstantBuffer->Release();
	if (m_pInputLayout) m_pInputLayout->Release();
	if (m_pVShader) m_pVShader->Release();
	if (m_pPShader) m_pPShader->Release();
	if (g_pTexture0) g_pTexture0->Release();
	if (g_pSampler0) g_pSampler0->Release();

	delete m_pObject;
	/*ID3D11Debug* debug;
	HRESULT hr = m_pD3DDevice->QueryInterface(IID_PPV_ARGS(&debug));
	debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);*/
}