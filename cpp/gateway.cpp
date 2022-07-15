#include "../include/lib.h"
#include "../include/gateway.h"

// Verify that given input values are "legal"
bool Gateway::VrfyOrder() {
    bool ok = true;
    switch (this->action_id) {
    case action_BUY:
    case action_SELL: {
        ok &= this->curr_order.qty > 0;
        ok &= this->curr_order.price > 0;
        ok &= this->curr_order.ordtype != NOORDTYPE;
        ok &=!this->curr_order.ordid.empty();
    } break;
    case action_CANCEL: {
        ok &= !this->curr_order.ordid.empty();
    } break;
    case action_MODIFY: {
        ok &= this->curr_order.side != NOSIDE;
        ok &= this->curr_order.qty > 0;
        ok &= this->curr_order.price > 0;
        ok &= !this->curr_order.ordid.empty();
    } break;
    default: break;
    }
    return ok;
}

// Initialize order based on input
void Gateway::InitOrder() {
    switch (this->action_id) {
    case action_BUY:
    case action_SELL: {
        this->curr_order.side = action_id == action_BUY ? BUY : SELL;
        uint64_t ordtype = str_id(actions[1]);
        this->curr_order.ordtype = (ordtype == ordtype_IOC ? IOC : (ordtype == ordtype_GFD ? GFD : NOORDTYPE));
        this->curr_order.price = Str2ll(actions[2]);
        this->curr_order.qty = Str2ll(actions[3]);
        this->curr_order.ordid = actions[4];
    } break;
    case action_CANCEL: {
        this->curr_order.ordid = actions[1];
    } break;
    case action_MODIFY: {
        this->curr_order.ordid = actions[1];
        uint64_t side_id = str_id(actions[2]);
        this->curr_order.side = (side_id == action_BUY ? BUY : (side_id == action_SELL ? SELL : NOSIDE));
        this->curr_order.price = Str2ll(actions[3]);
        this->curr_order.qty = Str2ll(actions[4]);
    } break;
    default: break;
    }
    this->curr_order.ts = timer++;
}

// As per requirements, for every action type,
// check if the input length is valid
bool Gateway::VrfyInputLength() {
    size_t n = actions.size();
    bool ok = true;
    switch (this->action_id) {
    case action_BUY:
    case action_SELL:   ok &= n == 5; break;
    case action_CANCEL: ok &= n == 2; break;
    case action_MODIFY: ok &= n == 5; break;
    case action_PRINT:  ok &= n == 1; break;
    default: break;
    }
    return ok;
}

// Gateway receives input in string format from stdin
// 1. tokenize input into words
// 2. verify that input is complete and nothing is missing
// 3. create a new local order object based on input
void Gateway::Recv(std::string& inputcmd) {
    ClearOrder(this->curr_order);
    std::istringstream iss(inputcmd);
    std::string token;
    while (iss >> token) {
        this->actions.emplace_back(token);
    }
    this->action_id = str_id(actions[0]);
    if (VrfyInputLength()) {
        InitOrder();
    }
    actions.clear();
}

// Forward valid orders to matching engine only
// If the order is invalid, then the matching engine
// receives a default "action id" e.g. 0
std::pair<Order, uint64_t> Gateway::StreamMsg() {
    if (VrfyOrder()) {
        return std::make_pair(this->curr_order, this->action_id);
    }
    return std::make_pair(Order(), 0);
}