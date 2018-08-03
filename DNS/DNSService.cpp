//============================================================================
// Name        : DNSService.cpp
// Author      : kdjie
// Version     : 0.1
// Copyright   : @2008
// Description : 14166097@qq.com
//============================================================================

#include "DNSService.h"

#include <stdio.h>
#include <stdlib.h>

using namespace dns;

void CDNSService::RunLoop(uint16_t uPort)
{
	SOCKET SockS = socket(AF_INET, SOCK_DGRAM, 0);
	if (SockS == INVALID_SOCKET)
	{
		throw packet_exception("socket() failed!");
	}

	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(uPort);

	int ret = bind(SockS, (struct sockaddr*)&sin, sizeof(sin));
	if (ret == SOCKET_ERROR)
	{
	    __CloseSocket(SockS);
		throw packet_exception("bind() failed!");
	}

	while (true)
	{
		char szBuff[65536] = {0};
		struct sockaddr_in sinfrom;
		socklen_t sinlen = sizeof(sinfrom);
		int bytes_in = recvfrom(SockS, szBuff, sizeof(szBuff), 0, (struct sockaddr*)&sinfrom, &sinlen);
		if (bytes_in > 0)
		{
			printf("recvfrom %s >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> \n", inet_ntoa(*(struct in_addr*)&sinfrom.sin_addr));
			printf("datalen %d \n", bytes_in);

			std::string strPacket(szBuff, bytes_in);

			// 解析请求
			SDNSHead Head;
			std::vector<SQueryPacket> vecQuery;
			if (!__ParseDNSRequest(strPacket, Head, vecQuery))
			{
			    printf("ParseDNSRequest() failed! \n");
			    continue;
			}

			bool bSupport = true;

			// 打印
			__ShowDNSHead(Head);
			for (std::vector<SQueryPacket>::const_iterator c_iter = vecQuery.begin(); c_iter != vecQuery.end(); ++c_iter)
			{
			     __ShowDNSQuery(*c_iter);

			     if (c_iter->uQueryType != TYPE_A || c_iter->uQueryClass != CLASS_IN)
			     {
			        bSupport = false;
			     }
			}

			std::string strAnswer = "";

			// 不支持
			if (!bSupport)
			{
			    strAnswer = __MakeNotSupport(Head, vecQuery);
			}
			else
			{
			    strAnswer = __MakeStandAnswer(Head, vecQuery);
			}

			// 响应
			sendto(SockS, strAnswer.data(), strAnswer.size(), 0, (struct sockaddr*)&sinfrom, sizeof(sinfrom));
		}
	}

	__CloseSocket(SockS);
}

void CDNSService::__CloseSocket(SOCKET Socket)
{
#ifdef WIN32
    closesocket(Socket);
#endif
#ifdef __GNUC__
    close(Socket);
#endif
}

// 解析请求
bool CDNSService::__ParseDNSRequest(const std::string& strPacket, SDNSHead& Head, std::vector<SQueryPacket>& vecQuery)
{
    try
    {
        std::string strUnpack = strPacket;
        Head.Unmarshal(strUnpack);

        for (int i = 0; i < Head.uRequestCount; ++i)
        {
            SQueryPacket Query;
            Query.Unmarshal(strUnpack);
            vecQuery.push_back(Query);
        }
    }
    catch (const std::exception& e)
    {
        printf("catch exception:%s \n", e.what());
        return false;
    }

    return true;
}

void CDNSService::__ShowDNSHead(const SDNSHead& Head)
{
    printf("Head: \n");
    printf("\t Id:%d \n", Head.uId);
    printf("\t QR:%d \n", getQR(Head.uFlags));
    printf("\t OpCode:%d \n", getOpCode(Head.uFlags));
    printf("\t RCode:%d \n", getRCode(Head.uFlags));
    printf("\t AA:%d \n", isSetAA(Head.uFlags));
    printf("\t TC:%d \n", isSetTC(Head.uFlags));
    printf("\t RD:%d \n", isSetRD(Head.uFlags));
    printf("\t RA:%d \n", isSetRA(Head.uFlags));
    printf("\t RequestCount:%d \n", Head.uRequestCount);
    printf("\t AnswerCount:%d \n", Head.uAnswerCount);
    printf("\t AuthCount:%d \n", Head.uAuthCount);
    printf("\t ExtraCount:%d \n", Head.uExtraCount);
}

void CDNSService::__ShowDNSQuery(const SQueryPacket& Query)
{
    printf("Query: \n");
    printf("\t name:%s \n", qname2str(Query.vecQName).c_str());
    printf("\t type:%d \n", Query.uQueryType);
    printf("\t class:%d \n", Query.uQueryClass);
}

void CDNSService::__ShowStandAnswer(const SAnswerPacket& Answer)
{
    printf("Answer: \n");
    printf("\t name:%s \n", qname2str(Answer.vecQName).c_str());
    printf("\t type:%d \n", Answer.uQueryType);
    printf("\t class:%d \n", Answer.uQueryClass);
    printf("\t ttl:%d \n", Answer.uTtl);
    printf("\t addrlen:%d \n", Answer.uAddrLen);
    uint32_t uAddrL = ntohl(Answer.uAddr);
    printf("\t addr:%s \n", inet_ntoa(*(struct in_addr*)&uAddrL) );
}

std::string CDNSService::__MakeStandAnswer(SDNSHead& Head, std::vector<SQueryPacket>& vecQuery)
{
    std::string strPack;

    Head.uFlags = setQR(Head.uFlags, QR_ANSWER);
    Head.uFlags = setAA(Head.uFlags, 1);
    Head.uFlags = setRCode(Head.uFlags, RCODE_SUCCESS);
    Head.uAnswerCount = vecQuery.size();

    strPack += Head.Marshal();

    for (std::vector<SQueryPacket>::iterator iter = vecQuery.begin(); iter != vecQuery.end(); ++iter)
    {
        strPack += iter->Marshal();
    }

    for (std::vector<SQueryPacket>::iterator iter = vecQuery.begin(); iter != vecQuery.end(); ++iter)
    {
        SAnswerPacket Answer;
        Answer.vecQName = iter->vecQName;
        Answer.uQueryType = iter->uQueryType;
        Answer.uQueryClass = iter->uQueryClass;
        Answer.uAddr = ntohl( inet_addr("127.0.0.1") );

        // 打印
        __ShowStandAnswer(Answer);

        strPack += Answer.Marshal();
    }

    return strPack;
}

std::string CDNSService::__MakeNotSupport(SDNSHead& Head, std::vector<SQueryPacket>& vecQuery)
{
    std::string strPack;

    Head.uFlags = setQR(Head.uFlags, QR_ANSWER);
    Head.uFlags = setRCode(Head.uFlags, RCODE_NOT_SUPPORT);
    Head.uAnswerCount = 0;

    strPack += Head.Marshal();

    return strPack;
}
