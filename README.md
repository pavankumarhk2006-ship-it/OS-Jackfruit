# Multi-Container Runtime (OS Mini Project)

## 📌 Overview

This project implements a lightweight container runtime in C. It allows execution of isolated workloads using Linux primitives such as `fork()`, `chroot()`, and `execvp()`. The runtime supports running CPU-bound and memory-bound workloads inside containerized environments and observing system resource utilization.

---

## 🧠 Objectives

* Build a basic container runtime in C
* Execute workloads inside isolated root filesystems
* Analyze CPU and memory behavior using system tools
* Understand process isolation and resource utilization

---

## 🏗️ Architecture

### 1. User-Space Runtime (`engine.c`)

* Manages container lifecycle
* Uses:

  * `fork()` → create process
  * `chroot()` → filesystem isolation
  * `execvp()` → execute workload

### 2. Workloads

* `cpu_hog` → CPU-intensive workload
* `memory_hog` → Memory-intensive workload
* `io_pulse` → I/O workload

---

## ⚙️ Setup Instructions

### Clone Repository

```bash
git clone https://github.com/<your-username>/OS-Jackfruit.git
cd OS-Jackfruit/boilerplate
```

### Install Dependencies

```bash
sudo apt update
sudo apt install -y build-essential linux-headers-$(uname -r)
```

### Build

```bash
make
```

---

## 📂 Root Filesystem Setup

```bash
mkdir rootfs-base
wget https://dl-cdn.alpinelinux.org/alpine/v3.20/releases/x86_64/alpine-minirootfs-3.20.3-x86_64.tar.gz
tar -xzf alpine-minirootfs-3.20.3-x86_64.tar.gz -C rootfs-base

cp -a rootfs-base rootfs-alpha
```

---

## 🚀 Running Containers

Copy workloads:

```bash
cp cpu_hog ../rootfs-alpha/
cp memory_hog ../rootfs-alpha/
```

---

## 🔥 CPU Workload Demo

```bash
sudo ./engine run alpha ../rootfs-alpha /cpu_hog
```

Monitor:

```bash
top
```

### Observation

* CPU usage reaches ~100%
* Process consumes maximum CPU

---

## 🧠 Memory Workload Demo

```bash
sudo ./engine run alpha ../rootfs-alpha /memory_hog
```

Monitor:

```bash
top
```

### Observation

* Memory usage increases gradually
* Process consumes high %MEM

---

## 📸 Screenshots to Include

* CPU usage (~100%) in `top`
* Memory usage increase in `top`
* Container start output (`Starting container: alpha`)

---

## 🧪 Analysis

### CPU Workload

* Infinite loop
* No I/O → maximum CPU utilization

### Memory Workload

* Allocates memory in chunks
* Demonstrates memory pressure

---

## ⚠️ Challenges Faced

* Fixing kernel module compilation issues
* Debugging low CPU utilization
* Ensuring correct binary execution in container
* Handling runtime errors

---

## ✅ Conclusion

This project demonstrates a functional container runtime capable of executing workloads in isolated environments and analyzing system resource usage effectively.

---

## 🎤 Viva Questions

**What is a container?**
A lightweight isolated environment sharing the host kernel.

**How is isolation achieved?**
Using `chroot()` and process separation.

**Why does cpu_hog use high CPU?**
Because it runs a tight infinite loop.

**How do you monitor system resources?**
Using `top`.

---

## 👨‍💻 Author

Name: **Pavan**
Course: Operating Systems
Project: Multi-Container Runtime

