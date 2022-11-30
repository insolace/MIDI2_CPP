/**********************************************************
 * MIDI 2.0 Library 
 * Author: Andrew Mee
 * 
 * MIT License
 * Copyright 2021 Andrew Mee
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * ********************************************************/

#include "../include/midi2Processor.h"
 
#include <cstdlib>


midi2Processor::midi2Processor(uint8_t CIVer) {
    midiCIVer = CIVer;
}

midi2Processor::~midi2Processor() {

}

void midi2Processor::createCIHeader(uint8_t* sysexHeader, MIDICI midiCiHeader){
	sysexHeader[0]=S7UNIVERSAL_NRT;
	sysexHeader[1]=midiCiHeader.deviceId;//MIDI_PORT;
	sysexHeader[2]=S7MIDICI;
	sysexHeader[3]=midiCiHeader.ciType;
	sysexHeader[4]=midiCiHeader.ciVer;
	setBytesFromNumbers(sysexHeader, midiCiHeader.localMUID, 5, 4);
	setBytesFromNumbers(sysexHeader, midiCiHeader.remoteMUID, 9, 4);
}

void midi2Processor::endSysex7(uint8_t group){

    midici.erase(group);
    syExMessInt.erase(group);
}

void midi2Processor::startSysex7(uint8_t group){
    //MIDICI newMidiCI;
    //umpSysex7Internal newSInt;
    midici[group] =  MIDICI();
    syExMessInt[group] = umpSysex7Internal();
}

void midi2Processor::processSysEx(uint8_t group, uint8_t s7Byte){
	if(syExMessInt[group].pos == 0){
		if(s7Byte == S7UNIVERSAL_NRT || s7Byte == S7UNIVERSAL_RT){
            syExMessInt[group].realtime =  s7Byte;
			syExMessInt[group].pos++;
			return;
		}
	}
	
	if(syExMessInt[group].realtime == S7UNIVERSAL_NRT || syExMessInt[group].realtime == S7UNIVERSAL_RT){
		if(syExMessInt[group].pos == 1){
			midici[group].deviceId =  s7Byte;
			syExMessInt[group].pos++;
			return;
		}
		
		if(syExMessInt[group].pos == 2){
            syExMessInt[group].universalId =  s7Byte;
			syExMessInt[group].pos++;
			return;
		}
	}
	
	if(syExMessInt[group].realtime == S7UNIVERSAL_NRT){
		switch(syExMessInt[group].universalId){
			/*
			case 0x00: // Sample DUMP
			case 0x01:
			case 0x02:
			case 0x03:
			case 0x05: // Sample Dump Extensions
				break;
			 */

            #ifdef M2_ENABLE_MTC
            case 0x04: // MIDI Time Code
			    //MIDI Cueing Set-Up Messages - F0 7E <device ID> 04 <sub-ID 2> hr mn sc fr ff sl sm <add. info.> F7
				break;
            #endif

			#ifndef M2_DISABLE_IDREQ  
			case S7IDREQUEST:
				processDeviceID(group, s7Byte);
				break;
            #endif

			/*
			case 0x07 : // File Dump
				break;
			 */

            #if defined(M2_ENABLE_MIDI_TUNING_STANDARD) || defined(M2_ENABLE_GM2)
			case 0x08 : // MIDI Tuning Standard (Non-Real Time)
			    //BULK TUNING DUMP REQUEST - F0 7E <device ID> 08 00 <tt- tuning program number (0 – 127)> F7
			    //BULK TUNING DUMP - F0 7E <device ID> 08 01 tt <tuning name(16)> [xx yy zz](128) chksum F7
			    //BULK TUNING DUMP REQUEST (BANK) - F0 7E 08 03 <bb - bank: 0-127> tt F7
			    //KEY-BASED TUNING DUMP - F0 7E <device ID> 08 04 bb tt <tuning name>(16) [xx yy zz](128) chksum F7
			    //SINGLE NOTE TUNING CHANGE (BANK) F0 7E <device ID> 08 07 bb tt ll
                    //[<MIDI key number> <frequency data for that key>(3)](ll) F7
                //SCALE/OCTAVE TUNING DUMP, 1 byte format - F0 7E <device ID> 08 05 bb tt <tuning name>(16) [xx](12) chksum F7
                //SCALE/OCTAVE TUNING DUMP, 2 byte format - F0 7E <device ID> 08 06 bb tt <tuning name>(16) [xx yy](12) chksum F7
                //SCALE/OCTAVE TUNING 1-BYTE FORM - F0 7E <device ID> 08 08 <ff - channel/options>
                    //<gg -channel byte 2> <hh -channel byte 3> [ss](12)  F7
                ///SCALE/OCTAVE TUNING 2-BYTE FORM - F0 7E <device ID> 08 09 ff gg hh [ss tt](12) F7
				break;
            #endif

            #if defined(M2_ENABLE_GM1) || defined(M2_ENABLE_GM2)
			case 0x09 : // General MIDI
                // GM2 System On - F0 7E <device ID> 09 03 F7
                // GM1 System On - F0 7E <device ID> 09 01 F7
                // GM System Off - F0 7E <device ID> 09 02 F7
				break;
            #endif

			/*case 0x0A : // Downloadable Sounds
				break;
			case 0x0B : // File Reference Message
				break;
			case 0x0C : // MIDI Visual Control
			    //FOH 7EH Dev OCH 01H {. . .} F7H
				break;	
			case 0x7B : // End of File
				break;	
			case 0x7C : // Wait
				break;			
			case 0x7D : // Cancel
				break;	
			case 0x7E : // NAK
				break;
			case 0x7F : // ACK
				break;
			*/
			case S7MIDICI: // MIDI-CI
				processMIDICI(group, s7Byte);
				break;
			
		}
	}else if(syExMessInt[group].realtime == S7UNIVERSAL_RT) {
		//This block of code represents potential future Universal SysEx Work
		switch(syExMessInt[group].universalId){
            #ifdef M2_ENABLE_MTC
			case 0x01: // MIDI Time Code
		        //Full Message - F0 7F <device ID> 01 <sub-ID 2> hr mn sc fr F7
		        //User Bits Message - F0 7F <device ID> 01 <sub-ID 2> u1 u2 u3 u4 u5 u6 u7 u8 u9 F7
				break
            #endif

			/*case 0x02: // MIDI Show Control
				break;
			case 0x03: // Notation Information
				break;*/

            #ifdef M2_ENABLE_GM2
			case 0x04: // Device Control
		        //GLOBAL PARAMETER CONTROL - F0 7F <device ID> 04 05 sw pw vw [[sh sl] ... ] [pp vv] ... F7
                //MASTER FINE TUNING - F0 7F <device ID> 04 03 lsb msb F7
                //MASTER COARSE TUNING - F0 7F <device ID> 04 04 00 msb F7
				break;
            #endif

            #ifdef M2_ENABLE_MTC
			case 0x05: // Real Time MTC Cueing
		        //Real Time MIDI Cueing Set-Up Message - F0 7F <device ID> 05 <sub-id #2> sl sm <additional info.> F7
				break;
            #endif

            /*case 0x06: // MIDI Machine Control Commands
				break;
			case 0x07 : // MIDI Machine Control Responses
				break;*/

            #if defined(M2_ENABLE_MIDI_TUNING_STANDARD) || defined(M2_ENABLE_GM2)
			case 0x08 : // MIDI Tuning Standard (Real Time)
		        //SINGLE NOTE TUNING CHANGE F0 7F <device ID> 08 02 <tt tuning program number (0 – 127)>
		            //<ll - number of changes> [<MIDI key number> <frequency data for that key>(3)](ll) F7
                //SINGLE NOTE TUNING CHANGE (BANK) F0 7F <device ID> 08 07 bb tt ll
                    //[<MIDI key number> <frequency data for that key>(3)](ll) F7
                //SCALE/OCTAVE TUNING 1-BYTE FORM - F0 7F <device ID> 08 08 <ff - channel/options>
                    //<gg -channel byte 2> <hh - channel byte 3> [ss](12)  F7
                //SCALE/OCTAVE TUNING 2-BYTE FORM - F0 7F <device ID> 08 09 ff gg hh [ss tt](12) F7
				break;
            #endif

            #ifdef M2_ENABLE_GM2
			case 0x09 : // Controller Destination Setting (See GM2 Documentation)
                //Channel Pressure/Polyphonic Key Pressure - F0 7F <device ID> 09 01/02 0n [pp rr] ... F7
                //Control Change - F0 7F <device ID> 09 03 0n cc [pp rr] ... F7
				break;
			case 0x0A : // Key-based Instrument Control
                //KEY-BASED INSTRUMENT CONTROL - F0 7F <device ID> 0A 01 0n kk [nn vv] .. F7
				break;
            #endif

            /*case 0x0B : // Scalable Polyphony MIDI MIP Message
				break;
			case 0x0C : // Mobile Phone Control Message
				break;	
		    */
		}
	}else if (recvUnknownSysEx != nullptr){
        recvUnknownSysEx(group, &syExMessInt[group], s7Byte);
	}
	syExMessInt[group].pos++;
}

void midi2Processor::processMIDICI(uint8_t group, uint8_t s7Byte){
    qDebug() << "processMIDICI called - s7b: " << s7Byte;

    if (s7Byte == 48)
    {
        qDebug() << "Property Exchange!!";
    }
    //printf("s7 Byte %d\n", s7Byte);
	if(syExMessInt[group].pos == 3){
		midici[group].ciType =  s7Byte;
	}
	
	if(syExMessInt[group].pos == 4){
	    midici[group].ciVer =  s7Byte;
	} 
	if(syExMessInt[group].pos >= 5 && syExMessInt[group].pos <= 8){
        midici[group].remoteMUID += s7Byte << (7 * (syExMessInt[group].pos - 5));
	}
	
	if(syExMessInt[group].pos >= 9 && syExMessInt[group].pos <= 12){
        midici[group].localMUID += s7Byte << (7 * (syExMessInt[group].pos - 9));
	}
	
	if(syExMessInt[group].pos >= 12
       && midici[group].localMUID != M2_CI_BROADCAST
       && checkMUID && !checkMUID(group, midici[group].localMUID)
        ){
		return; //Not for this device
	}
	
	//break up each Process based on ciType
    if(syExMessInt[group].pos >= 12) {
        switch (midici[group].ciType) {
            case MIDICI_DISCOVERYREPLY: //Discovery Reply
            case MIDICI_DISCOVERY: { //Discovery Request
                if (syExMessInt[group].pos >= 13 && syExMessInt[group].pos <= 23) {
                    syExMessInt[group].buffer1[syExMessInt[group].pos - 13] = s7Byte;
                }
                if (syExMessInt[group].pos == 24) {
                    syExMessInt[group].intbuffer1[0] = s7Byte; // ciSupport
                }
                if (syExMessInt[group].pos >= 25 && syExMessInt[group].pos <= 28) {
                    syExMessInt[group].intbuffer1[1] += s7Byte << (7 * (syExMessInt[group].pos - 25)); //maxSysEx
                }

                bool complete = false;
                if (syExMessInt[group].pos == 28 && midici[group].ciVer == 1) {
                    complete = true;
                }
                else if (syExMessInt[group].pos == 28){
                    syExMessInt[group].intbuffer1[2] = s7Byte; //output path id
                    if(midici[group].ciType==MIDICI_DISCOVERY) {
                        complete = true;
                    }
                }
                else if (syExMessInt[group].pos == 29){
                    syExMessInt[group].intbuffer1[3] = s7Byte; //fbIdx id
                    if(midici[group].ciType==MIDICI_DISCOVERYREPLY) {
                        complete = true;
                    }
                }

                /*else if (syExMessInt[group].pos == 29){
                    syExMessInt[group].intbuffer1[3] = s7Byte; //product id Length
                }else if (syExMessInt[group].pos >= 30 && syExMessInt[group].pos <= 30+syExMessInt[group].intbuffer1[3]){
                    syExMessInt[group].buffer1[syExMessInt[group].pos - 30 + 11] = s7Byte; //product ID
                }*/
//                if (syExMessInt[group].pos == 30 + syExMessInt[group].intbuffer1[3]){
//                    complete = true;
//                }

                if (complete) {
                    //debug("  - Discovery Request 28 ");
                    qDebug() << "processMIDICI called - complete";
                    if(midici[group].ciType==MIDICI_DISCOVERY) {
                        if (recvDiscoveryRequest != nullptr) recvDiscoveryRequest(
                                group,
                                midici[group],
                                {syExMessInt[group].buffer1[0],syExMessInt[group].buffer1[1],syExMessInt[group].buffer1[2]},
                                {syExMessInt[group].buffer1[3], syExMessInt[group].buffer1[4]},
                                {syExMessInt[group].buffer1[5], syExMessInt[group].buffer1[6]},
                                {syExMessInt[group].buffer1[7], syExMessInt[group].buffer1[8],
                                 syExMessInt[group].buffer1[9], syExMessInt[group].buffer1[10]},
                                syExMessInt[group].intbuffer1[0],
                                syExMessInt[group].intbuffer1[1],
                                syExMessInt[group].intbuffer1[2]
                                //syExMessInt[group].intbuffer1[3],
                               // &(syExMessInt[group].buffer1[11])
                        );
                    }else{
                        if (recvDiscoveryReply != nullptr) recvDiscoveryReply(
                                group,
                                midici[group],
                                {syExMessInt[group].buffer1[0],syExMessInt[group].buffer1[1],syExMessInt[group].buffer1[2]},
                                {syExMessInt[group].buffer1[3], syExMessInt[group].buffer1[4]},
                                {syExMessInt[group].buffer1[5], syExMessInt[group].buffer1[6]},
                                {syExMessInt[group].buffer1[7], syExMessInt[group].buffer1[8],
                                 syExMessInt[group].buffer1[9], syExMessInt[group].buffer1[10]},
                                syExMessInt[group].intbuffer1[0],
                                syExMessInt[group].intbuffer1[1],
                                syExMessInt[group].intbuffer1[2],
                                syExMessInt[group].intbuffer1[3]
                                //&(syExMessInt[group].buffer1[11])
                        );
                    }
                }
                break;
            }

            case MIDICI_INVALIDATEMUID: //MIDI-CI Invalidate MUID Message

                if (syExMessInt[group].pos >= 13 && syExMessInt[group].pos <= 16) {
                    syExMessInt[group].buffer1[syExMessInt[group].pos - 13] = s7Byte;
                }

                //terminate MUID
                if (syExMessInt[group].pos == 16 && recvInvalidateMUID != nullptr) {
                    uint32_t terminateMUID = syExMessInt[group].buffer1[0]
                            + ((uint32_t)syExMessInt[group].buffer1[1] << 7)
                            + ((uint32_t)syExMessInt[group].buffer1[2] << 14)
                            + ((uint32_t)syExMessInt[group].buffer1[3] << 21);
                    recvInvalidateMUID(group, midici[group], terminateMUID);
                }
                break;
            case MIDICI_ENDPOINTINFO:{
                if (syExMessInt[group].pos == 13 && midici[group].ciVer > 1 && recvEndPointInfo!= nullptr) {
                    recvEndPointInfo(group, midici[group],s7Byte); // uint8_t origSubID,
                }
                break;
            }
            case MIDICI_ENDPOINTINFO_REPLY:{
                bool complete = false;
                if(midici[group].ciVer < 2) return;
                if (syExMessInt[group].pos == 13 && recvEndPointInfo!= nullptr) {
                    syExMessInt[group].intbuffer1[0] = s7Byte;
                }
                if(syExMessInt[group].pos == 14 || syExMessInt[group].pos == 15){
                    syExMessInt[group].intbuffer1[1] += s7Byte << (7 * (syExMessInt[group].pos - 14 ));
                    return;
                }
                if (syExMessInt[group].pos >= 16 && syExMessInt[group].pos <= 15 + syExMessInt[group].intbuffer1[1]){
                    syExMessInt[group].buffer1[syExMessInt[group].pos - 16] = s7Byte; //Info Data
                }if (syExMessInt[group].pos == 16 + syExMessInt[group].intbuffer1[1]){
                    complete = true;
                }

                if (complete) {
                    recvEndPointInfoReply(group, midici[group],
                                     syExMessInt[group].intbuffer1[0],
                                     syExMessInt[group].intbuffer1[1],
                                     syExMessInt[group].buffer1
                                     );
                }
                break;
            }
            case MIDICI_ACK:
            case MIDICI_NAK: {
                bool complete = false;

                if (syExMessInt[group].pos == 13 && midici[group].ciVer == 1) {
                    complete = true;
                } else if (syExMessInt[group].pos == 13 && midici[group].ciVer > 1) {
                    syExMessInt[group].intbuffer1[0] = s7Byte; // uint8_t origSubID,
                }

                if (syExMessInt[group].pos == 14) {
                    syExMessInt[group].intbuffer1[1] = s7Byte; //statusCode
                }

                if (syExMessInt[group].pos == 15) {
                    syExMessInt[group].intbuffer1[2] = s7Byte; //statusData
                }

                if (syExMessInt[group].pos >= 16 && syExMessInt[group].pos <= 20){
                    syExMessInt[group].buffer1[syExMessInt[group].pos - 16] = s7Byte; //ackNakDetails
                }

                if(syExMessInt[group].pos == 21 || syExMessInt[group].pos == 22){
                    syExMessInt[group].intbuffer1[3] += s7Byte << (7 * (syExMessInt[group].pos - 21 ));
                    return;
                }

                if (syExMessInt[group].pos >= 23 && syExMessInt[group].pos <= 23 + syExMessInt[group].intbuffer1[3]){
                    syExMessInt[group].buffer1[syExMessInt[group].pos - 23] = s7Byte; //product ID
                }
                if (syExMessInt[group].pos == 23 + syExMessInt[group].intbuffer1[3]){
                    complete = true;
                }

                if (complete) {
                    uint8_t ackNakDetails[5] = {syExMessInt[group].buffer1[0], syExMessInt[group].buffer1[1],
                                                syExMessInt[group].buffer1[2],
                                                syExMessInt[group].buffer1[3],
                                                syExMessInt[group].buffer1[4]};

                    if (midici[group].ciType == MIDICI_NAK && recvNAK != nullptr)
                        recvNAK(
                            group,
                            midici[group],
                            syExMessInt[group].intbuffer1[0],
                            syExMessInt[group].intbuffer1[1],
                            syExMessInt[group].intbuffer1[2],
                            ackNakDetails,
                            syExMessInt[group].intbuffer1[3],
                            syExMessInt[group].buffer1
                    );

                    if (midici[group].ciType == MIDICI_ACK && midici[group].ciVer > 1 && recvACK != nullptr)
                        recvACK(
                            group,
                            midici[group],
                            syExMessInt[group].intbuffer1[0],
                            syExMessInt[group].intbuffer1[1],
                            syExMessInt[group].intbuffer1[2],
                            ackNakDetails,
                            syExMessInt[group].intbuffer1[3],
                            syExMessInt[group].buffer1
                        );
                }
                break;
            }

#ifdef M2_ENABLE_PROTOCOL
            case MIDICI_PROTOCOL_NEGOTIATION:
            case MIDICI_PROTOCOL_NEGOTIATION_REPLY:
            case MIDICI_PROTOCOL_SET:
            case MIDICI_PROTOCOL_TEST:
            case MIDICI_PROTOCOL_TEST_RESPONDER:
            case MIDICI_PROTOCOL_CONFIRM:
                processProtocolSysex(group, s7Byte);
                break;
#endif

#ifndef M2_DISABLE_PROFILE
            case MIDICI_PROFILE_INQUIRY: //Profile Inquiry
            case MIDICI_PROFILE_INQUIRYREPLY: //Reply to Profile Inquiry
            case MIDICI_PROFILE_SETON: //Set Profile On Message
            case MIDICI_PROFILE_SETOFF: //Set Profile Off Message
            case MIDICI_PROFILE_ENABLED: //Set Profile Enabled Message
            case MIDICI_PROFILE_DISABLED: //Set Profile Disabled Message
            case MIDICI_PROFILE_SPECIFIC_DATA: //ProfileSpecific Data
                processProfileSysex(group, s7Byte);
                break;
#endif


#ifndef M2_DISABLE_PE
            case MIDICI_PE_CAPABILITY: //Inquiry: Property Exchange Capabilities
            case MIDICI_PE_CAPABILITYREPLY: //Reply to Property Exchange Capabilities
            case MIDICI_PE_GET:  // Inquiry: Get Property Data
            case MIDICI_PE_GETREPLY: // Reply To Get Property Data - Needs Work!
            case MIDICI_PE_SET: // Inquiry: Set Property Data
            case MIDICI_PE_SETREPLY: // Reply To Inquiry: Set Property Data
            case MIDICI_PE_SUB: // Inquiry: Subscribe Property Data
            case MIDICI_PE_SUBREPLY: // Reply To Subscribe Property Data
            case MIDICI_PE_NOTIFY: // Notify
                processPESysex(group, s7Byte);
                break;
#endif

#ifndef M2_DISABLE_PROCESSINQUIRY
            case MIDICI_PI_CAPABILITY:
            case MIDICI_PI_CAPABILITYREPLY:
            case MIDICI_PI_MM_REPORT:
            case MIDICI_PI_MM_REPORT_REPLY:
            case MIDICI_PI_MM_REPORT_END:
                processPISysex(group, s7Byte);
                break;
#endif
            default:
                if (recvUnknownMIDICI) {
                    recvUnknownMIDICI(group, &syExMessInt[group], midici[group], s7Byte);
                }
                break;

        }
    }
}

void midi2Processor::clearUMP(){
    messPos = 0;
    umpMess[0]=0;
    umpMess[1]=0;
    umpMess[2]=0;
    umpMess[3]=0;
}

void midi2Processor::processUMP(uint32_t UMP){
    qDebug() << "processUMP called, ump: " << UMP << " messPos: " << messPos;

    umpMess[messPos] = UMP;
		
	uint8_t mt = (umpMess[0] >> 28)  & 0xF;
	uint8_t group = (umpMess[0] >> 24) & 0xF;

	
	if(messPos == 0
        && (mt <= UMP_M1CVM || mt==0x6 || mt==0x7)
            ){ //32bit Messages

		if(mt == UMP_UTILITY){ //32 bits Utility Messages
			uint8_t status = (umpMess[0] >> 20) & 0xF;
			uint16_t timing = (umpMess[0] >> 16) & 0xFFFF;
			
			switch(status){
				case UTILITY_NOOP: // NOOP
				//if(group== 0 && noop != nullptr) noop();
				break;
				case UTILITY_JRCLOCK: // JR Clock Message
					if(recvJRClock != nullptr) recvJRClock(/*group, */timing);
					break;
				case UTILITY_JRTS: //JR Timestamp Message
					if(recvJRTimeStamp != nullptr) recvJRTimeStamp(/*group, */timing);
					break;
				case UTILITY_PROTOCOL_REQUEST: //JR Protocol Req
				    if(recvJRProtocolReq != nullptr) recvJRProtocolReq(/*group, */(umpMess[0] >> 2) & 1,
				                                   (umpMess[0] >> 1) & 1, umpMess[0] & 1);
				    break;
				case UTILITY_PROTOCOL_NOTIFY: //JR Protocol Req
				    if(recvJRProtocolNotify != nullptr) recvJRProtocolNotify(/*group, */(umpMess[0] >> 2) & 1,
				                                                                  (umpMess[0] >> 1) & 1, umpMess[0] & 1);
				    break;
			}
			
		} else 
		if(mt == UMP_SYSTEM){ //32 bits System Real Time and System Common Messages (except System Exclusive)
			//Send notice
			uint8_t status = umpMess[0] >> 16 & 0xFF;
			switch(status){
				case TIMING_CODE:
				{
					uint8_t timing = (umpMess[0] >> 8) & 0x7F;
					if(timingCode != nullptr) timingCode(group, timing);
				}
				break;
				case SPP:
				{
					uint16_t position = ((umpMess[0] >> 8) & 0x7F)  + ((umpMess[0] & 0x7F) << 7);
					if(songPositionPointer != nullptr) songPositionPointer(group, position);
				}
				break;
				case SONG_SELECT:
				{
					uint8_t song = (umpMess[0] >> 8) & 0x7F;
					if(songSelect != nullptr) songSelect(group, song);
				}
				break;
				case TUNEREQUEST:
				    if(tuneRequest != nullptr) tuneRequest(group);
				break;
				case TIMINGCLOCK:
				    if(timingClock != nullptr) timingClock(group);
				break;
				case SEQSTART:
				    if(seqStart != nullptr) seqStart(group);
				break;
				case SEQCONT:
				    if(seqCont != nullptr) seqCont(group);
				break;
				case SEQSTOP:
				    if(seqStop != nullptr) seqStop(group);
				break;
				case ACTIVESENSE:
				    if(activeSense != nullptr) activeSense(group);
				break;
				case SYSTEMRESET:
				    if(systemReset != nullptr) systemReset(group);
				break;
			}
		
		} else 
		if(mt == UMP_M1CVM){ //32 Bits MIDI 1.0 Channel Voice Messages
			uint8_t status = umpMess[0] >> 16 & 0xF0;
			uint8_t channel = (umpMess[0] >> 16) & 0xF;
			uint8_t val1 = (umpMess[0] >> 8) & 0x7F;
			uint8_t val2 = umpMess[0] & 0x7F;

			switch(status){
				case NOTE_OFF: //Note Off
					if(midiNoteOff != nullptr) midiNoteOff(group, (uint8_t)UMP_M1CVM, channel, val1, scaleUp(val2,7,16), 0, 0);
					break;
				case NOTE_ON: //Note On
					if(midiNoteOn != nullptr) midiNoteOn(group, (uint8_t)UMP_M1CVM, channel, val1, scaleUp(val2,7,16), 0, 0);
					break;
				case KEY_PRESSURE: //Poly Pressure
					if(polyPressure != nullptr) polyPressure(group, (uint8_t)UMP_M1CVM, channel, val1, scaleUp(val2,7,32));
					break;	
				case CC: //CC
					if(controlChange != nullptr) controlChange(group, (uint8_t)UMP_M1CVM, channel, val1, scaleUp(val2,7,32));
					break;
				case PROGRAM_CHANGE: //Program Change Message
					if(programChange != nullptr) programChange(group, (uint8_t)UMP_M1CVM, channel, val1, false, 0, 0);
					break;
				case CHANNEL_PRESSURE: //Channel Pressure
					if(channelPressure != nullptr) channelPressure(group, (uint8_t)UMP_M1CVM, channel, scaleUp(val1,7,32));
					break;
				case PITCH_BEND: //PitchBend
					if(pitchBend != nullptr) pitchBend(group, (uint8_t)UMP_M1CVM, channel, scaleUp((val2 << 7) + val1,14,32));
					break;		
			}				
		}
        return;
		
	}else		
	if(messPos == 1
       && (mt == UMP_SYSEX7 || mt == UMP_M2CVM || mt==0x8 || mt==0x9  || mt==0xA)
        ){ //64bit Messages
		if(mt == UMP_SYSEX7){ //64 bits Data Messages (including System Exclusive)
			
			uint8_t numbytes  = (umpMess[0] >> 16) & 0xF;
			uint8_t status = (umpMess[0] >> 20) & 0xF;
			if(status == 0 || status == 1){
				startSysex7(group);
			}
			
			if(numbytes > 0)processSysEx(group, (umpMess[0] >> 8) & 0x7F);
			if(numbytes > 1)processSysEx(group, umpMess[0] & 0x7F);
			if(numbytes > 2)processSysEx(group, (umpMess[1] >> 24) & 0x7F);
			if(numbytes > 3)processSysEx(group, (umpMess[1] >> 16) & 0x7F);
			if(numbytes > 4)processSysEx(group, (umpMess[1] >> 8) & 0x7F);
			if(numbytes > 5)processSysEx(group, umpMess[1] & 0x7F);
			
			if(status == 0 || status == 3){
				endSysex7(group);
			}
		} else 
		if(mt == UMP_M2CVM){//64 bits MIDI 2.0 Channel Voice Messages
		
			uint8_t status = (umpMess[0] >> 16) & 0xF0;
			uint8_t channel = (umpMess[0] >> 16) & 0xF;
			uint8_t val1 = (umpMess[0] >> 8) & 0xFF;
			uint8_t val2 = umpMess[0] & 0xFF;
			
			switch(status){
				case NOTE_OFF: //Note Off
					if(midiNoteOff != nullptr) midiNoteOff(group, (uint8_t)UMP_M2CVM, channel, val1, umpMess[1] >> 16, val2, umpMess[1] & 65535);
					break;
				
				case NOTE_ON: //Note On
					if(midiNoteOn != nullptr) midiNoteOn(group, (uint8_t)UMP_M2CVM, channel, val1, umpMess[1] >> 16, val2, umpMess[1] & 65535);
					break;
					
				case KEY_PRESSURE: //Poly Pressure
					if(polyPressure != nullptr) polyPressure(group, (uint8_t)UMP_M2CVM, channel, val1, umpMess[1]);
					break;	
				
				case CC: //CC
					if(controlChange != nullptr) controlChange(group, (uint8_t)UMP_M2CVM, channel, val1, umpMess[1]);
					break;	
				
				case RPN: //RPN
					if(rpn != nullptr) rpn(group, channel, val1, val2, umpMess[1]);
					break;	
				
				case NRPN: //NRPN
					if(nrpn != nullptr) nrpn(group, channel, val1, val2, umpMess[1]);
					break;	
				
				case RPN_RELATIVE: //Relative RPN
					if(rrpn != nullptr) rrpn(group, channel, val1, val2, (int32_t)umpMess[1]/*twoscomplement*/);
					break;	

				case NRPN_RELATIVE: //Relative NRPN
					if(rnrpn != nullptr) rnrpn(group, channel, val1, val2, (int32_t)umpMess[1]/*twoscomplement*/);
					break;
				
				case PROGRAM_CHANGE: //Program Change Message
					if(programChange != nullptr) programChange(group, (uint8_t)UMP_M2CVM, channel, umpMess[1] >> 24, umpMess[0] & 1 , (umpMess[1] >> 8) & 0x7f , umpMess[1] & 0x7f);
					break;

				case CHANNEL_PRESSURE: //Channel Pressure
					if(channelPressure != nullptr) channelPressure(group, (uint8_t)UMP_M2CVM, channel, umpMess[1]);
					break;

				case PITCH_BEND: //PitchBend
					if(pitchBend != nullptr) pitchBend(group, (uint8_t)UMP_M2CVM, channel, umpMess[1]);
					break;	
					
				case PITCH_BEND_PERNOTE: //Per Note PitchBend 6
					if(perNotePB != 0) perNotePB(group, channel, val1, umpMess[1]);
					break;

				case NRPN_PERNOTE: //Assignable Per-Note Controller 1
                    if(nrpnPerNote != nullptr) nrpnPerNote(group, channel, val1, val2, umpMess[1]);
					break;	
					
				case RPN_PERNOTE: //Registered Per-Note Controller 0 
                    if(rpnPerNote != nullptr) rpnPerNote(group, channel, val1, val2, umpMess[1]);
					break;	
					
				case PERNOTE_MANAGE: //Per-Note Management Message
                    if(perNoteManage != nullptr) perNoteManage(group, channel, val1, (bool)(val2 & 2), (bool)(val2 & 1));
					break;	
					
			}
		}
        messPos =0;
	}else		
    if(messPos == 2
       && (mt == 0xB || mt == 0xC)
            ){ //96bit Messages
        messPos =0;
    }else
    if(messPos == 3
             && (mt == UMP_DATA || mt >= 0xD)
    ){ //128bit Messages

        if(mt == UMP_MIDI_ENDPOINT) { //128 bits UMP Stream Messages
            uint16_t status = (umpMess[0] >> 16) & 0x3FF;
            uint8_t form = umpMess[0] >> 24 & 0x3;


            switch(status) {
                case MIDIENDPOINT: {
                    uint8_t filter = umpMess[1] & 0xFF;
                    if (midiEndpoint != nullptr) midiEndpoint(filter);
                    break;
                }
                case MIDIENDPOINT_INFO_NOTIFICATION:
                    //TODO Add this handler
                    break;
                case MIDIENDPOINT_DEVICEINFO_NOTIFICATION:
                    if(midiEndpointDeviceInfo != nullptr) {

                        // these have to be explicitly cast as (uint8_t) or it will cause an error in some compilers
                        std::array<uint8_t, 3> manuId = {
                            (uint8_t)((umpMess[1] >> 16) & 0x7F),
                            (uint8_t)((umpMess[1] >> 8) & 0x7F),
                            (uint8_t)(umpMess[1] & 0x7F) };

                        std::array<uint8_t, 2> familyId = {
                            (uint8_t)((umpMess[2] >> 24) & 0x7F),
                            (uint8_t)((umpMess[2] >> 16) & 0x7F)
                        };

                        std::array<uint8_t, 2> modelId = {
                            (uint8_t)((umpMess[2] >> 8) & 0x7F),
                            (uint8_t)(umpMess[2] & 0x7F)
                        };

                        std::array<uint8_t, 4> version = {
                            (uint8_t)((umpMess[3] >> 24) & 0x7F),
                            (uint8_t)((umpMess[3] >> 16) & 0x7F),
                            (uint8_t)((umpMess[3] >> 8) & 0x7F),
                            (uint8_t)(umpMess[3] & 0x7F)
                        };

                        midiEndpointDeviceInfo(manuId, familyId, modelId, version);
                    }
                    break;
                case MIDIENDPOINT_NAME_NOTIFICATION:
                case MIDIENDPOINT_PRODID_NOTIFICATION:
                    //TODO Add this handler
                    //if(midiEndpointName != nullptr) midiEndpointName(form,0,"");
                    //if(midiEndpointProdId != nullptr) midiEndpointProdId(form,0,"");
                    break;
                case FUNCTIONBLOCK:{
                    uint8_t filter = umpMess[0] & 0xFF;
                    uint8_t fbIdx = (umpMess[0] >> 8) & 0xFF;
                    if(functionBlock != nullptr) functionBlock(fbIdx, filter);
                    break;
                }

                case FUNCTIONBLOCK_INFO_NOTFICATION:
                    if(functionBlockInfo != nullptr) {
                        uint8_t fbIdx = (umpMess[0] >> 8) & 0x7F;
                        functionBlockInfo(
                                fbIdx, //fbIdx
                                (umpMess[0] >> 15) & 0x1, // active
                                umpMess[0] & 0x3, //dir
                                ((umpMess[1] >> 24) & 0x1F), //first group
                                ((umpMess[1] >> 16) & 0x1F), // group length
                                (umpMess[1] >> 15) & 0x1, // midiCIValid
                                ((umpMess[1] >> 8) & 0x7F), //midiCIVersion
                                ((umpMess[0]>>2)  & 0x3), //isMIDI 1
                                (umpMess[1]  & 0xFF) // max Streams
                        );
                    }
                    break;
                case FUNCTIONBLOCK_NAME_NOTIFICATION:
                    //if(functionBlockName != nullptr) functionBlockName(fbIdx, form,0,"");
                    break;
            }

        }else
        if(mt == UMP_DATA){ //128 bits Data Messages (including System Exclusive 8)
            uint8_t status = (umpMess[0] >> 20) & 0xF;
            //SysEx 8
            if(status <= 3){
                //SysEx 8
                /*uint8_t numbytes  = (umpMess[0] >> 16) & 0xF;
                uint8_t streamId  = (umpMess[0] >> 8) & 0xFF;
                if(status == 0 || status == 1){
                    startSysex7(group); //streamId
                }

                if(numbytes > 0)processSysEx(group, umpMess[0] & 0xFF);
                if(numbytes > 1)processSysEx(group, (umpMess[1] >> 24) & 0xFF);
                if(numbytes > 2)processSysEx(group, (umpMess[1] >> 16) & 0xFF);
                if(numbytes > 3)processSysEx(group, (umpMess[1] >> 8) & 0xFF);
                if(numbytes > 4)processSysEx(group, umpMess[1] & 0xFF);

                if(numbytes > 5)processSysEx(group, (umpMess[2] >> 24) & 0xFF);
                if(numbytes > 6)processSysEx(group, (umpMess[2] >> 16) & 0xFF);
                if(numbytes > 7)processSysEx(group, (umpMess[2] >> 8) & 0xFF);
                if(numbytes > 8)processSysEx(group, umpMess[2] & 0xFF);

                if(numbytes > 9)processSysEx(group, (umpMess[3] >> 24) & 0xFF);
                if(numbytes > 10)processSysEx(group, (umpMess[3] >> 16) & 0xFF);
                if(numbytes > 11)processSysEx(group, (umpMess[3] >> 8) & 0xFF);
                if(numbytes > 12)processSysEx(group, umpMess[3] & 0xFF);

                if(status == 0 || status == 3){
                    endSysex7(group);
                }*/

            }else if(status == 8 || status ==9){
                //Beginning of Mixed Data Set
                //uint8_t mdsId  = (umpMess[0] >> 16) & 0xF;

                if(status == 8){
                    /*uint16_t numValidBytes  = umpMess[0] & 0xFFFF;
                    uint16_t numChunk  = (umpMess[1] >> 16) & 0xFFFF;
                    uint16_t numOfChunk  = umpMess[1] & 0xFFFF;
                    uint16_t manuId  = (umpMess[2] >> 16) & 0xFFFF;
                    uint16_t deviceId  = umpMess[2] & 0xFFFF;
                    uint16_t subId1  = (umpMess[3] >> 16) & 0xFFFF;
                    uint16_t subId2  = umpMess[3] & 0xFFFF;*/
                }else{
                    // MDS bytes?
                }

            }

        }
        else
        if(mt == UMP_FLEX_DATA){ //128 bits Data Messages (including System Exclusive 8)
            uint8_t statusBank = (umpMess[0] >> 8) & 0xFF;
            uint8_t status = umpMess[0] & 0xFF;
            uint8_t channel = (umpMess[0] >> 16) & 0xF;
            uint8_t addrs = (umpMess[0] >> 18) & 0b11;
            uint8_t form = (umpMess[0] >> 20) & 0b11;
            //SysEx 8
            switch (statusBank){
                case FLEXDATA_COMMON:{ //Common/Configuration for MIDI File, Project, and Track
                    switch (status){
                        case FLEXDATA_COMMON_TEMPO: { //Set Tempo Message
                            if(flexTempo != nullptr) flexTempo(group, umpMess[1]);
                            break;
                        }
                        case FLEXDATA_COMMON_TIMESIG: { //Set Time Signature Message
                            if(flexTimeSig != nullptr) flexTimeSig(group,
                                                                 (umpMess[1] >> 24) & 0xFF,
                                                                 (umpMess[1] >> 16) & 0xFF,
                                                                 (umpMess[1] >> 8) & 0xFF
                                   );
                            break;
                        }
                        case FLEXDATA_COMMON_METRONOME: { //Set Metronome Message
                            if(flexMetronome != nullptr) flexMetronome(group,
                                                                   (umpMess[1] >> 24) & 0xFF,
                                                                   (umpMess[1] >> 16) & 0xFF,
                                                                   (umpMess[1] >> 8) & 0xFF,
                                                                   umpMess[1] & 0xFF,
                                                                   (umpMess[2] >> 24) & 0xFF,
                                                                   (umpMess[2] >> 16) & 0xFF
                                );
                            break;
                        }
                        case FLEXDATA_COMMON_KEYSIG: { //Set Key Signature Message
                            if(flexKeySig != nullptr) flexKeySig(group, addrs, channel,
                                                                   (umpMess[1] >> 24) & 0xFF,
                                                                   (umpMess[1] >> 16) & 0xFF
                                );
                            break;
                        }
                        case FLEXDATA_COMMON_CHORD: { //Set Chord Message
                            if(flexChord != nullptr) flexChord(group, addrs, channel,
                                                                       (umpMess[1] >> 28) & 0xF, //chShrpFlt
                                                                       (umpMess[1] >> 24) & 0xF, //chTonic
                                                                       (umpMess[1] >> 16) & 0xFF, //chType
                                                                       (umpMess[1] >> 12) & 0xF, //chAlt1Type
                                                                       (umpMess[1] >> 8) & 0xF,//chAlt1Deg
                                                                       (umpMess[1] >> 4) & 0xF,//chAlt2Type
                                                                       umpMess[1] & 0xF,//chAlt2Deg
                                                                       (umpMess[2] >> 28) & 0xF,//chAlt3Type
                                                                       (umpMess[2] >> 24) & 0xF,//chAlt3Deg
                                                                       (umpMess[2] >> 20) & 0xF,//chAlt4Type
                                                                       (umpMess[2] >> 16) & 0xF,//chAlt4Deg
                                                                       (umpMess[3] >> 28) & 0xF,//baShrpFlt
                                                                    (umpMess[3] >> 24) & 0xF,//baTonic
                                                                (umpMess[3] >> 16) & 0xFF,//baType
                                                               (umpMess[3] >> 12) & 0xF,//baAlt1Type
                                                               (umpMess[3] >> 8) & 0xF,//baAlt1Deg
                                                               (umpMess[3] >> 4) & 0xF,//baAlt2Type
                                                               umpMess[1] & 0xF//baAlt2Deg
                                );
                            break;
                        }
                    }
                    break;
                }
                case FLEXDATA_PERFORMANCE: //Performance Events
                case FLEXDATA_LYRIC:{ //Lyric Events
                    uint8_t textLength = 0;
                    uint8_t text[12];
                    for(uint8_t i = 1; i<=3; i++){
                        for(uint8_t j = 24; j>=0; j=j-8){
                            uint8_t c = (umpMess[i] >> j) & 0xFF;
                            if(c){
                                text[textLength++]=c;
                            }
                        }
                    }
                    if(statusBank== FLEXDATA_LYRIC && flexLyric != nullptr) flexLyric(group, form, addrs, channel, status, text, textLength);
                    if(statusBank== FLEXDATA_PERFORMANCE && flexPerformance != nullptr) flexPerformance(group, form, addrs, channel, status, text, textLength);
                    break;

                }
            }

        }
		messPos =0;
	} else {
		messPos++;
	}
	
}

void midi2Processor::sendDiscovery(uint8_t ciType, uint8_t group, uint32_t srcMUID, uint32_t destMUID,
                                   std::array<uint8_t, 3> manuId, std::array<uint8_t, 2> familyId,
                                   std::array<uint8_t, 2> modelId, std::array<uint8_t, 4> version,
                                          uint8_t ciSupport, uint16_t sysExMax,
                                          uint8_t outputPathId,
                                            uint8_t fbIdx
                                          //uint8_t productInstanceIdLength, uint8_t* productInstanceId
                   ){
	if(sendOutSysex == nullptr) return;
	
	uint8_t sysex[13];
    MIDICI midiCiHeader;
    midiCiHeader.ciType = ciType;
    midiCiHeader.localMUID = srcMUID;
    midiCiHeader.remoteMUID = destMUID;
    midiCiHeader.ciVer = midiCIVer;
    createCIHeader(sysex, midiCiHeader);
	sendOutSysex(group,sysex,13,1);


    sysex[0] = manuId[0];
    sysex[1] = manuId[1];
    sysex[2] = manuId[2];
    sysex[3] = familyId[0];
    sysex[4] = familyId[1];
    sysex[5] = modelId[0];
    sysex[6] = modelId[1];
    sysex[7] = version[0];
    sysex[8] = version[1];
    sysex[9] = version[2];
    sysex[10] = version[3];
	sendOutSysex(group,sysex,11,2);
	//Capabilities
	sysex[0]=ciSupport;
	setBytesFromNumbers(sysex, sysExMax, 1, 4);
    if(midiCIVer<2){
        sendOutSysex(group,sysex,5,3);
        return;
    }
    sysex[5]=outputPathId;

    if(ciType==MIDICI_DISCOVERY){
        sendOutSysex(group,sysex,6,3);
    }else{
        sysex[6]=fbIdx;
        sendOutSysex(group,sysex,7,3);
    }

}

void midi2Processor::sendDiscoveryRequest(uint8_t group, uint32_t srcMUID,
                                          std::array<uint8_t, 3> manuId, std::array<uint8_t, 2> familyId,
                                          std::array<uint8_t, 2> modelId, std::array<uint8_t, 4> version,
                                          uint8_t ciSupport, uint16_t sysExMax,
                                          uint8_t outputPathId
                                          //uint8_t productInstanceIdLength, uint8_t* productInstanceId
) {
    sendDiscovery(MIDICI_DISCOVERY, group, srcMUID, M2_CI_BROADCAST,
            manuId, familyId,
            modelId, version,
            ciSupport, sysExMax,
            outputPathId,
            0
            //productInstanceIdLength,
            //productInstanceId
    );
}

void midi2Processor::sendDiscoveryReply(uint8_t group,  uint32_t srcMUID, uint32_t destMuid,
                                        std::array<uint8_t, 3> manuId, std::array<uint8_t, 2> familyId,
                                        std::array<uint8_t, 2> modelId, std::array<uint8_t, 4> version,
                                        uint8_t ciSupport, uint16_t sysExMax,
                                        uint8_t outputPathId,
                                        uint8_t fbIdx

                                        //uint8_t productInstanceIdLength, uint8_t* productInstanceId
){
    sendDiscovery(MIDICI_DISCOVERYREPLY, group, srcMUID, destMuid,
                  manuId, familyId,
                  modelId, version,
                  ciSupport, sysExMax,
                  outputPathId,
                  fbIdx
                  //productInstanceIdLength, productInstanceId
    );
}

void midi2Processor::sendEndpointInfoRequest(uint8_t group, uint32_t srcMUID, uint32_t destMuid, uint8_t status) {

    if(midiCIVer<2 || sendOutSysex == nullptr) return;
    uint8_t sysex[14];
    MIDICI midiCiHeader;
    midiCiHeader.ciType = MIDICI_ENDPOINTINFO;
    midiCiHeader.ciVer = midiCIVer;
    midiCiHeader.localMUID = srcMUID;
    midiCiHeader.remoteMUID = destMuid;
    createCIHeader(sysex, midiCiHeader);
    sysex[13] = status;
    sendOutSysex(group,sysex,14,0);
}

void midi2Processor::sendEndpointInfoReply(uint8_t group, uint32_t srcMUID, uint32_t destMuid, uint8_t status,
                                           uint16_t infoLength, uint8_t* infoData) {
    if(midiCIVer<2 || sendOutSysex == nullptr) return;
    uint8_t sysex[16];
    MIDICI midiCiHeader;
    midiCiHeader.ciType = MIDICI_ENDPOINTINFO_REPLY;
    midiCiHeader.ciVer = midiCIVer;
    midiCiHeader.localMUID = srcMUID;
    midiCiHeader.remoteMUID = destMuid;
    createCIHeader(sysex, midiCiHeader);
    sysex[13] = status;
    setBytesFromNumbers(sysex, infoLength, 14, 2);
    sendOutSysex(group,sysex,16,1);
    sendOutSysex(group,infoData,infoLength,3);
}

void midi2Processor::sendACKNAK(uint8_t ciType, uint8_t group, uint32_t srcMUID, uint32_t destMuid,
                                uint8_t originalSubId, uint8_t statusCode,
                             uint8_t statusData, uint8_t* ackNakDetails, uint16_t messageLength,
                             uint8_t* ackNakMessage){
	if(sendOutSysex == nullptr) return;
	uint8_t sysex[13];
    MIDICI midiCiHeader;
    midiCiHeader.ciType = ciType;
    midiCiHeader.ciVer = midiCIVer;
    midiCiHeader.localMUID = srcMUID;
    midiCiHeader.remoteMUID = destMuid;
    createCIHeader(sysex, midiCiHeader);
    if(midiCIVer<2){
        sendOutSysex(group,sysex,13,0);
        return;
    }
    sendOutSysex(group,sysex,13,1);
    sysex[0]=originalSubId;
    sysex[1]=statusCode;
    sysex[2]=statusData;
    sendOutSysex(group,sysex,3,2);
    sendOutSysex(group,ackNakDetails,5,2);
    setBytesFromNumbers(sysex, messageLength, 0, 2);
    sendOutSysex(group,sysex,2,2);
    sendOutSysex(group,ackNakMessage,messageLength,3);
}

void midi2Processor::sendACK(uint8_t group, uint32_t srcMUID, uint32_t destMuid, uint8_t originalSubId, uint8_t statusCode,
                                uint8_t statusData, uint8_t* ackNakDetails, uint16_t messageLength,
                                uint8_t* ackNakMessage) {

    sendACKNAK(MIDICI_ACK, group, srcMUID, destMuid, originalSubId, statusCode, statusData, ackNakDetails,
               messageLength, ackNakMessage);

}

void midi2Processor::sendNAK(uint8_t group, uint32_t srcMUID, uint32_t destMuid, uint8_t originalSubId, uint8_t statusCode,
                             uint8_t statusData, uint8_t* ackNakDetails, uint16_t messageLength,
                             uint8_t* ackNakMessage) {

    sendACKNAK(MIDICI_NAK, group, srcMUID, destMuid, originalSubId, statusCode, statusData, ackNakDetails,
               messageLength, ackNakMessage);

}


void midi2Processor::sendInvalidateMUID(uint8_t group, uint32_t srcMUID, uint32_t terminateMuid){
	if(sendOutSysex == nullptr) return;
	uint8_t sysex[13];
    MIDICI midiCiHeader;
    midiCiHeader.ciType = MIDICI_INVALIDATEMUID;
    midiCiHeader.ciVer = midiCIVer;
    midiCiHeader.localMUID = srcMUID;
    createCIHeader(sysex, midiCiHeader);
	sendOutSysex(group,sysex,13,1);
	setBytesFromNumbers(sysex, terminateMuid, 0, 4);
	sendOutSysex(group,sysex,4,3);
}


#ifndef M2_DISABLE_IDREQ
void midi2Processor::sendIdentityRequest (uint8_t group){
	if(sendOutSysex == nullptr) return;
	uint8_t sysex[]={S7UNIVERSAL_NRT,MIDI_PORT,S7IDREQUEST,0x01};
	sendOutSysex(group,sysex,4,0);
}
void midi2Processor::processDeviceID(uint8_t group, uint8_t s7Byte){
	if(syExMessInt[group].pos == 3 && s7Byte == 0x01){
        if(recvIdRequest != nullptr) recvIdRequest(group);
        return;
	}
	if(syExMessInt[group].pos == 3 && s7Byte == 0x02){
		//Identity Reply
		syExMessInt[group].buffer1[0] = s7Byte;
	}
	if(syExMessInt[group].buffer1[0] == 0x02){
		if(syExMessInt[group].pos >= 4 && syExMessInt[group].pos <= 14){
			syExMessInt[group].buffer1[syExMessInt[group].pos-3] = s7Byte;
		}

		if (syExMessInt[group].pos == 14 && recvIdResponse != nullptr){
			uint8_t manuIdR[3] = {syExMessInt[group].buffer1[1], syExMessInt[group].buffer1[2], syExMessInt[group].buffer1[3]};
			uint8_t famIdR[2] = {syExMessInt[group].buffer1[4], syExMessInt[group].buffer1[5]};
			uint8_t modelIdR[2] = {syExMessInt[group].buffer1[6], syExMessInt[group].buffer1[7]};
			uint8_t verR[4] = {syExMessInt[group].buffer1[8], syExMessInt[group].buffer1[9], syExMessInt[group].buffer1[10], syExMessInt[group].buffer1[11]};
            recvIdResponse(group, manuIdR, famIdR, modelIdR, verR);
		}
	}
}
#endif





