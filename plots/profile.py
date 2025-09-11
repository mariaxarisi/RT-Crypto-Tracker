import os
from datetime import datetime, timedelta
import matplotlib.dates as mdates
import matplotlib.patches as mpatches
import matplotlib.pyplot as plt

cur_dir = os.path.abspath(os.path.dirname(__file__))

# -------------------------
# Helper functions
# -------------------------

def filter_one_day(timestamps_dt, values, colors, start_time, day=0):
    day_start = datetime.fromtimestamp(start_time / 1000) + timedelta(days=day)
    day_end = day_start + timedelta(days=1)
    mask = [(t >= day_start and t < day_end) for t in timestamps_dt]
    filtered_ts = [t for t, keep in zip(timestamps_dt, mask) if keep]
    filtered_vals = [v for v, keep in zip(values, mask) if keep]
    filtered_colors = [c for c, keep in zip(colors, mask) if keep]
    return filtered_ts, filtered_vals, filtered_colors

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
    fig, ax1 = plt.subplots(figsize=(20, 6))
    for i in range(len(timestamps_dt)-1):
        ax1.plot(timestamps_dt[i:i+2], [1, 1], color=matched_colors[i], linewidth=15)
    ax1.set_yticks([])
    ax1.set_xlabel("Time")
    ax1.set_title(f"Correlation Timeline for {symbol}")
    ax1.xaxis.set_major_formatter(mdates.DateFormatter("%m-%d %H:%M"))
    plt.xticks(rotation=45)
    ax2 = ax1.twinx()
    ax2.plot(timestamps_dt, corr_values, color="black", linewidth=1.5, label="Correlation Strength")
    ax2.set_ylabel("Pearson Correlation")
    ax2.set_ylim(0, 1)
    legend_handles = [mpatches.Patch(color=color, label=f'{symbols[idx]} ({occurrences[idx]:.2f}%)')
                      for idx, color in symbol_colors.items()]
    ax1.legend(handles=legend_handles, title="Max Correlated With", bbox_to_anchor=(1.05, 1), loc='upper left')
    ax2.legend(loc="upper right")
    plt.tight_layout()
    plt.savefig(os.path.join(cur_dir, f'../assets/correlations/{symbol}.png'))
    plt.close()

def plot_time_series(data_file, output_file, ylabel, scale=1):
    timestamps, values = [], []
    with open(data_file, "r") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            ts, val = line.split(",")
            ts = datetime.fromtimestamp(int(ts)/1000)
            val = float(val)/scale
            timestamps.append(ts)
            values.append(val)
    plt.figure(figsize=(12,6))
    plt.plot(timestamps, values, marker="o", linestyle="-")
    plt.title(output_file.split("/")[-1].replace("-", " ").replace(".png", "").title())
    plt.xlabel("Time")
    plt.ylabel(ylabel)
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(output_file)
    plt.close()

# -------------------------
# Main execution
# -------------------------

# Plot correlations
symbols = ["BTC-USDT", "ADA-USDT", "ETH-USDT", "DOGE-USDT", "XRP-USDT", "SOL-USDT", "LTC-USDT", "BNB-USDT"]
symbol_colors = {0:'red',1:'blue',2:'green',3:'orange',4:'purple',5:'brown',6:'pink',7:'cyan'}
start_time = 1756404660000
end_time = 1756616820000
timestamps = range(int(start_time/1000), int(end_time/1000)+60, 60)
timestamps_dt = [datetime.fromtimestamp(t) for t in timestamps]
correlations, matched_colors, occurrences = load_correlations(symbols, symbol_colors, cur_dir)
for i in range(len(symbols)):
    for j in range(len(occurrences[i])):
        occurrences[i][j] = occurrences[i][j]/len(timestamps)*100
for i, symbol in enumerate(symbols):
    ts_day, corr_day, colors_day = filter_one_day(timestamps_dt, correlations[i], matched_colors[i], start_time, day=0.5)
    plot_symbol_correlations(symbol, ts_day, corr_day, colors_day, occurrences[i], symbol_colors, cur_dir)

# Plot average delay
plot_time_series(os.path.join(cur_dir,"../logs/stats/avg-delay.csv"),
                 os.path.join(cur_dir,"../assets/mv-average-thread-delay.png"),
                 ylabel="Delay (sec)",
                 scale=1000)

# Plot CPU usage
plot_time_series(os.path.join(cur_dir,"../logs/stats/cpu-usage.csv"),
                 os.path.join(cur_dir,"../assets/CPU-usage.png"),
                 ylabel="Usage (%)",
                 scale=1)

# Plot Pearson delay
plot_time_series(os.path.join(cur_dir,"../logs/stats/pearson-delay.csv"),
                 os.path.join(cur_dir,"../assets/pearson-thread-delay.png"),
                 ylabel="Delay (sec)",
                 scale=1000)
