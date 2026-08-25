import random
import engine
from engine import Side, OrderType
from market_maker import MarketMaker
from pricer import black_scholes


def to_ticks(price):
    return round(price * 100)


def run_simulation(steps=120, seed=42, half_spread=0.10, skew=0.02, quote_size=10):
    random.seed(seed)
    S, K, T, r, sigma, kind = 100, 100, 1.0, 0.05, 0.20, "call"
    fair = black_scholes(S, K, T, r, sigma, kind)

    book = engine.OrderBook()
    mm = MarketMaker(half_spread=half_spread, skew=skew)

    cash = 0.0
    mm_bid_id = mm_ask_id = None
    inventory_history, pnl_history = [], []

    for _ in range(steps):
        if mm_bid_id is not None: book.cancel(mm_bid_id)
        if mm_ask_id is not None: book.cancel(mm_ask_id)

        bid, ask = mm.quote(S, K, T, r, sigma, kind)
        mm_bid_id = book.submit(Side.Buy,  OrderType.Limit, to_ticks(bid), quote_size).order_id
        mm_ask_id = book.submit(Side.Sell, OrderType.Limit, to_ticks(ask), quote_size).order_id

        side = random.choice([Side.Buy, Side.Sell])
        size = random.randint(1, 5)
        result = book.submit(side, OrderType.Market, 0, size)

        for t in result.trades:
            if t.resting_id == mm_ask_id:
                cash += (t.price / 100) * t.quantity
                mm.inventory -= t.quantity
            elif t.resting_id == mm_bid_id:
                cash -= (t.price / 100) * t.quantity
                mm.inventory += t.quantity

        pnl = cash + mm.inventory * fair
        inventory_history.append(mm.inventory)
        pnl_history.append(pnl)

    return inventory_history, pnl_history