#pragma once

#include "PartObject.h"

BEGIN(Client)

class CSoketObject abstract : public CPartObject
{
public:
	struct SOKETOBJECT_DESC : public CPartObject::PARTOBJ_DESC
	{
		const _float4x4* pCombinedTransformationMatrix;
	};

protected:
	CSoketObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSoketObject(const CSoketObject& rhs);
	virtual ~CSoketObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(const _float& fTimeDelta) override;
	virtual void Tick(const _float& fTimeDelta) override;
	virtual void Late_Tick(const _float& fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	const _float4x4* m_pSocketMatrix = { nullptr };

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;

};

END