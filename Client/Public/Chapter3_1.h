#pragma once
#include "MainQuest.h"

/*�뷡 ���� �� �Ͻ�Ű�߸��� ��ȭ*/
BEGIN(Client)
class CChapter3_1 :
   public CMainQuest
{
private:
	CChapter3_1();
	virtual ~CChapter3_1() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual _bool Execute() override;

public:
	static CChapter3_1* Create(void* pArg);
	virtual void Free();
};
END
