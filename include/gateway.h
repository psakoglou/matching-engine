#include <vector>
#include <string>
#include "../include/lib.h"

// Gateway
// receive message from stdin, process them, 
// then stream them to matching engine
class Gateway {
private:
    std::vector<std::string> actions;
    uint64_t action_id;
    Order curr_order;
public:
    Gateway() = default;
    void Recv(std::string& input);
    std::pair<Order, uint64_t> StreamMsg();
private:
    bool VrfyInputLength();
    void InitOrder();
    bool VrfyOrder();
};