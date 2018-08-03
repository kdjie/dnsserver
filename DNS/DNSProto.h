//============================================================================
// Name        : DNSProto.h
// Author      : kdjie
// Version     : 0.1
// Copyright   : @2008
// Description : 14166097@qq.com
//============================================================================

#pragma once

#include "DNSPacket.h"

#ifdef WIN32
#include <winsock.h>
#endif

#ifdef __GNUC__
#include <netdb.h>
#endif

namespace dns
{
    // 请求/应答定义
    enum
    {
        QR_QUERY = 0,                // 请求
        QR_ANSWER = 1                // 应答
    };

    // 查询种类定义
    enum
    {
        OPCODE_QUERY = 0,           // 标准查询
        OPCODE_IQUERY = 1,          // 反向查询
        OPCODE_STATUS = 2           // 服务器状态查询
    };

    // 响应代码定义
    enum
    {
        RCODE_SUCCESS = 0,          // 成功
        RCODE_FORMAT_ERROR = 1,     // 格式错误
        RCODE_SERVER_FAILD = 2,     // 服务器失败
        RCODE_NAME_ERROR = 3,       // 名字错误
        RCODE_NOT_SUPPORT = 4,      // 不支持
        RCODE_REFUSED = 5           // 拒绝
    };

    // 首部标志各字段的偏移定义
    enum
    {
        FLAG_OFFSET_QR = 15,        // 请求/应答标志位偏移
        FLAG_OFFSET_OPCODE = 11,    // 查询种类标志位偏移
        FLAG_OFFSET_AA = 10,        // 授权应答标志位偏移
        FLAG_OFFSET_TC = 9,         // 截断应答标志位偏移
        FLAG_OFFSET_RD = 8,         // 要求递归标志位偏移
        FLAG_OFFSET_RA = 7,         // 支持递归标志位偏移
        FLAG_OFFSET_ZERO = 4,       // 3位保留标志位偏移
        FLAG_OFFSET_RCODE = 0       // 响应代码标志位偏移
    };

    // 首部标志各字段掩码定义
    enum
    {
        FLAG_MASK_QR = 0x0001,      // 取请求/应答标志位掩码
        FLAG_MASK_OPCODE = 0x000F,  // 取查询种类标志位掩码
        FLAG_MASK_AA = 0x0001,      // 取授权应答标志位掩码
        FLAG_MASK_TC = 0x0001,      // 取截断应答标志位掩码
        FLAG_MASK_RD = 0x0001,      // 取要求递归标志位掩码
        FLAG_MASK_RA = 0x0001,      // 取支持递归标志位掩码
        FLAG_MASK_ZERO = 0x0007,    // 取3位保留标志位掩码
        FLAG_MASK_RCODE = 0x000F    // 取响应代码标志位掩码
    };

    // 获取查询/应答
    inline uint16_t getQR(uint16_t uFlags)
    {
        return ( (uFlags >> FLAG_OFFSET_QR) & FLAG_MASK_QR );
    }
    // 设置查询/应答
    inline uint16_t setQR(uint16_t uFlags, uint16_t uV)
    {
        return ( uFlags | ( (uV & FLAG_MASK_QR) << FLAG_OFFSET_QR ) );
    }

    // 获取查询种类
    inline uint16_t getOpCode(uint16_t uFlags)
    {
        return ( (uFlags >> FLAG_OFFSET_OPCODE) & FLAG_MASK_OPCODE );
    }
    // 设置查询种类
    inline uint16_t setOpCode(uint16_t uFlags, uint16_t uV)
    {
        return ( uFlags | ( (uV & FLAG_MASK_OPCODE) << FLAG_OFFSET_OPCODE ) );
    }

    // 获取响应代码
    inline uint16_t getRCode(uint16_t uFlags)
    {
        return ( (uFlags >> FLAG_OFFSET_RCODE) & FLAG_MASK_RCODE );
    }
    // 设置响应代码
    inline uint16_t setRCode(uint16_t uFlags, uint16_t uV)
    {
        return ( uFlags | ( (uV & FLAG_MASK_RCODE) << FLAG_OFFSET_RCODE ) );
    }

    // 获取AA授权应答标志
    inline bool isSetAA(uint16_t uFlags)
    {
        return ( (uFlags >> FLAG_OFFSET_AA) & FLAG_MASK_AA ) == FLAG_MASK_AA ;
    }
    // 设置AA授权应答标志
    inline uint16_t setAA(uint16_t uFlags, uint16_t uV)
    {
        return ( uFlags | ( (uV & FLAG_MASK_AA) << FLAG_OFFSET_AA ) );
    }

    // 获取TC截断应答标志
    inline bool isSetTC(uint16_t uFlags)
    {
        return ( (uFlags >> FLAG_OFFSET_TC) & FLAG_MASK_TC ) == FLAG_MASK_TC ;
    }
    // 设置TC截断应答标志
    inline uint16_t setTC(uint16_t uFlags, uint16_t uV)
    {
        return ( uFlags | ( (uV & FLAG_MASK_TC) << FLAG_OFFSET_TC ) );
    }

    // 获取RD要求递归标志
    inline bool isSetRD(uint16_t uFlags)
    {
        return ( (uFlags >> FLAG_OFFSET_RD) & FLAG_MASK_RD ) == FLAG_MASK_RD ;
    }
    // 设置RD要求递归标志
    inline uint16_t setRD(uint16_t uFlags, uint16_t uV)
    {
        return ( uFlags | ( (uV & FLAG_MASK_RD) << FLAG_OFFSET_RD ) );
    }

    // 获取RA支持递归标志
    inline bool isSetRA(uint16_t uFlags)
    {
        return ( (uFlags >> FLAG_OFFSET_RA) & FLAG_MASK_RA ) == FLAG_MASK_RA ;
    }
    // 设置RA支持递归标志
    inline uint16_t setRA(uint16_t uFlags, uint16_t uV)
    {
        return ( uFlags | ( (uV & FLAG_MASK_RA) << FLAG_OFFSET_RA ) );
    }

    // 首部定义
    struct SDNSHead
    {
        uint16_t uId;
        uint16_t uFlags;
        uint16_t uRequestCount;
        uint16_t uAnswerCount;
        uint16_t uAuthCount;
        uint16_t uExtraCount;

        SDNSHead()
        {
            uId = 0;
            uFlags = 0;
            uRequestCount = 0;
            uAnswerCount = 0;
            uAuthCount = 0;
            uExtraCount = 0;
        }

        std::string Marshal()
        {
            std::string strPack;
            strPack << (uint16_t)htons(uId) << (uint16_t)htons(uFlags) << (uint16_t)htons(uRequestCount) << (uint16_t)htons(uAnswerCount) << (uint16_t)htons(uAuthCount) << (uint16_t)htons(uExtraCount);
            return strPack;
        }
        void Unmarshal(std::string& strUnpack)
        {
            strUnpack >> uId >> uFlags >> uRequestCount >> uAnswerCount >> uAuthCount >> uExtraCount;

            uId = ntohs(uId);
            uFlags = ntohs(uFlags);
            uRequestCount = ntohs(uRequestCount);
            uAnswerCount = ntohs(uAnswerCount);
            uAuthCount = ntohs(uAuthCount);
            uExtraCount = ntohs(uExtraCount);
        }
    };

    // 查询分类定义
    enum
    {
        CLASS_IN = 1,           // 互联网地址
        CLASS_CS = 2,           // CSNET
        CLASS_CH = 3,           // CHAOS
        CLASS_HS = 4            // HS
    };

    // 查询类型定义
    enum
    {
        TYPE_A = 1,             // 地址
        TYPE_NS = 2,            // 授权服务器
        TYPE_CNAME = 5,         // CNAME
        TYPE_PTR = 12,          // 指针记录
        TYPE_MX = 15            // 邮件交换记录
    };

    // 查询报文定义
    struct SQueryPacket
    {
        std::vector<std::string> vecQName;
        uint16_t uQueryType;
        uint16_t uQueryClass;

        SQueryPacket()
        {
            uQueryType = TYPE_A;
            uQueryClass = CLASS_IN;
        }

        std::string Marshal()
        {
            std::string strPack;
            strPack << vecQName << (uint16_t)htons(uQueryType) << (uint16_t)htons(uQueryClass);
            return strPack;
        }
        void Unmarshal(std::string& strUnpack)
        {
            strUnpack >> vecQName >> uQueryType >> uQueryClass;

            uQueryType = ntohs(uQueryType);
            uQueryClass = ntohs(uQueryClass);
        }
    };

    // 应答报文格式
    struct SAnswerPacket
    {
        std::vector<std::string> vecQName;
        uint16_t uQueryType;
        uint16_t uQueryClass;
        uint32_t uTtl;
        uint16_t uAddrLen;
        uint32_t uAddr;

        SAnswerPacket()
        {
            uQueryType = TYPE_A;
            uQueryClass = CLASS_IN;
            uTtl = 255;
            uAddrLen = 4;
            uAddr = 0;
        }

        std::string Marshal()
        {
            std::string strPack;
            strPack << vecQName << (uint16_t)htons(uQueryType) << (uint16_t)htons(uQueryClass) << (uint32_t)htonl(uTtl) << (uint16_t)htons(uAddrLen) << (uint32_t)htonl(uAddr);
            return strPack;
        }
        void Unmarshal(std::string& strUnpack)
        {
            strUnpack >> vecQName >> uQueryType >> uQueryClass >> uTtl >> uAddrLen >> uAddr;

            uQueryType = ntohs(uQueryType);
            uQueryClass = ntohs(uQueryClass);
            uTtl = ntohl(uTtl);
            uAddrLen = ntohs(uAddrLen);
            uAddr = ntohl(uAddr);
        }
    };
}
