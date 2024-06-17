
#include "SoundMgr.h"

CSoundMgr::CSoundMgr()
{
	m_pSystem = nullptr;
}


CSoundMgr::~CSoundMgr()
{
	Free();
}

HRESULT CSoundMgr::Initialize()
{
	Ready_Sound();

	return S_OK;
}

void Engine::CSoundMgr::Ready_Sound()
{
	// ���带 ����ϴ� ��ǥ��ü�� �����ϴ� �Լ�
	FMOD_System_Create(&m_pSystem, FMOD_VERSION);
	// 1. �ý��� ������, 2. ����� ����ä�� �� , �ʱ�ȭ ���) 
	FMOD_System_Init(m_pSystem, 32, FMOD_INIT_NORMAL, NULL);


	LoadSoundFile();
}

_bool CSoundMgr::PlaySound_W(const wstring pSoundKey, CHANNELID eID, float fVolume)
{
	map<const wstring, FMOD_SOUND*>::iterator iter;
	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_mapSound.begin(), m_mapSound.end(),
		[&](auto& iter)->bool
		{
			return pSoundKey == iter.first;
		});

	if (iter == m_mapSound.end())
		return FMOD_ERR_FILE_NOTFOUND;

	FMOD_BOOL bPlay = FALSE;
	_bool isPlay = false;

	if (FMOD_Channel_IsPlaying(m_pChannelArr[eID], &bPlay))
	{
		FMOD_System_PlaySound(m_pSystem, iter->second, nullptr, FALSE, &m_pChannelArr[eID]);
		isPlay = true;
	}

	FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);

	FMOD_System_Update(m_pSystem);

	return isPlay;
}

void CSoundMgr::PlayBGM(const wstring pSoundKey, float fVolume)
{
	map<const wstring, FMOD_SOUND*>::iterator iter;

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)->bool
		{
			return pSoundKey == iter.first;
		});

	if (iter == m_mapSound.end())
		return;

	FMOD_System_PlaySound(m_pSystem, iter->second, nullptr,  FALSE, &m_pChannelArr[SOUND_BGM]);
	FMOD_Channel_SetMode(m_pChannelArr[SOUND_BGM], FMOD_LOOP_NORMAL);
	FMOD_Channel_SetVolume(m_pChannelArr[SOUND_BGM], fVolume);
	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::StopSound(CHANNELID eID)
{
	FMOD_Channel_Stop(m_pChannelArr[eID]);
}

void CSoundMgr::StopAll()
{
	for (int i = 0; i < SOUND_END; ++i)
		FMOD_Channel_Stop(m_pChannelArr[i]);
}

void CSoundMgr::SetChannelVolume(CHANNELID eID, float fVolume)
{
	FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);

	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::LoadSoundFile()
{
	_wfinddata64_t fd;
	__int64 handle = _wfindfirst64(L"../../Client/Bin/Resources/Sound/*.*", &fd);
	if (handle == -1 || handle == 0)
		return;

	int iResult = 0;

	char szCurPath[128] = "../../Client/Bin/Resources/Sound/";
	char szFullPath[128] = "";
	char szFilename[MAX_PATH];
	while (iResult != -1)
	{
		WideCharToMultiByte(CP_UTF8, 0, fd.name, -1, szFilename, sizeof(szFilename), NULL, NULL);
		strcpy_s(szFullPath, szCurPath);
		strcat_s(szFullPath, szFilename);
		FMOD_SOUND* pSound = nullptr;

		FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_3D|0x00000020 , 0, &pSound);
		if (eRes == FMOD_OK)
		{
			int iLength = strlen(szFilename) + 1;

			_tchar* pSoundKey = new _tchar[iLength];
			ZeroMemory(pSoundKey, sizeof(_tchar) * iLength);
			MultiByteToWideChar(CP_ACP, 0, szFilename, iLength, pSoundKey, iLength);

			m_mapSound.emplace(pSoundKey, pSound);
		}
		iResult = _wfindnext64(handle, &fd);
	}
	FMOD_System_Update(m_pSystem);
	_findclose(handle);
}

CSoundMgr* CSoundMgr::Create()
{
	CSoundMgr* pInstance = new CSoundMgr();

	if (FAILED(pInstance->Initialize()))
		Safe_Release(pInstance);

	return pInstance;
}

void Engine::CSoundMgr::Free()
{
	for (auto& Mypair : m_mapSound)
	{
		FMOD_Sound_Release(Mypair.second);
	}
	m_mapSound.clear();

	FMOD_System_Release(m_pSystem);
	FMOD_System_Close(m_pSystem);
}
