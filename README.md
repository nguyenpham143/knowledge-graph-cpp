# Knowledge Graph & Directed Graph Model (CO2003 - Assignment 3)

A C++ implementation of a generic **Directed Graph Model** (`DGraphModel<T>`) using adjacency lists, serving as the foundational engine for a semantic **Knowledge Graph** (`KnowledgeGraph`). The system supports graph traversals, multi-hop relation exploration, ancestral hierarchy analysis, and shortest-path reasoning. Developed as Assignment 3 for the **Data Structures and Algorithms (CO2003)** course at Ho Chi Minh City University of Technology (HCMUT - VNU-HCM), Semester 251.

## Overview

A Knowledge Graph models real-world entities and directional semantic relationships as a directed, weighted graph. This project builds the engine from the ground up without using third-party graph libraries:

### 1. Generic Directed Graph Engine (`DGraphModel<T>`)
- **Adjacency List Architecture**: Encapsulated using `VertexNode<T>` and `Edge<T>` with dual-pointer cross-referencing for $O(1)$ in-degree and out-degree tracking.
- **Dynamic Topology**: Amortized $O(1)$ node insertion, edge connection/disconnection with duplicate handling, and automatic cascade cleanup upon node deletion.
- **Traversals**:
  - **Breadth-First Search (BFS)**: Explores reachable vertices level by level.
  - **Depth-First Search (DFS)**: Explores deep branches sequentially.
- **Extensible Callbacks**: Supports custom vertex equality (`vertexEQ`) and serialization (`vertex2str`) function pointers.

### 2. Semantic Knowledge Graph Engine (`KnowledgeGraph`)
- **Entity & Relation Management**: Manages named entities and directed weighted relations (e.g., conceptual dependencies, semantic links).
- **Multi-Hop Entity Expansion (`getRelatedEntities`)**: Performs a depth-bounded BFS up to a specified exploration radius (default `depth = 2`) to discover neighborhood entities without cycles.
- **Ancestral & Dependency Reasoning**:
  - `getParents(u)`: Identifies all immediate prerequisite/incoming nodes in $O(V)$ time.
  - `dfsFindAncestors(start)`: Performs a reverse-DFS traversal to trace the full ancestral hierarchy and root dependencies.
- **Dijkstra's Shortest Path (`getShortestDist`)**: Calculates the minimum path cost between two entities over weighted directed edges.
- **Optimal Common Ancestor (`findCommonAncestors`)**: Finds the lowest/optimal common ancestor minimizing aggregate shortest distance:
  $$\min_{A \in \text{Ancestors}} \big(\text{dist}(A, e_1) + \text{dist}(A, e_2)\big)$$

## Project Structure

```text
.
├── include/
│   ├── main.h                                  # Exception classes & standard includes
│   ├── utils.h                                 # Coordinate & geometry helper definitions
│   └── KnowledgeGraph.h                        # Declarations for Edge, VertexNode, DGraphModel, KnowledgeGraph
├── src/
│   ├── main.cpp                                # Demonstration and verification runner
│   └── KnowledgeGraph.cpp                      # Full graph algorithms & template implementations
├── specs/
│   ├── _251_CO2003___DSA__Assignment_3_EN.pdf  # Assignment specification (English)
│   └── _251_CO2003___DSA__Assignment_3_VI.pdf  # Assignment specification (Vietnamese)
├── .gitignore                                  # Git ignore rules
├── LICENSE                                     # MIT License
├── README.md                                   # Project documentation
└── run.sh                                      # Build and execution script
```

## Getting Started

### Prerequisites

- A C++ compiler supporting **C++17** (e.g., `g++` or `clang++`).

### Build & Run

Compile and execute using `g++`:

```bash
g++ -std=c++17 -o main src/main.cpp src/KnowledgeGraph.cpp -I include
./main
```

Or execute the provided shell script:

```bash
bash run.sh
```

## License

This project is licensed under the [MIT License](LICENSE).
