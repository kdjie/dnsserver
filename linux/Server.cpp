#include <stdio.h>
#include <stdlib.h>

#include "DNS/DNSService.h"

int main(int argc, char* argv[])
{
    using namespace dns;

    CDNSService Service;
    Service.RunLoop(10053);
    return 0;
}
