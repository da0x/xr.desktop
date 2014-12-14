//C++

/*
----------------------------------------------------
The Desktop Project
------------------

Copyright 2004 Daher Alfawares

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

----------------------------------------------------
*/

#include "DShared.h"
#include "DSys.h"
#include "DAudio_Main.h"

//#include <FMod_errors.h>


namespace DAudio {

	DSys::Var_bool	sv_bSoundEnable				("s_soundEnable",	"1");
	DSys::Var_int	sv_iSound_MaxChannels		("s_maxChannels",	"32");
	DSys::Var_int	sv_iSound_Frequency			("s_frequency",		"44100");
	DSys::Var_float	sv_fSound_Volume			("s_volume",		"0.5");



	void Init()
	{
#if 0
		if( ! sv_bSoundEnable )
		{
			DSys::Logger::Print("Skipping FMod initialization...");
			return;
		}
		DSys::Logger::Print("---- FMod Initializing ----");
		Logger_EnterLevel();
		
		int error = FSOUND_Init(sv_iSound_Frequency, sv_iSound_MaxChannels, 0);
		if(error)
		{
			DSys::Logger::Error(FMOD_ErrorString(error));
		}
		else
		{
			DSys::Logger::Print("FMod Initialized Successfully...");
		}

		DSys::Logger::Print("Setting audio frequency to %d Hz...", sv_iSound_Frequency.intval());
		DSys::Logger::Print("Setting max software channels to %d channels...", sv_iSound_MaxChannels.intval());

		if (FSOUND_GetVersion() < FMOD_VERSION)
		{
			DSys::Logger::Error("You are using the wrong DLL version!  You should be using FMOD %.02f\n", FMOD_VERSION);
			return;
		}		
		
		DSys::Logger::Print("Using Direct Sound output device...");
		FSOUND_SetOutput(FSOUND_OUTPUT_DSOUND);


		unsigned int caps = 0;

        FSOUND_GetDriverCaps(FSOUND_GetDriver(), &caps);
        DSys::Logger::Print("--------------------");
		DSys::Logger::Print("Driver capabilities:");
		DSys::Logger::Print("--------------------");
        if (!caps)
            DSys::Logger::Print("This driver will support software mode only.\n  It does not properly support 3D sound hardware.");
        if (caps & FSOUND_CAPS_HARDWARE)
            DSys::Logger::Print("Driver supports hardware 3D sound!");
        if (caps & FSOUND_CAPS_EAX2)
            DSys::Logger::Print("Driver supports EAX 2 reverb!");
        if (caps & FSOUND_CAPS_EAX3)
            DSys::Logger::Print("Driver supports EAX 3 reverb!");
    
		DSys::Logger::Print("\n");

		

		switch (FSOUND_GetOutput())
		{
			case FSOUND_OUTPUT_NOSOUND: DSys::Logger::Print("FSOUND Output Method : FSOUND_OUTPUT_NOSOUND"); break;
			case FSOUND_OUTPUT_WINMM:   DSys::Logger::Print("FSOUND Output Method : FSOUND_OUTPUT_WINMM"); break;
			case FSOUND_OUTPUT_DSOUND:  DSys::Logger::Print("FSOUND Output Method : FSOUND_OUTPUT_DSOUND"); break;
			case FSOUND_OUTPUT_ASIO:    DSys::Logger::Print("FSOUND Output Method : FSOUND_OUTPUT_ASIO"); break;
			case FSOUND_OUTPUT_OSS:     DSys::Logger::Print("FSOUND Output Method : FSOUND_OUTPUT_OSS"); break;
			case FSOUND_OUTPUT_ESD:     DSys::Logger::Print("FSOUND Output Method : FSOUND_OUTPUT_ESD"); break;
			case FSOUND_OUTPUT_ALSA:    DSys::Logger::Print("FSOUND Output Method : FSOUND_OUTPUT_ALSA"); break;       
		};

		DSys::Logger::Print("Autodetecting mixxer...");
		FSOUND_SetMixer(FSOUND_MIXER_AUTODETECT);
		switch (FSOUND_GetMixer())
		{
			case FSOUND_MIXER_BLENDMODE:     DSys::Logger::Print("FSOUND Mixer         : FSOUND_MIXER_BLENDMODE"); break;
			case FSOUND_MIXER_MMXP5:         DSys::Logger::Print("FSOUND Mixer         : FSOUND_MIXER_MMXP5"); break;
			case FSOUND_MIXER_MMXP6:         DSys::Logger::Print("FSOUND Mixer         : FSOUND_MIXER_MMXP6"); break;
			case FSOUND_MIXER_QUALITY_FPU:   DSys::Logger::Print("FSOUND Mixer         : FSOUND_MIXER_QUALITY_FPU"); break;
			case FSOUND_MIXER_QUALITY_MMXP5: DSys::Logger::Print("FSOUND Mixer         : FSOUND_MIXER_QUALITY_MMXP5"); break;
			case FSOUND_MIXER_QUALITY_MMXP6: DSys::Logger::Print("FSOUND Mixer         : FSOUND_MIXER_QUALITY_MMXP6"); break;
		};

		DSys::Logger::Print("FSOUND Driver        : %s", FSOUND_GetDriverName(FSOUND_GetDriver()));
		DSys::Logger::Print("Hardware 3D channels : %d", FSOUND_GetNumHardwareChannels());

        
		if(sv_fSound_Volume.floatval() > 1 || sv_fSound_Volume.floatval() < 0)
		{
			sv_fSound_Volume.setval("0.5");
			sv_fSound_Volume.update();
			DSys::Logger::Warning("sv_fSound_Volume is out of range resetting to default...");
		}
		DSys::Logger::Print("Setting volume to %f%%...", sv_fSound_Volume.floatval()*100.0f);
		FSOUND_SetVolume(FSOUND_ALL, (int)(255 * sv_fSound_Volume.floatval()));
#endif
	}

	void Update()
	{
#if 0
		FSOUND_Update();
#endif
	}

	void Shutdown()
	{
#if 0
		DSys::Logger::Print("FMod Shutdown...");
		FSOUND_Close();
#endif
	}
}