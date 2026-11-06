#include "KnowledgeGraph.h"

// =============================================================================
// Class Edge Implementation
// =============================================================================

template <class T>
Edge<T>::Edge(VertexNode<T>* from, VertexNode<T>* to, float weight) {
    this->from = from;
    this->to = to;
    this->weight = weight;
}

template <class T>
string Edge<T>::toString() {
    stringstream ss;
    ss << "(";

    if (from) {
        if (from->vertex2str) ss << from->vertex2str(from->vertex);
        else ss << from->vertex;
    } else {
        ss << "null";
    }

    ss << ", ";

    if (to) {
        if (to->vertex2str) ss << to->vertex2str(to->vertex);
        else ss << to->vertex;
    } else {
        ss << "null";
    }

    ss << ", " << to_string(weight) << ")";
    return ss.str();
}

template <class T>
bool Edge<T>::equals(Edge<T>* edge) {
    if (!edge) return false;
    return (this->from == edge->from && this->to == edge->to);
}

template <class T>
bool Edge<T>::edgeEQ(Edge<T>*& edge1, Edge<T>*& edge2) {
    if (!edge1 || !edge2) return false;
    return edge1->equals(edge2);
}


// =============================================================================
// Class VertexNode Implementation
// =============================================================================

template <class T>
VertexNode<T>::VertexNode(T vertex, bool (*vertexEQ)(T&, T&), string (*vertex2str)(T&)) {
    this->vertex = vertex;
    this->vertexEQ = vertexEQ;
    this->vertex2str = vertex2str;
    this->inDegree_ = 0;
    this->outDegree_ = 0;
}

template <class T>
VertexNode<T>::~VertexNode() {
    vector<Edge<T>*> tempEdges = adList;

    this->adList.clear();

    for (auto edge : tempEdges) {
        VertexNode<T>* otherNode = nullptr;

        if (edge->from == this) {
            otherNode = edge->to;
        } else {
            otherNode = edge->from;
        }

        if (otherNode && otherNode != this) {
            auto& otherList = otherNode->adList;
            for (auto it = otherList.begin(); it != otherList.end(); it++) {
                if (*it == edge) {
                    otherList.erase(it);
                    break;
                }
            }
        }

        delete edge;
    }
}

template <class T>
T& VertexNode<T>::getVertex() {
    return this->vertex;
}

template <class T>
void VertexNode<T>::connect(VertexNode<T>* to, float weight) {
    if (!to) return;
    
    for (auto edge : this->adList) {
        if (edge->from == this && (edge->to == to || (edge->to && edge->to->equals(to)))) {
            edge->weight = weight;
            return;
        }
    }

    Edge<T>* newEdge = new Edge<T>(this, to, weight);

    this->adList.push_back(newEdge);
    if (this != to) {
        to->adList.push_back(newEdge);
    }

    this->outDegree_++;
    to->inDegree_++;
}

template <class T>
Edge<T>* VertexNode<T>::getEdge(VertexNode<T>* to) {
    if (!to) return nullptr;
    for (auto edge : this->adList) {
        if (edge->from == this && edge->to->equals(to)) {
            return edge;
        }
    }
    return nullptr;
}

template <class T>
bool VertexNode<T>::equals(VertexNode<T>* node) {
    if (!node) return false;
    if (this->vertexEQ) {
        return this->vertexEQ(this->vertex, node->vertex);
    }
    return this->vertex == node->vertex;
}

template <class T>
void VertexNode<T>::removeTo(VertexNode<T>* to) {
    if (!to) return;
    
    Edge<T>* targetEdge = nullptr;
    for (auto it = this->adList.begin(); it != this->adList.end(); it++) {
        if ((*it)->from == this && (*it)->to->equals(to)) {
            targetEdge = *it;
            this->adList.erase(it);
            break;
        }
    }

    if (targetEdge) {
        for (auto it = to->adList.begin(); it != to->adList.end(); it++) {
            if (*it == targetEdge) {
                to->adList.erase(it);
                break;
            }
        }

        delete targetEdge;
        this->outDegree_--;
        to->inDegree_--;
    }
}

template <class T>
int VertexNode<T>::inDegree() {
    return this->inDegree_;
}

template <class T>
int VertexNode<T>::outDegree() {
    return this->outDegree_;
}

template <class T>
string VertexNode<T>::toString() {
    stringstream ss;
    ss << "(";
    if (this->vertex2str) {
        ss << this->vertex2str(this->vertex);
    } else {
        ss << this->vertex;
    }
    ss << ", " << inDegree_ << ", " << outDegree_ << ", [";

    for (int i = 0; i < (int)adList.size(); i++) {
        ss << adList[i]->toString();
        if (i < (int)adList.size() - 1) {
            ss << ", ";
        }
    }
    ss << "])";

    return ss.str();
}


// =============================================================================
// Class DGraphModel Implementation
// =============================================================================

template <class T>
DGraphModel<T>::DGraphModel(bool (*vertexEQ)(T&, T&), string (*vertex2str)(T&)) {
    this->vertexEQ = vertexEQ;
    this->vertex2str = vertex2str;
}

template <class T>
DGraphModel<T>::~DGraphModel() {
    this->clear();
}

template <class T>
void DGraphModel<T>::add(T vertex) {
    if (!contains(vertex)) {
        VertexNode<T>* newNode = new VertexNode<T>(vertex, this->vertexEQ, this->vertex2str);
        this->nodeList.push_back(newNode);
    }
}

template <class T>
void DGraphModel<T>::connect(T from, T to, float weight) {
    VertexNode<T>* nodeFrom = getVertexNode(from);
    if (!nodeFrom) {
        if (vertex2str) throw VertexNotFoundException("Vertex not found: " + vertex2str(from));
        else throw VertexNotFoundException();
    }

    VertexNode<T>* nodeTo = getVertexNode(to);
    if (!nodeTo) {
        if (vertex2str) throw VertexNotFoundException("Vertex not found: " + vertex2str(to));
        else throw VertexNotFoundException();
    }

    nodeFrom->connect(nodeTo, weight);
}

template <class T>
void DGraphModel<T>::disconnect(T from, T to) {
    VertexNode<T>* nodeFrom = getVertexNode(from);
    if (!nodeFrom) {
        if (vertex2str) throw VertexNotFoundException("Vertex not found: " + vertex2str(from));
        else throw VertexNotFoundException();
    }

    VertexNode<T>* nodeTo = getVertexNode(to);
    if (!nodeTo) {
        if (vertex2str) throw VertexNotFoundException("Vertex not found: " + vertex2str(to));
        else throw VertexNotFoundException();
    }

    nodeFrom->removeTo(nodeTo);
}

template <class T>
bool DGraphModel<T>::contains(T vertex) {
    return getVertexNode(vertex) != nullptr;
}

template <class T>
VertexNode<T>* DGraphModel<T>::getVertexNode(T& vertex) {
    for (auto node : this->nodeList) {
        if (this->vertexEQ) {
            if (this->vertexEQ(node->getVertex(), vertex)) {
                return node;
            }
        }
        else {
            if (node->getVertex() == vertex) {
                return node;
            }
        }
    }
    return nullptr;
}

template <class T>
string DGraphModel<T>::vertex2Str(VertexNode<T>& node) {
    return node.toString();
}

template <class T>
string DGraphModel<T>::edge2Str(Edge<T>& edge) {
    return edge.toString();
}

template <class T>
int DGraphModel<T>::size() {
    return this->nodeList.size();
}

template <class T>
bool DGraphModel<T>::empty() {
    return this->nodeList.empty();
}

template <class T>
void DGraphModel<T>::clear() {
    vector<Edge<T>*> edgesToDelete;

    for (auto node : this->nodeList) {
        for (auto edge : node->adList) {
            if (edge->from == node) {
                edgesToDelete.push_back(edge);
            }
        }
    }

    for (auto node : this->nodeList) {
        node->adList.clear();
    }

    for (auto edge : edgesToDelete) {
        delete edge;
    }

    for (auto node : this->nodeList) {
        delete node;
    }

    this->nodeList.clear();
}

template <class T>
vector<T> DGraphModel<T>::vertices() {
    vector<T> results;
    for (auto node : this->nodeList) {
        results.push_back(node->getVertex());
    }
    return results;
}

template <class T>
float DGraphModel<T>::weight(T from, T to) {
    VertexNode<T>* nodeFrom = getVertexNode(from);
    VertexNode<T>* nodeTo = getVertexNode(to);

    if (!nodeFrom || !nodeTo) throw VertexNotFoundException();

    Edge<T>* edge = nodeFrom->getEdge(nodeTo);
    if (!edge) {
        throw EdgeNotFoundException();
    }

    return edge->weight;
}

template <class T>
vector<Edge<T>*> DGraphModel<T>::getOutwardEdges(T from) {
    VertexNode<T>* nodeFrom = getVertexNode(from);
    if (!nodeFrom) {
        if (vertex2str) throw VertexNotFoundException("Vertex not found: " + vertex2str(from));
        else throw VertexNotFoundException();
    }

    vector<Edge<T>*> outward;
    for (auto edge : nodeFrom->adList) {
        if (edge->from == nodeFrom) {
            outward.push_back(edge);
        }
    }

    return outward;
}

template <class T>
bool DGraphModel<T>::connected(T from, T to) {
    VertexNode<T>* nodeFrom = getVertexNode(from);
    if (!nodeFrom) {
        if (vertex2str) throw VertexNotFoundException("Vertex not found: " + vertex2str(from));
        else throw VertexNotFoundException();
    }

    VertexNode<T>* nodeTo = getVertexNode(to);
    if (!nodeTo) {
        if (vertex2str) throw VertexNotFoundException("Vertex not found: " + vertex2str(to));
        else throw VertexNotFoundException();
    }

    return nodeFrom->getEdge(nodeTo) != nullptr;
}

template <class T>
int DGraphModel<T>::inDegree(T vertex) {
    VertexNode<T>* node = getVertexNode(vertex);
    if (!node) throw VertexNotFoundException();
    return node->inDegree();
}

template <class T>
int DGraphModel<T>::outDegree(T vertex) {
    VertexNode<T>* node = getVertexNode(vertex);
    if (!node) throw VertexNotFoundException();
    return node->outDegree();
}

template <class T>
string DGraphModel<T>::toString() {
    stringstream ss;
    ss << "[";
    for (int i = 0; i < (int)nodeList.size(); i++) {
        ss << nodeList[i]->toString();
        if (i < (int)nodeList.size() - 1) {
            ss << ", ";
        }
    }
    ss << "]";
    return ss.str();
}

template <class T>
string DGraphModel<T>::BFS(T start) {
    VertexNode<T>* startNode = getVertexNode(start);
    if (!startNode) {
        throw VertexNotFoundException();
    }

    vector<VertexNode<T>*> queue;
    vector<VertexNode<T>*> visited;

    queue.push_back(startNode);
    visited.push_back(startNode);

    stringstream ss;
    ss << "[";

    bool first = true;

    int head = 0;
    while (head < (int)queue.size()) {
        VertexNode<T>* u = queue[head];
        head++;

        if (!first) ss << ", ";
        if (this->vertex2str) {
            ss << this->vertex2str(u->getVertex());
        } else {
            ss << u->toString();
        }
        first = false;

        for (auto edge : u->adList) {
            if (edge->from != u) continue;

            VertexNode<T>* v = edge->to;
            bool isVisited = false;
            for (auto node : visited) {
                if (node == v) {
                    isVisited = true;
                    break;
                }
            }

            if (!isVisited) {
                visited.push_back(v);
                queue.push_back(v);
            }
        }
    }

    ss << "]";
    return ss.str();
}

template <class T>
string DGraphModel<T>::DFS(T start) {
    VertexNode<T>* startNode = getVertexNode(start);
    if (!startNode) {
        throw VertexNotFoundException();
    }

    vector<VertexNode<T>*> stack;
    vector<VertexNode<T>*> visited;

    stack.push_back(startNode);

    stringstream ss;
    ss << "[";
    bool first = true;

    while (!stack.empty()) {
        VertexNode<T>* u = stack.back();
        stack.pop_back();

        bool isVisited = false;
        for (auto node : visited) {
            if (node == u) {
                isVisited = true;
                break;
            }
        }

        if (!isVisited) {
            visited.push_back(u);
            
            if (!first) ss << ", ";
            if (this->vertex2str) {
                ss << this->vertex2str(u->getVertex());
            } else {
                ss << u->toString();
            }
            first = false;

            for (int i = (int)u->adList.size() - 1; i >= 0; i--) {
                Edge<T>* edge = u->adList[i];
                if (edge->from != u) continue;

                VertexNode<T>* v = edge->to;
                bool isNeighborVisited = false;
                for (auto node : visited) {
                    if (node == v) {
                        isNeighborVisited = true;
                        break;
                    }
                }

                if (!isNeighborVisited) {
                    stack.push_back(v);
                }
            }
        }
    }

    ss << "]";
    return ss.str();
}


// =============================================================================
// Class KnowledgeGraph Implementation
// =============================================================================

KnowledgeGraph::KnowledgeGraph() {}

void KnowledgeGraph::addEntity(string entity) {
    if (graph.contains(entity)) {
        throw EntityExistsException("Entity (" + entity + ") already exists!");
    }

    graph.add(entity);
    entities.push_back(entity);
}

void KnowledgeGraph::addRelation(string from, string to, float weight) {
    if (!graph.contains(from)) {
        throw EntityNotFoundException("Entity (" + from + ") not found!");
    }
    if (!graph.contains(to)) {
        throw EntityNotFoundException("Entity (" + to + ") not found!");
    }

    graph.connect(from, to, weight);
}

vector<string> KnowledgeGraph::getAllEntities() {
    return this->entities;
}

vector<string> KnowledgeGraph::getNeighbors(string entity) {
    if (!graph.contains(entity)) {
        throw EntityNotFoundException("Entity (" + entity + ") not found!");
    }

    vector<Edge<string>*> edges = graph.getOutwardEdges(entity);
    vector<string> neighbors;
    for (auto edge : edges) {
        neighbors.push_back(edge->to->getVertex());
    } 

    return neighbors;
}

string KnowledgeGraph::bfs(string start) {
    if (!graph.contains(start)) {
        throw EntityNotFoundException("Entity (" + start + ") not found!");
    }

    return graph.BFS(start);
}

string KnowledgeGraph::dfs(string start) {
    if (!graph.contains(start)) {
        throw EntityNotFoundException("Entity (" + start + ") not found!");
    }

    return graph.DFS(start);
}

bool KnowledgeGraph::isReachable(string from, string to) {
    if (!graph.contains(from)) {
        throw EntityNotFoundException("Entity (" + from + ") not found!");
    }
    if (!graph.contains(to)) {
        throw EntityNotFoundException("Entity (" + to + ") not found!");
    }

    vector<string> queue;
    vector<string> visited;

    queue.push_back(from);
    visited.push_back(from);

    int head = 0;
    while (head < (int)queue.size()) {
        string u = queue[head];
        head++;
        if (u == to) return true;

        vector<Edge<string>*> edges = graph.getOutwardEdges(u);
        for (auto edge : edges) {
            string v = edge->to->getVertex();

            bool isVisited = false;
            for (const string& visitedNode : visited) {
                if (visitedNode == v) {
                    isVisited = true;
                    break;
                }
            }

            if (!isVisited) {
                visited.push_back(v);
                queue.push_back(v);
            }
        }
    }

    return false;
}

string KnowledgeGraph::toString() {
    return graph.toString();
}

vector<string> KnowledgeGraph::getRelatedEntities(string entity, int depth) {
    if (!graph.contains(entity)) {
        throw EntityNotFoundException("Entity (" + entity + ") not found!");
    }

    vector<string> result;
    vector<string> visited;

    vector<string> queueNames;
    vector<int> queueDepths;

    queueNames.push_back(entity);
    queueDepths.push_back(0);
    visited.push_back(entity);

    int head = 0;
    while (head < (int)queueNames.size()) {
        string u = queueNames[head];
        int d = queueDepths[head];
        head++;

        if (d >= depth) continue;

        vector<Edge<string>*> edges = graph.getOutwardEdges(u);
        for (auto edge : edges) {
            string v = edge->to->getVertex();

            bool isVisited = false;
            for (const string& visitedNode : visited) {
                if (visitedNode == v) {
                    isVisited = true;
                    break;
                }
            }

            if (!isVisited) {
                visited.push_back(v);
                queueNames.push_back(v);
                queueDepths.push_back(d + 1);
                result.push_back(v);
            }
        }
    }

    return result;
}

vector<string> KnowledgeGraph::getParents(string u) {
    vector<string> parents;
    for (const string& candidate : entities) {
        if (candidate == u) continue;
        if (graph.connected(candidate, u)) {
            parents.push_back(candidate);
        }
    }
    return parents;
}

vector<string> KnowledgeGraph::dfsFindAncestors(string start) {
    vector<string> ancestors;
    vector<string> stack;
    vector<string> visited;

    stack.push_back(start);
    visited.push_back(start);
    ancestors.push_back(start);

    while (!stack.empty()) {
        string u = stack.back();
        stack.pop_back();

        vector<string> parents = getParents(u);
        for (const string& p : parents) {
            bool isVisited = false;
            for (const string& v : visited) {
                if (v == p) {
                    isVisited = true;
                    break;
                }
            }

            if (!isVisited) {
                visited.push_back(p);
                stack.push_back(p);
                ancestors.push_back(p);
            }
        }
    }

    return ancestors;
}

float KnowledgeGraph::getShortestDist(string from, string to) {
    if (from == to) return 0;

    // distances: store <node_name, current_shortest_dist>
    // visited: store visited nodes
    struct NodeDist {
        string name;
        float dist;
    };
    vector<NodeDist> dists;
    vector<string> visited;

    dists.push_back({from, 0.0f});

    while (true) {
        int bestIdx = -1;
        float minVal = 1e9;

        for (int i = 0; i < (int)dists.size(); i++) {
            bool isVisited = false;
            for (const string& v : visited) {
                if (v == dists[i].name) {
                    isVisited = true;
                    break;
                }
            }

            if (!isVisited && dists[i].dist < minVal) {
                minVal = dists[i].dist;
                bestIdx = i;
            }
        }

        if (bestIdx == -1) break;

        string u = dists[bestIdx].name;
        float d = dists[bestIdx].dist;
        visited.push_back(u);

        if (u == to) return d;

        if (graph.contains(u)) {
            vector<Edge<string>*> edges = graph.getOutwardEdges(u);
            for (auto edge : edges) {
                string v = edge->to->getVertex();
                float w = edge->weight;
                float newDist = d + w;

                bool found = false;
                for (auto& nd : dists) {
                    if (nd.name == v) {
                        if (newDist < nd.dist) {
                            nd.dist = newDist;
                        }
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    dists.push_back({v, newDist});
                }
            }
        }
    }

    return 1e9;
}

string KnowledgeGraph::findCommonAncestors(string entity1, string entity2) {
    if (!graph.contains(entity1)) {
        throw EntityNotFoundException("Entity (" + entity1 + ") not found!");
    }
    if (!graph.contains(entity2)) {
        throw EntityNotFoundException("Entity (" + entity2 + ") not found!");
    }

    vector<string> list1 = dfsFindAncestors(entity1);
    vector<string> list2 = dfsFindAncestors(entity2);

    vector<string> commonAncestors;
    for (const string& a1 : list1) {
        for (const string& a2 : list2) {
            if (a1 == a2) {
                commonAncestors.push_back(a1);
                break;
            }
        }
    }

    if (commonAncestors.empty()) {
        return "No common ancestor";
    }

    string bestAncestor = "";
    float minTotalDist = 1e9;
    bool foundAny = false;

    for (const string& ancestor : commonAncestors) {
        float d1 = getShortestDist(ancestor, entity1);
        float d2 = getShortestDist(ancestor, entity2);

        if (d1 < 1e9 && d2 < 1e9) {
            float totalDist = d1 + d2;
            if (totalDist < minTotalDist) {
                minTotalDist = totalDist;
                bestAncestor = ancestor;
                foundAny = true;
            }
        }
    }

    if (!foundAny) return "No common ancestor";
    return bestAncestor;
}


// =============================================================================
// Explicit Template Instantiation
// =============================================================================

template class Edge<string>;
template class Edge<int>;
template class Edge<float>;
template class Edge<char>;

template class VertexNode<string>;
template class VertexNode<int>;
template class VertexNode<float>;
template class VertexNode<char>;

template class DGraphModel<string>;
template class DGraphModel<int>;
template class DGraphModel<float>;
template class DGraphModel<char>;