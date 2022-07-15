#include <iostream>

#include "../include/gateway.h"
#include "../include/matcher.h"
#include "../include/lib.h"

int main() {
    // Engine receives input from stdin
    // Input is initially processed by
    // a "gateway" process and then is 
    // forwarded to matching engine
    std::string inputcmd;
    Gateway gw;
    Meng meng;
    while (std::getline(std::cin, inputcmd)) {
        gw.Recv(inputcmd);
        meng.Update(gw.StreamMsg());
    }
    return 0;
}