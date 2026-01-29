# ⚔️ DDoS SYN Flood: Performance Analysis (C vs. Python)

This project performs a comparative study of network packet injection speeds between a low-level **C** implementation and a high-level **Python** script. The goal is to simulate a **SYN Flood** attack and measure the throughput and latency of generating malicious traffic.

> ⚠️ **DISCLAIMER:** This software is for **EDUCATIONAL PURPOSES ONLY**. It is intended to test network stress and compare programming language performance in a controlled lab environment. Do not use this tool on targets without prior mutual consent.

## 🧪 Project Overview

The project consists of two implementations of the same attack vector (TCP SYN Flood) to analyze the overhead introduced by high-level libraries compared to raw system calls.

### 1. C Implementation (`attack1.c`)
* **Technique:** Uses **Raw Sockets** (`SOCK_RAW`) to bypass the operating system's network stack.
* **Packet Construction:** Manually defines `iphdr` (IP Header) and `tcphdr` (TCP Header) structures.
* **Checksum:** Implements a custom checksum calculation function.
* **Performance:** Expected to have high throughput due to direct kernel interaction and minimal overhead.
* **Measurement:** Uses `clock_gettime` for nanosecond-precision tracking of send times.

### 2. Python Implementation (`attack5.py`)
* **Technique:** Uses the **Scapy** library for packet manipulation.
* **Packet Construction:** High-level object-oriented packet crafting (`IP()/TCP()`).
* **Performance:** Expected to be slower due to Python interpreter overhead and Scapy's abstraction layers.
* **Measurement:** Uses `time.time()` and the `statistics` module to calculate mean and standard deviation.

## 📊 Metrics & Analysis
Both scripts perform the following:
1.  **Loop:** Send a burst of **1,000,000 packets** (100 loops of 10,000 packets).
2.  **Measure:** Record the time taken to send each packet.
3.  **Output:** Generate report files (`syns_results_c1.txt`, `syns_results_p.txt`) containing:
    * Time per packet (ms).
    * Total execution time.
    * Average send time.
    * Standard Deviation (Jitter).

## 🛠️ Tech Stack
* **Language:** C (Standard libraries, `<sys/socket.h>`, `<netinet/ip.h>`)
* **Language:** Python 3 (Scapy, Statistics)
* **Protocol:** TCP/IP (SYN Flag set)

## 🚀 How to Run

**Note:** Both scripts require **Root/Administrator** privileges to craft raw packets.

### Running the C Attack
1. Compile the code:
   ```bash
   gcc attack1.c -o attack_c -lm
