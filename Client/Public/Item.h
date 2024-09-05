#pragma once

#include "GameObject.h"
#include "Client_Defines.h"
#include "Decal.h"

BEGIN(Engine)
class CShader;
class CModel;
class CNavigation;
class CTexture;
END

BEGIN(Client)

//Item : �÷��̾ ��� ��ȣ�ۿ��ϴ� �͵�

class CItem final : public CGameObject
{
private:
	const _float BRIGHT_DISTANCE = 5.f;
	const _float THROW_TIME = 1.f;

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

	enum ITEM_MODE
	{
		ITEM_IDLE, // �׳� ���
		ITEM_BRIGHT,
		ITEM_GRAB, // �������
		ITEM_DEAD, 
		ITEM_END
	};

	enum SHADER_NUM {
		SHADER_DEFAULT, //0
		SHADER_DEFAULT_BRIGHT, //1 -> BRIGHT �ɶ��� NONBELND
		SHADER_DISSOLVE, //2

		// ������ ���� �Ʒ�
		SHADER_LIGHTDEPTH, //3
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
		int					iNaviNum;

		int					iDecalNum;
		DECAL_DESC_IO*		pDecal;

		int					iColliderNum;
		OBJCOLLIDER_DESC*	pColliderDesc;

		const _float4x4*	vPlayerMatrix;
		_float4x4			vOffsetMatrix;
	}MAPOBJ_DESC;

	typedef struct tThrowInfoDesc
	{
		_float4 vThrowDir;
		_float fThrowSpeed;
	}THROW_INFO_DESC;


private:
	CItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItem(const CItem& rhs);
	virtual ~CItem() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(const _float& fTimeDelta) override;
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Late_Tick(const _float& fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_LightDepth() override;

	virtual void ImpulseResolution(CGameObject* pTargetObject, _float fDistance = 0.5f) override;
	virtual void Attack_Event(CGameObject* pHitObject, _bool isItem = false) override;

public:
	CTransform*			Get_Transform() { return m_pTransformCom; }

	// ���̰� ���� ���� ����� �޾ƿ��� �Լ�
	void				Set_ParentMatrix(const _float4x4* vParentMatrix) { m_vParentMatrix = vParentMatrix;  }

	CCollider*			Get_AABBCollider() { return m_pColliderCom; }
	CCollider*			Get_OBBCollider() { return m_pOBBColliderCom; }

	ITEM_MODE			Get_ItemMode() { return m_eItemMode; }
	void				Set_ItemMode(CItem::ITEM_MODE mode) { m_eItemMode = mode; }
	void				Set_Grab(bool bGrab) { m_bCurGrab = bGrab;  m_eItemMode = ITEM_GRAB;}
	void				Attacking(_bool isOn = true) { m_isAttacking = isOn; }
	void				Set_Dissolve() { m_bDissovle = true; }

public:
	_uint				Get_ItemLife() { return m_iLife; }
	_bool				Decrease_Life();		// ���Ƚ�� ���� (���Ƚ���� ���� ������ ��Ȳ�̸� false�� �����Ѵ�)
	void				Throw_On(THROW_INFO_DESC& Desc);

private:
	void				Throwing(const _float& fTimeDelta);

private:
	void				Set_Item_Mode();

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	class CSystemManager* m_pSystemManager = { nullptr };
	class CCollision_Manager* m_pCollisionManager = { nullptr };
	CCollider* m_pColliderCom = { nullptr }; //AABB ����
	CCollider* m_pOBBColliderCom = { nullptr }; //AABB ����
	CNavigation* m_pNavigationCom = { nullptr };

	CTexture* m_pDissolveTextureCom = { nullptr };

private:
	_bool m_isFirst = { true };
	_bool m_isAttacking = { false };
	vector<CDecal*>			m_vDecals;
	//vector<CCollider*>		m_vColliders;
	int						m_iLayerNum;
	wstring					m_wstrModelName;
	int						m_iShaderPassNum = { 0 };
	int						m_iObjectType = { 0 };

	float					m_fBrightTime = { 0 };
	bool					m_bBright = { false };

	//bool					m_bRimLight;
	//_float2					m_fRimPartsUV;
	//float					m_isRimLight; // 0.f ����, 0.1 ���Ѵ�, 0.2 ����, 0.3 �ı��� 0.4 ��� => ��� ���
	//int						m_iRimLightTime;

	_float4x4				m_vOffsetMatrix; // ��ġ ������
	const _float4x4*		m_vParentMatrix; // �θ� (��)
	const _float4x4*		m_pPlayerMatrix; // �÷��̾� ��ġ

	_float4x4				m_WorldMatrix;

	ITEM_MODE				m_eItemMode;

	bool					m_bCurGrab = { false }; // ���� ����ִ���

	_int					m_iLife = { 3 };		// ���� Ÿ���� �� �ִ� Ƚ��
	
	_bool					m_isThrowing = { false };
	_float					m_fThrowTimer = { 0.f };
	THROW_INFO_DESC			m_ThrowInfo{};


	_float					m_fDissolveTime = { 0 };
	_bool					m_bDissovle = { false };
	

public:
	HRESULT Add_Components(void* pArg);
	HRESULT Bind_ShaderResources();

public:
	static CItem* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END