import matplotlib.pyplot as plt
import os
from datetime import datetime

cur_dir = os.path.abspath(os.path.dirname(__file__))
data_file = os.path.join(cur_dir, "../logs/stats/pearson-delay.csv")
output_file = os.path.join(cur_dir, "../assets/pearson-delay.png")

timestamps = []
avg_delays = []

with open(data_file, "r") as f:
    for line in f:
        line = line.strip()
        if not line:
            continue
        ts, delay = line.split(",")
        ts = datetime.fromtimestamp(int(ts) / 1000)
        delay = float(delay)

        timestamps.append(ts)
        avg_delays.append(delay / 1000)

plt.figure(figsize=(12, 6))
plt.plot(timestamps, avg_delays, marker="o", linestyle="-")
plt.title("Pearson Delay Over Time")
plt.xlabel("Time")
plt.ylabel("Pearson Delay (sec)")
plt.grid(True)
plt.tight_layout()
plt.savefig(output_file)
plt.close()
