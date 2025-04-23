#  Operating System for Multi-Agent Robotic System

This project presents an **Operating System (OS)** developed for a **multi-agent robotic system** using **processes, threads**, and **IPC (Inter-Process Communication)** on a **multi-core processor**. The system is designed to control **50 autonomous robots** navigating a `100x100` environment and collaboratively estimating the **width of an exit**.

Each robot acts as an independent process, communicates with others, and synchronizes access to shared resources, effectively mimicking a distributed robotic environment.



## ✨ Features

-  **Multi-Agent Control**: Handles 50 robot agents operating simultaneously.
-  **Multithreading & Multiprocessing**: Efficient use of multi-core processors for concurrent operations.
-  **Inter-Process Communication (IPC)**: Implemented using pipes to allow robots to share their exit width estimations.
-  **Process Synchronization**: Utilized semaphores and mutexes to prevent race conditions.
-  **Memory Management**: Secure allocation and deallocation of memory for all robot processes.
-  **Process Scheduling**: Integrated a custom scheduling mechanism for optimal CPU usage.
-  **High Accuracy**: Achieved a 95% accuracy rate in real-time exit width estimation.



## 🧪 Key Learnings & Results

- Designed a communication system for autonomous agents using **pipes (IPC)**.
- Implemented **process synchronization** via **semaphores** and **mutexes** to handle shared resource access.
- Built thread-safe memory management to prevent leaks and ensure efficient use.
- Managed **concurrent execution** using threads and multiprocessing principles.
- Created a **scheduling strategy** for resource optimization across cores.



## 🛠️ Technologies Used

- C / C++
- POSIX Threads (pthreads)
- Semaphores & Mutexes
- Pipes (IPC)
- Linux OS (for process and memory management)

---

## 🚀 Getting Started

1. Clone the repository:
   ```bash
   git clone https://github.com/mujtaba510/Operating-System-for-Multi-Agent-Robotic-System.git

