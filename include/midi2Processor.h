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
#include <cstdio>
#include <cstdint>
#include <cstdlib>

#ifndef M2_DISABLE_PE
#include <cstring>
#include <map>
#endif

#include "utils.h"

class midi2Processor{
    
  private:
  
	uint32_t umpMess[4]{};
	uint8_t messPos=0;

    std::map<uint8_t,MIDICI> midici;
    std::map<uint8_t,umpSysex7Internal> syExMessInt;

    // Message type 0x0  callbacks
    void (*recvJRClock)(/*uint8_t group, */uint16_t timing) = nullptr;
    void (*recvJRTimeStamp)(/*uint8_t group, */uint16_t timestamp) = nullptr;
    void (*recvJRProtocolReq)(/*uint8_t group,*/ bool m2, bool m1, bool jr) = nullptr;
    void (*recvJRProtocolNotify)(/*uint8_t group,*/ bool m2, bool m1, bool jr) = nullptr;

    // MIDI 1 and 2 CVM  callbacks
    void (*midiNoteOff)(uint8_t group, uint8_t mt, uint8_t channel, uint8_t noteNumber, uint16_t velocity, uint8_t attributeType,
            uint16_t attributeData) = nullptr;
    void (*midiNoteOn)(uint8_t group, uint8_t mt, uint8_t channel, uint8_t noteNumber, uint16_t velocity, uint8_t attributeType,
            uint16_t attributeData) = nullptr;
    void (*controlChange)(uint8_t group, uint8_t mt, uint8_t channel, uint8_t index, uint32_t value) = nullptr;
    void (*rpn)(uint8_t group, uint8_t channel, uint8_t bank, uint8_t index, uint32_t value) = nullptr;
    void (*nrpn)(uint8_t group, uint8_t channel, uint8_t bank, uint8_t index, uint32_t value) = nullptr;
    void (*rnrpn)(uint8_t group, uint8_t channel, uint8_t bank, uint8_t index, int32_t value) = nullptr;
    void (*rrpn)(uint8_t group, uint8_t channel, uint8_t bank, uint8_t index, int32_t value) = nullptr;
    void (*polyPressure)(uint8_t group, uint8_t mt, uint8_t channel, uint8_t noteNumber, uint32_t pressure) = nullptr;
    void (*perNotePB)(uint8_t group, uint8_t channel, uint8_t noteNumber, uint32_t pitch) = nullptr;
    void (*nrpnPerNote)(uint8_t group, uint8_t channel, uint8_t noteNumber, uint8_t index, uint32_t value) = nullptr;
    void (*rpnPerNote)(uint8_t group, uint8_t channel, uint8_t noteNumber, uint8_t index, uint32_t value) = nullptr;
    void (*perNoteManage)(uint8_t group, uint8_t channel, uint8_t noteNumber, bool detach, bool reset) = nullptr;
    void (*channelPressure)(uint8_t group, uint8_t mt, uint8_t channel, uint32_t pressure) = nullptr;
    void (*pitchBend)(uint8_t group, uint8_t mt, uint8_t channel, uint32_t value) = nullptr;
    void (*programChange)(uint8_t group, uint8_t mt, uint8_t channel, uint8_t program, bool bankValid, uint8_t bank,
            uint8_t index) = nullptr;

    //System Messages  callbacks
    void (*timingCode)(uint8_t group, uint8_t timeCode) = nullptr;
    void (*songSelect)(uint8_t group, uint8_t song) = nullptr;
    void (*songPositionPointer)(uint8_t group, uint16_t position) = nullptr;
    void (*tuneRequest)(uint8_t group) = nullptr;
    void (*timingClock)(uint8_t group) = nullptr;
    void (*seqStart)(uint8_t group) = nullptr;
    void (*seqCont)(uint8_t group) = nullptr;
    void (*seqStop)(uint8_t group) = nullptr;
    void (*activeSense)(uint8_t group) = nullptr;
    void (*systemReset)(uint8_t group) = nullptr;

    // Message Type 0xD  callbacks
    void (*flexTempo)(uint8_t group, uint32_t num10nsPQN) = nullptr;
    void (*flexTimeSig)(uint8_t group, uint8_t numerator, uint8_t denominator, uint8_t num32Notes) = nullptr;
    void (*flexMetronome)(uint8_t group, uint8_t numClkpPriCli, uint8_t bAccP1, uint8_t bAccP2, uint8_t bAccP3,
            uint8_t numSubDivCli1, uint8_t numSubDivCli2) = nullptr;
    void (*flexKeySig)(uint8_t group, uint8_t addrs, uint8_t channel, uint8_t sharpFlats, uint8_t tonic) = nullptr;
    void (*flexChord)(uint8_t group, uint8_t addrs, uint8_t channel, uint8_t chShrpFlt, uint8_t chTonic,
            uint8_t chType, uint8_t chAlt1Type, uint8_t chAlt1Deg, uint8_t chAlt2Type, uint8_t chAlt2Deg,
            uint8_t chAlt3Type, uint8_t chAlt3Deg, uint8_t chAlt4Type, uint8_t chAlt4Deg, uint8_t baShrpFlt, uint8_t baTonic,
            uint8_t baType, uint8_t baAlt1Type, uint8_t baAlt1Deg, uint8_t baAlt2Type, uint8_t baAlt2Deg) = nullptr;
    void (*flexPerformance)(uint8_t group, uint8_t form, uint8_t addrs, uint8_t channel, uint8_t status, uint8_t* text,
            uint8_t textLength) = nullptr;
    void (*flexLyric)(uint8_t group, uint8_t form, uint8_t addrs, uint8_t channel, uint8_t status, uint8_t* text,
                            uint8_t textLength) = nullptr;

    // Message Type 0xF  callbacks
    void (*midiEndpoint)(uint8_t filter) = nullptr;
    void (*functionBlock)(uint8_t fbIdx, uint8_t filter) = nullptr;
    void (*midiEndpointInfo)(uint8_t numFuncBlocks, bool m2, bool m1, bool rxjr, bool txjr)
            = nullptr;
    void (*midiEndpointDeviceInfo)(std::array<uint8_t, 3> manuId, std::array<uint8_t, 2> familyId,
                             std::array<uint8_t, 2> modelId, std::array<uint8_t, 4> version) = nullptr;
    void (*midiEndpointName)(uint8_t form, uint8_t nameLength, uint8_t* name) = nullptr;
    void (*midiEndpointProdId)(uint8_t form, uint8_t prodIdLength, uint8_t* prodId) = nullptr;
    void (*functionBlockInfo)(uint8_t fbIdx, bool active,
            uint8_t direction, uint8_t firstGroup, uint8_t groupLength,
            bool midiCIValid, uint8_t midiCIVersion, uint8_t isMIDI1, uint8_t maxS8Streams) = nullptr;
    void (*functionBlockName)(uint8_t fbIdx, uint8_t form, uint8_t nameLength, uint8_t* name) = nullptr;

    //MIDI-CI  callbacks

    // allow these callbacks to work with member functions
    std::function<bool(uint8_t group, uint32_t muid)>
                        checkMUID = nullptr;
    std::function<void(uint8_t group, MIDICI ciDetails,
                       std::array<uint8_t, 3> manuId, std::array<uint8_t, 2> familyId,
                       std::array<uint8_t, 2> modelId, std::array<uint8_t, 4> version, uint8_t ciSupport,
                       uint16_t maxSysex, uint8_t outputPathId
                       /*, uint8_t productInstanceIdLength,
                       uint8_t* productInstanceId*/
                       )> recvDiscoveryRequest = nullptr;

    void (*recvDiscoveryReply)(uint8_t group, MIDICI ciDetails, std::array<uint8_t, 3> manuId, std::array<uint8_t, 2> familyId,
                               std::array<uint8_t, 2> modelId, std::array<uint8_t, 4> version, uint8_t ciSupport, uint16_t maxSysex,
            uint8_t outputPathId,
            uint8_t fbIdx
            ) = nullptr;
    void (*recvEndPointInfo)(uint8_t group, MIDICI ciDetails, uint8_t status) = nullptr;
    void (*recvEndPointInfoReply)(uint8_t group, MIDICI ciDetails, uint8_t status, uint16_t infoLength,
                                  uint8_t* infoData) = nullptr;
    void (*recvNAK)(uint8_t group, MIDICI ciDetails, uint8_t origSubID, uint8_t statusCode,
                    uint8_t statusData, uint8_t* ackNakDetails, uint16_t messageLength,
                    uint8_t* ackNakMessage) = nullptr;
    void (*recvACK)(uint8_t group, MIDICI ciDetails, uint8_t origSubID, uint8_t statusCode,
                    uint8_t statusData, uint8_t* ackNakDetails, uint16_t messageLength,
                    uint8_t* ackNakMessage) = nullptr;
    void (*recvInvalidateMUID)(uint8_t group, MIDICI ciDetails, uint32_t terminateMuid) = nullptr;
    void (*recvUnknownMIDICI)(uint8_t group, umpSysex7Internal * syExMess, MIDICI ciDetails, uint8_t s7Byte) = nullptr;

    //other callbacks
    void (*recvUnknownSysEx)(uint8_t group, umpSysex7Internal * syExMess, uint8_t s7Byte) = nullptr;

    // make this work with class member functions
    std::function<void(uint8_t group, uint8_t *sysex ,uint16_t length, uint8_t state)>
                        sendOutSysex = nullptr;

    //internal
    void endSysex7(uint8_t group);
    void startSysex7(uint8_t group);
    void processSysEx(uint8_t group, uint8_t s7Byte);
    void processMIDICI(uint8_t group, uint8_t s7Byte);
    void sendDiscovery(uint8_t ciType, uint8_t group, uint32_t srcMUID, uint32_t destMUID, std::array<uint8_t, 3> manuId, std::array<uint8_t, 2> familyId,
                       std::array<uint8_t, 2> modelId, std::array<uint8_t, 4> version, uint8_t ciSupport, uint16_t sysExMax,
                       uint8_t outputPathId,
                       uint8_t fbIdx
                       );
    void sendACKNAK(uint8_t ciType, uint8_t group, uint32_t srcMUID, uint32_t destMuid, uint8_t originalSubId,
                   uint8_t statusCode, uint8_t statusData, uint8_t* ackNakDetails, uint16_t messageLength,
                   uint8_t* ackNakMessage);
    
  public:
	//This Device's Data
	//uint32_t m2procMUID = 0;
	uint8_t midiCIVer=2;


    midi2Processor(uint8_t CIVer);
	~midi2Processor();

	void clearUMP();
    void processUMP(uint32_t UMP);
    void sendDiscoveryRequest(uint8_t group, uint32_t srcMUID, std::array<uint8_t, 3> manuId, std::array<uint8_t, 2> familyId,
                              std::array<uint8_t, 2> modelId, std::array<uint8_t, 4> version,
                              uint8_t ciSupport, uint16_t sysExMax,
                              uint8_t outputPathId
                              );
    void sendDiscoveryReply(uint8_t group, uint32_t srcMUID, uint32_t destMuid,  std::array<uint8_t, 3> manuId, std::array<uint8_t, 2> familyId,
                            std::array<uint8_t, 2> modelId, std::array<uint8_t, 4> version,
                              uint8_t ciSupport, uint16_t sysExMax,
                            uint8_t outputPathId,
                            uint8_t fbIdx
                            );

    void sendEndpointInfoRequest(uint8_t group, uint32_t srcMUID, uint32_t destMuid, uint8_t status);
    void sendEndpointInfoReply(uint8_t group, uint32_t srcMUID, uint32_t destMuid, uint8_t status,
                               uint16_t infoLength, uint8_t* infoData);


	void sendACK(uint8_t group, uint32_t srcMUID, uint32_t destMuid, uint8_t originalSubId, uint8_t statusCode,
                 uint8_t statusData, uint8_t* ackNakDetails, uint16_t messageLength, uint8_t* ackNakMessage);
	void sendNAK(uint8_t group, uint32_t srcMUID, uint32_t destMuid, uint8_t originalSubId, uint8_t statusCode,
                 uint8_t statusData, uint8_t* ackNakDetails, uint16_t messageLength, uint8_t* ackNakMessage);
	void sendInvalidateMUID(uint8_t group, uint32_t srcMUID, uint32_t terminateMuid);
    static void createCIHeader(uint8_t* sysexHeader, MIDICI midiCiHeader);


	//-----------------------Handlers ---------------------------

    inline void setJRClock(void (*fptr)(/*uint8_t group,*/ uint16_t timing)){ recvJRClock = fptr;}
    inline void setJRTimeStamp(void (*fptr)(/*uint8_t group,*/ uint16_t timestamp)){ recvJRTimeStamp = fptr;}
    inline void setJRProtocolReq(void (*fptr)(/*uint8_t group,*/ bool m2, bool m1, bool jr)){ recvJRProtocolReq = fptr;}
    inline void setJRProtocolNotify(void (*fptr)(/*uint8_t group,*/ bool m2, bool m1, bool jr)){ recvJRProtocolNotify = fptr;}

	inline void setNoteOff(void (*fptr)(uint8_t group, uint8_t mt ,uint8_t channel, uint8_t noteNumber, uint16_t velocity,
            uint8_t attributeType, uint16_t attributeData)){ midiNoteOff = fptr; }
	inline void setNoteOn(void (*fptr)(uint8_t group, uint8_t mt ,uint8_t channel, uint8_t noteNumber, uint16_t velocity,
            uint8_t attributeType, uint16_t attributeData)){ midiNoteOn = fptr; }
	inline void setControlChange(void (*fptr)(uint8_t group, uint8_t mt, uint8_t channel, uint8_t index, uint32_t value)){
        controlChange = fptr; }
	inline void setRPN(void (*fptr)(uint8_t group, uint8_t channel,uint8_t bank,  uint8_t index, uint32_t value)){
        rpn = fptr; }
	inline void setNRPN(void (*fptr)(uint8_t group, uint8_t channel,uint8_t bank,  uint8_t index, uint32_t value)){
        nrpn = fptr; }
	inline void setRelativeNRPN(void (*fptr)(uint8_t group, uint8_t channel,uint8_t bank,  uint8_t index,
            int32_t value/*twoscomplement*/)){ rnrpn = fptr; }
	inline void setRelativeRPN(void (*fptr)(uint8_t group, uint8_t channel,uint8_t bank,  uint8_t index,
            int32_t value/*twoscomplement*/)){ rrpn = fptr; }
	inline void setPolyPressure(void (*fptr)(uint8_t group, uint8_t mt, uint8_t channel, uint8_t noteNumber, uint32_t pressure)){
        polyPressure = fptr; }

    inline void setRpnPerNote(void (*fptr)(uint8_t group, uint8_t channel, uint8_t noteNumber, uint8_t index,
            uint32_t value)){rpnPerNote = fptr; }
    inline void setNrpnPerNote(void (*fptr)(uint8_t group, uint8_t channel, uint8_t noteNumber, uint8_t index,
            uint32_t value)){nrpnPerNote = fptr; }

    inline void setPerNoteManage(void (*fptr)(uint8_t group, uint8_t channel, uint8_t noteNumber,
            bool detach, bool reset)){perNoteManage = fptr; }
    inline void setPerNotePB(void (*fptr)(uint8_t group, uint8_t channel, uint8_t noteNumber,
                                              uint32_t value)){perNotePB = fptr; }

	inline void setChannelPressure(void (*fptr)(uint8_t group, uint8_t mt, uint8_t channel, uint32_t pressure)){
        channelPressure = fptr; }
	inline void setPitchBend(void (*fptr)(uint8_t group, uint8_t mt, uint8_t channel, uint32_t value)){ pitchBend = fptr; }
	inline void setProgramChange(void (*fptr)(uint8_t group, uint8_t mt, uint8_t channel, uint8_t program, bool bankValid,
            uint8_t bank, uint8_t index)){ programChange = fptr; }
	//TODO per note etc

	inline void setTimingCode(void (*fptr)(uint8_t group, uint8_t timeCode)){ timingCode = fptr; }
	inline void setSongSelect(void (*fptr)(uint8_t group,uint8_t song)){ songSelect = fptr; }
	inline void setSongPositionPointer(void (*fptr)(uint8_t group,uint16_t position)){ songPositionPointer = fptr; }
	inline void setTuneRequest(void (*fptr)(uint8_t group)){ tuneRequest = fptr; }
	inline void setTimingClock(void (*fptr)(uint8_t group)){ timingClock = fptr; }
	inline void setSeqStart(void (*fptr)(uint8_t group)){ seqStart = fptr; }
	inline void setSeqCont(void (*fptr)(uint8_t group)){ seqCont = fptr; }
	inline void setSeqStop(void (*fptr)(uint8_t group)){ seqStop = fptr; }
	inline void setActiveSense(void (*fptr)(uint8_t group)){ activeSense = fptr; }
	inline void setSystemReset(void (*fptr)(uint8_t group)){ systemReset = fptr; }


	inline void setMidiEndpoint(void (*fptr)(uint8_t filter)){ midiEndpoint = fptr; }

	inline void setMidiEndpointNameNotify(void (*fptr)(uint8_t form, uint8_t nameLength, uint8_t* name)){
        midiEndpointName = fptr; }
    inline void setMidiEndpointProdIdNotify(void (*fptr)(uint8_t form, uint8_t nameLength, uint8_t* name)){
        midiEndpointProdId = fptr; }
	inline void setMidiEndpointInfoNotify(void (*fptr)(uint8_t numOfFuncBlocks, bool m2, bool m1, bool rxjr, bool txjr)){
        midiEndpointInfo = fptr; }
    inline void setMidiEndpointDeviceInfoNotify(void (*fptr)(std::array<uint8_t, 3> manuId, std::array<uint8_t, 2> familyId,
                                                       std::array<uint8_t, 2> modelId, std::array<uint8_t, 4> version)){
        midiEndpointDeviceInfo = fptr; }

    inline void setFunctionBlock(void (*fptr)(uint8_t filter, uint8_t fbIdx)){ functionBlock = fptr; }
    inline void setFunctionBlockNotify(void (*fptr)(uint8_t fbIdx, bool active,
                            uint8_t direction, uint8_t firstGroup, uint8_t groupLength,
                            bool midiCIValid, uint8_t midiCIVersion, uint8_t isMIDI1, uint8_t maxS8Streams)){ functionBlockInfo = fptr; }
    inline void setFunctionBlockNotify(void (*fptr)(uint8_t fbIdx, uint8_t form, uint8_t nameLength, uint8_t* name)){
        functionBlockName = fptr; }


    // using std::function lets these work with class member functions
    inline void setCheckMUID(std::function<bool(uint8_t group, uint32_t muid)> fptr){
        checkMUID = fptr; }
    inline void setRawSysEx(std::function<void(uint8_t group, uint8_t *sysex ,uint16_t length, uint8_t state)> fptr){
        sendOutSysex = fptr; }    
    inline void setRecvDiscovery(std::function<void(uint8_t group, MIDICI ciDetails,
                                                    std::array<uint8_t, 3> manuId,
                                                    std::array<uint8_t, 2> familyId,
                                                    std::array<uint8_t, 2> modelId,
                                                    std::array<uint8_t, 4> version,
                                                    uint8_t ciSupport, uint16_t maxSysex,
                                                    uint8_t outputPathId
                                                    //, uint8_t productInstanceIdLength, uint8_t* productInstanceId
                                                    )>  fptr){
        recvDiscoveryRequest = fptr;}


    inline void setRecvDiscoveryReply(void (*fptr)(uint8_t group,
                                                   MIDICI ciDetails,
                                                   std::array<uint8_t, 3> manuId,
                                                   std::array<uint8_t, 2> familyId,
                                                   std::array<uint8_t, 2> modelId,
                                                   std::array<uint8_t, 4> version,
                                                   uint8_t ciSupport,
                                                   uint16_t maxSysex,
                                                   uint8_t outputPathId,
                                                   uint8_t fbIdx
                                                   //, uint8_t productInstanceIdLength, uint8_t* productInstanceId
                                                   )){ recvDiscoveryReply = fptr;}
	inline void setRecvNAK(void (*fptr)(uint8_t group, MIDICI ciDetails, uint8_t origSubID, uint8_t statusCode,
                                        uint8_t statusData, uint8_t* ackNakDetails, uint16_t messageLength,
                                        uint8_t* ackNakMessage)){ recvNAK = fptr;}
    inline void setRecvACK(void (*fptr)(uint8_t group, MIDICI ciDetails, uint8_t origSubID, uint8_t statusCode,
                                        uint8_t statusData, uint8_t* ackNakDetails, uint16_t messageLength,
                                        uint8_t* ackNakMessage)){ recvACK = fptr;}

    inline void setRecvEndpointInfo(void (*fptr)(uint8_t group, MIDICI ciDetails, uint8_t status)){ recvEndPointInfo = fptr;}
    inline void setRecvEndpointInfoReply(void (*fptr)(uint8_t group, MIDICI ciDetails, uint8_t status, uint16_t infoLength,
                                                 uint8_t* infoData)){ recvEndPointInfoReply = fptr;}

	inline void setRecvInvalidateMUID(void (*fptr)(uint8_t group, MIDICI ciDetails, uint32_t terminateMuid)){
        recvInvalidateMUID = fptr;}
	inline void setRecvUnknownMIDICI(void (*fptr)(uint8_t group, umpSysex7Internal * syExMess, MIDICI ciDetails,
            uint8_t s7Byte)){ recvUnknownMIDICI = fptr;}
	inline void setRecvUnknownSysEx(void (*fptr)(uint8_t group, umpSysex7Internal * syExMess, uint8_t s7Byte)){
        recvUnknownSysEx = fptr;}

	
#ifndef M2_DISABLE_IDREQ
  private:
	void (*recvIdRequest)(uint8_t group) = nullptr;
	void (*recvIdResponse)(uint8_t group, std::array<uint8_t, 3> manuId, std::array<uint8_t, 2> familyId,
                                                  std::array<uint8_t, 2> modelId, std::array<uint8_t, 4> version) = nullptr;
  public:
	void sendIdentityRequest (uint8_t group);
    inline void setHandleId(void (*fptr)(uint8_t group)){ recvIdRequest = fptr;}
    inline void setHandleIdResponse(void (*fptr)(uint8_t group, std::array<uint8_t, 3> manuId, std::array<uint8_t, 2> familyId,
                                                  std::array<uint8_t, 2> modelId, std::array<uint8_t, 4> version)){ recvIdResponse = fptr;}
#endif

#ifdef M2_ENABLE_PROTOCOL
  private:
    void processProtocolSysex(uint8_t group, uint8_t s7Byte);
    void (*recvProtocolAvailable)(uint8_t group, MIDICI ciDetails, uint8_t authorityLevel, uint8_t* protocol) = nullptr;
    void (*recvSetProtocol)(uint8_t group, MIDICI ciDetails, uint8_t authorityLevel, uint8_t* protocol) = nullptr;
    void (*recvSetProtocolConfirm)(uint8_t group, MIDICI ciDetails, uint8_t authorityLevel) = nullptr;
    void (*recvProtocolTest)(uint8_t group, MIDICI ciDetails, uint8_t authorityLevel, bool testDataAccurate) = nullptr;
  public:
    void sendProtocolNegotiation(uint8_t group, uint32_t srcMUID, uint32_t destMuid,
                                 uint8_t authorityLevel, uint8_t numProtocols, uint8_t* protocols, uint8_t* currentProtocol);
    void sendProtocolNegotiationReply(uint8_t group, uint32_t srcMUID, uint32_t destMuid,
                                      uint8_t authorityLevel, uint8_t numProtocols, uint8_t* protocols);
    void sendSetProtocol(uint8_t group, uint32_t srcMUID, uint32_t destMuid,
                     uint8_t authorityLevel, uint8_t* protocol);
    void sendProtocolTest(uint8_t group, uint32_t srcMUID, uint32_t destMuid,
                         uint8_t authorityLevel);
    void sendProtocolTestResponder(uint8_t group, uint32_t srcMUID, uint32_t destMuid,
                         uint8_t authorityLevel);


    inline void setRecvProtocolAvailable(void (*fptr)(uint8_t group, MIDICI ciDetails, uint8_t authorityLevel,
            uint8_t* protocol)){ recvProtocolAvailable = fptr;}
    inline void setRecvSetProtocol(void (*fptr)(uint8_t group, MIDICI ciDetails, uint8_t authorityLevel,
            uint8_t* protocol)){ recvSetProtocol = fptr;}
    inline void setRecvSetProtocolConfirm(void (*fptr)(uint8_t group, MIDICI ciDetails, uint8_t authorityLevel)){
        recvSetProtocolConfirm = fptr;}
    inline void setRecvSetProtocolTest(void (*fptr)(uint8_t group, MIDICI ciDetails, uint8_t authorityLevel,
            bool testDataAccurate)){ recvProtocolTest = fptr;}

#endif

#ifndef M2_DISABLE_PROFILE
  private:
    void (*recvProfileInquiry)(uint8_t group, MIDICI ciDetails) = nullptr;
    void (*recvSetProfileEnabled)(uint8_t group, MIDICI ciDetails, std::array<uint8_t, 5> profile, uint8_t numberOfChannels) = nullptr;
    void (*recvSetProfileRemoved)(uint8_t group, MIDICI ciDetails, std::array<uint8_t, 5>) = nullptr;
    void (*recvSetProfileDisabled)(uint8_t group, MIDICI ciDetails, std::array<uint8_t, 5>, uint8_t numberOfChannels) = nullptr;
    void (*recvSetProfileOn)(uint8_t group, MIDICI ciDetails, std::array<uint8_t, 5> profile, uint8_t numberOfChannels) = nullptr;
    void (*recvSetProfileOff)(uint8_t group, MIDICI ciDetails, std::array<uint8_t, 5> profile) = nullptr;
    void (*recvProfileDetails)(uint8_t group, MIDICI ciDetails, std::array<uint8_t, 5> profile, uint16_t datalen, uint8_t*  data,
            uint16_t part, bool lastByteOfSet) = nullptr;
    void (*recvSetProfileDetailsInquiry)(uint8_t group, MIDICI ciDetails, std::array<uint8_t, 5> profile, uint8_t InquiryTarget) = nullptr;
    void (*recvSetProfileDetailsReply)(uint8_t group, MIDICI ciDetails, std::array<uint8_t, 5> profile, uint8_t InquiryTarget,
            uint16_t datalen, uint8_t*  data) = nullptr;

    void processProfileSysex(uint8_t group, uint8_t s7Byte);
    void sendProfileMessage(uint8_t group, uint32_t srcMUID, uint32_t destMuid,  uint8_t destination, std::array<uint8_t, 5> profile,
                            uint8_t numberOfChannels,uint8_t ciType);
  public:
	inline void setRecvProfileInquiry(void (*fptr)(uint8_t group, MIDICI ciDetails)){ recvProfileInquiry = fptr;}
	inline void setRecvProfileEnabled(void (*fptr)(uint8_t group, MIDICI ciDetails, std::array<uint8_t, 5>, uint8_t numberOfChannels)){
        recvSetProfileEnabled = fptr;}
	inline void setRecvSetProfileRemoved(void (*fptr)(uint8_t group, MIDICI ciDetails, std::array<uint8_t, 5>)){
        recvSetProfileRemoved = fptr;}
	inline void setRecvProfileDisabled(void (*fptr)(uint8_t group, MIDICI ciDetails, std::array<uint8_t, 5>, uint8_t numberOfChannels)){
        recvSetProfileDisabled = fptr;}
	inline void setRecvProfileOn(void (*fptr)(uint8_t group, MIDICI ciDetails, std::array<uint8_t, 5> profile, uint8_t numberOfChannels)){
        recvSetProfileOn = fptr;}
	inline void setRecvProfileOff(void (*fptr)(uint8_t group, MIDICI ciDetails, std::array<uint8_t, 5> profile)){
        recvSetProfileOff = fptr;}
	inline void setRecvProfileDetails(void (*fptr)(uint8_t group, MIDICI ciDetails, std::array<uint8_t, 5> profile, uint16_t datalen,
            uint8_t*  data, uint16_t part, bool lastByteOfSet)){ recvProfileDetails = fptr;}
    inline void setRecvProfileDetailsInquiry(void (*fptr)(uint8_t group, MIDICI ciDetails, std::array<uint8_t, 5> profile,
            uint8_t InquiryTarget)){recvSetProfileDetailsInquiry = fptr;}
    inline void setRecvProfileDetailsReply(void (*fptr)(uint8_t group, MIDICI ciDetails, std::array<uint8_t, 5> profile,
            uint8_t InquiryTarget, uint16_t datalen, uint8_t*  data)){recvSetProfileDetailsReply = fptr;}

	void sendProfileListRequest(uint8_t group, uint32_t srcMUID, uint32_t destMuid,  uint8_t destination);
	void sendProfileListResponse(uint8_t group, uint32_t srcMUID, uint32_t destMuid, uint8_t destination,
	        uint8_t profilesEnabledLen, uint8_t* profilesEnabled, uint8_t profilesDisabledLen , 
			uint8_t* profilesDisabled );

    void sendProfileAdd(uint8_t group, uint32_t srcMUID, uint32_t destMuid, uint8_t destination,
                        std::array<uint8_t, 5> profile);
    void sendProfileRemove(uint8_t group, uint32_t srcMUID, uint32_t destMuid, uint8_t destination,
                        std::array<uint8_t, 5> profile);

	void sendProfileOn(uint8_t group, uint32_t srcMUID, uint32_t destMuid, uint8_t destination,
                       std::array<uint8_t, 5> profile, uint8_t numberOfChannels);
	void sendProfileOff(uint8_t group, uint32_t srcMUID, uint32_t destMuid, uint8_t destination,
                        std::array<uint8_t, 5> profile);
	void sendProfileEnabled(uint8_t group, uint32_t srcMUID, uint32_t destMuid, uint8_t destination,
                            std::array<uint8_t, 5> profile, uint8_t numberOfChannels);
	void sendProfileDisabled(uint8_t group, uint32_t srcMUID, uint32_t destMuid, uint8_t destination,
                             std::array<uint8_t, 5> profile, uint8_t numberOfChannels);

    void sendProfileSpecificData(uint8_t group, uint32_t srcMUID, uint32_t destMuid, uint8_t destination,
                                 std::array<uint8_t, 5> profile, uint16_t datalen, uint8_t*  data);
    void sendProfileDetailsInquiry(uint8_t group,  uint32_t srcMUID, uint32_t destMuid, uint8_t destination,
                                   std::array<uint8_t, 5> profile, uint8_t InquiryTarget);
    void sendProfileDetailsReply(uint8_t group,  uint32_t srcMUID, uint32_t destMuid, uint8_t destination,
                                 std::array<uint8_t, 5> profile, uint8_t InquiryTarget, uint16_t datalen, uint8_t*  data);
#endif



#ifndef M2_DISABLE_PE
  private:
    std::map<reqId,peHeader> peRquestDetails;

    uint8_t numRequests;
    void (*recvPECapabilities)(uint8_t group, MIDICI ciDetails, uint8_t numSimulRequests, uint8_t majVer, uint8_t minVer) = nullptr;
    void (*recvPECapabilitiesReplies)(uint8_t group, MIDICI ciDetails, uint8_t numSimulRequests, uint8_t majVer, uint8_t minVer) = nullptr;
    void (*recvPEGetInquiry)(uint8_t group, MIDICI ciDetails, peHeader requestDetails) = nullptr;
    void (*recvPESetReply)(uint8_t group, MIDICI ciDetails, peHeader requestDetails) = nullptr;
    void (*recvPESubReply)(uint8_t group, MIDICI ciDetails, peHeader requestDetails) = nullptr;
    void (*recvPENotify)(uint8_t group, MIDICI ciDetails, peHeader requestDetails) = nullptr;
    void (*recvPESetInquiry)(uint8_t group, MIDICI ciDetails, peHeader requestDetails, uint16_t bodyLen, uint8_t*  body,
                             bool lastByteOfChunk, bool lastByteOfSet) = nullptr;
    void (*recvPESubInquiry)(uint8_t group, MIDICI ciDetails, peHeader requestDetails, uint16_t bodyLen, uint8_t*  body,
                             bool lastByteOfChunk, bool lastByteOfSet) = nullptr;

    void cleanupRequest(reqId peRequestIdx);
    void processPEHeader(uint8_t group, reqId peRequestIdx, uint8_t s7Byte);
    void processPESysex(uint8_t group, uint8_t s7Byte);
    void sendPEWithBody(uint8_t group, uint32_t srcMUID, uint32_t destMuid, uint8_t requestId,
                         uint16_t headerLen, uint8_t* header, uint16_t numberOfChunks, uint16_t numberOfThisChunk,
                         uint16_t bodyLength , uint8_t* body , uint8_t ciType);
    void sendPEHeaderOnly(uint8_t group, uint32_t srcMUID, uint32_t destMuid, uint8_t requestId,
                          uint16_t headerLen, uint8_t* header, uint8_t ciType);
  public:
	void sendPECapabilityRequest(uint8_t group,uint32_t srcMUID, uint32_t destMuid,  uint8_t numSimulRequests, uint8_t majVer, uint8_t minVer);
	void sendPECapabilityReply(uint8_t group, uint32_t srcMUID, uint32_t destMuid,  uint8_t numSimulRequests, uint8_t majVer, uint8_t minVer);

	void sendPEGet(uint8_t group, uint32_t srcMUID, uint32_t destMuid, uint8_t requestId,
	        uint16_t headerLen, uint8_t* header);

    void sendPESet(uint8_t group, uint32_t srcMUID, uint32_t destMuid, uint8_t requestId,
                   uint16_t headerLen, uint8_t* header, uint16_t numberOfChunks, uint16_t numberOfThisChunk,
                   uint16_t bodyLength , uint8_t* body);

    void sendPESub(uint8_t group, uint32_t srcMUID, uint32_t destMuid, uint8_t requestId,
                   uint16_t headerLen, uint8_t* header, uint16_t numberOfChunks, uint16_t numberOfThisChunk,
                   uint16_t bodyLength , uint8_t* body);
	
	void sendPEGetReply(uint8_t group, uint32_t srcMUID, uint32_t destMuid, uint8_t requestId,
	        uint16_t headerLen, uint8_t* header, uint16_t numberOfChunks, uint16_t numberOfThisChunk,
			uint16_t bodyLength , uint8_t* body );

    void sendPEGetReplyStreamStart(uint8_t group, uint32_t srcMUID, uint32_t destMuid,  uint8_t requestId,
                                   uint16_t headerLen, uint8_t* header, uint16_t numberOfChunks,
                                   uint16_t numberOfThisChunk, uint16_t bodyLength);
    void sendPEGetReplyStreamContinue(uint8_t group, uint16_t partialLength, uint8_t* part, bool last );
			
	void sendPESubReply(uint8_t group, uint32_t srcMUID, uint32_t destMuid, uint8_t requestId,
	        uint16_t headerLen, uint8_t* header);		
	void sendPENotify(uint8_t group, uint32_t srcMUID, uint32_t destMuid, uint8_t requestId,
	        uint16_t headerLen, uint8_t* header);
	void sendPESetReply(uint8_t group, uint32_t srcMUID, uint32_t destMuid, uint8_t requestId,
	        uint16_t headerLen, uint8_t* header);		
			
    inline void setPECapabilities(void (*fptr)(uint8_t group, MIDICI ciDetails, uint8_t numSimulRequests, uint8_t majVer, uint8_t minVer)){
        recvPECapabilities = fptr;}
    inline void setPECapabilitiesReply(void (*fptr)(uint8_t group, MIDICI ciDetails, uint8_t numSimulRequests, uint8_t majVer, uint8_t minVer)){
        recvPECapabilitiesReplies = fptr;}
    inline void setRecvPEGetInquiry(void (*fptr)(uint8_t group, MIDICI ciDetails,  peHeader requestDetails)){
        recvPEGetInquiry = fptr;}
    inline void setRecvPESetReply(void (*fptr)(uint8_t group, MIDICI ciDetails,  peHeader requestDetails)){
        recvPESetReply = fptr;}
    inline void setRecvPESubReply(void (*fptr)(uint8_t group, MIDICI ciDetails,  peHeader requestDetails)){
        recvPESubReply = fptr;}
    inline void setRecvPENotify(void (*fptr)(uint8_t group, MIDICI ciDetails,  peHeader requestDetails)){
        recvPENotify = fptr;}
    inline void setRecvPESetInquiry(void (*fptr)(uint8_t group, MIDICI ciDetails,  peHeader requestDetails,
            uint16_t bodyLen, uint8_t*  body, bool lastByteOfChunk, bool lastByteOfSet)){ recvPESetInquiry = fptr;}
    inline void setRecvPESubInquiry(void (*fptr)(uint8_t group, MIDICI ciDetails,  peHeader requestDetails,
            uint16_t bodyLen, uint8_t*  body, bool lastByteOfChunk, bool lastByteOfSet)){ recvPESubInquiry = fptr;}
#endif


#ifndef M2_DISABLE_PROCESSINQUIRY
private:
    void (*recvPICapabilities)(uint8_t group, MIDICI ciDetails) = nullptr;
    void (*recvPICapabilitiesReply)(uint8_t group, MIDICI ciDetails, uint8_t supportedFeatures) = nullptr;

    void (*recvPIMMReport)(uint8_t group, MIDICI ciDetails, uint8_t MDC, uint8_t systemBitmap,
            uint8_t chanContBitmap, uint8_t chanNoteBitmap) = nullptr;
    void (*recvPIMMReportReply)(uint8_t group, MIDICI ciDetails, uint8_t systemBitmap,
                           uint8_t chanContBitmap, uint8_t chanNoteBitmap) = nullptr;
    void (*recvPIMMReportEnd)(uint8_t group, MIDICI ciDetails) = nullptr;

    void processPISysex(uint8_t group, uint8_t s7Byte);
public:

    void sendPICapabilityRequest(uint8_t group,uint32_t srcMUID, uint32_t destMuid);
    void sendPICapabilityReply(uint8_t group, uint32_t srcMUID, uint32_t destMuid,  uint8_t supportedFeatures);

    void sendPIMMReport(uint8_t group, uint32_t srcMUID, uint32_t destMuid, uint8_t destination, uint8_t MDC,  uint8_t systemBitmap,
                        uint8_t chanContBitmap, uint8_t chanNoteBitmap);
    void sendPIMMReportReply(uint8_t group, uint32_t srcMUID, uint32_t destMuid, uint8_t destination, uint8_t systemBitmap,
                        uint8_t chanContBitmap, uint8_t chanNoteBitmap);
    void sendPIMMReportEnd(uint8_t group, uint32_t srcMUID, uint32_t destMuid, uint8_t destination);


    inline void setRecvPICapabilities(void (*fptr)(uint8_t group, MIDICI ciDetails)){
        recvPICapabilities = fptr;}
    inline void setRecvPICapabilitiesReply(void (*fptr)(uint8_t group, MIDICI ciDetails, uint8_t supportedFeatures)){
        recvPICapabilitiesReply = fptr;}

    inline void setRecvPIMMReport(void (*fptr)(uint8_t group, MIDICI ciDetails, uint8_t MDC, uint8_t systemBitmap,
                                                    uint8_t chanContBitmap, uint8_t chanNoteBitmap)){
        recvPIMMReport = fptr;}
    inline void setRecvPIMMReportReply(void (*fptr)(uint8_t group, MIDICI ciDetails, uint8_t systemBitmap,
                                                    uint8_t chanContBitmap, uint8_t chanNoteBitmap)){
        recvPIMMReportReply = fptr;}
    inline void setRecvPIMMEnd(void (*fptr)(uint8_t group, MIDICI ciDetails)){recvPIMMReportEnd = fptr;}
#endif
	
};
