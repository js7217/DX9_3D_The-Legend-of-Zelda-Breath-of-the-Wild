#include "SoundMgr.h"

IMPLEMENT_SINGLETON(CSoundMgr)

CSoundMgr::CSoundMgr()
	: m_pSystem(nullptr)
{
}

HRESULT CSoundMgr::Ready_SoundManger()
{
	FMOD_System_Create(&m_pSystem);
	FMOD_System_Init(m_pSystem, 32, FMOD_INIT_NORMAL, NULL);
	FMOD_System_CreateChannelGroup(m_pSystem, "Master", &m_pMasterGroup);

	if (FAILED(Load_SoundFiles()))
		return E_FAIL;

	return S_OK;
}

void CSoundMgr::Play_Sound(const _tchar * pSoundKey, SOUNDID eID)
{
	auto& iter_find = find_if(m_MapSound.begin(), m_MapSound.end(),
		[&pSoundKey](auto& MyPair)
	{
		return !lstrcmp(pSoundKey, MyPair.first);
	});

	if (m_MapSound.end() == iter_find)
		return;

	FMOD_BOOL isPlay;
	if (FMOD_Channel_IsPlaying(m_pChannel[eID], &isPlay))
	{
		FMOD_System_PlaySound(m_pSystem, iter_find->second, m_pMasterGroup, FALSE,
			&m_pChannel[eID]);
	}

	//FMOD_System_Update(m_pSystem);
}

void CSoundMgr::Play_Instance_Sound(const _tchar * pSoundKey, SOUNDID eID)
{
	auto& iter_find = find_if(m_MapSound.begin(), m_MapSound.end(),
		[&pSoundKey](auto& MyPair)
	{
		return !lstrcmp(pSoundKey, MyPair.first);
	});

	if (m_MapSound.end() == iter_find)
		return;

	FMOD_System_PlaySound(m_pSystem, iter_find->second, m_pMasterGroup, FALSE,
		&m_pChannel[eID]);
	//FMOD_System_Update(m_pSystem);
}

void CSoundMgr::Play_3D_Sound(const _tchar * pSoundKey, SOUNDID eID, NxVec3 nxPos)
{
	_float fDist = m_nxPlayerPos.distance(nxPos);

	if (fDist > 20.f)
	{
		//FMOD_System_Update(m_pSystem);
		return;
	}

	auto& iter_find = find_if(m_MapSound.begin(), m_MapSound.end(),
		[&pSoundKey](auto& MyPair)
	{
		return !lstrcmp(pSoundKey, MyPair.first);
	});

	if (m_MapSound.end() == iter_find)
		return;
	FMOD_BOOL isPlay;
	if (FMOD_Channel_IsPlaying(m_pChannel[eID], &isPlay))
	{
		FMOD_System_PlaySound(m_pSystem, iter_find->second, m_pMasterGroup, FALSE,
			&m_pChannel[eID]);
	}

	if (fDist < 20.f && fDist > 16.f)
		FMOD_Channel_SetVolume(m_pChannel[eID], 0.2f);
	else if (fDist < 16.f && fDist > 12.f)
		FMOD_Channel_SetVolume(m_pChannel[eID], 0.35f);
	else if (fDist < 12.f && fDist > 8.f)
		FMOD_Channel_SetVolume(m_pChannel[eID], 0.5f);
	else if (fDist < 8.f && fDist > 4.f)
		FMOD_Channel_SetVolume(m_pChannel[eID], 0.65f);
	else if ((fDist < 4.f))
		FMOD_Channel_SetVolume(m_pChannel[eID], 0.8f);

	//FMOD_System_Update(m_pSystem);
}

void CSoundMgr::Play_BGM(const _tchar * pSoundKey)
{
	auto& iter_find = find_if(m_MapSound.begin(), m_MapSound.end(),
		[&pSoundKey](auto& MyPair)
	{
		return !lstrcmp(pSoundKey, MyPair.first);
	});

	if (m_MapSound.end() == iter_find)
		return;

	FMOD_BOOL isPlay;
	if (FMOD_Channel_IsPlaying(m_pChannel[BGM], &isPlay))
	{
		FMOD_System_PlaySound(m_pSystem, iter_find->second, m_pMasterGroup, FALSE,
			&m_pChannel[BGM]);
	}

	FMOD_Channel_SetMode(m_pChannel[BGM], FMOD_LOOP_NORMAL);
	//FMOD_System_Update(m_pSystem);
}

void CSoundMgr::Play_Loop_Sound(const _tchar * pSoundKey, NxVec3 nxPos, _uint iStart, _uint iEnd, SOUNDID eID)
{
	_float fDist = m_nxPlayerPos.distance(nxPos);

	if (fDist > 20.f)
	{
		FMOD_Channel_Stop(m_pChannel[eID]);
		return;
	}


	auto& iter_find = find_if(m_MapSound.begin(), m_MapSound.end(),
		[&pSoundKey](auto& MyPair)
	{
		return !lstrcmp(pSoundKey, MyPair.first);
	});

	if (m_MapSound.end() == iter_find)
		return;
	
	_uint iLength = 0;
	//FMOD_Sound_GetLength(iter_find->second, &iLength, FMOD_TIMEUNIT_PCM);
	FMOD_Channel_GetPosition(m_pChannel[eID], &iLength, FMOD_TIMEUNIT_PCM);

	//FMOD_BOOL isPlay;
	//if (FMOD_Channel_IsPlaying(m_pChannel[FIELD_LOOP], &isPlay))
	//{
	//	FMOD_System_PlaySound(m_pSystem, iter_find->second, m_pMasterGroup, FALSE,
	//		&m_pChannel[FIELD_LOOP]);
	//	FMOD_Channel_SetPosition(m_pChannel[FIELD_LOOP], iStart, FMOD_TIMEUNIT_PCM);
	//}

	if (iLength >= iEnd)
		FMOD_Channel_SetPosition(m_pChannel[eID], iStart, FMOD_TIMEUNIT_PCM);

	FMOD_BOOL isPlay;
	if (iLength == 0 && FMOD_Channel_IsPlaying(m_pChannel[FIELD_LOOP], &isPlay))
	{
		FMOD_System_PlaySound(m_pSystem, iter_find->second, m_pMasterGroup, FALSE,	&m_pChannel[eID]);
		FMOD_System_Update(m_pSystem);
	}
	
	if (fDist < 20.f && fDist > 16.f)
		FMOD_Channel_SetVolume(m_pChannel[eID], 0.2f);
	else if (fDist < 16.f && fDist > 12.f)
		FMOD_Channel_SetVolume(m_pChannel[eID], 0.35f);
	else if (fDist < 12.f && fDist > 8.f)
		FMOD_Channel_SetVolume(m_pChannel[eID], 0.5f);
	else if (fDist < 8.f && fDist > 4.f)
		FMOD_Channel_SetVolume(m_pChannel[eID], 0.65f);
	else if ((fDist < 4.f))
		FMOD_Channel_SetVolume(m_pChannel[eID], 0.8f);

	//FMOD_Channel_SetLoopPoints(m_pChannel[FIELD_LOOP], iStart, FMOD_TIMEUNIT_PCM, iEnd, FMOD_TIMEUNIT_PCM);
}

void CSoundMgr::Set_Loop_Channel(_uint iStart, _uint iEnd)
{
	FMOD_Channel_SetLoopPoints(m_pChannel[FIELD_LOOP], iStart, FMOD_TIMEUNIT_PCM, iEnd, FMOD_TIMEUNIT_PCM);
}

void CSoundMgr::Set_Volume_Channel(SOUNDID eID, float fVol)
{
	FMOD_Channel_SetVolume(m_pChannel[eID], fVol);
	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::Set_Volume_AllChannel(float fVol)
{
	for (_uint i = 0; i < MAX_CHENNEL; ++i)
	{
		if (i == BGM)
			continue;

		FMOD_Channel_SetVolume(m_pChannel[i], fVol);
	}
	FMOD_System_Update(m_pSystem);

}


void CSoundMgr::Stop_SoundChannel(SOUNDID eID)
{
	FMOD_Channel_Stop(m_pChannel[eID]);
	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::Stop_AllChannel()
{
	for (int i = 0; i < MAX_CHENNEL; ++i)
		FMOD_Channel_Stop(m_pChannel[i]);

	FMOD_System_Update(m_pSystem);
}

HRESULT CSoundMgr::Load_SoundFiles()
{
	_finddata_t fd;
	intptr_t Handle = _findfirst("../../Resources/Sound/*.*", &fd);

	if (0 == Handle)
		return E_FAIL;

	int iResult = 0;
	iResult = _findnext(Handle, &fd);

	char szCurPath[256] = "../../Resources/Sound/";
	char szFullPath[256] = "";

	while (iResult != -1)
	{
		strcpy_s(szFullPath, szCurPath);
		strcat_s(szFullPath, fd.name);

		FMOD_SOUND* pSound = nullptr;

		FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_DEFAULT, 0, &pSound);

		if (FMOD_OK == eRes)
		{
			int iLength = int(strlen(fd.name) + 1);

			TCHAR* pSoundKey = new TCHAR[iLength];
			ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);
			MultiByteToWideChar(CP_ACP, 0, fd.name, iLength, pSoundKey, iLength);


			m_MapSound.emplace(pSoundKey, pSound);

		}
		iResult = _findnext(Handle, &fd);
	}

	FMOD_System_Update(m_pSystem);

	return S_OK;
}

void CSoundMgr::Free()
{
	for_each(m_MapSound.begin(), m_MapSound.end(), [](auto& MyPair)
	{
		delete[] MyPair.first;
		FMOD_Sound_Release(MyPair.second);
	});
	m_MapSound.clear();

	FMOD_System_Release(m_pSystem);
	FMOD_System_Close(m_pSystem);
}
