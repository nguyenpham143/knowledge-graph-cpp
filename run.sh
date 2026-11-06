#!/bin/bash
set -e

echo "=== Compiling Knowledge Graph (CO2003 - Assignment 3) ==="
g++ -std=c++17 -o main src/main.cpp src/KnowledgeGraph.cpp -I include

echo "=== Running Test Suite ==="
./main
