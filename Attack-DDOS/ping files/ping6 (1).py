import subprocess
import time
import statistics
import re

TARGET_IP = "192.168.1.55"
INTERVAL = 5
results = []
i = 0

try:
    while True:
        result = subprocess.run(["ping", "-c", "1", TARGET_IP], capture_output=True, text=True)
        match = re.search(r'time=([\d.]+) ms', result.stdout)
        if match:
            rtt = float(match.group(1))
            results.append((i + 1, rtt))
            print(f"Ping {i+1}: {rtt:.2f} ms")
            i += 1
        time.sleep(INTERVAL)

except KeyboardInterrupt:
    print("\nStopping and saving results...")
    with open("pings_results_p.txt", "w") as f:
        for ping_num, rtt in results:
            f.write(f"Ping {ping_num}: {rtt:.2f} ms\n")
        avg = sum(rtt for _, rtt in results) / len(results)
        std = statistics.stdev(rtt for _, rtt in results) if len(results) > 1 else 0
        f.write(f"\nAverage RTT: {avg:.2f} ms\n")
        f.write(f"Standard Deviation: {std:.2f} ms\n")
    print(f"Saved {len(results)} ping results to 'pings_results_p.txt'")
