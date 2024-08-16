#pragma once
#include "Base.h"

#include "Client_Defines.h"
BEGIN(Client)
class CFightManager : public CBase
{
	DECLARE_SINGLETON(CFightManager)

private:
	CFightManager();
	virtual ~CFightManager() = default;

public:
	_bool isFightStage() { return m_isFightStage; }

public:
	void Set_FightStage(_bool isFightStage);

public:
	void Tick(const _float& fTimeDelta);

private:
	class CUIManager* m_pUIManager = { nullptr };
private:
	_bool m_isFightStage = { false };

public:
	virtual void Free();
};
END