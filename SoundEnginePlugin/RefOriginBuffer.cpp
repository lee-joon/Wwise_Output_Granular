#include "RefOriginBuffer.h"

RefOriginBuffer::RefOriginBuffer(AkUInt32 BufferLength)
	: m_BufferLength(BufferLength)

{
	m_OrginBuffer = new AkSampleType[m_BufferLength]{};
}

void RefOriginBuffer::SetOriginBuffer(AkSampleType* CopyBuffer, AkUInt16 BufferSize)
{
	AkUInt32 EndBuffer = m_Processorbuffer + BufferSize;
	if (EndBuffer > m_BufferLength)
	{
		memcpy(m_OrginBuffer + m_Processorbuffer,	CopyBuffer,											sizeof(AkSampleType) * (m_BufferLength - m_Processorbuffer));
		memcpy(m_OrginBuffer,						CopyBuffer + (m_BufferLength - m_Processorbuffer),	sizeof(AkSampleType) * (EndBuffer - m_BufferLength));
	}
	else
	{
		memcpy(m_OrginBuffer + m_Processorbuffer, CopyBuffer, sizeof(AkSampleType) * BufferSize);
	}

	m_Processorbuffer = EndBuffer % m_BufferLength;
}

void RefOriginBuffer::SetOriginBuffer(AkAudioBuffer* CopyBuffers, AkUInt16 BufferSize, AkUInt32 Channel)
{
	AkReal32 Average = 1.f / (AkReal32)Channel;
	AkUInt32 EndBuffer = m_Processorbuffer + BufferSize;
	if (EndBuffer > m_BufferLength)
	{
		AkUInt32 memsetSize = m_BufferLength - m_Processorbuffer;
		memset(m_OrginBuffer + m_Processorbuffer,	0,	sizeof(AkSampleType) * memsetSize);
		memset(m_OrginBuffer,						0,  sizeof(AkSampleType)* (EndBuffer - m_BufferLength));
	}
	else
	{
		memset(m_OrginBuffer + m_Processorbuffer, 0, sizeof(AkSampleType) * BufferSize);
	}
	
	for (int CN = 0; CN < Channel; ++CN)
	{
		AkSampleType* in_buffer = CopyBuffers->GetChannel(CN);
		for (AkUInt32 it = 0; it < BufferSize; ++it)
		{
			m_OrginBuffer[(m_Processorbuffer + it) % m_BufferLength] += in_buffer[it] * Average;
		}
	}
	m_Processorbuffer = EndBuffer % m_BufferLength;
}

AkSampleType* RefOriginBuffer::GetOriginBuffer()
{
	return m_OrginBuffer;
}

AkUInt32 RefOriginBuffer::GetBufferLength()
{
	return m_BufferLength;
}

AkUInt32 RefOriginBuffer::GetProcessorBuffer()
{
	return m_Processorbuffer;
}
