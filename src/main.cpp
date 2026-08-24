#include <cstdint>
#include <iostream>
#include <vector>
#include <algorithm>

// Specifies std::uint8_t so that enum only takes 1 byte instead of 4 as enum defaults to int
enum class Side : std::uint8_t { Buy, Sell };
// A limit order stops if the ask is above its price; a market order never does
enum class OrderType : std::uint8_t { Limit, Market };

struct Order {
    std::uint64_t id;
    Side          side;
    OrderType     type;
    std::int64_t  price;      // integer ticks — never a float
    std::uint64_t quantity;
    std::uint64_t sequence;   // arrival order, for time priority
};

struct OrderBook {
    // A vector is C++'s dynamic array, like Python list
    std::vector<Order> bids;   // buy orders
    std::vector<Order> asks;   // sell orders

    // Index of the highest bid / lowest ask, or -1 if that side is empty.
    int best_bid_index() {
        if (bids.empty()) return -1;
        int best = 0;
        for (int i = 1; i < (int)bids.size(); i++)
            if (bids[i].price > bids[best].price) best = i;
        return best;
    }
    int best_ask_index() {
        if (asks.empty()) return -1;
        int best = 0;
        for (int i = 1; i < (int)asks.size(); i++)
            if (asks[i].price < asks[best].price) best = i;
        return best;
    }

    void submit(Order incoming) {
        if (incoming.side == Side::Buy) {
            // Match against asks while the buy crosses the cheapest ask
            while (incoming.quantity > 0) {
                int idx = best_ask_index();
                // Can't trade
                if (idx == -1) break; 
                if (incoming.type == OrderType::Limit && asks[idx].price > incoming.price) break;
                std::uint64_t fill = std::min(incoming.quantity, asks[idx].quantity);
                std::cout << "TRADE  " << fill << " @ " << asks[idx].price << "\n";
                incoming.quantity  -= fill;
                asks[idx].quantity -= fill;
                if (asks[idx].quantity == 0) asks.erase(asks.begin() + idx);
            }
            // Only a limit order rests its remainder; a market remainder is cancelled
            if (incoming.quantity > 0 && incoming.type == OrderType::Limit)
                bids.push_back(incoming);           // rest remainder
        } else {
            // Mirror image: a sell matches against bids while it crosses the best bid.
            while (incoming.quantity > 0) {
                int idx = best_bid_index();
                if (idx == -1) break;
                if (incoming.type == OrderType::Limit && bids[idx].price < incoming.price) break;
                std::uint64_t fill = std::min(incoming.quantity, bids[idx].quantity);
                std::cout << "TRADE  " << fill << " @ " << bids[idx].price << "\n";
                incoming.quantity  -= fill;
                bids[idx].quantity -= fill;
                if (bids[idx].quantity == 0) bids.erase(bids.begin() + idx);
            }
            if (incoming.quantity > 0 && incoming.type == OrderType::Limit)
                asks.push_back(incoming);
        }
    }

    void print() {
        std::cout << "  ASKS:\n";
        for (const Order& o : asks)
            std::cout << "    #" << o.id << "  " << o.quantity << " @ " << o.price << "\n";
        std::cout << "  BIDS:\n";
        for (const Order& o : bids)
            std::cout << "    #" << o.id << "  " << o.quantity << " @ " << o.price << "\n";
    }
};

int main() {
    OrderBook book;
    book.submit({1, Side::Sell, OrderType::Limit, 10100, 150, 0});
    book.submit({2, Side::Sell, OrderType::Limit, 10200, 100, 1});

    std::cout << "Book before:\n";
    book.print();

    std::cout << "\nIncoming: LIMIT BUY 200 @ 10150\n";
    book.submit({3, Side::Buy, OrderType::Limit, 10150, 200, 2});

    std::cout << "\nIncoming: MARKET BUY 300 (no price limit)\n";
    book.submit({4, Side::Buy, OrderType::Market, 0, 300, 2});

    std::cout << "\nBook after:\n";
    book.print();
    return 0;
}