#pragma once

#include "GameObject.h"
#include "Client_Defines.h"
#include "Decal.h"

BEGIN(Engine)
class CShader;
class CModel;
class CNeoShader;
class CCollider;
class CVIBuffer_AABBCube;
class CComputeShader;
END

BEGIN(Client)

// Map - Local ��ġ�� ������� �ø�
// ���� ���� �������� �ø�

class CMap final : public CGameObject
{
public:
	enum OBJECT_TYPE {
		CONSTRUCTION, /* �׳� ���๰ */
		ROAD,
		ITEM,
		MONSTER_RUSH,
		PLAYER,
		SKY,
		LIGHT,
		MONSTER_WPA,
		MONSTER_SHAKEDOWN,
		MONSTER_YONEDA,
		MONSTER_KUZE,
		LARGE_CONSTRUCTION,
		OBJ_END
	};

	enum SHADER_NUM {
		SHADER_DEFAULT_MAP, //0
		SHADER_GLASS_DEFAULT, //1
		SHADER_DECAL_MASK, //2
		SHADER_DECAL_LIGHT, //3
		SHADER_SIGN, //4
		SHADER_LAMP, //5
		SHADER_DECAL_BLEND, //6
		SHADER_BLOOM_WHITE, //7
		SHADER_SIGN_MASK, //8
		SHADER_STRONGBLOOM, //9
		SHADER_COMPULSORY_DECALBLEND, //10
		SHADER_DYNAMIC_SIGN_SMALL, // 11
		SHADER_DYNAMIC_SIGN_BLOOM, // 12
		SHADER_DYNAMIC_SIGN_FAST, // 13
		SHADER_GLASS_COLOR, //14

		// ������ ���� �Ʒ�
		SHADER_LIGHTDEPTH, //14
		SHADER_FAR,
		SHADER_END
	};


public:
	typedef struct tMapObjDesc : public CGameObject::GAMEOBJECT_DESC
	{
		XMMATRIX			vStartPos;
		int					iLayer;
		wstring				wstrModelName;
		int					iShaderPass;
		int					iObjType;

		int					iDecalNum;
		DECAL_DESC_IO* pDecal;

		int					iColliderNum;
		OBJCOLLIDER_DESC* pColliderDesc;

		bool				bCull;
		const _float4x4* vPlayerMatrix;
	}MAPOBJ_DESC;


	struct ObjectData
	{
		XMMATRIX worldViewProjMatrix;
		float g_fFar;
		float padding[3]; // �е� �߰� (16����Ʈ ������ ���߱� ����)
	};


private:
	CMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMap(const CMap& rhs);
	virtual ~CMap() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(const _float& fTimeDelta) override;
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Late_Tick(const _float& fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_LightDepth() override;
	virtual HRESULT Render_OcculusionDepth() override;
	virtual HRESULT Check_OcculusionCulling() override;

	_bool isOcculusionDepth();
	HRESULT Near_Render(_uint iRenderState);
	HRESULT Far_Render(_uint iRenderState);
public:
	CTransform* Get_Transform() { return m_pTransformCom; }

public:
	int							Get_ObjPlaceDesc(OBJECTPLACE_DESC* objplaceDesc);
	MAPOBJ_DESC					Get_MapObjDesc_For_AddList();

	void						Edit_GameObject_Information(MAPOBJ_DESC	mapDesc);
	CMap::MAPOBJ_DESC			Send_GameObject_Information();

private:
	void						Add_Renderer(const _float& fTimeDelta);
	void						Add_Renderer_Far(const _float& fTimeDelta);

private:
	CShader*	m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CNeoShader* m_pMaterialCom = { nullptr };
	class CSystemManager* m_pSystemManager = { nullptr };

	// �ø��� ���� �𵨰� ���̴�
	CShader*				m_pCubeShaderCom = { nullptr }; //aabb ť�� �׸���
	CVIBuffer_AABBCube*		m_pVIBufferCom = { nullptr };
	CComputeShader*			m_pComputeShaderCom = { nullptr };


	ID3D11Buffer*			m_pObjectDataBuffer = { nullptr };
	ID3D11Buffer*			m_pOutputBufferStaging = { nullptr };

private:
	_bool					m_isFirst = { true };
	vector<CDecal*>			m_vDecals;
	vector<CCollider*>		m_vColliders;

	// ���� �����׷��� �ٸ���
	vector<int>				m_vRenderDefaulMeshIndex; // RENDER_NONBLEND -> �Ϲ� �ǹ� + sign ����. sign�� shaderpass�� �ٸ��� �α�
	vector<int>				m_vRenderGlassMeshIndex; // RENDER_GLASS
	vector<int>				m_vMaskMeshIndex; // RENDER_NONBLEND + mask
	vector<int>				m_vDecalLightMeshIndex; // RENDER_EFFECT + mask
	vector<int>				m_vSignMeshIndex; // �׳� ȯ�� ����
	vector<int>				m_vLampMeshIndex; // �׳� ȯ�� ����
	vector<int>				m_vDecalBlendMeshIndex; // �׳� ȯ�� ����
	vector<int>				m_vBloomIndex; // �׳� ȯ�� ����
	vector<int>				m_vMaskSignIndex;
	vector<int>				m_vDynamicSignIndex;
	vector<int>				m_vStrongBloomIndex;
	vector<int>				m_vCompulsoryDecalBlendMeshIndex;

	_int					m_iLayerNum;
	wstring					m_wstrModelName;
	_int					m_iShaderPassNum = { 0 };
	_int					m_iObjectType = { 0 };
	_float					m_fWaterDeltaTime = { 0 };
	_bool					m_bCull = { false };


	const _float4x4*		m_pPlayerMatrix; // �÷��̾� ��ġ
	_matrix					m_CubeWorldMatrix;


	bool					m_bCompulsoryAlpha = { false };

	float					m_fDynamicTime = { 0 };
	float					m_fTimer = { 0 };
	bool					m_bPositive = { false };


	bool					m_bOcculusionCulling = { false };
	bool					m_bExcludeSSAO = { false }; //SSAO ����

private:
	ID3D11Query* m_pOcclusionQuery = { nullptr };

private:
	_bool m_isFar = { false }; // �÷��̾�� ��ü�� �� ���� �ֳ�?
	_bool m_isShow = { false };

public:
	HRESULT Add_Components(void* pArg);
	HRESULT Bind_ShaderResources();
	HRESULT Reset_Bind();

public:
	static CMap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END