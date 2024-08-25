#include "UIMenu.h"

#include "UIManager.h"
#include "UI_Object.h"
#include "Group.h"
#include "Btn.h"
#include"GameInstance.h"
CUIMenu::CUIMenu()
	:CUIScene{}
{
}

CUIMenu::CUIMenu(const CUIMenu& rhs)
	:CUIScene{rhs}
{
}

HRESULT CUIMenu::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	if (FAILED(__super::Initialize(pDevice, pContext,pArg)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMenu::Tick(const _float& fTimeDelta)
{

	__super::Tick(fTimeDelta);

	//���� �ִϸ��̼� ������ ��� �ൿ �ڵ�
	if (m_isAnimFin)
	{

		if (-1 != m_iCurButton)
		{
			for (auto& iter : m_EventUI)
				iter->Tick(fTimeDelta);
		}
	}

	return S_OK;
}

HRESULT CUIMenu::Late_Tick(const _float& fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);


	//���� �ִϸ��̼� ������ ��� �ൿ �ڵ�
	if (m_isAnimFin)
	{

		if (Over_InterSect())
		{
			OverAction();
		}


		if (-1 != m_iCurButton)
		{
			for (auto& iter : m_EventUI)
				iter->Late_Tick(fTimeDelta);
		}
	}

	if (!m_isAnimFin)
	{
		Check_AimFin();

		m_pGameInstance->PlaySound_W(TEXT("4681 [30].wav"), SOUND_UI, 1.f);
	}
		
	return S_OK;
}

void CUIMenu::Action()
{


	switch (m_iCurButton)
	{
	case 0://�κ�
	{
		CUIManager::GetInstance()->Open_Scene(TEXT("Inven"));
		break;
	}
	case 1://��ų
	{
		CUIManager::GetInstance()->Open_Scene(TEXT("SkillMenu"));
		break;
	}
	case 2://����
	{

		break;
	}

	default:
		break;
	}
}

void CUIMenu::OverAction()
{
	//�̺�Ʈ����(�ڱⰡ ���� ui ������ ����ؼ� ����)
	// 1. ����Ʈ 2.���� 3.��ƼŬ 
	//�������� �׷����� �����ؼ� ���� ����Ʈ,��ƼŬ�� ����ϰ� �������
	_matrix ButtonWorld = m_Button[m_iCurButton]->Get_TransformCom()->Get_WorldMatrix();
	_vector Position = ButtonWorld.r[3];
	ButtonWorld=XMMatrixTranslation(XMVectorGetX(Position), XMVectorGetY(Position), 0.f);
	//ButtonWorld=XMMatrixTranslation(-100.f, 100.f, 0.f);

	if(m_iCurButton !=m_iPrevButton)
	{
		m_pGameInstance->StopSound(SOUND_UI);
		m_pGameInstance->PlaySound_W(TEXT("4681 [9].wav"), SOUND_UI, 1.f);

		m_Button[m_iCurButton]->Show_UI();

		if (-1 != m_iPrevButton)
		{
			m_Button[m_iPrevButton]->Close_UI();
		}
	}

	
	switch (m_iCurButton)
	{
	case 0 :
	{
		
		m_EventUI[0]->Show_UI();
		m_EventUI[0]->Get_TransformCom()->Set_WorldMatrix(ButtonWorld);

		//�׷����� ���� (����â �� ��Ƶα�)
		//��ư ��ȣ �޾Ƽ� ���� ų �̹��� ����(isPlay)
		dynamic_cast<CGroup*>(m_EventUI[1])->Show_Choice(m_iCurButton);


		m_EventUI[2]->Show_UI();
		m_EventUI[2]->Get_TransformCom()->Set_WorldMatrix(ButtonWorld);
		break;
	}
	case 1:
	{
		m_EventUI[0]->Show_UI();
		m_EventUI[0]->Get_TransformCom()->Set_WorldMatrix(ButtonWorld);

		//�׷����� ���� (����â �� ��Ƶα�)
		//��ư ��ȣ �޾Ƽ� ���� ų �̹��� ����(isPlay)
		dynamic_cast<CGroup*>(m_EventUI[1])->Show_Choice(m_iCurButton);

		m_EventUI[2]->Show_UI();
		m_EventUI[2]->Get_TransformCom()->Set_WorldMatrix(ButtonWorld);
		break;
	}

	default:
		break;
	}
}

CUIMenu* CUIMenu::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CUIMenu* pInstance = new CUIMenu();
	if(FAILED(pInstance->Initialize(pDevice , pContext, pArg)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CUIMenu::Free()
{
	__super::Free();
}
