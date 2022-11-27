#include "../include/midi2ProcessInquiry.h"
#include "../include/midi2Processor.h"

#ifndef M2_DISABLE_PROCESSINQUIRY
void midi2Processor::processPISysex(uint8_t group, uint8_t s7Byte) {
    if(midici[group].ciVer == 1) return;

    switch (midici[group].ciType) {
        case MIDICI_PI_CAPABILITY: {
            if (syExMessInt[group].pos == 12 && recvPICapabilities != nullptr) {
                recvPICapabilities(group,midici[group]);
            }
            break;
        }
        case MIDICI_PI_CAPABILITYREPLY: {
            if (syExMessInt[group].pos == 13 && recvPICapabilitiesReply != nullptr) {
                recvPICapabilitiesReply(group,midici[group],s7Byte);
            }
            break;
        }
        case MIDICI_PI_MM_REPORT_END: {
            if (syExMessInt[group].pos == 12 && recvPIMMReportEnd != nullptr) {
                recvPIMMReportEnd(group,midici[group]);
            }
            break;
        }
        case MIDICI_PI_MM_REPORT:{
            if (syExMessInt[group].pos == 13) {//MDC
                syExMessInt[group].buffer1[0] = s7Byte;
            }
            if (syExMessInt[group].pos == 14) {//Bitmap of requested System Message Types
                syExMessInt[group].buffer1[1] = s7Byte;
            }
            if (syExMessInt[group].pos == 16) {//Bitmap of requested Channel Controller Message Types
                syExMessInt[group].buffer1[2] = s7Byte;
            }
            if (syExMessInt[group].pos == 17 &&recvPIMMReport != nullptr){
                    recvPIMMReport(group,midici[group],
                                   syExMessInt[group].buffer1[0],
                                   syExMessInt[group].buffer1[1],
                                   syExMessInt[group].buffer1[2],
                                   s7Byte);
            }
            break;
        }
        case MIDICI_PI_MM_REPORT_REPLY: {
            if (syExMessInt[group].pos == 13) {//Bitmap of requested System Message Types
                syExMessInt[group].buffer1[0] = s7Byte;
            }
            if (syExMessInt[group].pos == 15) {//Bitmap of requested Channel Controller Message Types
                syExMessInt[group].buffer1[1] = s7Byte;
            }
            if (syExMessInt[group].pos == 16 && recvPIMMReportReply != nullptr){
                    recvPIMMReportReply(group,midici[group],
                                   syExMessInt[group].buffer1[0],
                                   syExMessInt[group].buffer1[1],
                                   s7Byte);
            }
            break;
        }
        default: {
            break;
        }
    }
}


void midi2Processor::sendPICapabilityRequest(uint8_t group, uint32_t srcMUID, uint32_t destMuid){
    if(sendOutSysex == nullptr || midiCIVer==1) return;
    uint8_t sysex[13];
    MIDICI midiCiHeader;
    midiCiHeader.ciType = MIDICI_PI_CAPABILITY;
    midiCiHeader.localMUID = srcMUID;
    midiCiHeader.remoteMUID = destMuid;
    midiCiHeader.ciVer = midiCIVer;
    createCIHeader(sysex, midiCiHeader);
    sendOutSysex(group,sysex,13,0);
}

void midi2Processor::sendPICapabilityReply(uint8_t group, uint32_t srcMUID, uint32_t destMuid, uint8_t supportedFeatures){
    if(sendOutSysex == nullptr || midiCIVer==1) return;
    uint8_t sysex[14];
    MIDICI midiCiHeader;
    midiCiHeader.ciType = MIDICI_PI_CAPABILITYREPLY;
    midiCiHeader.localMUID = srcMUID;
    midiCiHeader.remoteMUID = destMuid;
    midiCiHeader.ciVer = midiCIVer;
    createCIHeader(sysex, midiCiHeader);
    sysex[13] = supportedFeatures;
    sendOutSysex(group,sysex,14,0);
}


void midi2Processor::sendPIMMReport(uint8_t group, uint32_t srcMUID, uint32_t destMuid, uint8_t destination,
                                    uint8_t MDC,  uint8_t systemBitmap,
                                    uint8_t chanContBitmap, uint8_t chanNoteBitmap){
    if(sendOutSysex == nullptr || midiCIVer==1) return;
    uint8_t sysex[18];
    MIDICI midiCiHeader;
    midiCiHeader.ciType = MIDICI_PI_MM_REPORT;
    midiCiHeader.localMUID = srcMUID;
    midiCiHeader.remoteMUID = destMuid;
    midiCiHeader.deviceId = destination;
    midiCiHeader.ciVer = midiCIVer;
    createCIHeader(sysex, midiCiHeader);
    sysex[13] = MDC;
    sysex[14] = systemBitmap;
    sysex[15] = 0;
    sysex[16] = chanContBitmap;
    sysex[17] = chanNoteBitmap;
    sendOutSysex(group,sysex,18,0);
}

void midi2Processor::sendPIMMReportReply(uint8_t group, uint32_t srcMUID, uint32_t destMuid, uint8_t destination,
                                    uint8_t systemBitmap,
                                    uint8_t chanContBitmap, uint8_t chanNoteBitmap){
    if(sendOutSysex == nullptr || midiCIVer==1) return;
    uint8_t sysex[17];
    MIDICI midiCiHeader;
    midiCiHeader.ciType = MIDICI_PI_MM_REPORT_REPLY;
    midiCiHeader.localMUID = srcMUID;
    midiCiHeader.remoteMUID = destMuid;
    midiCiHeader.deviceId = destination;
    midiCiHeader.ciVer = midiCIVer;
    createCIHeader(sysex, midiCiHeader);
    sysex[13] = systemBitmap;
    sysex[14] = 0;
    sysex[15] = chanContBitmap;
    sysex[16] = chanNoteBitmap;
    sendOutSysex(group,sysex,17,0);
}

void midi2Processor::sendPIMMReportEnd(uint8_t group, uint32_t srcMUID, uint32_t destMuid, uint8_t destination){
    if(sendOutSysex == nullptr || midiCIVer==1) return;
    uint8_t sysex[13];
    MIDICI midiCiHeader;
    midiCiHeader.ciType = MIDICI_PI_MM_REPORT_END;
    midiCiHeader.localMUID = srcMUID;
    midiCiHeader.remoteMUID = destMuid;
    midiCiHeader.deviceId = destination;
    midiCiHeader.ciVer = midiCIVer;
    createCIHeader(sysex, midiCiHeader);
    sendOutSysex(group,sysex,13,0);
}



#endif