#pragma once
#include <AK/SoundEngine/Common/IAkPlugin.h>


class RefOriginBuffer
{
private:
	AkSampleType* m_OrginBuffer;

	//���� � ���۸� �����ϰ� �ִ��� üũ��
	AkUInt32 m_Processorbuffer = 0;

	// �� ������ ����
	AkUInt32 m_BufferLength;

public:
	RefOriginBuffer(AkUInt32 BufferLength);
	void SetOriginBuffer(AkSampleType* CopyBuffer, AkUInt16 m_BufferSize);
	void SetOriginBuffer(AkAudioBuffer* CopyBuffers, AkUInt16 m_BufferSize, AkUInt32 numObjs);

	AkSampleType* GetOriginBuffer();
	AkUInt32 GetBufferLength();
	AkUInt32 GetProcessorBuffer();

};
