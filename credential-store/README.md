# In-Memory Credential Store (C++20)

A production-style C++ implementation of an interface-based credential storage system.

## Features
- Add, update, and remove credentials
- Prevent duplicate site entries
- Retrieve credentials by site
- List all stored sites
- Input validation and edge-case handling

## Design Principles
- Interface-based architecture (`ICredentialStore`)
- Encapsulation and abstraction
- RAII-compliant resource management
- Const correctness
- Separation of interface and implementation
- Clean error handling
- STL-based data storage (`std::unordered_map`)

## Build Instructions

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic main.cpp InMemoryCredentialStore.cpp -o credential_store_app
./credential_store_app
```

## Concepts Practiced
- Object-oriented design
- Interface segregation
- Memory safety
- Defensive programming
- Modular architecture
