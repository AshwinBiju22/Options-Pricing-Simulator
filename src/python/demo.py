import engine
from engine import Side, OrderType

book = engine.OrderBook()

# rest two sells (engine assigns ids 1, 2)
book.submit(Side.Sell, OrderType.Limit, 1000, 50)
print(f"Submitted Sell Order 1: 50 @ 1000")

book.submit(Side.Sell, OrderType.Limit, 1100, 50)
print(f"Submitted Sell Order 2: 50 @ 1100")

book.submit(Side.Sell, OrderType.Limit, 1200, 100)
print(f"Submitted Sell Order 3: 100 @ 1200")


# a buy that crosses the book
result = book.submit(Side.Buy, OrderType.Limit, 1150, 200)
print(f"Submitted Buy Order 4: 200 @ 1150")

print(f"Order #{result.order_id} produced {len(result.trades)} trade(s):")
for t in result.trades:
    print(f"  {t.quantity} @ {t.price}  (#{t.aggressor_id} hit #{t.resting_id})")

print("\nBook now:")
print("  asks:", [(o.id, o.quantity, o.price) for o in book.asks])
print("  bids:", [(o.id, o.quantity, o.price) for o in book.bids])

ba = book.best_ask()
if ba is not None:
    print(f"\nbest ask: {ba.quantity} @ {ba.price}")

bb = book.best_bid()
if bb is not None:
    print(f"\nbest bid: {bb.quantity} @ {bb.price}")

print("cancel #2:", book.cancel(2))
