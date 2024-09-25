#pragma once
#include "MainQuest.h"

/*»æÀïÀÌ¶û ÀüÅõ Àü*/
BEGIN(Client)
class CChapter6_1 :
   public CMainQuest
{
private:
	CChapter6_1();
	virtual ~CChapter6_1() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual _bool Execute() override;

private:
	class CShakedown* m_pShakedown = { nullptr };

private:
	_float m_fDuration = { 5.f };
	_float m_fTotalTime = { 0.f };

public:
	static CChapter6_1* Create(void* pArg);
	virtual void Free();
};
END
