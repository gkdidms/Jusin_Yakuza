#pragma once
#include "MainQuest.h"

//������ ù ��ȭ
BEGIN(Client)
class CChapter7_0 :
   public CMainQuest
{
private:
	CChapter7_0();
	virtual ~CChapter7_0() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual _bool Execute() override;

public:
	static CChapter7_0* Create(void* pArg);
	virtual void Free();
};
END
