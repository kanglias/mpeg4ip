/*
 * The contents of this file are subject to the Mozilla Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 * 
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 * 
 * The Original Code is MPEG4IP.
 * 
 * The Initial Developer of the Original Code is Cisco Systems Inc.
 * Portions created by Cisco Systems Inc. are
 * Copyright (C) Cisco Systems Inc. 2002.  All Rights Reserved.
 * 
 * Contributor(s): 
 *	Bill May wmay@cisco.com	
 */

#include "mp4live.h"
#include "audio_encoder.h"
#include "mp4av.h"

/*
 * This looks like a fairly bogus set of routines; however, the
 * code makes it really easy to add your own codecs here, include
 * the mp4live library, write your own main, and go.
 * Just add the codecs you want before the call to the base routines
 */

CAudioEncoder* AudioEncoderCreate(const char* encoderName)
{
  // add codecs here
  return AudioEncoderBaseCreate(encoderName);
}

MediaType get_audio_mp4_fileinfo (CLiveConfig *pConfig,
				  bool *mpeg4,
				  bool *isma_compliant,
				  uint8_t *audioProfile,
				  uint8_t **audioConfig,
				  uint32_t *audioConfigLen,
				  uint8_t *mp4_audio_type)
{
  return get_base_audio_mp4_fileinfo(pConfig, mpeg4, isma_compliant, 
				     audioProfile, audioConfig, 
				     audioConfigLen, mp4_audio_type);
}

media_desc_t *create_audio_sdp (CLiveConfig *pConfig,
				bool *mpeg4,
				bool *isma_compliant,
				uint8_t *audioProfile,
				uint8_t **audioConfig,
				uint32_t *audioConfigLen)
{
  return create_base_audio_sdp(pConfig, mpeg4, isma_compliant,
			       audioProfile, audioConfig, audioConfigLen);
}
				
void create_mp4_audio_hint_track (CLiveConfig *pConfig, 
				  MP4FileHandle mp4file,
				  MP4TrackId trackId)
{
  create_base_mp4_audio_hint_track(pConfig, mp4file, trackId);
}

bool get_audio_rtp_info (CLiveConfig *pConfig,
			 MediaType *audioFrameType,
			 uint32_t *audioTimeScale,
			 uint8_t *audioPayloadNumber,
			 uint8_t *audioPayloadBytesPerPacket,
			 uint8_t *audioPayloadBytesPerFrame,
			 uint8_t *audioQueueMaxCount,
			 audio_set_rtp_header_f *audio_set_header,
			 audio_set_rtp_jumbo_frame_f *audio_set_jumbo,
			 void **ud)
{
  return get_base_audio_rtp_info(pConfig, 
				 audioFrameType, 
				 audioTimeScale,
				 audioPayloadNumber, 
				 audioPayloadBytesPerPacket,
				 audioPayloadBytesPerFrame, 
				 audioQueueMaxCount,
				 audio_set_header, 
				 audio_set_jumbo, 
				 ud);
}

