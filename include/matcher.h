#include <list>
#include <map>
#include <unordered_map>
#include "../include/lib.h"

// Aux data structure to help with 0(1) deletion, modification
// Gives quick access to price, side, ordertype and the position of
// the order to the order list.
// That way we can delete in O(1)
struct OrderInfo {
    int64_t price;
    Side side;
    OrdType ordtype;
    std::list<Order>::iterator ord_pos;
    OrderInfo() = default;
    OrderInfo(int64_t price_, Side side_, OrdType ordtype_, std::list<Order>::iterator ord_pos_) 
        : price(price_), side(side_), ordtype(ordtype_), ord_pos(ord_pos_) {}
};

// Node with order list and total qty / price
// I use a linked list here for O(1) insertion/deletion
// By pushing back in the list, we maintain order
struct OrderList {
    uint64_t total_qty;
    std::list<Order> orderq;
};

// order book structure
// decreasing bids (sort key : price)
// increasing asks (sort key : price)
struct OrderBook {
    std::map<int64_t, OrderList, std::greater<int64_t>> bids;
    std::map<int64_t, OrderList>                        asks;
};

// Matching engine
class Meng {
private:
    OrderBook                                   book;
    std::unordered_map<std::string, OrderInfo>  orders;
    Order                                       curr_order;
    uint64_t                                    action_id;
public:
    Meng() = default;
    void Update(const std::pair<Order, uint64_t>& msg);
private:
    inline void Exec();
    inline void Insert();
    inline void Match();
    inline void Cancel();
    inline void CancelOrder(const std::string& ordid);
    inline void Modify();
    inline void Print();
};