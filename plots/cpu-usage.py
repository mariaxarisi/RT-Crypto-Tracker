import matplotlib.pyplot as plt
import os
from datetime import datetime

cur_dir = os.path.abspath(os.path.dirname(__file__))
data_file = os.path.join(cur_dir, "../logs/stats/cpu-usage.csv")
output_file = os.path.join(cur_dir, "../assets/cpu-usage.png")

timestamps = []
cpu_usages = []

with open(data_file, "r") as f:
    for line in f:
        line = line.strip()
        if not line:
            continue
        ts, cpu = line.split(",")
        ts = datetime.fromtimestamp(int(ts) / 1000)
        cpu = float(cpu)

        timestamps.append(ts)
        cpu_usages.append(cpu)

plt.figure(figsize=(12, 6))
plt.plot(timestamps, cpu_usages, marker="o", linestyle="-")
plt.title("CPU Usage Over Time")
plt.xlabel("Time")
plt.ylabel("CPU Usage (%)")
plt.grid(True)
plt.tight_layout()
plt.savefig(output_file)
plt.close()
