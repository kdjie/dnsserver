//============================================================================
// Name        : DNSService.h
// Author      : kdjie
// Version     : 0.1
// Copyright   : @2008
// Description : 14166097@qq.com
//============================================================================

#pragma once

#include "DNSProto.h"

#ifdef WIN32
#include <winsock.h>
typedef int socklen_t;
#endif

#ifdef __GNUC__
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define SOCKET_ERROR (-1)
#define INVALID_SOCKET (-1)
typedef int SOCKET;
#endif

namespace dns
{
    class CDNSService
    {
    public:
        void RunLoop(uint16_t uPort);

    private:
        void __CloseSocket(SOCKET Socket);
        bool __ParseDNSRequest(const std::string& strPacket, SDNSHead& Head, std::vector<SQueryPacket>& vecQuery);
        void __ShowDNSHead(const SDNSHead& Head);
        void __ShowDNSQuery(const SQueryPacket& Query);
        void __ShowStandAnswer(const SAnswerPacket& Answer);
        std::string __MakeStandAnswer(SDNSHead& Head, std::vector<SQueryPacket>& vecQuery);
        std::string __MakeNotSupport(SDNSHead& Head, std::vector<SQueryPacket>& vecQuery);
    };
}
