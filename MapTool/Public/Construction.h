#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CConstruction final : public CGameObject
{
//public:
//	enum OBJECT_TYPE {
//		INTERACT_OBJ,
//		NONINTERACT_OBJ,
//		MONSTER,
//		OBJ_END
//	};

public:
	typedef struct tMapObjDesc : public CGameObject::GAMEOBJECT_DESC
	{
		XMMATRIX		vStartPos;
		int				iLayer;
		wstring			wstrModelName;
		int				iShaderPass;
		int				iObjType;
	}MAPOBJ_DESC;


private:
	CConstruction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CConstruction(const CConstruction& rhs);
	virtual ~CConstruction() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(const _float& fTimeDelta) override;
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Late_Tick(const _float& fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	CTransform*					Get_Transform() { return m_pTransformCom; }

public:
	int							Get_ObjPlaceDesc(OBJECTPLACE_DESC*	objplaceDesc);
	MAPOBJ_DESC					Get_MapObjDesc_For_AddList();

	/* 맵 로딩 후 배치된 게임오브젝트 수정을 위한 함수들 */
	void										Edit_GameObject_Information(CConstruction::MAPOBJ_DESC	mapDesc);
	CConstruction::MAPOBJ_DESC					Send_GameObject_Information();

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

private:
	int						m_iLayerNum = { 0 };
	wstring					m_wstrModelName;
	int						m_iShaderPassNum = { 0 };
	int						m_iObjectType = { 0 };

public:
	HRESULT Add_Components(void* pArg);
	HRESULT Bind_ShaderResources();

public:
	static CConstruction* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END