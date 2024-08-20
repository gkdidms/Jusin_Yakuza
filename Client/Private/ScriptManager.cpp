#include "ScriptManager.h"

CScriptManager::CScriptManager()
{
}

const CScriptManager::SCRIPT_INFO CScriptManager::Get_Script(_uint iIndex, _int iScriptIndex)
{
	map<_int, SCRIPT_INFO>& Scripts = m_Scripts[iIndex];

	auto iter = Scripts.find(iScriptIndex);

	if (iter == Scripts.end())
		return SCRIPT_INFO(TEXT("끝"), TEXT("종료"));
	
	return iter->second;
}

HRESULT CScriptManager::Initialize()
{
	//대사 작성하기
	Ready_Script_1();
	Ready_Script_2();
	Ready_Script_3();
	Ready_Script_4();
	Ready_Script_5();
	Ready_Script_6();
	Ready_Script_7();

	return S_OK;
}

void CScriptManager::Ready_Script_1()
{
	map<_int, SCRIPT_INFO> Scrpit_1 = {};
	_int iIndex = 0;

	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("( ...니시키와 만나기로 했던 곳,\\ 이 근처일 텐데 녀석은 아직 안온 건가 ? )")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("( [724106(뭐하고 있어 ? )]\\내가 묻고 싶은 말이야......) ")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("( 어쩔 수 없지, 니시키를 찾아서 좀 돌아다녀 볼까 )")));

	m_Scripts.emplace_back(Scrpit_1);
}

void CScriptManager::Ready_Script_2()
{
	map<_int, SCRIPT_INFO> Scrpit = {};
	_int iIndex = 0;
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("늦었잖아. 왜 이렇게 늦게온거야?")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("잠깐 앞에서 시비가 걸려서 말이야.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("너도 참 여전하구나.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("그건 그렇고, 너도 제법 반듯해졌군.\\이미 훌륭한 야쿠자야. 오늘 수금도 이미 끝난 거지 ? ")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("뭐 그렇지....")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("카자마 어르신이 빵에서 나오는 건 내년 이니까,\\ 그때까지 나도 조금은 요령을 터득해 둬야겠지.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("헤에.... 그럼 먼저 그 후줄근한 외모부터 바꿔봐.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("응?")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("너도 사실은 알고 있지? 사람들은 차의 엠블럼이나 입고 있는 옷 브랜드,\\ 그리고 이 도지마구미의 문장을 보고 솔직하게 말을 듣지.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("야쿠자는 “허세”가 최고야.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("그런 건가....")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("그런 거다. \\일반인 주제에 악덕업자 놈한테 빌린 돈으로 놀러 다니는 바보....")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("이 거리의 놈들한테 사람의 내면 같은 것은 보이지 않아.\\놈들에게 있어선 겉모습이 전부라고.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("칫... 알았다. 그만 됐어. 그것보다 마시러 가자면서?")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("그랬었지. ...좋아, 가끔은 핑크 골목이라도 갈까?")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("오늘은 내가 쏜다! 너도 가끔은 즐기라고! 알았냐!?")));

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
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("너, 이 가게는 자주 오냐?")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("응? 이걸로 2번째인데... 왜?")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("보틀 키핑 같은 걸 하고 있으니까. 단골인가 했지.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("맥주나 깔짝대는 것보다 허세가 있잖아?")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("가게 사람들한테 얼굴도 알릴 수 있고.\\난 저렴하게 보이는 건 싫으니까.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("오늘은 진지한 이야기는 하지말자! 가라오케라도 해서 질러 보자고!")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("훗, 알았어. 오늘은 끝까지 달려주지.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("가라오케, Let's Go!")));

	m_Scripts.emplace_back(Scrpit);
}

void CScriptManager::Ready_Script_5()
{
	map<_int, SCRIPT_INFO> Scrpit = {};
	_int iIndex = 0;
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("그러고보니 최근에 쿠제랑 마찰이 있었다며.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("아아......")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("어쩌다가 쿠제와 엮였는지 모르겠지만, 조심하라고.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("쿠제가 널 찾고 있어.\\카자마 어르신과 관련된 일로 찾는 것일수 있으니 찾아가 보는게 좋겠어.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("동흥 크레디트로 가봐.\\그쪽으로 가면 쿠제에 관한 얘기를 들을 수 있을꺼야.")));

	m_Scripts.emplace_back(Scrpit);
}

void CScriptManager::Ready_Script_6()
{
	map<_int, SCRIPT_INFO> Scrpit = {};
	_int iIndex = 0;
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("( [동흥 크레디트]로 가보라고 했지...... )")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("( 가서 쿠제에 관한 얘기를 듣자 )")));

	m_Scripts.emplace_back(Scrpit);
}

void CScriptManager::Ready_Script_7()
{
	map<_int, SCRIPT_INFO> Scrpit = {};
	_int iIndex = 0;
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("( 이곳인가...... )")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("( 분명 적들이 있을거다.\\ 조심해서 들어가보자. )")));

	m_Scripts.emplace_back(Scrpit);
}

void CScriptManager::Ready_Script_8()
{
	map<_int, SCRIPT_INFO> Scrpit = {};
	_int iIndex = 0;

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
