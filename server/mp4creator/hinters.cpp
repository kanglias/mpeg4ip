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
 * Copyright (C) Cisco Systems Inc. 2000, 2001.  All Rights Reserved.
 * 
 * Contributor(s): 
 *		Dave Mackie		dmackie@cisco.com
 */

/* 
 * Notes:
 *  - file formatted with tabstops == 4 spaces 
 */

#include <mp4creator.h>
#include <hinters.h>

void AudioConsecutiveHinter( 
	MP4FileHandle mp4File, 
	MP4TrackId mediaTrackId, 
	MP4TrackId hintTrackId,
	MP4Duration sampleDuration, 
	u_int8_t perPacketHeaderSize,
	u_int8_t perSampleHeaderSize,
	u_int8_t maxSamplesPerPacket,
	AudioSampleSizer pSizer,
	AudioConcatenator pConcatenator,
	AudioFragmenter pFragmenter)
{
	u_int32_t numSamples = 
		MP4GetTrackNumberOfSamples(mp4File, mediaTrackId);

	u_int16_t bytesThisHint = perPacketHeaderSize;
	u_int16_t samplesThisHint = 0;
	MP4SampleId* pSampleIds = 
		new MP4SampleId[maxSamplesPerPacket];

	for (MP4SampleId sampleId = 1; sampleId <= numSamples; sampleId++) {

		u_int32_t sampleSize = 
			(*pSizer)(mp4File, mediaTrackId, sampleId);

		// sample won't fit in this packet
		// or we've reached the limit on samples per packet
		if (sampleSize + perSampleHeaderSize > MaxPayloadSize - bytesThisHint 
		  || samplesThisHint == maxSamplesPerPacket) {

			if (samplesThisHint > 0) {
				(*pConcatenator)(mp4File, mediaTrackId, hintTrackId,
					samplesThisHint, pSampleIds,
					samplesThisHint * sampleDuration);
			}

			// start a new hint 
			samplesThisHint = 0;
			bytesThisHint = perPacketHeaderSize;

			// fall thru
		}

		// sample is less than remaining payload size
		if (sampleSize + perSampleHeaderSize 
		  <= MaxPayloadSize - bytesThisHint) {

			// add it to this hint
			bytesThisHint += (sampleSize + perSampleHeaderSize);
			pSampleIds[samplesThisHint++] = sampleId;

		} else { 
			// jumbo frame, need to fragment it
			(*pFragmenter)(mp4File, mediaTrackId, hintTrackId,
				sampleId, sampleSize, sampleDuration);

			// start a new hint 
			samplesThisHint = 0;
			bytesThisHint = perPacketHeaderSize;
		}
	}

	delete [] pSampleIds;
}

void AudioInterleaveHinter( 
	MP4FileHandle mp4File, 
	MP4TrackId mediaTrackId, 
	MP4TrackId hintTrackId,
	MP4Duration sampleDuration, 
	u_int8_t stride, 
	u_int8_t bundle,
	AudioConcatenator pConcatenator)
{
	u_int32_t numSamples = 
		MP4GetTrackNumberOfSamples(mp4File, mediaTrackId);

	MP4SampleId* pSampleIds = new MP4SampleId[bundle];

	for (u_int32_t i = 1; i <= numSamples; i += stride * bundle) {
		for (u_int32_t j = 0; j < stride; j++) {
			u_int32_t k;
			for (k = 0; k < bundle; k++) {

				MP4SampleId sampleId = i + j + (k * stride);

				// out of samples for this bundle
				if (sampleId > numSamples) {
					break;
				}

				// add sample to this hint
				pSampleIds[k] = sampleId;
			}

			if (k == 0) {
				break;
			}

			// compute hint duration
			// note this is used to control the RTP timestamps 
			// that are emitted for the packet,
			// it isn't the actual duration of the samples in the packet
			MP4Duration hintDuration;
			if (j + 1 == stride) {
				// at the end of the track
				if (i + (stride * bundle) > numSamples) {
					hintDuration = ((numSamples - i) - j) * sampleDuration;
				} else {
					hintDuration = ((stride * bundle) - j) * sampleDuration;
				}
			} else {
				hintDuration = sampleDuration;
			}

			// write hint
			(*pConcatenator)(mp4File, mediaTrackId, hintTrackId,
				k, pSampleIds, hintDuration);
		}
	}

	delete [] pSampleIds;
}
