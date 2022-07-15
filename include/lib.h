#include <string>
#include <sstream>

#ifndef LIB
#define LIB

// a very special case of system clock
// used to timestamp each order
static uint64_t timer = 0;

// Convert small strings to u64 for faster comparison
// It's faster to compare ints than strings
// The macros below map small strings to u64 ints
#define STR3(S0, S1, S2) uint64_t(S2 << 16 | S1 << 8 | S0)
#define STR4(S0, S1, S2, S3) uint64_t(S3 << 24 | S2 << 16 | S1 << 8 | S0)
#define STR5(S0, S1, S2, S3, S4) (STR4(S0, S1, S2, S3) | uint64_t(S4) << 32)
#define STR6(S0, S1, S2, S3, S4, S5) (STR3(S0, S1, S2) | uint64_t(STR3(S3, S4, S5)) << 32)

// Predefined input, hashed
// We expect to receive these, so we precompute their values
#define action_BUY STR3('B','U','Y')
#define action_SELL STR4('S','E','L','L')
#define action_CANCEL STR6('C','A','N','C','E','L')
#define action_MODIFY STR6('M','O','D','I','F','Y')
#define action_PRINT STR5('P','R','I','N','T')

#define ordtype_IOC STR3('I','O','C')
#define ordtype_GFD STR3('G','F','D')

// We expect the action verb of the input to be either one of
// the below length. It's ok to hard code in this case. 
// String comparisons are expensive
uint64_t str_id(std::string action);

// easier to deal with enums
enum Side {
    NOSIDE,
    BUY,
    SELL,
};
enum OrdType {
    NOORDTYPE,
    IOC,
    GFD,
};

// Order data type
// Holds basic information of order + timestamp of gateway
struct Order {
    Side side;
    OrdType ordtype;
    int64_t price;
    int64_t qty;
    std::string ordid;
    uint64_t ts; // not currently using it
};

// We expect the action verb of the input to be either one of
// the below length. It's ok to hard code in this case. 
// String comparisons are expensive
inline uint64_t str_id(std::string action) {
    size_t n = action.size();
    switch (n) {
    // return immediately, might save a few instructions
    case 3:
        return STR3(action[0], action[1], action[2]);
    case 4:
        return STR4(action[0], action[1], action[2], action[3]);
    case 5: 
        return STR5(action[0], action[1], action[2], action[3], action[4]);
    case 6:
        return STR6(action[0], action[1], action[2], action[3], action[4], action[5]);
    default: break;
    }
    return -1;
}

inline void ClearOrder(Order& order) {
    order.price    = -1;
    order.qty      = -1;
    order.ordid    = "";
    order.ordtype  = NOORDTYPE;
    order.side     = NOSIDE;
}

// Read string number to int64
inline int64_t Str2ll(std::string s) {
    std::stringstream stream(s);
    int64_t n;
    stream >> n;
    return n;
}

#endif