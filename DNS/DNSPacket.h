//============================================================================
// Name        : DNSPacket.h
// Author      : kdjie
// Version     : 0.1
// Copyright   : @2008
// Description : 14166097@qq.com
//============================================================================

#pragma once

#include <string>
#include <exception>
#include <vector>

#ifdef WIN32
typedef __int8 uint8_t;
typedef __int16 uint16_t;
typedef __int32 uint32_t;
#endif

#ifdef __GNUC__
#include <stdint.h>
#endif

namespace dns
{

    class packet_exception :
        public std::exception
     {
     public:
        packet_exception(const std::string& text) : m_text(text) {}
        packet_exception(const packet_exception& e) : m_text(e.m_text) {}
        virtual ~packet_exception() throw() {}
        virtual const char* what() const throw() { return m_text.c_str(); }
     private:
        std::string m_text;
     };

	inline std::string & operator << (std::string & buf, uint8_t u8)
	{
	    buf.append((const char*)&u8, 1);
	    return buf;
	}

	inline std::string & operator << (std::string & buf, uint16_t u16)
	{
	    buf.append((const char*)&u16, 2);
	    return buf;
	}

	inline std::string & operator << (std::string & buf, uint32_t u32)
	{
	    buf.append((const char*)&u32, 4);
	    return buf;
	}

	inline std::string & operator << (std::string & buf, const std::string & str)
	{
	    if (str.size() > 255)
	    {
	        throw packet_exception("str too big");
	    }

        uint8_t u8 = str.size();
	    buf.append((const char*)&u8, 1);
	    buf.append(str.data(), u8);
	    return buf;
	}

	inline std::string & operator << (std::string & buf, const std::vector<std::string> & vecstr)
	{
	    for (std::vector<std::string>::const_iterator c_iter = vecstr.begin(); c_iter != vecstr.end(); ++c_iter)
	    {
	        buf << *c_iter;
	    }
	    buf << (uint8_t)0;
	    return buf;
	}

	inline std::string & operator >> (std::string & buf, uint8_t & u8)
	{
	    if (buf.size() < 1)
	    {
	        throw packet_exception("data not enough");
	    }

	    u8 = *(uint8_t*)buf.data();
	    buf.erase(0, 1);
	    return buf;
	}

	inline std::string & operator >> (std::string & buf, uint16_t & u16)
	{
	    if (buf.size() < 2)
	    {
	        throw packet_exception("data not enough");
	    }

	    u16 = *(uint16_t*)buf.data();
	    buf.erase(0, 2);
	    return buf;
	}

	inline std::string & operator >> (std::string & buf, uint32_t & u32)
	{
	    if (buf.size() < 4)
	    {
	        throw packet_exception("data not enough");
	    }

	    u32 = *(uint32_t*)buf.data();
	    buf.erase(0, 4);
	    return buf;
	}

	inline std::string & operator >> (std::string & buf, std::string & str)
	{
	    uint8_t u8 = 0;
	    buf >> u8;

	    if (u8 > 0)
	    {
	        if ((uint32_t)buf.size() < (uint32_t)u8)
	        {
	            throw packet_exception("data not enough");
	        }

            str.append(buf.data(), u8);
	        buf.erase(0, u8);
	    }

	    return buf;
	}

	inline std::string & operator >> (std::string & buf, std::vector<std::string> & vecstr)
	{
	    for (;;)
	    {
	        std::string str = "";
	        buf >> str;
	        if (str == "")
	            break;

	        vecstr.push_back(str);
	    }
	    return buf;
	}

	inline std::string qname2str(const std::vector<std::string> & vecstr)
	{
	    std::string strQName = "";

	    for (std::vector<std::string>::const_iterator c_iter = vecstr.begin(); c_iter != vecstr.end(); ++c_iter)
	    {
	        if (strQName != "")
	            strQName += ".";
	        strQName += (*c_iter);
	    }

	    return strQName;
	}
}
