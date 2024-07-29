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
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("사채업자"), TEXT("어 그래... 부탁한 일은 어땠어?")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("끝났습니다. 봉투가 없어서... \\이대로 괜찮겠습니까?")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("사채업자"), TEXT("그런 거 신경 쓰지 않아도 돼.\\봉투가 없어도 돈은 돈이니까")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("사채업자"), TEXT("도지마구미는 일이 빨리 끝나서 좋아.... \\자 이거. 약속했던 10%다. 수고했어")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("고맙습니다.")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("사채업자"), TEXT("그리고 이건 내 개인적인 마음이다.")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("아니, 그건 ...")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("사채업자"), TEXT("\"반반\"이 상식인 요즘 세상에 싼 가격으로 해줬잖아. \\보통이라면 현역한테 이런 싼 가격으로 회수 부탁하지 못하지.")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("하지만...")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("사채업자"), TEXT("동년배 꼬맹이들이 만 엔 지폐 뿌리면서 택시 잡는 시대에, \\현역인 자네가 돈도 없으면 모양이 안 나잖아.")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("사채업자"), TEXT("뭐, 이건 나하고의 첫 일을 끝낸 기념 같은 거야. \\...받아줘.")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("...감사합니다.")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("사채업자"), TEXT("훗, 그런데 바보한테 돈 빌려주면 뒷일이 너무 귀찮아진다니까.\\ 30만엔 따위 그냥 무시해도 상관은 없지만.")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("사채업자"), TEXT("회수해두지 않으면 장사꾼으로서 무시당할 수도 있으니까... 안 그런가?")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("...예, ...뭐.")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("사채업자"), TEXT("나도 이 동네에서 꽤 오랫동안 사채꾼을 하고 있으니 잘알지...\\ 요즘 세상에 돈의 진짜 가치를 알고 있는 놈은 거의 없어.")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("사채업자"), TEXT("오히려 배운 게 없는 자네 쪽이 고학력의 애송이보다 훨씬 알고 있는 느낌이 드는데. 어때?")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("저는 잘 모르겠습니다. \\...그럼 이만.")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("사채업자"), TEXT("이봐. 같이 일해보지 않겠나 ...형씨. \\자네 실력이면 야쿠자 하는 것보다 훨씬 간단히 더 많은 돈을 벌 수 있어.")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("사채업자"), TEXT("좋은 밥 배불리 먹고 예쁜 여자도 마음껏 안을 수 있지.\\그뿐만이 아냐.돈만 있으면 불행한 인간을 행복하게 만들어 줄 수도 있어.")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("사채업자"), TEXT("야쿠자는 밑바닥이 길어....\\더 빨리 출세하고 싶다면, 이쪽에 와서 돈을 버는 것도 좋은 선택이라고 생각하는데 ? ")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("3년 전에 그 말을 들었으면 그렇게 했을지도 모르겠군요.")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("하지만 한 번 건달이 된 이상... \\시간을 되돌릴 수는 없습니다.")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("사채업자"), TEXT("그게 대답... 인가.")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("야쿠자가 누군가를 행복하게 하는 것 따위, 불가능 하겠죠....")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("사채업자"), TEXT("훗, 글쎄.... 난 야쿠자가 된 적 없어서 잘 모르겠다.")));
	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("사채업자"), TEXT("뭐, 생각이 바뀌면 언제든 연락해. \\...그럼.")));

	Scrpit_1.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("칫, 사채꾼 주제에... 짜증나는군.")));

	m_Scripts.emplace_back(Scrpit_1);
}

void CScriptManager::Ready_Script_2()
{
	map<_int, SCRIPT_INFO> Scrpit = {};
	_int iIndex = 0;
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("(...니시키와 만나기로 했던 곳,\\ 이 근처일 텐데 녀석은 아직 안온 건가 ?)")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("([724106(뭐하고 있어 ? )]라고 하는 주제에....\\할 수 없지.잠깐 니시키를 찾으러 다녀볼까.)")));

	m_Scripts.emplace_back(Scrpit);
}

void CScriptManager::Ready_Script_3()
{
	map<_int, SCRIPT_INFO> Scrpit = {};
	_int iIndex = 0;
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("그건 그렇고, 너도 제법 반듯해졌군. 이미 훌륭한 야쿠자야.오늘 수금도 이미 끝난 거지 ? ")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("뭐 그렇지....")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("이제 카자마 어르신도 조금은 안심하시겠지?")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("훗, 글쎄. 아쉽게도 난 믿을 게 싸움 실력밖에 없어. 너처럼 요령이 좋지 못하니까.")));
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("니시키야마"), TEXT("무슨 말이냐. 이런 바보 같은 시대잖아? 즐기지 않으면 손해라니까.조금은 머리를 쓰라고....")));
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

void CScriptManager::Ready_Script_4()
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

void CScriptManager::Ready_Script_5()
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

void CScriptManager::Ready_Script_6()
{
	map<_int, SCRIPT_INFO> Scrpit = {};
	_int iIndex = 0;
	Scrpit.emplace(iIndex++, SCRIPT_INFO(TEXT("키류"), TEXT("너, 이 가게는 자주 오냐?")));

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
