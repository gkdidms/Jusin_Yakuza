#pragma once
#include "MainQuest.h"

/* �Ͻ�Ű�߸��� ��ȭ */
BEGIN(Client)
class CChapter1_1 :
   public CMainQuest
{
private:
	CChapter1_1();
	virtual ~CChapter1_1() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual _bool Execute() override;

public:
	_bool m_isTitleFinished = { false };

public:
	static CChapter1_1* Create(void* pArg);
	virtual void Free();
};
END
