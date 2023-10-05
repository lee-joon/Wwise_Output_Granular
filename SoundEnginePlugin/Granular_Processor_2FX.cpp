/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

Apache License Usage

Alternatively, this file may be used under the Apache License, Version 2.0 (the
"Apache License"); you may not use this file except in compliance with the
Apache License. You may obtain a copy of the Apache License at
http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing, software distributed
under the Apache License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES
OR CONDITIONS OF ANY KIND, either express or implied. See the Apache License for
the specific language governing permissions and limitations under the License.

  Copyright (c) 2023 Audiokinetic Inc.
*******************************************************************************/

#include "Granular_Processor_2FX.h"
#include "../Granular_Processor_2Config.h"

#include <AK/AkWwiseSDKVersion.h>

AK::IAkPlugin* CreateGranular_Processor_2FX(AK::IAkPluginMemAlloc* in_pAllocator)
{
	return AK_PLUGIN_NEW(in_pAllocator, Granular_Processor_2FX());
}

AK::IAkPluginParam* CreateGranular_Processor_2FXParams(AK::IAkPluginMemAlloc* in_pAllocator)
{
	return AK_PLUGIN_NEW(in_pAllocator, Granular_Processor_2FXParams());
}

AK_IMPLEMENT_PLUGIN_FACTORY(Granular_Processor_2FX, AkPluginTypeEffect, Granular_Processor_2Config::CompanyID, Granular_Processor_2Config::PluginID)

Granular_Processor_2FX::Granular_Processor_2FX()
	: m_pParams(nullptr)
	, m_pAllocator(nullptr)
	, m_pContext(nullptr)
{
}

Granular_Processor_2FX::~Granular_Processor_2FX()
{
	//AkMixerInputMap<AkUInt64, GranularManager>::Iterator it = m_OutBuffer.Begin();
	//while (it != m_OutBuffer.End())
	//{
	//	it->pUserData->ClearBuffer();
	//}
}

AKRESULT Granular_Processor_2FX::Init(AK::IAkPluginMemAlloc* in_pAllocator, AK::IAkEffectPluginContext* in_pContext, AK::IAkPluginParam* in_pParams, AkAudioFormat& in_rFormat)
{
	m_pParams = (Granular_Processor_2FXParams*)in_pParams;
	m_pAllocator = in_pAllocator;
	m_pContext = in_pContext;

	m_OutBuffer.Init(in_pAllocator);
	m_BufferSize = 512;// in_rFormat.uBitsPerSample; //<-- 변경

	m_MaxLength = static_cast<AkUInt32>(m_pParams->RTPC.MaxLength);
	m_MinLength = static_cast<AkUInt32>(m_pParams->RTPC.MinLength);

	return AK_Success;
}

AKRESULT Granular_Processor_2FX::Term(AK::IAkPluginMemAlloc* in_pAllocator)
{
	AK_PLUGIN_DELETE(in_pAllocator, this);

	return AK_Success;
}

AKRESULT Granular_Processor_2FX::Reset()
{
	return AK_Success;
}

AKRESULT Granular_Processor_2FX::GetPluginInfo(AkPluginInfo& out_rPluginInfo)
{
	out_rPluginInfo.eType = AkPluginTypeEffect;
	out_rPluginInfo.bIsInPlace = false;
	out_rPluginInfo.bCanProcessObjects = true;
	out_rPluginInfo.uBuildVersion = AK_WWISESDK_VERSION_COMBINED;
	return AK_Success;
}

void Granular_Processor_2FX::Execute(
		const AkAudioObjects& in_objects,	///< Input objects and object buffers.
		const AkAudioObjects& out_objects	///< Output objects and object buffers.
	)
{
	//AkUInt16 uMaxFramesProcessed = 0;
	//for (AkUInt32 objIdx = 0; objIdx < in_objects.uNumObjects; ++objIdx)
	//{
	//	const AkUInt32 uNumChannels = in_objects.ppObjectBuffers[objIdx]->NumChannels();

	//	AkUInt16 uFramesProcessed;
	//	for (AkUInt32 i = 0; i < uNumChannels; ++i)
	//	{
	//		// Peek into object metadata if desired.
	//		AkAudioObject* pObject = in_objects.ppObjects[objIdx];
	//		
	//		AkReal32* AK_RESTRICT pBuf = (AkReal32* AK_RESTRICT)in_objects.ppObjectBuffers[objIdx]->GetChannel(i);

	//		uFramesProcessed = 0;
	//		while (uFramesProcessed < in_objects.ppObjectBuffers[objIdx]->uValidFrames)
	//		{
	//			// Get input object's samples
	//			++uFramesProcessed;
	//		}
	//		if (uFramesProcessed > uMaxFramesProcessed)
	//			uMaxFramesProcessed = uFramesProcessed;
	//	}
	//}
	//
	//for (AkUInt32 objIdx = 0; objIdx < out_objects.uNumObjects; ++objIdx)
	//{
	//	const AkUInt32 uNumChannels = out_objects.ppObjectBuffers[objIdx]->NumChannels();

	//	AkUInt16 uFramesProcessed;
	//	for (AkUInt32 i = 0; i < uNumChannels; ++i)
	//	{
	//		// Set output object's metadata if desired.
	//		AkAudioObject* pObject = out_objects.ppObjects[objIdx];
	//		
	//		AkReal32* AK_RESTRICT pBuf = (AkReal32* AK_RESTRICT)out_objects.ppObjectBuffers[objIdx]->GetChannel(i);

	//		uFramesProcessed = 0;
	//		while (uFramesProcessed < out_objects.ppObjectBuffers[objIdx]->uValidFrames)
	//		{
	//			// Fill output object's samples
	//			++uFramesProcessed;
	//		}
	//	}
	//	
	//	out_objects.ppObjectBuffers[objIdx]->uValidFrames = uMaxFramesProcessed;
	//	if (uMaxFramesProcessed > 0)
	//		out_objects.ppObjectBuffers[objIdx]->eState = AK_DataReady;
	//	else
	//		out_objects.ppObjectBuffers[objIdx]->eState = AK_NoMoreData;
	//}


	/////////////////////////////////////////////////////////////////////////////////////////////////////////

 	AKASSERT(in_objects.uNumObjects > 0);

	for (AkUInt32 i = 0; i < in_objects.uNumObjects; ++i)
	{
		AkAudioObject	* inobj	= in_objects.ppObjects[i];
		GranularManager	* pEntry = m_OutBuffer.Exists(inobj->key);
		if (pEntry == nullptr)
		{
			pEntry = m_OutBuffer.AddInput(inobj->key);
			if (pEntry)
			{
				pEntry->m_numInObj = i;
				AkUInt32 numObjsOut = 1;
				if (inobj->positioning.behavioral.spatMode != AK_SpatializationMode_None)
				{
					AkReal32 fRandom = rand() / ((AkReal32)RAND_MAX);  // 더가벼운 rand사용하기// random value로 변경해야함
					numObjsOut = (AkUInt32)(fRandom * (AK_MAX_GENERATED_OBJECTS - 1) + 1.f);
				}

				
				
				//AkAudioBuffer** arNewBuffer		 =	(AkAudioBuffer**)AkAlloca(numObjsOut * sizeof(AkAudioBuffer*));	//이거 문제 일 수 도 있음! 이거 위주로 체크해 보자!
				AkAudioObject** arNewObjects	 =	(AkAudioObject**)AkAlloca(numObjsOut * sizeof(AkAudioObject*));

				AkAudioObjects outputObjects;
				outputObjects.uNumObjects = numObjsOut;
				outputObjects.ppObjectBuffers = nullptr;
				outputObjects.ppObjects = arNewObjects;

				AkChannelConfig TempChannelConfig = in_objects.ppObjectBuffers[i]->GetChannelConfig();
				TempChannelConfig.uNumChannels = 1;//setmono
				TempChannelConfig.SetStandard(AK_SPEAKER_SETUP_MONO);
				//TempChannelConfig.eConfigType = AK_ChannelConfigType_Anonymous;//AK_ChannelConfigType_Objects;
																																									   
				if (m_pContext->CreateOutputObjects(TempChannelConfig, outputObjects) == AK_Success)
				{
					for (AkUInt32 iObj = 0; iObj < numObjsOut; iObj++)
					{
						AkAudioObject* pObject = arNewObjects[iObj];
						pObject->positioning.behavioral.spatMode = AK_SpatializationMode_PositionOnly;//AK_SpatializationMode_None;//inobj->positioning.behavioral.spatMode;
						pObject->positioning.threeD = inobj->positioning.threeD;

						AkVector pos;
						pos.Zero();
						ComputeRandomPosition(inobj->positioning.threeD.xform.Position(), pos);
						pObject->positioning.threeD.xform.SetPosition(pos);

						pEntry->Push_Back_Manager(pObject->key, m_MaxLength/*p*/, m_MinLength/*p*/);//<-- 변경
					}

					//AkUInt32 chanNum = in_objects.ppObjectBuffers[i]->NumChannels();
					//for
					pEntry->SetRefOriginBuffer(new RefOriginBuffer(84200));//<-- 변경


					pEntry->SetNumChanOut(numObjsOut);
					pEntry->MatchChannels();//
				}
			}
		}
	}

	AkAudioObjects outputObjects;
	outputObjects.uNumObjects = 0;
	outputObjects.ppObjectBuffers = nullptr;
	outputObjects.ppObjects = nullptr;
	m_pContext->GetOutputObjects(outputObjects);

	if (outputObjects.uNumObjects > 0)
	{
		outputObjects.ppObjectBuffers	= (AkAudioBuffer**)AkAlloca(outputObjects.uNumObjects * sizeof(AkAudioBuffer*));
		outputObjects.ppObjects			= (AkAudioObject**)AkAlloca(outputObjects.uNumObjects * sizeof(AkAudioObject*));

		m_pContext->GetOutputObjects(outputObjects);

		AkMixerInputMap<AkUInt64, GranularManager>::Iterator it = m_OutBuffer.Begin();
		while (it != m_OutBuffer.End())
		{
			AkAudioBuffer** pInObjBuffer = in_objects.ppObjectBuffers;
			AkAudioBuffer* pinbuf = nullptr;
			it->pUserData->SetState(GranularManager::In_ObjState::DEATH);
			for (AkUInt32 i = 0; i < in_objects.uNumObjects; ++i)
			{
				if (it->key == in_objects.ppObjects[i]->key)
				{
					pinbuf = pInObjBuffer[i];
					AkUInt32 Chan = pinbuf->NumChannels();
					if (Chan == 1)
					{
						it->pUserData->m_RefOriginBuffer->SetOriginBuffer(pinbuf->GetChannel(0), pinbuf->uValidFrames);
					}
					else
					{
						it->pUserData->m_RefOriginBuffer->SetOriginBuffer(pinbuf, pinbuf->uValidFrames, Chan);
					}
					it->pUserData->SetState(GranularManager::In_ObjState::ALIVE);
					break;
				}
			}


			//in obj가 있는 경우
			if (pinbuf != nullptr)
			{
				for (AkUInt32 manSize = 0; manSize < it->pUserData->GetManager()->size(); ++manSize)
				{
					outputBufferManager* outManager = &((*it->pUserData->GetManager())[manSize]);
					for (AkUInt32 numOut = 0; numOut < outputObjects.uNumObjects; ++numOut)
					{
						if ((outManager->GetKey() == outputObjects.ppObjects[numOut]->key) && outputObjects.ppObjects[numOut])
						{
							AkAudioBuffer* TempOutObj = outputObjects.ppObjectBuffers[numOut];
							TempOutObj->uValidFrames = pinbuf->uValidFrames;
							if (outManager->ProcessorRendering(TempOutObj->GetChannel(0), TempOutObj->uValidFrames, TempOutObj->eState))
							{
								AkVector pos;
								pos.Zero();
								ComputeRandomPosition(in_objects.ppObjects[it->pUserData->m_numInObj]->positioning.threeD.xform.Position(), pos);
								outputObjects.ppObjects[numOut]->positioning.threeD.xform.SetPosition(pos);
							}

							//pinbuf->eState = AKRESULT::AK_DataReady;//pinbuf 가 nullptr일경우???
							break;
						}
					}
				}
			}
			else
			{
				for (AkUInt32 manSize = 0; manSize < it->pUserData->GetManager()->size(); ++manSize)
				{
					outputBufferManager* outManager = &((*it->pUserData->GetManager())[manSize]);
					for (AkUInt32 numOut = 0; numOut < outputObjects.uNumObjects; ++numOut)
					{
						if ((outManager->GetKey() == outputObjects.ppObjects[numOut]->key) && outputObjects.ppObjects[numOut])
						{
							AkAudioBuffer* TempOutObj = outputObjects.ppObjectBuffers[numOut];
							TempOutObj->uValidFrames = TempOutObj->MaxFrames();
							outManager->ProcessorRendering(TempOutObj->GetChannel(0), TempOutObj->uValidFrames, TempOutObj->eState);
							//pinbuf->eState = AKRESULT::AK_DataReady;//pinbuf 가 nullptr일경우???
							break;
						}
					}
				}
			}

			if (it->pUserData->IsOkDestroied())
			{
				it = m_OutBuffer.EraseSwap(it);
				//pinbuf->eState = AKRESULT::AK_NoMoreData;
			}
			else
			{
				++it;
			}

		}
	}
}

void Granular_Processor_2FX::ComputeRandomPosition(const AkVector& Position, AkVector& CopyVector)
{

	CopyVector.X = Position.X + m_Noise.nextValue() * m_pParams->RTPC.Pos_Range;
	CopyVector.Y = Position.Y + m_Noise.nextValue() * m_pParams->RTPC.Pos_Range;
	CopyVector.Z = Position.Z + m_Noise.nextValue() * m_pParams->RTPC.Pos_Range;

}
