#pragma once
#include "objfilemodel.h"
class Model
{
private:
	ID3D11Device*			m_pD3DDevice;
	ID3D11DeviceContext*	m_pImmediateContext;

	ObjFileModel*			m_pObject;
	ID3D11VertexShader*		m_pVShader;
	ID3D11PixelShader*		m_pPShader;
	ID3D11InputLayout*		m_pInputLayout;
	ID3D11Buffer*			m_pConstantBuffer;

	ID3D11ShaderResourceView* g_pTexture0;
	ID3D11SamplerState* g_pSampler0;

	XMVECTOR g_directional_light_shines_from;
	XMVECTOR g_directional_light_colour;
	XMVECTOR g_ambient_light_colour;

	float m_x, m_y, m_z;
	float m_xangle, m_zangle, m_yangle;
	float m_scale;

public:
	Model(ID3D11Device*, ID3D11DeviceContext*);
	~Model();
	int LoadObjModel(char*);
	void Draw(XMMATRIX*, XMMATRIX*);
	void AddTexture(char* filename);

	void LookAt_XZ(float x, float z);
	void MoveForward(float distance);

	//getter
	float GetXPos();
	float GetYPos();
	float GetZPos();
		  
	float GetXAngle();
	float GetYAngle();
	float GetZAngle();
	
	float GetScale();
	//setter
	void SetXPos(float);
	void SetYPos(float);
	void SetZPos(float);

	void SetXAngle(float);
	void SetYAngle(float);
	void SetZAngle(float);

	void SetScale(float);
};