#pragma once
#include "MainQuest.h"

/*������ɿ��� �Ͻ�Ű�߸��� ��ȭ*/
BEGIN(Client)
class CChapter3_0 :
   public CMainQuest
{
private:
	CChapter3_0();
	virtual ~CChapter3_0() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual _bool Execute() override;

private:
	class CNishiki* m_pNishiki = { nullptr };

public:
	static CChapter3_0* Create(void* pArg);
	virtual void Free();
};
END
