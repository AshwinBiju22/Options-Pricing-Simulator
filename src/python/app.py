import streamlit as st
from simulation import run_simulation

st.title("Options Market-Making Simulator")
st.caption("A C++ matching engine, a Black-Scholes pricer, and an inventory-skewing market maker.")

# --- controls in the sidebar ---
st.sidebar.header("Parameters")
steps       = st.sidebar.slider("Steps", 20, 500, 120)
half_spread = st.sidebar.slider("Half spread ($)", 0.01, 0.50, 0.10, 0.01)
skew        = st.sidebar.slider("Inventory skew", 0.0, 0.10, 0.02, 0.005)
seed        = st.sidebar.number_input("Random seed", value=42, step=1)

# --- run the simulation with the chosen parameters ---
inventory, pnl = run_simulation(
    steps=steps, seed=int(seed), half_spread=half_spread, skew=skew
)

# --- headline numbers ---
c1, c2, c3 = st.columns(3)
c1.metric("Final PnL", f"${pnl[-1]:.2f}")
c2.metric("Final inventory", inventory[-1])
c3.metric("Trades", steps)

# --- charts ---
st.subheader("PnL over time")
st.line_chart(pnl)

st.subheader("Inventory over time")
st.line_chart(inventory)