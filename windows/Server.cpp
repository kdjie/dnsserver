#include <stdio.h>
#include <stdlib.h>

#include "DNS/DNSService.h"

#pragma comment(lib, "ws2_32")

int main(int argc, char* argv[])
{
    WSADATA wsa;
    WSAStartup(0x0202, &wsa);

    using namespace dns;

    CDNSService Service;
    Service.RunLoop(10053);
    return 0;
}
