#pragma once
#include "MainQuest.h"

/*�Ͻ�Ű�߸��� �����鼭 ��ȭ*/
BEGIN(Client)
class CChapter2_0 :
   public CMainQuest
{
private:
	CChapter2_0();
	virtual ~CChapter2_0() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual _bool Execute() override;

public:
	class CSubtitle* m_pSubtitle = { nullptr };
	_bool m_isTitleFinished = { false };

public:
	static CChapter2_0* Create(void* pArg);
	virtual void Free();
};
END
