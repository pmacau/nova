#pragma once

#include <vector>
#include <glm/vec2.hpp>

// For tile coordinates, we use ivec2 (from glm)
using ivec2 = glm::ivec2;

// A simple A* node.
struct Node {
    ivec2 position;
    ivec2 parent;
    int G; // Cost from start to current node.
    int H; // Heuristic cost from current node to goal.
    
    Node() : position(0), parent(0), G(0), H(0) {}
    Node(const ivec2& pos, const ivec2& parent, int G, int H)
        : position(pos), parent(parent), G(G), H(H) {}

    bool operator==(const Node& rhs) const { return this->position == rhs.position; }

	bool operator==(const ivec2& pos) const { return this->position == pos; }

    bool operator<(const Node& rhs) const {
        return getScore() < rhs.getScore();
    }
    
    int getScore() const { return G + H; }
};

// A hash function for ivec2
struct IVec2Hash {
    std::size_t operator()(const ivec2& v) const {
        std::hash<int> hasher;
        size_t h1 = hasher(v.x);
        size_t h2 = hasher(v.y);
        return h1 ^ (h2 << 1);
    }
};

class Pathfinder {
public:
    // Returns a vector of tile indices (ivec2) representing the path.
    // If no path is found, returns an empty vector.
    std::vector<ivec2> findPath(const ivec2& start, const ivec2& goal);
};