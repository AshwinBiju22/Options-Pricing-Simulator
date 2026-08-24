#pragma once
#include <cstdint>
#include <vector>
#include <algorithm>   // std::min

namespace qe {

enum class Side { Buy, Sell };
enum class OrderType { Limit, Market };

struct Order {
    std::uint64_t id;         // assigned by the engine in arrival order (so it also encodes time priority)
    Side          side;
    OrderType     type;
    std::int64_t  price;      // ignored for Market orders
    std::uint64_t quantity;
};

// A record of one fill.
struct Trade {
    std::uint64_t aggressor_id;   // the incoming order that crossed the book
    std::uint64_t resting_id;     // the resting order it matched against
    std::int64_t  price;          // executed at the resting order's price
    std::uint64_t quantity;
};

// What submit() gives back: the id the engine assigned, plus any fills.
struct SubmitResult {
    std::uint64_t      order_id;
    std::vector<Trade> trades;
};

struct OrderBook {
    std::vector<Order> bids;
    std::vector<Order> asks;
    std::uint64_t next_id = 1;    // engine-managed; increments on every submit

    // ---- top of book (internal index helpers; -1 if that side is empty) ----
    int best_bid_index() const {
        if (bids.empty()) return -1;
        int best = 0;
        for (int i = 1; i < (int)bids.size(); i++)
            if (bids[i].price > bids[best].price ||
               (bids[i].price == bids[best].price && bids[i].id < bids[best].id))
                best = i;
        return best;
    }
    int best_ask_index() const {
        if (asks.empty()) return -1;
        int best = 0;
        for (int i = 1; i < (int)asks.size(); i++)
            if (asks[i].price < asks[best].price ||
               (asks[i].price == asks[best].price && asks[i].id < asks[best].id))
                best = i;
        return best;
    }

    const Order* best_bid() const {
        int i = best_bid_index();
        return i == -1 ? nullptr : &bids[i];
    }
    const Order* best_ask() const {
        int i = best_ask_index();
        return i == -1 ? nullptr : &asks[i];
    }

    // ---- cancel a resting order by id ----
    bool cancel(std::uint64_t id) {
        for (int i = 0; i < (int)bids.size(); i++)
            if (bids[i].id == id) { bids.erase(bids.begin() + i); return true; }
        for (int i = 0; i < (int)asks.size(); i++)
            if (asks[i].id == id) { asks.erase(asks.begin() + i); return true; }
        return false;
    }

    // ---- submit an order: match it, rest any remainder, return id + trades ----
    SubmitResult submit(Side side, OrderType type, std::int64_t price, std::uint64_t quantity) {
        Order incoming{ next_id++, side, type, price, quantity };
        std::vector<Trade> trades;

        if (side == Side::Buy) {
            while (incoming.quantity > 0) {
                int idx = best_ask_index();
                if (idx == -1) break;
                if (type == OrderType::Limit && asks[idx].price > incoming.price) break;
                std::uint64_t fill = std::min(incoming.quantity, asks[idx].quantity);
                trades.push_back({ incoming.id, asks[idx].id, asks[idx].price, fill });
                incoming.quantity  -= fill;
                asks[idx].quantity -= fill;
                if (asks[idx].quantity == 0) asks.erase(asks.begin() + idx);
            }
            if (incoming.quantity > 0 && type == OrderType::Limit)
                bids.push_back(incoming);
        } else {
            while (incoming.quantity > 0) {
                int idx = best_bid_index();
                if (idx == -1) break;
                if (type == OrderType::Limit && bids[idx].price < incoming.price) break;
                std::uint64_t fill = std::min(incoming.quantity, bids[idx].quantity);
                trades.push_back({ incoming.id, bids[idx].id, bids[idx].price, fill });
                incoming.quantity  -= fill;
                bids[idx].quantity -= fill;
                if (bids[idx].quantity == 0) bids.erase(bids.begin() + idx);
            }
            if (incoming.quantity > 0 && type == OrderType::Limit)
                asks.push_back(incoming);
        }
        return { incoming.id, trades };
    }
};

}