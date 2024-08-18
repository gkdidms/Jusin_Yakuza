#pragma once
#include "Base.h"

#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstane;
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
	void Set_FightStage(_bool isFightStage);
	void Set_StreetFight(_bool isStreetFight) { m_isStreetFight = isStreetFight; }

public:
	void Tick(const _float& fTimeDelta);

private:
	CGameInstance* m_pGameInstance = { nullptr };
	class CUIManager* m_pUIManager = { nullptr };

private:
	_bool m_isFightStage = { false };
	_bool m_isStreetFight = { false }; // 길에서 싸우는 싸움인지 아닌지 체크

	_float m_fFinishDuration = { 3.f };
	_float m_fFinishTime = { 0.f };

public:
	virtual void Free();
};
END