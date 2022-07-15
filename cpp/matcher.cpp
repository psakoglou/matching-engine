#include <iostream>
#include "../include/matcher.h"

// Matching core
inline void Meng::Match() {
    // best match lives at the top of bids and asks
    // if top bid price >= top ask price => match
    // after you match, update the book accordingly
    // i.e. either remove a completed order, or update is qty
    while (book.bids.begin() != book.bids.end() && book.asks.begin() != book.asks.end() && book.bids.begin()->first >= book.asks.begin()->first) {
        int64_t bid_qty = book.bids.begin()->second.orderq.front().qty;
        int64_t ask_qty = book.asks.begin()->second.orderq.front().qty;
        std::string& bid_ordid = book.bids.begin()->second.orderq.front().ordid;
        std::string& ask_ordid = book.asks.begin()->second.orderq.front().ordid;
        int64_t bid_prc   = book.bids.begin()->second.orderq.front().price;
        int64_t ask_prc   = book.asks.begin()->second.orderq.front().price;
        int64_t trade_qty = ask_qty;
        if (bid_qty > ask_qty) {
            book.bids[bid_prc].orderq.begin()->qty -= ask_qty;
            book.bids[bid_prc].total_qty -= ask_qty;
            book.asks.erase(book.asks.begin());
            orders.erase(ask_ordid);
        } else if (bid_qty == ask_qty) {
            book.bids.erase(book.bids.begin());
            orders.erase(bid_ordid); 
            book.asks.erase(book.asks.begin());
            orders.erase(ask_ordid);            
        } else {
            trade_qty = bid_qty;
            book.asks[ask_prc].orderq.begin()->qty -= bid_qty;
            book.asks[ask_prc].total_qty -= bid_qty;
            book.bids.erase(book.bids.begin());
            orders.erase(bid_ordid);
        }
        std::cout << "TRADE " << bid_ordid << " " << bid_prc << " " << trade_qty << " " << ask_ordid << " " << ask_prc << " " << trade_qty << std::endl;
    }
}

// Delete order from book, based on order id
inline void Meng::CancelOrder(const std::string& ordid) {
    auto order = orders.find(ordid);
    if (order != orders.end()) {
        if (order->second.side == BUY) {
            book.bids[order->second.price].total_qty -= order->second.ord_pos->qty;
            book.bids[order->second.price].orderq.erase(order->second.ord_pos);
            if (book.bids[order->second.price].orderq.empty()) {
                book.bids.erase(order->second.price);
            }
        }
        if (order->second.side == SELL) {
            book.asks[order->second.price].total_qty -= order->second.ord_pos->qty;
            book.asks[order->second.price].orderq.erase(order->second.ord_pos);
            if (book.asks[order->second.price].orderq.empty()) {
                book.asks.erase(order->second.price);
            }
        }
        orders.erase(order);
    }
} 

inline void Meng::Insert() {
    // don't insert a duplicate order id
    if (orders.find(curr_order.ordid) != orders.end()) {
        return;
    }
    // Get the pointer of the order list entry
    std::list<Order>::iterator pos;
    if (curr_order.side == BUY) {
        book.bids[curr_order.price].total_qty += curr_order.qty;
        book.bids[curr_order.price].orderq.push_back(curr_order);
        pos = book.bids[curr_order.price].orderq.end();
    }
    if (curr_order.side == SELL) {
        book.asks[curr_order.price].total_qty += curr_order.qty;
        book.asks[curr_order.price].orderq.push_back(curr_order);
        pos = book.asks[curr_order.price].orderq.end();
    }
    // hash the order for quicker access
    orders[curr_order.ordid] = OrderInfo(curr_order.price, curr_order.side, curr_order.ordtype, --pos);
    Match();
    // If we insert an IOC and we can't match it right away 
    // i.e. if it's still in the book, then cancel it
    if (curr_order.ordtype == IOC && orders.find(curr_order.ordid) != orders.end()) {
        CancelOrder(curr_order.ordid);
    }
}

inline void Meng::Cancel() {
    CancelOrder(curr_order.ordid);
}

// Modify order
// Since we have all information for the updated order
// it's easier to cancel the old version of the order and
// then insert it again to the right place
inline void Meng::Modify() {
    auto order = orders.find(curr_order.ordid);
    if (order != orders.end()) {
        CancelOrder(order->first);
        Insert();
        Match();
    }
}

// Print order book in specified format
inline void Meng::Print() {
    std::cout << "SELL:" << std::endl;
    for (auto elem : book.asks) {
        std::cout << elem.first << " " << elem.second.total_qty << std::endl;
    }
    std::cout << "BUY:" << std::endl;
    for (auto elem : book.bids) {
        std::cout << elem.first << " " << elem.second.total_qty << std::endl;
    }
}

// Execution router
// Trigger execution type based on order
// We only match when an order
inline void Meng::Exec() {
    switch (this->action_id) {
    case action_BUY:     
    case action_SELL:    Insert();     break;
    case action_CANCEL:  Cancel();     break;
    case action_MODIFY:  Modify();     break;
    case action_PRINT:   Print();      break;
    default:                           break;
    }
}

// Matching engine entry point
// Receive order and action id from Gateway
// Then execute order
void Meng::Update(const std::pair<Order, uint64_t>& msg) {
    curr_order = std::move(msg.first);
    this->action_id = msg.second;
    Exec();
    ClearOrder(this->curr_order);
}