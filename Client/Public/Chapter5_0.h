#pragma once
#include "MainQuest.h"

/*��Ÿ��� ���ͼ� ��ü���� �繫�ҷ� ����*/
BEGIN(Client)
class CChapter5_0 :
   public CMainQuest
{
private:
	CChapter5_0();
	virtual ~CChapter5_0() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual _bool Execute() override;

public:
	static CChapter5_0* Create(void* pArg);
	virtual void Free();
};
END
