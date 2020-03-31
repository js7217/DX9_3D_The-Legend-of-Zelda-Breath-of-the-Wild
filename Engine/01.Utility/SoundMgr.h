#pragma once

#include "Base.h"

BEGIN(ENGINE)

class ENGINE_DLL CSoundMgr :	 public CBase
{
	DECLARE_SINGLETON(CSoundMgr)

public:
	enum SOUNDID
	{
		BGM, EFFECT, ENVIROMENT, ENVIROMENT_WIND,
		LINK_FOOTSTEP,
		LINK_ATTACK0, LINK_ATTACK1,
		LINK_SHOUT0, LINK_SHOUT1,
		LINK_JUMP, LINK_BOMB, LINK_BOWDRAW, LINK_BOWRELEASE,
		LINK_ICEMAKE, LINK_ICEBREAK, LINK_TIMESTOP, LINK_MAGNET,
		DGN_DEFALULT, DGN_OBJECT0, DGN_OBJECT1, DGN_OBJECT2,
		FIELD_DEFAULT, FIELD_DOOR, FIELD_OBJECT0, FIELD_OBJECT1, FIELD_OBJECT2, FIELD_LOOP,
		UI_ITEMGET, UI_RUPEEGET,
		MAX_CHENNEL
	};

private:
	explicit CSoundMgr();
	virtual ~CSoundMgr() = default;

public:
	void Set_PlayerNxPos(NxVec3 nxPos) { m_nxPlayerPos = nxPos; }

public:
	HRESULT Ready_SoundManger();
	void Play_Sound(const _tchar* pSoundKey, SOUNDID eID);
	void Play_Instance_Sound(const _tchar* pSoundKey, SOUNDID eID);
	void Play_3D_Sound(const _tchar* pSoundKey, SOUNDID eID, NxVec3 nxPos);
	void Play_BGM(const _tchar* pSoundKey);
	void Play_Loop_Sound(const _tchar* pSoundKey, NxVec3 nxPos, _uint iStart, _uint iEnd, SOUNDID eID = FIELD_LOOP);
	void Set_Loop_Channel(_uint iStart, _uint iEnd);
	void Set_Volume_Channel(SOUNDID eID, float fVol);
	void Set_Volume_AllChannel(float fVol);
	void Stop_SoundChannel(SOUNDID eID);
	void Stop_AllChannel();
	void Update_SoundSystem() { FMOD_System_Update(m_pSystem); }

private:
	HRESULT Load_SoundFiles();

private:
	FMOD_SYSTEM*	m_pSystem;
	FMOD_CHANNEL*	m_pChannel[MAX_CHENNEL];
	FMOD_CHANNELGROUP* m_pMasterGroup;

	map<const TCHAR*, FMOD_SOUND*>	m_MapSound;
	typedef map<const TCHAR*, FMOD_SOUND*> SOUNDMAP;

private:
	NxVec3 m_nxPlayerPos;

public:
	virtual void Free();

};

END