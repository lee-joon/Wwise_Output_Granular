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

	vector<outputBufferManager>	 m_outpuBufferManager;	// 몇개 생성 했니?

	RefOriginBuffer* m_RefOriginBuffer;// 동적 할당하자~

	//렌덤 벨류로 몇게의 아웃풋을 만들었는지?
	AkUInt32 m_NumChanOut;

	// 밑에 있는 오프셋들이 참조 할 수 있게... 만들어야함.
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
	// 지금 어떤 버퍼를 참고 하고 있냐
	AkUInt32 m_GranularPointer = 0;
	// 몇 샘플 뒤에 렌더링 할 것 인지?
	AkUInt32 m_Timer = 0;

	//얼만큼 복사 할것인지?
	AkUInt32 m_GranularLength;
	//최소 복사 길이는 얼마인지?
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

