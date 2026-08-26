from pricer import black_scholes


class MarketMaker:
    """
    Quotes a two-sided market around an option's fair value,
    leaning its quotes against its current inventory to manage risk
    """

    def __init__(self, half_spread, skew):
        # Wider spread means more profit per trade but less fills, Earn more per trade vs Trade more often
        self.half_spread = half_spread # profit dial
        # how hard to lean quotes against inventory
        self.skew = skew # risk dial
        # Net position, + means long, - means short
        self.inventory = 0               

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