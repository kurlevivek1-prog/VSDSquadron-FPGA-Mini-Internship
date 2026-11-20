# 🚀 VSDSquadron FPGA Mini Internship

[![RISC-V](https://img.shields.io/badge/Architecture-RISC--V-orange?style=flat-square)](https://riscv.org/)
[![HDL](https://img.shields.io/badge/HDL-Verilog-blue?style=flat-square)](#)
[![Simulation](https://img.shields.io/badge/Simulation-iverilog%20%7C%20GTKWave-green?style=flat-square)](#)
[![Toolchain](https://img.shields.io/badge/Toolchain-RISC--V%20GCC-red?style=flat-square)](#)
[![Status](https://img.shields.io/badge/Status-Completed-success?style=flat-square)](#)

This repository contains all my design files, RTL implementations, testbenches, and documentation completed during the **VSDSquadron FPGA Mini Internship**.

---

## 📂 Repository Structure

| Task | Title | Description | Link |
|---|---|---|---|
| **Task 1** | **Toolchain & Setup** | Environment setup using GitHub Codespaces, `iverilog`, GTKWave, and RISC-V GCC toolchain | [📁 View Task 1](./Task-1/) |
| **Task 2** | **Simple GPIO IP** | Basic memory-mapped GPIO IP design, address decoding, and C driver verification | [📁 View Task 2](./Task-2/) |
| **Task 3** | **Multi-Register GPIO IP** | Production-style multi-register software-controlled GPIO IP with directional & data registers | [📁 View Task 3](./Task-3/) |
| **Task 4** | **Timer IP** | 32-bit countdown timer with one-shot/periodic modes, prescaler, and timeout flag; validated in simulation and hardware | [📁 View Task 4](./Task-4/) |
| **Task 5** | **Commercial-Grade IP Package** | User guide, register map, integration guide, and C firmware examples for Timer IP | [📁 View Task 5](./Task-5/) · [📦 IP Package](./ip/timer_ip/) |

---

## 📦 Packaged IP Cores

### ⏱️ [Timer IP (`ip/timer_ip`)](./ip/timer_ip/)
A fully packaged, commercial-grade 32-bit hardware timer peripheral designed for RISC-V memory-mapped SoCs:
- **RTL Source:** [`rtl/timer_ip.v`](./ip/timer_ip/rtl/timer_ip.v)
- **Features:** 32-bit counter, 16-bit prescaler, configurable one-shot and periodic modes, timeout interrupt/flag.
- **Documentation:** [User Guide](./ip/timer_ip/docs/IP_User_Guide.md), [Register Map](./ip/timer_ip/docs/Register_Map.md), [Integration Guide](./ip/timer_ip/docs/Integration_Guide.md), [Example Usage](./ip/timer_ip/docs/Example_Usage.md).
- **Software Drivers:** C firmware examples ([`software/`](./ip/timer_ip/software/)) including periodic LED blink and timer validation.

---

## 🛠️ Tools & Technologies Used
- **HDL:** Verilog
- **Simulation & Analysis:** `iverilog`, GTKWave
- **Toolchain:** RISC-V GCC (`riscv64-unknown-elf-gcc`)
- **Platform:** GitHub Codespaces & VSDSquadron FPGA SoC

---

<p align="center">
  <b>Vivek Sudam Kurle</b> • VIT Mumbai<br>
  <a href="https://linkedin.com/in/vivek-kurle-b045462b4"><img src="https://img.shields.io/badge/LinkedIn-0077B5?style=flat-square&logo=linkedin&logoColor=white" alt="LinkedIn" /></a>
  <a href="mailto:vivekkurle7@gmail.com"><img src="https://img.shields.io/badge/Email-D14836?style=flat-square&logo=gmail&logoColor=white" alt="Email" /></a>
  <a href="https://github.com/kurlevivek1-prog"><img src="https://img.shields.io/badge/GitHub-181717?style=flat-square&logo=github&logoColor=white" alt="GitHub" /></a>
</p>
