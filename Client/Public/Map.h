#pragma once

#include "GameObject.h"
#include "Client_Defines.h"
#include "Decal.h"

BEGIN(Engine)
class CShader;
class CModel;
class CNeoShader;
class CCollider;
END

BEGIN(Client)

// Map - Local 위치를 기반으로 컬링
// 도시 안의 빌딩들을 컬링

class CMap final : public CGameObject
{
public:
	enum OBJECT_TYPE {
		CONSTRUCTION, /* 그냥 건축물 */
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
		SHADER_GLASS, //1
		SHADER_DECAL_MASK, //2
		SHADER_DECAL_LIGHT, //3
		SHADER_SIGN, //4
		SHADER_LAMP, //5
		SHADER_DECAL_BLEND, //6
		SHADER_BLOOM_WHITE, //7
		SHADER_SIGN_MASK, //8
		SHADER_STRONGBLOOM, //9
		SHADER_COMPULSORY_DECALBLEND, //10
		SHADER_DYNAMIC_SIGN_SMALL,

		// 무조건 제일 아래
		SHADER_LIGHTDEPTH, //11
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
		bool				bLocalCull;
		const _float4x4* vPlayerMatrix;
	}MAPOBJ_DESC;


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

public:
	CTransform* Get_Transform() { return m_pTransformCom; }

public:
	int							Get_ObjPlaceDesc(OBJECTPLACE_DESC* objplaceDesc);
	MAPOBJ_DESC					Get_MapObjDesc_For_AddList();

	void						Edit_GameObject_Information(MAPOBJ_DESC	mapDesc);
	CMap::MAPOBJ_DESC			Send_GameObject_Information();


private:
	void						Add_Renderer(const _float& fTimeDelta);

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CNeoShader* m_pMaterialCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

	class CSystemManager* m_pSystemManager = { nullptr };

private:
	_bool m_isFirst = { true };
	vector<CDecal*>			m_vDecals;
	vector<CCollider*>		m_vColliders;

	// 따로 랜더그룹을 다르게
	vector<int>				m_vRenderDefaulMeshIndex; // RENDER_NONBLEND -> 일반 건물 + sign 포함. sign은 shaderpass를 다르게 두기
	vector<int>				m_vRenderGlassMeshIndex; // RENDER_GLASS
	vector<int>				m_vMaskMeshIndex; // RENDER_NONBLEND + mask
	vector<int>				m_vDecalLightMeshIndex; // RENDER_EFFECT + mask
	vector<int>				m_vSignMeshIndex; // 그냥 환한 전등
	vector<int>				m_vLampMeshIndex; // 그냥 환한 전등
	vector<int>				m_vDecalBlendMeshIndex; // 그냥 환한 전등
	vector<int>				m_vBloomIndex; // 그냥 환한 전등
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
	_bool					m_bLocalCull = { false }; // 도로mesh 전용


	const _float4x4* m_pPlayerMatrix; // 플레이어 위치


	bool					m_bCompulsoryAlpha = { false };

	float					m_fDynamicTime = { 0 };
	float					m_fTimer = { 0 };
	bool					m_bPositive = { false };

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