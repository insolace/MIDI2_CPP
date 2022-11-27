#include "../include/midi2Profiles.h"
#include "../include/midi2Processor.h"
#ifndef M2_DISABLE_PROFILE




void midi2Processor::processProfileSysex(uint8_t group, uint8_t s7Byte){
	switch (midici[group].ciType){
		case MIDICI_PROFILE_INQUIRY: //Profile Inquiry
        	if (syExMessInt[group].pos == 12 && recvProfileInquiry != nullptr){
				recvProfileInquiry(group, midici[group]);
			}
			break;
		case MIDICI_PROFILE_INQUIRYREPLY: { //Reply to Profile Inquiry
            //Enabled Profiles Length
            if (syExMessInt[group].pos == 13 || syExMessInt[group].pos == 14) {
                syExMessInt[group].intbuffer1[0] += s7Byte << (7 * (syExMessInt[group].pos - 13));
            }

            //Disabled Profile Length
            int enabledProfileOffset = syExMessInt[group].intbuffer1[0] * 5 + 13;
            if (
                    syExMessInt[group].pos == enabledProfileOffset
                    || syExMessInt[group].pos == 1 + enabledProfileOffset
                    ) {
                syExMessInt[group].intbuffer1[1] += s7Byte << (7 * (syExMessInt[group].pos - enabledProfileOffset));
            }

            if (syExMessInt[group].pos >= 15 && syExMessInt[group].pos < enabledProfileOffset) {
                uint8_t pos = (syExMessInt[group].pos - 13) % 5;
                syExMessInt[group].buffer1[pos] = s7Byte;
                if (pos == 4 && recvSetProfileEnabled != nullptr) {

                    recvSetProfileEnabled(group, midici[group], {syExMessInt[group].buffer1[0], syExMessInt[group].buffer1[1],
                                                                 syExMessInt[group].buffer1[2], syExMessInt[group].buffer1[3],
                                                                 syExMessInt[group].buffer1[4]},0);
                }
            }

            if (syExMessInt[group].pos >= 2 + enabledProfileOffset &&
                syExMessInt[group].pos < enabledProfileOffset + syExMessInt[group].intbuffer1[1] * 5) {
                uint8_t pos = (syExMessInt[group].pos - 13) % 5;
                syExMessInt[group].buffer1[pos] = s7Byte;
                if (pos == 4 && recvSetProfileDisabled != nullptr) {
                    recvSetProfileDisabled(group, midici[group], {syExMessInt[group].buffer1[0], syExMessInt[group].buffer1[1],
                                                                  syExMessInt[group].buffer1[2], syExMessInt[group].buffer1[3],
                                                                  syExMessInt[group].buffer1[4]}
                                                                  ,0);
                }
            }
            break;
        }

        case MIDICI_PROFILE_ADD:
        case MIDICI_PROFILE_REMOVE:
        case MIDICI_PROFILE_ENABLED:
        case MIDICI_PROFILE_DISABLED:
        case MIDICI_PROFILE_SETOFF:
		case MIDICI_PROFILE_SETON: { //Set Profile On Message
            bool complete = false;
            if (syExMessInt[group].pos >= 13 && syExMessInt[group].pos <= 17) {
                syExMessInt[group].buffer1[syExMessInt[group].pos - 13] = s7Byte;
            }
            if (syExMessInt[group].pos == 17 &&
                    (midici[group].ciVer == 1 || midici[group].ciType==MIDICI_PROFILE_ADD || midici[group].ciType==MIDICI_PROFILE_REMOVE)
            ){
                complete = true;
            }
            if(midici[group].ciVer > 1 && (syExMessInt[group].pos == 18 || syExMessInt[group].pos == 19)){
                syExMessInt[group].intbuffer1[0] += s7Byte << (7 * (syExMessInt[group].pos - 18 ));
            }
            if (syExMessInt[group].pos == 19 && midici[group].ciVer > 1){
                complete = true;
            }

            if(complete){
                if (midici[group].ciType == MIDICI_PROFILE_ADD && recvSetProfileDisabled != nullptr)
                    recvSetProfileDisabled(group, midici[group], {syExMessInt[group].buffer1[0], syExMessInt[group].buffer1[1],
                                                            syExMessInt[group].buffer1[2], syExMessInt[group].buffer1[3],
                                                            syExMessInt[group].buffer1[4]}, 0);

                if (midici[group].ciType == MIDICI_PROFILE_REMOVE && recvSetProfileRemoved != nullptr)
                    recvSetProfileRemoved(group, midici[group], {syExMessInt[group].buffer1[0], syExMessInt[group].buffer1[1],
                                                                  syExMessInt[group].buffer1[2], syExMessInt[group].buffer1[3],
                                                                  syExMessInt[group].buffer1[4]});

                if (midici[group].ciType == MIDICI_PROFILE_SETON && recvSetProfileOn != nullptr)
                    recvSetProfileOn(group, midici[group], {syExMessInt[group].buffer1[0], syExMessInt[group].buffer1[1],
                                                            syExMessInt[group].buffer1[2], syExMessInt[group].buffer1[3],
                                                            syExMessInt[group].buffer1[4]}, (uint8_t)syExMessInt[group].intbuffer1[0]);

                if (midici[group].ciType == MIDICI_PROFILE_SETOFF && recvSetProfileOff != nullptr)
                    recvSetProfileOff(group, midici[group], {syExMessInt[group].buffer1[0], syExMessInt[group].buffer1[1],
                                                             syExMessInt[group].buffer1[2], syExMessInt[group].buffer1[3],
                                                             syExMessInt[group].buffer1[4]});

                if (midici[group].ciType == MIDICI_PROFILE_ENABLED && recvSetProfileEnabled != nullptr)
                    recvSetProfileEnabled(group, midici[group], {syExMessInt[group].buffer1[0], syExMessInt[group].buffer1[1],
                                                                 syExMessInt[group].buffer1[2], syExMessInt[group].buffer1[3],
                                                                 syExMessInt[group].buffer1[4]}, (uint8_t)syExMessInt[group].intbuffer1[0]);

                if (midici[group].ciType == MIDICI_PROFILE_DISABLED && recvSetProfileDisabled != nullptr)
                    recvSetProfileDisabled(group, midici[group], {syExMessInt[group].buffer1[0], syExMessInt[group].buffer1[1],
                                                                  syExMessInt[group].buffer1[2], syExMessInt[group].buffer1[3],
                                                                  syExMessInt[group].buffer1[4]}, (uint8_t)syExMessInt[group].intbuffer1[0]);

            }
            break;
        }

        case MIDICI_PROFILE_DETAILS_INQUIRY:{
            if (syExMessInt[group].pos >= 13 && syExMessInt[group].pos <= 17) {
                syExMessInt[group].buffer1[syExMessInt[group].pos - 13] = s7Byte;
            }
            if (syExMessInt[group].pos == 19 && recvSetProfileDetailsInquiry != nullptr){ //Inquiry Target
                recvSetProfileDetailsInquiry(group, midici[group], {syExMessInt[group].buffer1[0], syExMessInt[group].buffer1[1],
                                                                    syExMessInt[group].buffer1[2], syExMessInt[group].buffer1[3],
                                                                    syExMessInt[group].buffer1[4]}, s7Byte);
            }

            break;
        }

        case MIDICI_PROFILE_DETAILS_REPLY:{
            if (syExMessInt[group].pos >= 13 && syExMessInt[group].pos <= 17) {
                syExMessInt[group].buffer1[syExMessInt[group].pos - 13] = s7Byte;
            }
            if (syExMessInt[group].pos == 19){//Inquiry Target
                syExMessInt[group].buffer1[5] = s7Byte;
            }

            if(syExMessInt[group].pos == 20 || syExMessInt[group].pos == 21){ //Inquiry Target Data length (dl)
                syExMessInt[group].intbuffer1[0] += s7Byte << (7 * (syExMessInt[group].pos - 20 ));
            }

            if (syExMessInt[group].pos >= 22 && syExMessInt[group].pos <= 22 + syExMessInt[group].intbuffer1[0]){
                syExMessInt[group].buffer1[syExMessInt[group].pos - 23 + 6] = s7Byte; //product ID
            }

            if (syExMessInt[group].pos == 22 + syExMessInt[group].intbuffer1[0] && recvSetProfileDetailsInquiry != nullptr){
                recvSetProfileDetailsReply(group, midici[group], {syExMessInt[group].buffer1[0], syExMessInt[group].buffer1[1],
                                                                  syExMessInt[group].buffer1[2], syExMessInt[group].buffer1[3],
                                                                  syExMessInt[group].buffer1[4]},
                                           syExMessInt[group].buffer1[5],
                                           syExMessInt[group].intbuffer1[0],
                                           &(syExMessInt[group].buffer1[6])
                                           );
            }

            break;
        }

        case MIDICI_PROFILE_SPECIFIC_DATA:
            //Profile
            if(syExMessInt[group].pos >= 13 && syExMessInt[group].pos <= 17){
                syExMessInt[group].buffer1[syExMessInt[group].pos-13] = s7Byte;
                return;
            }
            if(syExMessInt[group].pos >= 18 || syExMessInt[group].pos <= 21){ //Length of Following Profile Specific Data
                syExMessInt[group].intbuffer1[0] += s7Byte << (7 * (syExMessInt[group].pos - 18 ));
                syExMessInt[group].intbuffer1[1] = 1;
                return;
            }


            //******************

            uint16_t charOffset = (syExMessInt[group].pos - 22) % S7_BUFFERLEN;
            uint16_t dataLength = syExMessInt[group].intbuffer1[0];
            if(
                    (syExMessInt[group].pos >= 22 && syExMessInt[group].pos <= 21 + dataLength)
                    || 	(dataLength == 0 && syExMessInt[group].pos == 21)
                    ){
                if(dataLength != 0 )syExMessInt[group].buffer1[charOffset] = s7Byte;

                bool lastByteOfSet = (syExMessInt[group].pos == 21 + dataLength);

                if(charOffset == S7_BUFFERLEN -1
                   || syExMessInt[group].pos == 21 + dataLength
                   || dataLength == 0
                        ){
                    recvProfileDetails(group, midici[group], {syExMessInt[group].buffer1[0], syExMessInt[group].buffer1[1],
                                                              syExMessInt[group].buffer1[2], syExMessInt[group].buffer1[3],
                                                              syExMessInt[group].buffer1[4]}, charOffset+1, syExMessInt[group].buffer1, syExMessInt[group].intbuffer1[1], lastByteOfSet);
                    syExMessInt[group].intbuffer1[1]++;
                }
            }


            //***********

            break;
	}
}

void midi2Processor::sendProfileListRequest(uint8_t group, uint32_t srcMUID, uint32_t destMuid, uint8_t destination){
	if(sendOutSysex == nullptr) return;
	uint8_t sysex[13];
    MIDICI midiCiHeader;
    midiCiHeader.ciType = MIDICI_PROFILE_INQUIRY;
    midiCiHeader.localMUID = srcMUID;
    midiCiHeader.remoteMUID = destMuid;
    midiCiHeader.deviceId = destination;
    midiCiHeader.ciVer = midiCIVer;
    createCIHeader(sysex, midiCiHeader);
	sendOutSysex(group,sysex,13,0);
}


void midi2Processor::sendProfileListResponse(uint8_t group, uint32_t srcMUID, uint32_t destMuid,  uint8_t destination, uint8_t profilesEnabledLen, uint8_t* profilesEnabled, uint8_t profilesDisabledLen , uint8_t* profilesDisabled ){
	if(sendOutSysex == nullptr) return;
	uint8_t sysex[13];
    MIDICI midiCiHeader;
    midiCiHeader.ciType = MIDICI_PROFILE_INQUIRYREPLY;
    midiCiHeader.localMUID = srcMUID;
    midiCiHeader.remoteMUID = destMuid;
    midiCiHeader.deviceId = destination;
    midiCiHeader.ciVer = midiCIVer;
    createCIHeader(sysex, midiCiHeader);
	sendOutSysex(group,sysex,13,1);
	
	setBytesFromNumbers(sysex, profilesEnabledLen, 0, 2);
	sendOutSysex(group,sysex,2,2);
	sendOutSysex(group,profilesEnabled,profilesEnabledLen*5,2);
	
	setBytesFromNumbers(sysex, profilesDisabledLen, 0, 2);
	sendOutSysex(group,sysex,2,2);
	sendOutSysex(group,profilesDisabled,profilesDisabledLen*5,3);
}

void midi2Processor::sendProfileMessage(uint8_t group, uint32_t srcMUID, uint32_t destMuid,  uint8_t destination,
                                        std::array<uint8_t, 5> profile,
                                        uint8_t numberOfChannels, uint8_t ciType){
	if(sendOutSysex == nullptr) return;
	uint8_t sysex[13];
    MIDICI midiCiHeader;
    midiCiHeader.ciType = ciType;
    midiCiHeader.localMUID = srcMUID;
    midiCiHeader.remoteMUID = destMuid;
    midiCiHeader.deviceId = destination;
    midiCiHeader.ciVer = midiCIVer;
    createCIHeader(sysex, midiCiHeader);
	sendOutSysex(group,sysex,13,1);
    sysex[0] = profile[0];
    sysex[1] = profile[1];
    sysex[2] = profile[2];
    sysex[3] = profile[3];
    sysex[4] = profile[4];
    if(midiCIVer==1 || ciType==MIDICI_PROFILE_ADD || ciType==MIDICI_PROFILE_REMOVE){
        sendOutSysex(group,sysex,5,3);
        return;
    }
    sendOutSysex(group,sysex,5,2);

    setBytesFromNumbers(sysex, numberOfChannels, 0, 2);
    sendOutSysex(group,sysex,2,3);

}

void midi2Processor::sendProfileAdd(uint8_t group, uint32_t srcMUID, uint32_t destMuid, uint8_t destination,
                                    std::array<uint8_t, 5> profile){
    sendProfileMessage(group, srcMUID, destMuid, destination, profile, 0,
                       (uint8_t) MIDICI_PROFILE_ADD);
}

void midi2Processor::sendProfileRemove(uint8_t group, uint32_t srcMUID, uint32_t destMuid, uint8_t destination,
                                    std::array<uint8_t, 5> profile){
    sendProfileMessage(group, srcMUID, destMuid, destination, profile, 0,
                       (uint8_t) MIDICI_PROFILE_REMOVE);
}

void midi2Processor::sendProfileOn(uint8_t group, uint32_t srcMUID, uint32_t destMuid, uint8_t destination,
                                   std::array<uint8_t, 5> profile, uint8_t numberOfChannels){
    sendProfileMessage(group, srcMUID, destMuid, destination, profile, numberOfChannels,
                       (uint8_t) MIDICI_PROFILE_SETON);
}

void midi2Processor::sendProfileOff(uint8_t group, uint32_t srcMUID, uint32_t destMuid, uint8_t destination,
                                    std::array<uint8_t, 5> profile){
    sendProfileMessage(group, srcMUID, destMuid, destination, profile, 0,
                       (uint8_t) MIDICI_PROFILE_SETOFF);
}

void midi2Processor::sendProfileEnabled(uint8_t group, uint32_t srcMUID, uint32_t destMuid, uint8_t destination,
                                        std::array<uint8_t, 5> profile,
                                        uint8_t numberOfChannels){
    sendProfileMessage(group, srcMUID, destMuid, destination, profile, numberOfChannels,
                       (uint8_t) MIDICI_PROFILE_ENABLED);
}

void midi2Processor::sendProfileDisabled(uint8_t group, uint32_t srcMUID, uint32_t destMuid, uint8_t destination,
                                         std::array<uint8_t, 5> profile,
                                         uint8_t numberOfChannels){
    sendProfileMessage(group, srcMUID, destMuid, destination, profile, numberOfChannels,
                       (uint8_t) MIDICI_PROFILE_DISABLED);
}


void midi2Processor::sendProfileSpecificData(uint8_t group, uint32_t srcMUID, uint32_t destMuid, uint8_t destination,
                                             std::array<uint8_t, 5> profile, uint16_t datalen, uint8_t*  data){
    if(sendOutSysex == nullptr) return;
    uint8_t sysex[13];
    MIDICI midiCiHeader;
    midiCiHeader.ciType = MIDICI_PROFILE_SPECIFIC_DATA;
    midiCiHeader.localMUID = srcMUID;
    midiCiHeader.remoteMUID = destMuid;
    midiCiHeader.deviceId = destination;
    midiCiHeader.ciVer = midiCIVer;
    createCIHeader(sysex, midiCiHeader);
    sendOutSysex(group,sysex,13,1);
    sysex[0] = profile[0];
    sysex[1] = profile[1];
    sysex[2] = profile[2];
    sysex[3] = profile[3];
    sysex[4] = profile[4];
    sendOutSysex(group,sysex,5,2);
    setBytesFromNumbers(sysex, datalen, 0, 4);
    sendOutSysex(group,sysex,4,2);
    sendOutSysex(group,data,datalen,3);
}

void midi2Processor::sendProfileDetailsInquiry(uint8_t group,  uint32_t srcMUID, uint32_t destMuid, uint8_t destination,
                                               std::array<uint8_t, 5> profile, uint8_t InquiryTarget){
    if(sendOutSysex == nullptr || midiCIVer < 2) return;
    uint8_t sysex[13];
    MIDICI midiCiHeader;
    midiCiHeader.ciType = MIDICI_PROFILE_SPECIFIC_DATA;
    midiCiHeader.localMUID = srcMUID;
    midiCiHeader.remoteMUID = destMuid;
    midiCiHeader.deviceId = destination;
    midiCiHeader.ciVer = midiCIVer;
    createCIHeader(sysex, midiCiHeader);
    sendOutSysex(group,sysex,13,1);
    sysex[0] = profile[0];
    sysex[1] = profile[1];
    sysex[2] = profile[2];
    sysex[3] = profile[3];
    sysex[4] = profile[4];
    sysex[5] = InquiryTarget;
    sendOutSysex(group,sysex,6,3);
}

void midi2Processor::sendProfileDetailsReply(uint8_t group,  uint32_t srcMUID, uint32_t destMuid, uint8_t destination,
                                             std::array<uint8_t, 5> profile, uint8_t InquiryTarget, uint16_t datalen, uint8_t*  data){
    if(sendOutSysex == nullptr || midiCIVer < 2) return;
    uint8_t sysex[13];
    MIDICI midiCiHeader;
    midiCiHeader.ciType = MIDICI_PROFILE_SPECIFIC_DATA;
    midiCiHeader.localMUID = srcMUID;
    midiCiHeader.remoteMUID = destMuid;
    midiCiHeader.deviceId = destination;
    midiCiHeader.ciVer = midiCIVer;
    createCIHeader(sysex, midiCiHeader);
    sendOutSysex(group,sysex,13,1);
    sysex[0] = profile[0];
    sysex[1] = profile[1];
    sysex[2] = profile[2];
    sysex[3] = profile[3];
    sysex[4] = profile[4];
    sysex[5] = InquiryTarget;
    sendOutSysex(group,sysex,6,2);
    setBytesFromNumbers(sysex, datalen, 0, 2);
    sendOutSysex(group,sysex,3,2);
    sendOutSysex(group,data,datalen,3);
}

#endif
