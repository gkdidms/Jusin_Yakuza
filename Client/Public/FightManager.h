#pragma once
#include "Base.h"

class CFightManager : public CBase
{
private:
	CFightManager();
	virtual ~CFightManager() = default;

public:
	_bool isFightStage() { return m_isFightStage; }

public:
	void Set_FightStage(_bool isFightStage) { m_isFightStage = isFightStage; }

private:
	_bool m_isFightStage = { false };

public:
	virtual void Free();
};