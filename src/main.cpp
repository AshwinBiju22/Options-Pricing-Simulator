#include "order_book.hpp"
#include <iostream>

using namespace qe;

static void print_book(const OrderBook& book) {
    std::cout << "  ASKS:\n";
    for (const Order& o : book.asks)
        std::cout << "    #" << o.id << "  " << o.quantity << " @ " << o.price << "\n";
    std::cout << "  BIDS:\n";
    for (const Order& o : book.bids)
        std::cout << "    #" << o.id << "  " << o.quantity << " @ " << o.price << "\n";
}

int main() {
    OrderBook book;

    book.submit(Side::Sell, OrderType::Limit, 10100, 150);
    book.submit(Side::Sell, OrderType::Limit, 10200, 100);

    std::cout << "Book before:\n";
    print_book(book);

    std::cout << "\nIncoming: BUY 200 @ 10150\n";
    SubmitResult r = book.submit(Side::Buy, OrderType::Limit, 10150, 200);

    std::cout << "Order #" << r.order_id << " produced " << r.trades.size() << " trade(s):\n";
    for (const Trade& t : r.trades)
        std::cout << "  " << t.quantity << " @ " << t.price
                  << "   (#" << t.aggressor_id << " hit #" << t.resting_id << ")\n";

    std::cout << "\nBook after:\n";
    print_book(book);

    if (const Order* ba = book.best_ask())
        std::cout << "\nBest ask now: " << ba->quantity << " @ " << ba->price << "\n";
    std::cout << "Cancel #2 -> " << (book.cancel(2) ? "removed" : "not found") << "\n";

    return 0;
}