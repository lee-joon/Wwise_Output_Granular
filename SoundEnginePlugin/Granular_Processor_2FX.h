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

#ifndef Granular_Processor_2FX_H
#define Granular_Processor_2FX_H

#include "Granular_Processor_2FXParams.h"
#include <AK/Plugin/PluginServices/AkMixerInputMap.h>

#include "GranularSet.h"
#define AK_MAX_GENERATED_OBJECTS 5

/// See https://www.audiokinetic.com/library/edge/?source=SDK&id=soundengine__plugins__effects.html
/// for the documentation about effect plug-ins
class Granular_Processor_2FX
	: public AK::IAkOutOfPlaceObjectPlugin
{
public:
	Granular_Processor_2FX();
	~Granular_Processor_2FX();

	/// Plug-in initialization.
	/// Prepares the plug-in for data processing, allocates memory and sets up the initial conditions.
	AKRESULT Init(AK::IAkPluginMemAlloc* in_pAllocator, AK::IAkEffectPluginContext* in_pContext, AK::IAkPluginParam* in_pParams, AkAudioFormat& in_rFormat) override;

	/// Release the resources upon termination of the plug-in.
	AKRESULT Term(AK::IAkPluginMemAlloc* in_pAllocator) override;

	/// The reset action should perform any actions required to reinitialize the
	/// state of the plug-in to its original state (e.g. after Init() or on effect bypass).
	AKRESULT Reset() override;

	/// Plug-in information query mechanism used when the sound engine requires
	/// information about the plug-in to determine its behavior.
	AKRESULT GetPluginInfo(AkPluginInfo& out_rPluginInfo) override;

	/// Effect plug-in DSP execution.
	void Execute(
		const AkAudioObjects& in_objects,	///< Input objects and object audio buffers.
		const AkAudioObjects& out_objects	///< Output objects and object audio buffers.
		) override;
	AkVector ComputeRandomPosition(const AkVector& Position);

	AkMixerInputMap<AkUInt64, GranularManager> m_OutBuffer;
private:
	Granular_Processor_2FXParams* m_pParams;
	AK::IAkPluginMemAlloc* m_pAllocator;
	AK::IAkEffectPluginContext* m_pContext;

	NoiseGenerator m_Noise;
	AkUInt32 m_BufferSize;//<-- ����

	//
	AkUInt32 m_MaxLength;
	AkUInt32 m_MinLength;
	//AkUInt32 m_TotalLength;
	//

	//AkReal32 p_Range = 2000/*p*/;
};

#endif // Granular_Processor_2FX_H
