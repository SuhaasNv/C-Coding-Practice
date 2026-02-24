# C++ Coding Practice

Hi, I’m Suhaas. This repo is my personal C++ systems practice space.

I’m using it to get better at writing clean, production-style C++ code, not just solving isolated problems. The focus here is design clarity, correctness, thread safety, and maintainability.

## What I’m Practicing
- Modern C++20 style
- Interface-based design
- RAII and memory safety
- Concurrency and lock design
- Data structure tradeoffs
- Writing testable, readable code

## Projects
- `credential-store/`: In-memory credential store with interface/implementation separation and validation tests.
- `lru-cache/`: Thread-safe LRU cache using `std::unordered_map` + `std::list`.
- `sharded-lru-cache/`: Scalable sharded LRU cache with per-shard locking and concurrent benchmark output.

Each folder has its own README with build/run commands and design notes.

## Build Standard
I compile with strict flags to keep quality high:

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic
```

(`-pthread` is used for multithreaded modules.)

## Why This Repo Exists
I wanted a practice repo that I can confidently discuss in interviews: what I built, why I built it that way, what tradeoffs I made, and what I would improve next.

More modules will be added over time as I go deeper into systems-focused C++.
