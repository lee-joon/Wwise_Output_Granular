#pragma once
#include <AK/SoundEngine/Common/IAkPlugin.h>
#include <vector>
#include "NoiseGenerator.h"
#include "RefOriginBuffer.h"
using namespace std;

class outputBufferManager;

struct GranularManager
{

	enum class In_ObjState;

	~GranularManager();

	vector<outputBufferManager>	 m_outpuBufferManager;	// � ���� �ߴ�?

	RefOriginBuffer* m_RefOriginBuffer;// ���� �Ҵ�����~

	//���� ������ ����� �ƿ�ǲ�� ���������?
	AkUInt32 m_NumChanOut;

	// �ؿ� �ִ� �����µ��� ���� �� �� �ְ�... ��������.
	AkUInt32 m_DistroiedCounter = 0;
	In_ObjState m_state = In_ObjState::ALIVE;

	AkUInt32 m_numInObj;



	enum class In_ObjState
	{
		ALIVE,
		DEATH
	};


	bool IsOkDestroied();
	void MatchChannels();

	GranularManager::In_ObjState GetState() const;
	void SetState(GranularManager::In_ObjState state);

	AkUInt32 GetNumChanOut() const;
	void SetNumChanOut(AkUInt32 outnum);

	void DistroiedCount();

	void Push_Back_Manager(AkAudioObjectID key, AkUInt32 GranularLength, AkUInt32 MinGranularLength);
	void SetRefOriginBuffer(RefOriginBuffer* RefOriginBuffer);

	vector<outputBufferManager>* GetManager();
	RefOriginBuffer* GetRefBuffer();

	void ClearBuffer();
};


class outputBufferManager
{
private:
	

	AkAudioObjectID m_key;

	AkSampleType* m_ObjOriginBuffer = nullptr;
	// ���� � ���۸� ���� �ϰ� �ֳ�
	AkUInt32 m_GranularPointer = 0;
	// �� ���� �ڿ� ������ �� �� ����?
	AkUInt32 m_Timer = 0;

	//��ŭ ���� �Ұ�����?
	AkUInt32 m_GranularLength;
	//�ּ� ���� ���̴� ������?
	AkUInt32 m_MinGranularLength;
	AkUInt32 m_ResultLength = 0;

	RefOriginBuffer* m_orginBuffer = nullptr;
	GranularManager* m_MyManager = nullptr;
	

public:
	outputBufferManager(AkAudioObjectID key, AkUInt32 GranularLength, AkUInt32 MinGranularLength);
	enum class GranularState
	{
		ENPTY,
		READY,
		RENDERING,
		DESTROIED
	};
	GranularState m_state;
	static NoiseGenerator Noise;

	bool ProcessorRendering(AkSampleType* OutBuffer, AkUInt16 BufferSize, AKRESULT& out_result);

	void SetRefOriginBuffer(RefOriginBuffer* orginBuffer);
	void SetMyManager(GranularManager* MyManager);
	AkSampleType* GetObjOriginBuffer();
	AkAudioObjectID GetKey();
};

