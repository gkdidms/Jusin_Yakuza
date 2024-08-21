#pragma once
#include "Base.h"

#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CFightManager : public CBase
{
	DECLARE_SINGLETON(CFightManager)

private:
	CFightManager();
	virtual ~CFightManager() = default;

public:
	_bool isFightStage() { return m_isFightStage; }
	_bool isStreetFight() { return m_isStreetFight; }

public:
	void Set_FightStage(_bool isFightStage, class CMonsterGroup* pMonsterGroup = nullptr);
	void Set_StreetFight(_bool isStreetFight) { m_isStreetFight = isStreetFight; }

public:
	HRESULT Initialize();
	void Tick(const _float& fTimeDelta);

private:
	_bool Check_Stage_Clear();			// ��Ÿ��� ���� ���������� ���� ���Ͱ� �ִ����� üũ

private:
	CGameInstance* m_pGameInstance = { nullptr };
	class CTutorialManager* m_pTutorialManager = { nullptr };
	class CUIManager* m_pUIManager = { nullptr };

	class CMonsterGroup* m_pCurrentMonsterGroup = { nullptr };

private:
	_bool m_isFightStage = { false };
	_bool m_isStreetFight = { false }; // �濡�� �ο�� �ο����� �ƴ��� üũ

	_float m_fFinishDuration = { 3.f };
	_float m_fFinishTime = { 0.f };

	_bool m_isTitleEnd = { false };

public:
	virtual void Free();
};
END