import math

# Can be replaced by scipy
def norm_cdf(x):
    """
    Standard normal cumulative distribution function, N(x)
    """
    return 0.5 * (1.0 + math.erf(x / math.sqrt(2.0)))


def black_scholes(S, K, T, r, sigma, kind):
    """
    Black-Scholes price of a European option

    S     : spot price of the underlying
    K     : strike price
    T     : time to expiry, in years
    r     : risk-free interest rate (0.05 = 5%)
    sigma : volatility (0.20 = 20%)
    kind  : "call" or "put"
    """
    d1 = (math.log(S / K) + (r + 0.5 * sigma ** 2) * T) / (sigma * math.sqrt(T))
    d2 = d1 - sigma * math.sqrt(T)

    if kind == "call":
        return S * norm_cdf(d1) - K * math.exp(-r * T) * norm_cdf(d2)
    elif kind == "put":
        return K * math.exp(-r * T) * norm_cdf(-d2) - S * norm_cdf(-d1)
    else:
        raise ValueError("kind must be 'call' or 'put'")


if __name__ == "__main__":
    S, K, T, r, sigma = 100, 100, 1.0, 0.05, 0.20

    call = black_scholes(S, K, T, r, sigma, "call")
    put = black_scholes(S, K, T, r, sigma, "put")
    print(f"Call price: {call:.4f}")
    print(f"Put  price: {put:.4f}")

    lhs = call - put
    rhs = S - K * math.exp(-r * T)
    print(f"\nPut-call parity check:")
    print(f"  C - P        = {lhs:.6f}")
    print(f"  S - K e^(-rT) = {rhs:.6f}")
    print(f"  match: {math.isclose(lhs, rhs)}")
