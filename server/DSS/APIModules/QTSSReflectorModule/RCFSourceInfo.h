/*
 * Copyright (c) 1999 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * Copyright (c) 1999 Apple Computer, Inc.  All Rights Reserved.
 * The contents of this file constitute Original Code as defined in and are 
 * subject to the Apple Public Source License Version 1.1 (the "License").  
 * You may not use this file except in compliance with the License.  Please 
 * obtain a copy of the License at http://www.apple.com/publicsource and 
 * read it before using this file.
 * 
 * This Original Code and all software distributed under the License are 
 * distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, EITHER 
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES, 
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS 
 * FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT.  Please see the License for 
 * the specific language governing rights and limitations under the 
 * License.
 * 
 * 
 * @APPLE_LICENSE_HEADER_END@
 */
/*
	File:		RCFSourceInfo.h

	Contains:	This object takes input RCF data, and uses it to support the SourceInfo
				API.

	

*/

#ifndef __RCF_SOURCE_INFO_H__
#define __RCF_SOURCE_INFO_H__

#include "StrPtrLen.h"
#include "SourceInfo.h"
#include "RelayPrefsSource.h"
#include "StringParser.h"

class RCFSourceInfo : public SourceInfo
{
	public:
	
		// Uses the SDP Data to build up the StreamInfo structures
		RCFSourceInfo() {}
		RCFSourceInfo(RelayPrefsSource* inPrefsSource, UInt32 inStartIndex) { Parse(inPrefsSource, inStartIndex); }
		virtual ~RCFSourceInfo();
		
		// Parses out the SDP file provided, sets up the StreamInfo structures
		void	Parse(RelayPrefsSource* inPrefsSource, SInt32 inStartIndex);
		
		// Parses relay_destination lines and builds OutputInfo structs
		void 	ParseRelayDestinations(RelayPrefsSource* inPrefsSource, SInt32 inStartIndex);

};
#endif // __SDP_SOURCE_INFO_H__

