#pragma once

#include "Client_Defines.h"
#include "Level.h"

/* ȭ�鿡 �ε� ���� �����ش� + ���� ������ ���� ���ҽ��� �غ��ϳ�. */

BEGIN(Client)

class CLevel_Loading final : public CLevel
{
public:
	enum LOADER_TYPE { ANIM, MAP, OTEHR, LOADER_TYPE_END };

private:
	CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Loading() = default;

public:
	virtual HRESULT Initialize(LEVEL eNextLevel);
	virtual void Tick(const _float& fTimeDelta) override;

private:
	LEVEL				m_eNextLevel = { LEVEL_END };

	class CLoader* m_pLoader[3] = {nullptr};
	class CMultiLoader* m_pMultiLoader = { nullptr };

private:
	HRESULT Ready_Layer_BackGround(const wstring& strLayerTag);

public:
	static CLevel_Loading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevel);
	virtual void Free() override;
};

END