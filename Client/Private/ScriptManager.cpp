#include "ScriptManager.h"

CScriptManager::CScriptManager()
{
}

const CScriptManager::SCRIPT_INFO CScriptManager::Get_Script(_uint iIndex, _int iScriptIndex)
{
	map<_int, SCRIPT_INFO>& Scripts = m_Scripts[iIndex];

	auto iter = Scripts.find(iScriptIndex);

	if (iter == Scripts.end())
		return SCRIPT_INFO(TEXT("��"), TEXT("����"));
	
	return iter->second;
}

HRESULT CScriptManager::Initialize()
{
	//��� �ۼ��ϱ�
	Ready_Script_1();
	Ready_Script_2();
	Ready_Script_3();
	Ready_Script_4();
	Ready_Script_5();
	Ready_Script_6();
	Ready_Script_7();
	Ready_Script_8();
	Ready_Script_9();
	Ready_Script_10();

	return S_OK;
}

void CScriptManager::Ready_Script_1()
{
	map<_int, SCRIPT_INFO> Scrpit_1 = {};
	_int iIndex = 0;

	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("( ...�Ͻ�Ű�� ������� �ߴ� ��,\\ �� ��ó�� �ٵ� �༮�� ���� �ȿ� �ǰ� ? )")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("( [724106(���ϰ� �־� ? )]\\���� ���� ���� ���̾�......) ")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("( ��¿ �� ����, �Ͻ�Ű�� ã�Ƽ� �� ���ƴٳ� ���� )")));

	m_Scripts.emplace_back(Scrpit_1);
}

void CScriptManager::Ready_Script_2()
{
	map<_int, SCRIPT_INFO> Scrpit = {};
	_int iIndex = 0;
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("�Ͻ�Ű�߸�"), TEXT("�ʾ��ݾ�. �� �̷��� �ʰԿ°ž�?")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("��� �տ��� �ú� �ɷ��� ���̾�.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("�Ͻ�Ű�߸�"), TEXT("�ʵ� �� �����ϱ���.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("�Ͻ�Ű�߸�"), TEXT("�װ� �׷���, �ʵ� ���� �ݵ�������.\\�̹� �Ǹ��� �����ھ�. ���� ���ݵ� �̹� ���� ���� ? ")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("�� �׷���....")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("ī�ڸ� ����� ������ ������ �� ���� �̴ϱ�,\\ �׶����� ���� ������ ����� �͵��� �־߰���.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("�Ͻ�Ű�߸�"), TEXT("�쿡.... �׷� ���� �� ���ٱ��� �ܸ���� �ٲ��.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("��?")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("�Ͻ�Ű�߸�"), TEXT("�ʵ� ����� �˰� ����? ������� ���� �����̳� �԰� �ִ� �� �귣��,\\ �׸��� �� ������������ ������ ���� �����ϰ� ���� ����.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("�Ͻ�Ű�߸�"), TEXT("�����ڴ� ���㼼���� �ְ��.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("�׷� �ǰ�....")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("�Ͻ�Ű�߸�"), TEXT("�׷� �Ŵ�. \\�Ϲ��� ������ �Ǵ����� ������ ���� ������ � �ٴϴ� �ٺ�....")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("�Ͻ�Ű�߸�"), TEXT("�� �Ÿ��� ������� ����� ���� ���� ���� ������ �ʾ�.\\��鿡�� �־ �Ѹ���� ���ζ��.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("ĩ... �˾Ҵ�. �׸� �ƾ�. �װͺ��� ���÷� ���ڸ鼭?")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("�Ͻ�Ű�߸�"), TEXT("�׷�����. ...����, ������ ��ũ ����̶� ����?")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("�Ͻ�Ű�߸�"), TEXT("������ ���� ���! �ʵ� ������ �����! �˾ҳ�!?")));

	m_Scripts.emplace_back(Scrpit);
}

void CScriptManager::Ready_Script_3()
{
	map<_int, SCRIPT_INFO> Scrpit = {};
	_int iIndex = 0;


	m_Scripts.emplace_back(Scrpit);
}

void CScriptManager::Ready_Script_4()
{
	map<_int, SCRIPT_INFO> Scrpit = {};
	_int iIndex = 0;
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("��, �� ���Դ� ���� ����?")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("�Ͻ�Ű�߸�"), TEXT("��? �̰ɷ� 2��°�ε�... ��?")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("��Ʋ Ű�� ���� �� �ϰ� �����ϱ�. �ܰ��ΰ� ����.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("�Ͻ�Ű�߸�"), TEXT("���ֳ� ��¦��� �ͺ��� �㼼�� ���ݾ�?")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("�Ͻ�Ű�߸�"), TEXT("���� ��������� �󱼵� �˸� �� �ְ�.\\�� �����ϰ� ���̴� �� �����ϱ�.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("�Ͻ�Ű�߸�"), TEXT("������ ������ �̾߱�� ��������! ������ɶ� �ؼ� ���� ���ڰ�!")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("��, �˾Ҿ�. ������ ������ �޷�����.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("�������, Let's Go!")));

	m_Scripts.emplace_back(Scrpit);
}

void CScriptManager::Ready_Script_5()
{
	map<_int, SCRIPT_INFO> Scrpit = {};
	_int iIndex = 0;
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("�Ͻ�Ű�߸�"), TEXT("�׷����� �ֱٿ� ������ ������ �־��ٸ�.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("�ƾ�......")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("�Ͻ�Ű�߸�"), TEXT("��¼�ٰ� ������ �������� �𸣰�����, �����϶��.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("�Ͻ�Ű�߸�"), TEXT("������ �� ã�� �־�.\\ī�ڸ� ��Ű� ���õ� �Ϸ� ã�� ���ϼ� ������ ã�ư� ���°� ���ھ�.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("�Ͻ�Ű�߸�"), TEXT("���� ũ����Ʈ�� ����.\\�������� ���� ������ ���� ��⸦ ���� �� ��������.")));

	m_Scripts.emplace_back(Scrpit);
}

void CScriptManager::Ready_Script_6()
{
	map<_int, SCRIPT_INFO> Scrpit = {};
	_int iIndex = 0;
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("( [���� ũ����Ʈ]�� ������� ����...... )")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("( ���� ������ ���� ��⸦ ���� )")));

	m_Scripts.emplace_back(Scrpit);
}

void CScriptManager::Ready_Script_7()
{
	map<_int, SCRIPT_INFO> Scrpit = {};
	_int iIndex = 0;
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("( ���� ũ����Ʈ......\\����� ��ä������ �繫�ҿ���. )")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("( ����� ���� ���� �ٹ̴��� �� �� ���� �̻�,\\�����ؼ� ���� �� ������...... )")));

	m_Scripts.emplace_back(Scrpit);
}

void CScriptManager::Ready_Script_8()
{
	map<_int, SCRIPT_INFO> Scrpit = {};
	_int iIndex = 0;
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("( �̳༮��, �ٷ� �����ؿ��ٴ� ���� �ӻ�����. )")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("( �̰��� ������ ����......\\���� �������� ���η� ���� �ϴ� �ǰ�...... )")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("( ����� ���� ���� �������� ���η� ������. )")));

	m_Scripts.emplace_back(Scrpit);
}

void CScriptManager::Ready_Script_9()
{
	map<_int, SCRIPT_INFO> Scrpit = {};
	_int iIndex = 0;
	// Ű���� �������� ���߸鼭 ������. ���� �Ӹ� �����������
	// ���� �������
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("����"), TEXT("ī�ڸ� ���Ե� ������ �༮�� ��ī��Ʈ �߱�.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("����"), TEXT("���� ������ �ߴ� ������ �ʿ� ���������\\���θ��Ͱ� �ǰ� ���� ������.")));

	// Ű�� �� ���߱�
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("����......���� �� � �� ���� �Ǳ� ���� ����.\\���� ���� �ٹ̰� �ִ� ����?")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("����"), TEXT("�ּ��� ������ ��δ±���.")));

	m_Scripts.emplace_back(Scrpit);
}

void CScriptManager::Ready_Script_10()
{
	map<_int, SCRIPT_INFO> Scrpit = {};
	_int iIndex = 0;
	
	// ���� �� �Ʒ� -> �� ��
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("����"), TEXT("�ؿ� �ִ� ���� ���� ��� ���п�\\������ �ϳ� ����......")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("����"), TEXT("�������� ����ƺ��� ����, Ű��")));

	m_Scripts.emplace_back(Scrpit);
}

CScriptManager* CScriptManager::Create()
{
	CScriptManager* pInstance = new CScriptManager();

	if (FAILED(pInstance->Initialize()))
		Safe_Release(pInstance);

	return pInstance;
}

void CScriptManager::Free()
{
	for (auto& Script : m_Scripts)
	{
		Script.clear();
	}
}
