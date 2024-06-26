#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CDecal;
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
		int				iObjPropertyType;
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
	CTransform*									Get_Transform() { return m_pTransformCom; }
	CModel*										Get_Model() { return m_pModelCom; }

public:
	HRESULT										Add_Components(void* pArg);
	HRESULT										Bind_ShaderResources();

public:
	int											Get_ObjPlaceDesc(OBJECTPLACE_DESC*	objplaceDesc);
	MAPOBJ_DESC									Get_MapObjDesc_For_AddList();

	/* 맵 로딩 후 배치된 게임오브젝트 수정을 위한 함수들 */
	void										Edit_GameObject_Information(CConstruction::MAPOBJ_DESC	mapDesc);
	CConstruction::MAPOBJ_DESC					Send_GameObject_Information();


	/* 맵툴에서만 필요한 기능 - Decal 관련*/
	void										On_Find_DecalMesh(int* iNumMesh, int iCnt);
	void										Off_Find_DecalMesh();

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };


private:
	vector<CDecal*>			m_vDecals;
	int						m_iLayerNum = { 0 };
	wstring					m_wstrModelName;
	int						m_iShaderPassNum = { 0 };
	int						m_iObjectType = { 0 };
	int						m_iObjectPropertyType = { 0 };


	int*					m_pDecalMeshIndex = { 0 };
	int						m_iDecalMeshCnt = { 0 };
	int						m_bFindDecalMesh = { false };

public:
	static CConstruction* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END