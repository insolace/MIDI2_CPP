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

#ifndef UTILS_H
#define UTILS_H

#define M2_DISABLE_IDREQ 1
#define EXP_MIDICI_PE_EXPERIMENTAL_PATH 150
//#define M2_ENABLE_PROTOCOL 1
#define S7_BUFFERLEN 488

#include <QCoreApplication>
#include <cstdint>
#include <cmath>

#define NOTE_OFF 0x80
#define NOTE_ON 0x90
#define KEY_PRESSURE 0xA0
#define CC 0xB0
#define RPN 0x20
#define NRPN 0x30
#define RPN_RELATIVE 0x40
#define NRPN_RELATIVE 0x50
#define PROGRAM_CHANGE 0xC0
#define CHANNEL_PRESSURE 0xD0
#define PITCH_BEND 0xE0
#define PITCH_BEND_PERNOTE 0x60
#define NRPN_PERNOTE 0x10
#define RPN_PERNOTE 0x00
#define PERNOTE_MANAGE 0xF0

#define SYSEX_START 0xF0
#define TIMING_CODE 0xF1
#define SPP 0xF2
#define SONG_SELECT 0xF3
#define TUNEREQUEST 0xF6
#define SYSEX_STOP 0xF7
#define TIMINGCLOCK 0xF8
#define SEQSTART 0xFA
#define SEQCONT 0xFB
#define SEQSTOP 0xFC
#define ACTIVESENSE 0xFE
#define SYSTEMRESET 0xFF

#define UTILITY_NOOP 0x0
#define UTILITY_JRCLOCK 0x1
#define UTILITY_JRTS 0x2
#define UTILITY_DELTACLOCKTICK 0x3
#define UTILITY_DELTACLOCKSINCE 0x4
#define UTILITY_STARTOFSEQ 0x5
#define UTILITY_ENDOFFILE 0x6
#define UTILITY_PROTOCOL_REQUEST 0x9
#define UTILITY_PROTOCOL_NOTIFY 0xA

#define FLEXDATA_COMMON 0x00
#define FLEXDATA_COMMON_TEMPO 0x00
#define FLEXDATA_COMMON_TIMESIG 0x01
#define FLEXDATA_COMMON_METRONOME 0x02
#define FLEXDATA_COMMON_KEYSIG 0x05
#define FLEXDATA_COMMON_CHORD 0x06
#define FLEXDATA_PERFORMANCE 0x01
#define FLEXDATA_LYRIC 0x02

#define MIDIENDPOINT 0x000
#define MIDIENDPOINT_INFO_NOTIFICATION 0x001
#define MIDIENDPOINT_DEVICEINFO_NOTIFICATION 0x002
#define MIDIENDPOINT_NAME_NOTIFICATION 0x003
#define MIDIENDPOINT_PRODID_NOTIFICATION 0x004
#define FUNCTIONBLOCK 0x010
#define FUNCTIONBLOCK_INFO_NOTFICATION 0x011
#define FUNCTIONBLOCK_NAME_NOTIFICATION 0x012

#ifndef S7_BUFFERLEN
#define S7_BUFFERLEN	36
#endif
#define S7UNIVERSAL_NRT 0x7E
#define S7UNIVERSAL_RT 0x7F
#define S7IDREQUEST 0x06
#define S7MIDICI 0x0D

#define MIDICI_DISCOVERY 0x70
#define MIDICI_DISCOVERYREPLY 0x71
#define MIDICI_ENDPOINTINFO 0x72
#define MIDICI_ENDPOINTINFO_REPLY 0x73
#define MIDICI_INVALIDATEMUID 0x7E
#define MIDICI_ACK 0x7D
#define MIDICI_NAK 0x7F

#define MIDICI_PROTOCOL_NEGOTIATION 0x10
#define MIDICI_PROTOCOL_NEGOTIATION_REPLY 0x11
#define MIDICI_PROTOCOL_SET 0x12
#define MIDICI_PROTOCOL_TEST 0x13
#define MIDICI_PROTOCOL_TEST_RESPONDER 0x14
#define MIDICI_PROTOCOL_CONFIRM 0x15

#define MIDICI_PROFILE_INQUIRY 0x20
#define MIDICI_PROFILE_INQUIRYREPLY 0x21
#define MIDICI_PROFILE_SETON 0x22
#define MIDICI_PROFILE_SETOFF 0x23
#define MIDICI_PROFILE_ENABLED 0x24
#define MIDICI_PROFILE_DISABLED 0x25
#define MIDICI_PROFILE_ADD 0x26
#define MIDICI_PROFILE_REMOVE 0x27
#define MIDICI_PROFILE_DETAILS_INQUIRY 0x28
#define MIDICI_PROFILE_DETAILS_REPLY 0x29
#define MIDICI_PROFILE_SPECIFIC_DATA 0x2F

#define MIDICI_PE_CAPABILITY 0x30
#define MIDICI_PE_CAPABILITYREPLY 0x31
#define MIDICI_PE_GET 0x34
#define MIDICI_PE_GETREPLY 0x35
#define MIDICI_PE_SET 0x36
#define MIDICI_PE_SETREPLY 0x37
#define MIDICI_PE_SUB 0x38
#define MIDICI_PE_SUBREPLY 0x39
#define MIDICI_PE_NOTIFY 0x3F

#define MIDICI_PI_CAPABILITY 0x40
#define MIDICI_PI_CAPABILITYREPLY 0x41
#define MIDICI_PI_MM_REPORT 0x42
#define MIDICI_PI_MM_REPORT_REPLY 0x43
#define MIDICI_PI_MM_REPORT_END 0x44

#define MIDICI_PE_COMMAND_START 1
#define MIDICI_PE_COMMAND_END 2
#define MIDICI_PE_COMMAND_PARTIAL 3
#define MIDICI_PE_COMMAND_FULL 4
#define MIDICI_PE_COMMAND_NOTIFY 5

#define EXP_MIDICI_PE_ACTION_COPY 1
#define EXP_MIDICI_PE_ACTION_MOVE 2
#define EXP_MIDICI_PE_ACTION_DELETE 3
#define EXP_MIDICI_PE_ACTION_CREATE_DIR 4

#define MIDICI_PE_ASCII 1
#define MIDICI_PE_MCODED7 1
#define MIDICI_PE_MCODED7ZLIB 3

#define MIDI_PORT 0x7F
#define M2_CI_BROADCAST 0xFFFFFFF

#define PE_HEAD_STATE_IN_OBJECT          2
#define PE_HEAD_STATE_IN_STRING          4
#define PE_HEAD_STATE_IN_NUMBER          8
#define PE_HEAD_STATE_IN_BOOL            1

#define PE_HEAD_KEY		16
#define PE_HEAD_VALUE	32
#define PE_HEAD_BUFFERLEN	36

#define UMP_VER_MAJOR	1
#define UMP_VER_MINOR	1

#ifndef EXP_MIDICI_PE_EXPERIMENTAL_PATH
#define EXP_MIDICI_PE_EXPERIMENTAL_PATH 1
#endif

#define UMP_UTILITY 0x0
#define UMP_SYSTEM 0x1
#define UMP_M1CVM 0x2
#define UMP_SYSEX7 0x3
#define UMP_M2CVM 0x4
#define UMP_DATA 0x5
#define UMP_FLEX_DATA 0xD
#define UMP_MIDI_ENDPOINT 0xF

#ifdef MIDI1BYTESTREAMONLY
    #define UMPGROUPS 1
#else
    #define UMPGROUPS 16
#endif

typedef std::tuple<uint8_t, uint8_t> reqId;

struct MIDICI{
    MIDICI() : deviceId(MIDI_PORT),ciType(255),ciVer(1), remoteMUID(0), localMUID(0)  {}
    uint8_t deviceId;
    uint8_t ciType;
    uint8_t ciVer;
    uint32_t remoteMUID;
    uint32_t localMUID;

};

struct umpSysex7Internal {
    umpSysex7Internal() : realtime(0), universalId(0), peRequestIdx(255), pos(0), buffer1("") {}
    uint8_t realtime;
    uint8_t universalId;
    uint8_t peRequestIdx;
    uint16_t pos;
    uint8_t buffer1[S7_BUFFERLEN];
    /*
     * in Discovery this is [sysexID1,sysexID2,sysexID3,famId1,famid2,modelId1,modelId2,ver1,ver2,ver3,ver4,...product Id]
     * in Profiles this is [pf1, pf1, pf3, pf4, pf5]
     * in Protocols this is [pr1, pr2, pr3, pr4, pr5]
     */

    uint16_t intbuffer1[4];
    /* in Discovery this is [ciSupport, maxSysex, output path id, product id Length]
     * in Profile Inquiry Reply, this is [Enabled Profiles Length, Disabled Profile Length]
     * in Profile On/Off/Enabled/Disabled, this is [numOfChannels]
     * in PE this is [header length, Body Length]
     */
};


#ifndef M2_DISABLE_PE
struct peHeader {
    peHeader() : requestId(255),group(255),muid(0), resource(""),
                 command(0), action(0), resId(""), subscribeId(""), path(""),
                 offset(-1), limit(-1), status(0), partial(false),totalChunks(-1), numChunk(-1),
                 partialChunkCount(-1), mutualEncoding(-1), mediaType(""), _pvoid(nullptr),
                 _headerProp(0), _headerState(PE_HEAD_KEY + PE_HEAD_STATE_IN_OBJECT),
                 _headerPos(0) {}
    uint8_t requestId;
    uint8_t group;
    uint32_t muid;
    char resource[PE_HEAD_BUFFERLEN];
    uint8_t command;
    uint8_t action;
    char resId[PE_HEAD_BUFFERLEN];
    char subscribeId[PE_HEAD_BUFFERLEN];
    char path[EXP_MIDICI_PE_EXPERIMENTAL_PATH];
    int  offset;
    int  limit;
    int  status;
    bool partial;
    int  totalChunks;
    int  numChunk;
    int  partialChunkCount;
    int mutualEncoding;
    char mediaType[PE_HEAD_BUFFERLEN];
    void * _pvoid;
    uint8_t _headerProp;
    uint8_t _headerState;
    uint8_t _headerPos;

};
#endif



uint32_t scaleUp(uint32_t srcVal, uint8_t srcBits, uint8_t dstBits);

uint32_t scaleDown(uint32_t srcVal, uint8_t srcBits, uint8_t dstBits);


void setBytesFromNumbers(uint8_t* message, long number, uint8_t start, uint8_t amount);


#endif
