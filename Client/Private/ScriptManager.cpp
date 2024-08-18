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
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("이제 카자마 어르신도 조금은 안심하시겠지?")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("훗, 글쎄. 아쉽게도 난 믿을 게 싸움 실력밖에 없어.\\ 너처럼 요령이 좋지 못하니까.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("무슨 말이냐. 이런 바보 같은 시대잖아?\\ 즐기지 않으면 손해라니까.조금은 머리를 쓰라고....")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("카자마 어르신이 빵에서 나오는 건 내년 돼서야. 그때까지 나도 조금은 요령을 터득해 둬야겠지.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("헤에.... 그럼 먼저 그 후줄근한 외모부터 바꿔봐.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("응?")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("너도 가끔은 50만엔 짜리 양복이라도 입어봐. 그러면 세상이 보는 눈도 조금은 바뀌겠지.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("난 옷 같은 건 상관없어. 카자마 어르신도 항상 수수하게 입고다니시잖아.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("너 말이야. 그 사람은 조직의 와카가시라.... 도지마구미의 넘버 2잖아 ? 그 위치에 가면 그래도 통하지.건달 장사엔 어느 정도 “허세”가 필요하다고.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("네가 최근 차 산 것도 그 “허세”때문이냐?")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("이 괜찮은 라이터도 내 거야. 이제 그만 돌려줘.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("“허세” 부릴 거면 쩨쩨하게 굴지 마.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("너도 사실은 알고 있지? 사람들은 차의 엠블럼이나 입고 있는 옷 브랜드, 그리고 이 도지마구미의 문장을 보고 솔직하게 말을 듣지.야쿠자는 “허세”가 최고야.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("그런 건가....")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("그런 거다. 일반인 주제에 악덕업자 놈한테 빌린 돈으로 놀러 다니는 바보....이 거리의 놈들한테 사람의 내면 같은 것은 보이지 않아.놈들에게 있어선 겉모습이 전부라고.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("칫... 알았다. 그만 됐어. 그것보다 마시러 가자면서?")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("그랬었지. ...좋아, 가끔은 핑크 골목이라도 갈까? 그 근처에 좋은 가게가 있어.")));

	m_Scripts.emplace_back(Scrpit);
}

void CScriptManager::Ready_Script_3()
{
	map<_int, SCRIPT_INFO> Scrpit = {};
	_int iIndex = 0;

	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("너, 이 근처 가게 가본 적 있냐?")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("좋은 곳 나쁜 곳 포함해서 뭐든지 알아둬야 해.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("돈 아끼면서 모으는 것도 좋지만 이런 세상이야.돈은 쓰기에 따라 몇 배가 되기도 해.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("어쨌든 그쪽 정보를 얻으려면 거리에서 놀면서 안테나를 쳐 둬야지.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("이 동네에서 돈벌이라는건 그런 거야.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("귀여운 여대생 골라잡기래서 따라가 보면 아줌마뿐. 딱 5000엔이라더니 바가지 쓰고.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("그런 것도 다 공부라고 생각하면 돼.애초에 그렇게 생각하지 않으면 살아남을 수 없다니까!? ")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("...뭐야. 지, 지금 건 예를 든 거고, 별로 내가 실제로 체험한 건....")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("뭐 어쨌든 오늘은 내가 쏜다!너도 가끔은 즐기라고!알았냐!? ")));

	m_Scripts.emplace_back(Scrpit);
}

void CScriptManager::Ready_Script_4()
{
	map<_int, SCRIPT_INFO> Scrpit = {};
	_int iIndex = 0;
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("너, 이 가게는 자주 오냐?")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("응? 이걸로 2번째인데... 왜?")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("보틀 키핑 같은 걸 하고 있으니까. 단골인가 했지.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("맥주나 깔짝대는 것보다 허세가 있잖아? 가게 사람들한테 얼굴도 알릴 수 있고.난 저렴하게 보이는 건 싫으니까.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("훗, 그렇군. 넌 대단해. 확실히 “허세”라는 걸 중요하게 보고 있어.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("너도 이 세계에서 올라가고 싶으면 조금은 배워둬라.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("...")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("뭐 그래도 넌 그런 타입은 아니라는 건가?")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("난... 어르신의 뒤를 쫓아서 이 세계에 들어왔다. 고아였던 우리를 거둬준 그 분을 위해서... 내 몸을 던지는 정도밖에 능력이 없어.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("아니, 혹시나 넌 이대로가 좋을지도 모르지.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("네가 밑에서 열심히 해서 카자마 어르신이 출세하면 너한테도 출세의 기회가 올지도 몰라.그렇게 올라가는 방법도 있지.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("하지만 넌 카자마 어르신의 부하가 아니야.도지마구미의 조직원이다.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("어르신은 왜 우리를 카자마구미에 넣어주지 않았을까 ? 그야 도지마구미 쪽이 격은 위지만.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("그만해... 어르신 나름대로 생각이 있으시겠지.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("아아, 그렇군. 좋아! 진지한 이야기는 그만하자! 가라오케라도 해서 질러 보자고!")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("훗, 알았어. 오늘은 끝까지 달려주지.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("가라오케, Let's Go")));

	m_Scripts.emplace_back(Scrpit);
}

void CScriptManager::Ready_Script_5()
{
	map<_int, SCRIPT_INFO> Scrpit = {};
	_int iIndex = 0;


	m_Scripts.emplace_back(Scrpit);
}

void CScriptManager::Ready_Script_6()
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
