#include "KnowledgeGraph.h"
#include <iostream>
#include <cassert>
#include <vector>
#include <string>

using namespace std;

void testDGraphModel() {
    cout << "========================================" << endl;
    cout << "  1. Testing DGraphModel<string>" << endl;
    cout << "========================================" << endl;

    DGraphModel<string> graph;

    // Add vertices
    graph.add("NodeA");
    graph.add("NodeB");
    graph.add("NodeC");
    graph.add("NodeD");
    graph.add("NodeE");

    assert(graph.size() == 5);
    assert(graph.contains("NodeA"));
    assert(graph.contains("NodeC"));
    assert(!graph.contains("NodeZ"));
    cout << "[PASS] Node additions and containment check (size = " << graph.size() << ")" << endl;

    // Connect directed edges
    graph.connect("NodeA", "NodeB", 2.0f);
    graph.connect("NodeA", "NodeC", 4.0f);
    graph.connect("NodeB", "NodeD", 1.5f);
    graph.connect("NodeC", "NodeD", 2.5f);
    graph.connect("NodeD", "NodeE", 3.0f);

    assert(graph.connected("NodeA", "NodeB"));
    assert(graph.connected("NodeA", "NodeC"));
    assert(!graph.connected("NodeB", "NodeA")); // directed
    assert(graph.inDegree("NodeD") == 2);
    assert(graph.outDegree("NodeA") == 2);
    cout << "[PASS] Directed edge connectivity & degree invariants" << endl;

    // BFS & DFS
    string bfsResult = graph.BFS("NodeA");
    string dfsResult = graph.DFS("NodeA");
    cout << "       BFS from NodeA: " << bfsResult << endl;
    cout << "       DFS from NodeA: " << dfsResult << endl;
    cout << "[PASS] Graph traversals executed successfully" << endl;
    cout << endl;
}

void testKnowledgeGraph() {
    cout << "========================================" << endl;
    cout << "  2. Testing KnowledgeGraph Engine" << endl;
    cout << "========================================" << endl;

    KnowledgeGraph kg;

    // Populate academic knowledge ontology
    kg.addEntity("HCMUT");
    kg.addEntity("CSE");
    kg.addEntity("DSA");
    kg.addEntity("AI");
    kg.addEntity("GraphTheory");
    kg.addEntity("MachineLearning");

    kg.addRelation("HCMUT", "CSE", 1.0f);
    kg.addRelation("CSE", "DSA", 2.0f);
    kg.addRelation("CSE", "AI", 3.0f);
    kg.addRelation("DSA", "GraphTheory", 1.5f);
    kg.addRelation("AI", "MachineLearning", 2.0f);
    kg.addRelation("MachineLearning", "GraphTheory", 2.5f);

    cout << "[PASS] Entities and relations inserted" << endl;
    cout << "       All entities count: " << kg.getAllEntities().size() << endl;

    // 1. Reachability
    assert(kg.isReachable("HCMUT", "GraphTheory") == true);
    assert(kg.isReachable("GraphTheory", "HCMUT") == false);
    cout << "[PASS] Reachability check (HCMUT -> GraphTheory: true, GraphTheory -> HCMUT: false)" << endl;

    // 2. Related Entities (Multi-hop BFS)
    vector<string> depth1 = kg.getRelatedEntities("CSE", 1);
    cout << "       Related to CSE (depth=1): ";
    for (const auto& e : depth1) cout << e << " ";
    cout << endl;
    assert(depth1.size() == 2); // DSA, AI

    vector<string> depth2 = kg.getRelatedEntities("CSE", 2);
    cout << "       Related to CSE (depth=2): ";
    for (const auto& e : depth2) cout << e << " ";
    cout << endl;
    assert(depth2.size() >= 4);
    cout << "[PASS] Multi-hop entity neighborhood expansion" << endl;

    // 3. Shortest Path (Dijkstra)
    // HCMUT -> CSE -> DSA -> GraphTheory: 1.0 + 2.0 + 1.5 = 4.5
    // HCMUT -> CSE -> AI -> MachineLearning -> GraphTheory: 1.0 + 3.0 + 2.0 + 2.5 = 8.5
    float dist = kg.getShortestDist("HCMUT", "GraphTheory");
    cout << "       Shortest distance (HCMUT -> GraphTheory): " << dist << endl;
    assert(abs(dist - 4.5f) < 1e-4);
    cout << "[PASS] Dijkstra shortest path calculation: 4.5" << endl;

    // 4. Ancestors & Common Ancestor Finding
    vector<string> parents = kg.getParents("GraphTheory");
    cout << "       Direct parents of GraphTheory: ";
    for (const auto& p : parents) cout << p << " ";
    cout << endl;
    assert(parents.size() == 2); // DSA, MachineLearning

    string lca = kg.findCommonAncestors("DSA", "AI");
    cout << "       Optimal Common Ancestor of (DSA, AI): " << lca << endl;
    assert(lca == "CSE");
    cout << "[PASS] Optimal Common Ancestor identified: CSE" << endl;
    cout << endl;
}

int main() {
    cout << "==========================================================" << endl;
    cout << "  KnowledgeGraph & DGraphModel Test Suite (CO2003 - BTL3)" << endl;
    cout << "==========================================================" << endl;

    testDGraphModel();
    testKnowledgeGraph();

    cout << "==========================================================" << endl;
    cout << "  ALL VERIFICATION TESTS COMPLETED SUCCESSFULLY!" << endl;
    cout << "==========================================================" << endl;

    return 0;
}
