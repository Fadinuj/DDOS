import matplotlib.pyplot as plt
import numpy as np

# === גרף 1: היסטוגרמת SYN (Syn_pkts_p.png) ===
syn_times = []
with open("syns_results_p.txt", "r") as f:
    for line in f:
        if line.startswith("Packet"):
            try:
                time_ms = float(line.strip().split(": ")[1].replace(" ms", ""))
                syn_times.append(time_ms)
            except:
                continue

plt.figure()
plt.hist(syn_times, bins=50, log=True,color="magenta")
plt.xlabel("Time to send packet (ms)")
plt.ylabel("Number of packets (log scale)")
plt.title("Python Attack - Packet Send Time")
plt.grid(True)
plt.savefig("Syn_pkts_p.png")

# === גרף 2: היסטוגרמת פינגים (Pings_p.png) ===
ping_rtts = []
with open("pings_results_p.txt", "r") as f:
    for line in f:
        if line.startswith("Ping"):
            try:
                rtt = float(line.strip().split(": ")[1].replace(" ms", ""))
                ping_rtts.append(rtt)
            except:
                continue

plt.figure()
plt.hist(ping_rtts, bins=50, log=True,color="magenta")
plt.xlabel("Ping RTT (ms)")
plt.ylabel("Number of pings (log scale)")
plt.title("Python Attack - Ping RTT")
plt.grid(True)
plt.savefig("Pings_p.png")

