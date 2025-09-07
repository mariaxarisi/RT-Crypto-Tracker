import os
from datetime import datetime
import matplotlib.dates as mdates
import matplotlib.patches as mpatches
import matplotlib.pyplot as plt
import numpy as np
import seaborn as sns

cur_dir = os.path.abspath(os.path.dirname(__file__))

symbols = [
    "BTC-USDT", "ADA-USDT", "ETH-USDT", "DOGE-USDT",
    "XRP-USDT", "SOL-USDT", "LTC-USDT", "BNB-USDT"
]

symbol_colors = {
    0: 'red',     # BTC-USDT
    1: 'blue',    # ADA-USDT
    2: 'green',   # ETH-USDT
    3: 'orange',  # DOGE-USDT
    4: 'purple',  # XRP-USDT
    5: 'brown',   # SOL-USDT
    6: 'pink',    # LTC-USDT
    7: 'cyan'     # BNB-USDT
}

start_time = 1756404660000
end_time = 1756616820000
timestamps = range(int(start_time / 1000), int(end_time / 1000) + 60, 60)
timestamps_dt = [datetime.fromtimestamp(t) for t in timestamps]

def load_correlations(symbols, symbol_colors, cur_dir):
    correlations = [[] for _ in range(len(symbols))]
    matched_colors = [[] for _ in range(len(symbols))]
    occurrences = [[0 for _ in range(len(symbols))] for _ in range(len(symbols))]

    for i, symbol in enumerate(symbols):
        data_file = os.path.join(cur_dir, f'../logs/pearson/{symbol}.csv')
        with open(data_file, 'r') as f:
            for line in f:
                line = line.strip()
                if not line:
                    continue
                _, matched_symbol, corr_value = line.split(', ')
                correlations[i].append(float(corr_value))
                matched_colors[i].append(symbol_colors[symbols.index(matched_symbol)])
                occurrences[i][symbols.index(matched_symbol)] += 1
    return correlations, matched_colors, occurrences

def plot_symbol_correlations(symbol, timestamps_dt, corr_values, matched_colors, occurrences, symbol_colors, cur_dir):
    plt.figure(figsize=(20,6))
    plt.scatter(timestamps_dt, corr_values, c=matched_colors, alpha=0.8)

    legend_handles = [
        mpatches.Patch(color=color, label=f'{symbols[idx]} ({(occurrences[idx]/len(timestamps_dt)*100):.2f}%)') 
        for idx, color in symbol_colors.items()
    ]
    plt.legend(handles=legend_handles, title="Max Correlated With", bbox_to_anchor=(1.05,1), loc='upper left')
    plt.title(f"Correlation for {symbol}")
    plt.xlabel("Time")
    plt.ylabel("Pearson Correlation")
    plt.gca().xaxis.set_major_formatter(mdates.DateFormatter("%m-%d %H:%M"))
    plt.xticks(rotation=45)
    plt.tight_layout()
    plt.savefig(os.path.join(cur_dir, f'../assets/correlations/{symbol}.png'))
    plt.close()

# --- Main execution ---
correlations, matched_colors, occurrences = load_correlations(symbols, symbol_colors, cur_dir)

# Optional: Check data length
for i, corr_list in enumerate(correlations):
    if len(corr_list) != len(timestamps):
        print(f"Data length mismatch for {symbols[i]}: expected {len(timestamps)}, got {len(corr_list)}")

# Plot all scatter plots
for i, symbol in enumerate(symbols):
    plot_symbol_correlations(symbol, timestamps_dt, correlations[i], matched_colors[i], occurrences[i], symbol_colors, cur_dir)

