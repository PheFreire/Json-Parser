# Shadow-Header Dynamic Data Structures Framework

A dynamic array and hashmap framework written in pure C using hidden shadow headers (fat pointers), designed as the core foundation for a low-level, high-performance JSON parser.

## 🚀 Architecture & Overview

This framework implements generic, memory-efficient data structures in C without relying on complex wrapper structures or pointer chasing. Instead, it utilizes the **Shadow Header** (or *Fat Pointer*) memory arrangement paired with the C preprocessor.

When allocating a collection, metadata (such as capacity and actual elements allocated) is embedded in a hidden `Header` block placed immediately **before** the data address returned to the application layer.

### Memory Layout
```text
┌──────────────────────────────────┬──────────────────────────────────────────┐
│ Hidden Header Struct             │ Public User Data Pointer                 │
│ (reserved_size, allocated)       │ (int*, char**, JsonField*, etc.)         │
└──────────────────────────────────┴──────────────────────────────────────────┘
▲                                  ▲
│                                  └─ Raw array pointer exposed to your program
└─ Hidden metadata offset managed internally via pointer arithmetic
```

### Key Technical Pillars
- **Zero Syntax Overhead:** Access elements natively using standard subscript notation (`array[i]`) instead of abstract getters or functional syntax sugar.
- **Dynamic Address Re-binding:** Fully accounts for the volatile nature of `realloc`. Since memory block expansion might trigger physical relocations in the heap layout, metadata references are retrieved dynamically to avoid *dangling pointers*.
- **Self-Documenting Tests:** Leveraging the C preprocessor constraint evaluation, assertions utilize inline descriptive tracking (`assert(condition && "Error Message")`) for explicit crash reports.

## 📁 Project Structure

The codebase isolates application routines, library targets, and test components under an automated recursive `Makefile` structure:

```text
meu_projeto/
├── Makefile                # Bilingual dual-target build system (app / test_suite)
├── src/
│   ├── main.c              # Main application entry point (future JSON parser)
│   └── array/
│       ├── array.h         # Compile-time macros and inline shadow tracking
│       └── array.c         # Type-agnostic allocation management (_array_expand)
└── tests/
    ├── test_runner.c       # Master test engine triggering discrete test suites
    └── test_array.c        # Behavior-driven testing suite for dynamic array mechanics
```

## ✨ Implemented Milestones

- **Dynamic Vector Layout:** Fully robust array logic covering typed initializations, automatic exponential resizing (`capacity + (capacity >> 3) + padding`), arbitrary memory item insertions, element shifts via memory blocks (`memmove`), and safe freeing.
- **Unified Build Pipeline:** A modularized structure enforcing safe decoupling between the parsing executable (`make run`) and the isolated validation test suite (`make test`).

## 🛠️ Upcoming Roadmap

- [ ] **Generic Array Macro Conversion:** Migrate static typed elements into an overarching compile-time template interface capable of treating arbitrary types smoothly.
- [ ] **Hashtable Base Storage:** Structure baseline key-value storage layouts and chain collision array nodes.
- [ ] **String Hashing Engine:** Introduce robust string hash routines to map keys down into table slots efficiently.
- [ ] **Hashmap Collision Logic:** Build dynamic open addressing or separate chaining resolution mechanics with safe load-factor rehashing thresholds.
- [ ] **JSON Parse Machine:** Assemble the lexical scanning pipeline on top of the generic data structures framework.

## 💻 Getting Started

### Prerequisites
A standard modern C compiler supporting at least C99 (e.g., `gcc` or `clang`) along with standard terminal utility tools (`make`).

### Production Compilation and Execution
To compile and execute the main application:
```bash
make run
```

### Testing Evaluation
To compile and execute the behavior-driven test suite with explicit logging assertions:
```bash
make test
```

## 📄 License
This project is open-source and available under the [MIT License](LICENSE).

