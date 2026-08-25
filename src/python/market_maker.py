from pricer import black_scholes


class MarketMaker:
    """
    Quotes a two-sided market around an option's fair value,
    leaning its quotes against its current inventory to manage risk
    """

    def __init__(self, half_spread, skew):
        self.half_spread = half_spread   # base distance from fair value to each quote
        self.skew = skew                 # how hard to lean quotes against inventory
        self.inventory = 0               # net option position (+ = long, - = short)

    def quote(self, S, K, T, r, sigma, kind):
        """
        Return (bid, ask): the prices to post to buy and to sell this option
        """
        fair = black_scholes(S, K, T, r, sigma, kind)

        # Reservation price: shift the centre of our market against inventory.
        # Long (inventory > 0) -> reservation below fair -> both quotes drop.
        reservation = fair - self.inventory * self.skew

        bid = reservation - self.half_spread
        ask = reservation + self.half_spread
        return bid, ask