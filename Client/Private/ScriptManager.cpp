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

	return S_OK;
}

void CScriptManager::Ready_Script_1()
{
	map<_int, SCRIPT_INFO> Scrpit_1 = {};
	_int iIndex = 0;
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("��ä����"), TEXT("�� �׷�... ��Ź�� ���� ���?")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("�������ϴ�. ������ ���... \\�̴�� �����ڽ��ϱ�?")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("��ä����"), TEXT("�׷� �� �Ű� ���� �ʾƵ� ��.\\������ ��� ���� ���̴ϱ�")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("��ä����"), TEXT("���������̴� ���� ���� ������ ����.... \\�� �̰�. ����ߴ� 10%��. �����߾�")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("�����ϴ�.")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("��ä����"), TEXT("�׸��� �̰� �� �������� �����̴�.")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("�ƴ�, �װ� ...")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("��ä����"), TEXT("\"�ݹ�\"�� ����� ���� ���� �� �������� �����ݾ�. \\�����̶�� �������� �̷� �� �������� ȸ�� ��Ź���� ������.")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("������...")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("��ä����"), TEXT("����� �����̵��� �� �� ���� �Ѹ��鼭 �ý� ��� �ô뿡, \\������ �ڳװ� ���� ������ ����� �� ���ݾ�.")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("��ä����"), TEXT("��, �̰� ���ϰ��� ù ���� ���� ��� ���� �ž�. \\...�޾���.")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("...�����մϴ�.")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("��ä����"), TEXT("��, �׷��� �ٺ����� �� �����ָ� ������ �ʹ� ���������ٴϱ�.\\ 30���� ���� �׳� �����ص� ����� ������.")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("��ä����"), TEXT("ȸ���ص��� ������ �������μ� ���ô��� ���� �����ϱ�... �� �׷���?")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("...��, ...��.")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("��ä����"), TEXT("���� �� ���׿��� �� �������� ��ä���� �ϰ� ������ �߾���...\\ ���� ���� ���� ��¥ ��ġ�� �˰� �ִ� ���� ���� ����.")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("��ä����"), TEXT("������ ��� �� ���� �ڳ� ���� ���з��� �ּ��̺��� �ξ� �˰� �ִ� ������ ��µ�. �?")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("���� �� �𸣰ڽ��ϴ�. \\...�׷� �̸�.")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("��ä����"), TEXT("�̺�. ���� ���غ��� �ʰڳ� ...����. \\�ڳ� �Ƿ��̸� ������ �ϴ� �ͺ��� �ξ� ������ �� ���� ���� �� �� �־�.")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("��ä����"), TEXT("���� �� ��Ҹ� �԰� ���� ���ڵ� ������ ���� �� ����.\\�׻Ӹ��� �Ƴ�.���� ������ ������ �ΰ��� �ູ�ϰ� ����� �� ���� �־�.")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("��ä����"), TEXT("�����ڴ� �عٴ��� ���....\\�� ���� �⼼�ϰ� �ʹٸ�, ���ʿ� �ͼ� ���� ���� �͵� ���� �����̶�� �����ϴµ� ? ")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("3�� ���� �� ���� ������� �׷��� �������� �𸣰ڱ���.")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("������ �� �� �Ǵ��� �� �̻�... \\�ð��� �ǵ��� ���� �����ϴ�.")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("��ä����"), TEXT("�װ� ���... �ΰ�.")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("�����ڰ� �������� �ູ�ϰ� �ϴ� �� ����, �Ұ��� �ϰ���....")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("��ä����"), TEXT("��, �۽�.... �� �����ڰ� �� �� ��� �� �𸣰ڴ�.")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("��ä����"), TEXT("��, ������ �ٲ�� ������ ������. \\...�׷�.")));

	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("ĩ, ��ä�� ������... ¥�����±�.")));

	m_Scripts.emplace_back(Scrpit_1);
}

void CScriptManager::Ready_Script_2()
{
	map<_int, SCRIPT_INFO> Scrpit = {};
	_int iIndex = 0;
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("(...�Ͻ�Ű�� ������� �ߴ� ��,\\ �� ��ó�� �ٵ� �༮�� ���� �ȿ� �ǰ� ?)")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("([724106(���ϰ� �־� ? )]��� �ϴ� ������....\\�� �� ����.��� �Ͻ�Ű�� ã���� �ٳຼ��.)")));

	m_Scripts.emplace_back(Scrpit);
}

void CScriptManager::Ready_Script_3()
{
	map<_int, SCRIPT_INFO> Scrpit = {};
	_int iIndex = 0;
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("�Ͻ�Ű�߸�"), TEXT("�װ� �׷���, �ʵ� ���� �ݵ�������. �̹� �Ǹ��� �����ھ�.���� ���ݵ� �̹� ���� ���� ? ")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("�� �׷���....")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("�Ͻ�Ű�߸�"), TEXT("���� ī�ڸ� ��ŵ� ������ �Ƚ��Ͻð���?")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("��, �۽�. �ƽ��Ե� �� ���� �� �ο� �Ƿ¹ۿ� ����. ��ó�� ����� ���� ���ϴϱ�.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("�Ͻ�Ű�߸�"), TEXT("���� ���̳�. �̷� �ٺ� ���� �ô��ݾ�? ����� ������ ���ض�ϱ�.������ �Ӹ��� �����....")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("ī�ڸ� ����� ������ ������ �� ���� �ż���. �׶����� ���� ������ ����� �͵��� �־߰���.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("�Ͻ�Ű�߸�"), TEXT("�쿡.... �׷� ���� �� ���ٱ��� �ܸ���� �ٲ��.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("��?")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("�Ͻ�Ű�߸�"), TEXT("�ʵ� ������ 50���� ¥�� �纹�̶� �Ծ��. �׷��� ������ ���� ���� ������ �ٲ����.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("�� �� ���� �� �������. ī�ڸ� ��ŵ� �׻� �����ϰ� �԰�ٴϽ��ݾ�.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("�Ͻ�Ű�߸�"), TEXT("�� ���̾�. �� ����� ������ ��ī���ö�.... ������������ �ѹ� 2�ݾ� ? �� ��ġ�� ���� �׷��� ������.�Ǵ� ��翣 ��� ���� ���㼼���� �ʿ��ϴٰ�.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("�װ� �ֱ� �� �� �͵� �� ���㼼�������̳�?")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("�Ͻ�Ű�߸�"), TEXT("�� ������ �����͵� �� �ž�. ���� �׸� ������.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("���㼼�� �θ� �Ÿ� �����ϰ� ���� ��.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("�Ͻ�Ű�߸�"), TEXT("�ʵ� ����� �˰� ����? ������� ���� �����̳� �԰� �ִ� �� �귣��, �׸��� �� ������������ ������ ���� �����ϰ� ���� ����.�����ڴ� ���㼼���� �ְ��.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("�׷� �ǰ�....")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("�Ͻ�Ű�߸�"), TEXT("�׷� �Ŵ�. �Ϲ��� ������ �Ǵ����� ������ ���� ������ � �ٴϴ� �ٺ�....�� �Ÿ��� ������� ����� ���� ���� ���� ������ �ʾ�.��鿡�� �־ �Ѹ���� ���ζ��.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("ĩ... �˾Ҵ�. �׸� �ƾ�. �װͺ��� ���÷� ���ڸ鼭?")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("�Ͻ�Ű�߸�"), TEXT("�׷�����. ...����, ������ ��ũ ����̶� ����? �� ��ó�� ���� ���԰� �־�.")));

	m_Scripts.emplace_back(Scrpit);
}

void CScriptManager::Ready_Script_4()
{
	map<_int, SCRIPT_INFO> Scrpit = {};
	_int iIndex = 0;
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("�Ͻ�Ű�߸�"), TEXT("��, �� ��ó ���� ���� �� �ֳ�?")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("�Ͻ�Ű�߸�"), TEXT("���� �� ���� �� �����ؼ� ������ �˾Ƶ־� ��.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("�Ͻ�Ű�߸�"), TEXT("�� �Ƴ��鼭 ������ �͵� ������ �̷� �����̾�.���� ���⿡ ���� �� �谡 �Ǳ⵵ ��.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("�Ͻ�Ű�߸�"), TEXT("��·�� ���� ������ �������� �Ÿ����� ��鼭 ���׳��� �� �־���.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("�Ͻ�Ű�߸�"), TEXT("�� ���׿��� �����̶�°� �׷� �ž�.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("�Ͻ�Ű�߸�"), TEXT("�Ϳ��� ����� �����ⷡ�� ���� ���� ���ܸ���. �� 5000���̶���� �ٰ��� ����.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("�Ͻ�Ű�߸�"), TEXT("�׷� �͵� �� ���ζ�� �����ϸ� ��.���ʿ� �׷��� �������� ������ ��Ƴ��� �� ���ٴϱ�!? ")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("�Ͻ�Ű�߸�"), TEXT("...����. ��, ���� �� ���� �� �Ű�, ���� ���� ������ ü���� ��....")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("�Ͻ�Ű�߸�"), TEXT("�� ��·�� ������ ���� ���!�ʵ� ������ �����!�˾ҳ�!? ")));

	m_Scripts.emplace_back(Scrpit);
}

void CScriptManager::Ready_Script_5()
{
	map<_int, SCRIPT_INFO> Scrpit = {};
	_int iIndex = 0;
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("��, �� ���Դ� ���� ����?")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("�Ͻ�Ű�߸�"), TEXT("��? �̰ɷ� 2��°�ε�... ��?")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("��Ʋ Ű�� ���� �� �ϰ� �����ϱ�. �ܰ��ΰ� ����.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("�Ͻ�Ű�߸�"), TEXT("���ֳ� ��¦��� �ͺ��� �㼼�� ���ݾ�? ���� ��������� �󱼵� �˸� �� �ְ�.�� �����ϰ� ���̴� �� �����ϱ�.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("��, �׷���. �� �����. Ȯ���� ���㼼����� �� �߿��ϰ� ���� �־�.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("�Ͻ�Ű�߸�"), TEXT("�ʵ� �� ���迡�� �ö󰡰� ������ ������ ����ֶ�.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("...")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("�Ͻ�Ű�߸�"), TEXT("�� �׷��� �� �׷� Ÿ���� �ƴ϶�� �ǰ�?")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("��... ����� �ڸ� �ѾƼ� �� ���迡 ���Դ�. ��ƿ��� �츮�� �ŵ��� �� ���� ���ؼ�... �� ���� ������ �����ۿ� �ɷ��� ����.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("�Ͻ�Ű�߸�"), TEXT("�ƴ�, Ȥ�ó� �� �̴�ΰ� �������� ����.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("�Ͻ�Ű�߸�"), TEXT("�װ� �ؿ��� ������ �ؼ� ī�ڸ� ����� �⼼�ϸ� �����׵� �⼼�� ��ȸ�� ������ ����.�׷��� �ö󰡴� ����� ����.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("�Ͻ�Ű�߸�"), TEXT("������ �� ī�ڸ� ����� ���ϰ� �ƴϾ�.������������ �������̴�.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("�Ͻ�Ű�߸�"), TEXT("����� �� �츮�� ī�ڸ����̿� �־����� �ʾ����� ? �׾� ���������� ���� ���� ������.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("�׸���... ��� ������� ������ �����ð���.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("�Ͻ�Ű�߸�"), TEXT("�ƾ�, �׷���. ����! ������ �̾߱�� �׸�����! ������ɶ� �ؼ� ���� ���ڰ�!")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("��, �˾Ҿ�. ������ ������ �޷�����.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("�������, Let's Go")));

	m_Scripts.emplace_back(Scrpit);
}

void CScriptManager::Ready_Script_6()
{
	map<_int, SCRIPT_INFO> Scrpit = {};
	_int iIndex = 0;
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("Ű��"), TEXT("��, �� ���Դ� ���� ����?")));

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
