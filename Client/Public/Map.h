#pragma once

#include "GameObject.h"
#include "Client_Defines.h"
#include "Decal.h"

BEGIN(Engine)
class CShader;
class CModel;
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
		SHADER_DEFAULT_MAP,
		SHADER_GLASS,
		SHADER_DECAL_MASK,
		SHADER_DECAL_LIGHT,
		SHADER_SIGN,
		SHADER_LAMP,
		SHADER_DECAL_BLEND,
		SHADER_LIGHTDEPTH,
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
	CTransform*					Get_Transform() { return m_pTransformCom; }

public:
	int							Get_ObjPlaceDesc(OBJECTPLACE_DESC* objplaceDesc);
	MAPOBJ_DESC					Get_MapObjDesc_For_AddList();

	void						Edit_GameObject_Information(MAPOBJ_DESC	mapDesc);
	CMap::MAPOBJ_DESC			Send_GameObject_Information();

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	class CSystemManager* m_pSystemManager = { nullptr };

private:
	_bool m_isFirst = { true };
	vector<CDecal*>			m_vDecals;
	vector<CCollider*>		m_vColliders;

	// ���� �����׷��� �ٸ���
	vector<int>				m_vRenderDefaulMeshIndex; // RENDER_NONBLEND -> �Ϲ� �ǹ� + sign ����. sign�� shaderpass�� �ٸ��� �α�
	vector<int>				m_vRenderGlassMeshIndex; // RENDER_GLASS
	vector<int>				m_vDecalMeshIndex; // RENDER_NONBLEND + mask
	vector<int>				m_vDecalLightMeshIndex; // RENDER_EFFECT + mask
	vector<int>				m_vSignMeshIndex; // �׳� ȯ�� ����
	vector<int>				m_vLampMeshIndex; // �׳� ȯ�� ����
	vector<int>				m_vDecalBlendMeshIndex; // �׳� ȯ�� ����

	int						m_iLayerNum;
	wstring					m_wstrModelName;
	int						m_iShaderPassNum = { 0 };
	int						m_iObjectType = { 0 };
	float					m_fWaterDeltaTime = { 0 };

public:
	HRESULT Add_Components(void* pArg);
	HRESULT Bind_ShaderResources();

public:
	static CMap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END