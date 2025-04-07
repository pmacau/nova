#include "path_finder.hpp"
#include <util/priority_queue.hpp>
#include <map/map_system.hpp>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <algorithm>

static inline int heuristic(const ivec2& a, const ivec2& b) {
    // return std::abs(a.x - b.x) + std::abs(a.y - b.y);
    // return glm::distance((vec2) a,  (vec2) b) * 10.0f; // to match the dir cost
    // int dx = std::abs(a.x - b.x);
    // int dy = std::abs(a.y - b.y);
    // return 10 * (dx + dy) + (4 * std::min(dx, dy));
    return glm::distance((vec2) a,  (vec2) b) * 10.0f; // to match the dir cost
}

std::vector<ivec2> Pathfinder::findPath(const ivec2& start, const ivec2& goal, bool limitIterations) {
    std::vector<ivec2> path;
    
    PriorityQueue<Node, int> openSet;
    
    std::unordered_map<ivec2, ivec2, IVec2Hash> cameFrom;
    
    std::unordered_map<ivec2, int, IVec2Hash> gScore;
    gScore[start] = 0;
    
    std::unordered_map<ivec2, int, IVec2Hash> fScore;
    fScore[start] = heuristic(start, goal);
    
    openSet.put(Node(start, start, 0, fScore[start]), fScore[start]);
    
    const std::vector<Direction> directions = {
        { ivec2(0, -1), 10 },   // up
        { ivec2(1, 0), 10 },    // right
        { ivec2(0, 1), 10 },    // down
        { ivec2(-1, 0), 10 },   // left
        { ivec2(1, -1), 14 },   // up-right
        { ivec2(1, 1), 14 },    // down-right
        { ivec2(-1, 1), 14 },   // down-left
        { ivec2(-1, -1), 14 }   // up-left
    };

    int iterations = 0;
    const int maxIterations = 10000;

    Node bestNode(start, start, 0, fScore[start]);
    int bestScore = fScore[start];
    
    while (!openSet.empty() && (!limitIterations || iterations < maxIterations)) {
        iterations++;
        Node current = openSet.get();
        
        if (current.position == goal) {
            // Reconstruct path.
            ivec2 curPos = current.position;
            while (curPos != start) {
                path.push_back(curPos);
                curPos = cameFrom[curPos];
            }
            path.push_back(start);
            std::reverse(path.begin(), path.end());
            return path;
        }

        // Update the best node if current has a lower f-score.
        if (current.getScore() < bestScore) {
            bestScore = current.getScore();
            bestNode = current;
        }
        
        // For each neighbor.
        for (const auto& dir : directions) {
            ivec2 neighbor = current.position + dir.offset;
            // Check bounds using MapSystem.
            if (neighbor.x < 0 || neighbor.x >= MapSystem::map_width ||
                neighbor.y < 0 || neighbor.y >= MapSystem::map_height)
                continue;
            
            // Check if neighbor tile is walkable.
            if (!MapSystem::walkable_tile(MapSystem::get_tile_type_by_indices(neighbor.x, neighbor.y)))
                continue;
            
            int tentativeG = current.G + dir.cost; // Assuming cost 1 for each move.
            if (gScore.find(neighbor) == gScore.end() || tentativeG < gScore[neighbor]) {
                cameFrom[neighbor] = current.position;
                gScore[neighbor] = tentativeG;
                float h = heuristic(neighbor, goal);
                float f = tentativeG + h;
                fScore[neighbor] = f;
                openSet.put(Node(neighbor, current.position, tentativeG, h), f);
            }
        }
    }
    
    // we exceeded maxIterations or no path found
    // Reconstruct path from the best node encountered
    ivec2 curPos = bestNode.position;
    while (curPos != start) {
        path.push_back(curPos);
        curPos = cameFrom[curPos];
    }
    path.push_back(start);
    std::reverse(path.begin(), path.end());
    return path;
}