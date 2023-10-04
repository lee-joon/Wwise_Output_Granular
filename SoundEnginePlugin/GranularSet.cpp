#include "GranularSet.h"

GranularManager::~GranularManager()
{
	ClearBuffer();
}

bool GranularManager::IsOkDestroied()
{
	if (m_NumChanOut = m_DistroiedCounter)
	{
		delete m_RefOriginBuffer;

		return true;
	}

	return false;
}

void GranularManager::MatchChannels()
{
	for (AkUInt32 outNum = 0; outNum < m_NumChanOut; ++outNum)
	{
		outputBufferManager* tempOffset = &(m_outpuBufferManager[outNum]);
		tempOffset->SetRefOriginBuffer(m_RefOriginBuffer);
		tempOffset->SetMyManager(this);
	}
}

GranularManager::In_ObjState GranularManager::GetState() const
{
	return m_state;
}

void GranularManager::SetState(GranularManager::In_ObjState state)
{
	m_state = state;
}

AkUInt32 GranularManager::GetNumChanOut() const
{
	return m_NumChanOut;
}

void GranularManager::SetNumChanOut(AkUInt32 outnum)
{
	m_NumChanOut = outnum;
}


void GranularManager::DistroiedCount()
{
	++m_DistroiedCounter;
}

void GranularManager::Push_Back_Manager(AkAudioObjectID key, AkUInt32 GranularLength, AkUInt32 MinGranularLength)
{
	m_outpuBufferManager.push_back(outputBufferManager(key, GranularLength, MinGranularLength));
}

void GranularManager::SetRefOriginBuffer(RefOriginBuffer* RefOriginBuffer)
{
	m_RefOriginBuffer = RefOriginBuffer;
}

vector<outputBufferManager>* GranularManager::GetManager()
{
	return &m_outpuBufferManager;
}

RefOriginBuffer* GranularManager::GetRefBuffer()
{
	return m_RefOriginBuffer;
}

void GranularManager::ClearBuffer()
{

	if (m_RefOriginBuffer->GetOriginBuffer() != nullptr)
	{
		delete[] m_RefOriginBuffer->GetOriginBuffer();
	}
	

	for (AkUInt32 i = 0; i < m_outpuBufferManager.size(); i++)
	{
		
		if (m_outpuBufferManager[i].GetObjOriginBuffer() != nullptr)
		{
			delete[] m_outpuBufferManager[i].GetObjOriginBuffer();
		}
	}
}


NoiseGenerator outputBufferManager::Noise = NoiseGenerator();

outputBufferManager::outputBufferManager(AkAudioObjectID key, AkUInt32 GranularLength, AkUInt32 MinGranularLength)
	: m_key(key)
	, m_GranularLength(GranularLength)
	, m_MinGranularLength(MinGranularLength)
	, m_state(GranularState::ENPTY)
{
}

bool outputBufferManager::ProcessorRendering(AkSampleType* OutBuffer, AkUInt16 BufferSize, AKRESULT& out_result)
{

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (m_state == GranularState::ENPTY)
	{
		if (m_MyManager->GetState() == GranularManager::In_ObjState::DEATH)
		{
			m_MyManager->DistroiedCount();
			out_result = AKRESULT::AK_NoMoreData;
			return false;
		}
		//얼마 뒤부터 
		m_Timer				= outputBufferManager::Noise.BLnextValue(24000, 100);
		//어디서부터
		m_GranularPointer	= 0;
		//얼마의 크기를 복사 할것인가??
		m_ResultLength		= outputBufferManager::Noise.BLnextValue(m_GranularLength, m_MinGranularLength);
		//복사할 크기만큼 동적 생성
		m_ObjOriginBuffer	= new AkSampleType[m_ResultLength]{};

		//복사 대상의 버퍼 길이는 얼마인지?
		AkUInt32 BufferLength = m_orginBuffer->GetBufferLength();

		//원본 버퍼에서 얼마나 떨어저서 복사 할 것인지?????????
		AkUInt32 StartBuffer = (BufferLength + outputBufferManager::Noise.BSnextValue(BufferLength, m_ResultLength)) % BufferLength;

		if ((StartBuffer + m_ResultLength) > BufferLength)
		{
			AkUInt32 Copylength = StartBuffer + m_ResultLength - BufferLength;

			memcpy(m_ObjOriginBuffer,									m_orginBuffer->GetOriginBuffer() + StartBuffer, sizeof(AkSampleType) * (BufferLength - StartBuffer));
			memcpy(m_ObjOriginBuffer + (BufferLength - StartBuffer),	m_orginBuffer->GetOriginBuffer(),				sizeof(AkSampleType) * Copylength);
		}
		else
		{
			memcpy(m_ObjOriginBuffer,									m_orginBuffer->GetOriginBuffer() + StartBuffer,	sizeof(AkSampleType) * m_ResultLength);
		}

		m_state = GranularState::READY;
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (m_state == GranularState::READY)
	{
		AkInt32 Timer = m_Timer - BufferSize;

		if (Timer > 0)
		{
			memset(OutBuffer, 0, sizeof(AkSampleType) * BufferSize);
			m_Timer = (AkUInt32)Timer;
			out_result = AKRESULT::AK_DataReady;// 요거는 어떨지??
			return false;
		}
		else
		{
			m_Timer;//얼마만큼 지나고
			AkUInt32 bufferSize = (AkUInt32)(-1 * Timer);//할당 할 수있는 buffer의 크기
			if (m_ResultLength > bufferSize)
			{
				memset(OutBuffer, 0, sizeof(AkSampleType) * BufferSize);
				memcpy(OutBuffer + m_Timer, m_ObjOriginBuffer + m_GranularPointer, sizeof(AkSampleType) * (bufferSize % BufferSize));

				AkUInt32 MaxFadeIn;

				if ((bufferSize % BufferSize) > 10)
				{
					MaxFadeIn = 10;
				}
				{
					MaxFadeIn = bufferSize % BufferSize;
				}

				for (AkUInt32 FadeIn = 0; FadeIn < MaxFadeIn; ++FadeIn)
				{
					AkSampleType* StartBuffer = OutBuffer + (m_Timer + FadeIn);
					*StartBuffer *= (float)FadeIn / MaxFadeIn;
				}
				m_GranularPointer += bufferSize;
				m_state = GranularState::RENDERING;
				out_result = AKRESULT::AK_DataReady;
				return true;

			}
			else
			{
				memset(OutBuffer, 0, sizeof(AkSampleType) * BufferSize);
				memcpy(OutBuffer + m_Timer, m_ObjOriginBuffer + m_GranularPointer, sizeof(AkSampleType) * m_ResultLength);
				delete[] m_ObjOriginBuffer;
				m_ObjOriginBuffer = nullptr;
				m_state = GranularState::ENPTY;

				if (m_MyManager->m_state == GranularManager::In_ObjState::ALIVE)
				{
					out_result = AKRESULT::AK_DataReady;
				}
				else
				{
					out_result = AKRESULT::AK_NoMoreData;
				}
				return true;
			}
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (m_state == GranularState::RENDERING)
	{
		//그다음 버퍼에 나머지를 다 체울수 있는지?
		AkUInt32 RestBuffer = m_ResultLength - m_GranularPointer;

		if (RestBuffer > BufferSize)
		{
			//memset(OutBuffer, 0, sizeof(AkSampleType) * BufferSize);
			memcpy(OutBuffer, m_ObjOriginBuffer + m_GranularPointer, sizeof(AkSampleType) * BufferSize);
			m_GranularPointer += BufferSize;
			out_result = AKRESULT::AK_DataReady;
			return false;
		}
		else
		{
			memset(OutBuffer, 0, sizeof(AkSampleType) * BufferSize);
			memcpy(OutBuffer, m_ObjOriginBuffer + m_GranularPointer, sizeof(AkSampleType) * RestBuffer);

			AkUInt32 MaxFadeOut;

			if (RestBuffer > 10)
			{
				MaxFadeOut = 10;
			}
			{
				MaxFadeOut = RestBuffer;
			}

			for (AkUInt32 FadeOut = MaxFadeOut; FadeOut > 0; --FadeOut)
			{
				AkSampleType* StartBuffer = OutBuffer + (RestBuffer - FadeOut);
				*StartBuffer *= (float)FadeOut / MaxFadeOut;
			}

			delete[] m_ObjOriginBuffer;
			m_ObjOriginBuffer = nullptr;
			m_state = GranularState::ENPTY;

			if (m_MyManager->m_state == GranularManager::In_ObjState::ALIVE)
			{
				out_result = AKRESULT::AK_DataReady;
				return false;
			}
			else
			{
				out_result = AKRESULT::AK_NoMoreData;
				return false;
			}
			
			
		}
	}
}
void outputBufferManager::SetMyManager(GranularManager* MyManager)
{
	m_MyManager = MyManager;
}

AkSampleType* outputBufferManager::GetObjOriginBuffer()
{
	return m_ObjOriginBuffer;
}

AkAudioObjectID outputBufferManager::GetKey()
{
	return m_key;
}

void outputBufferManager::SetRefOriginBuffer(RefOriginBuffer* orginBuffer)
{
	m_orginBuffer = orginBuffer;
}

