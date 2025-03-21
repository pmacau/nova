#include "path_finder.hpp"
#include <util/priority_queue.hpp>
#include <map/map_system.hpp>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <algorithm>

// Heuristic function: Manhattan distance.
static inline int heuristic(const ivec2& a, const ivec2& b) {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

std::vector<ivec2> Pathfinder::findPath(const ivec2& start, const ivec2& goal) {
    std::vector<ivec2> path;
    
    PriorityQueue<Node, int> openSet;
    
    std::unordered_map<ivec2, ivec2, IVec2Hash> cameFrom;
    
    std::unordered_map<ivec2, int, IVec2Hash> gScore;
    gScore[start] = 0;
    
    std::unordered_map<ivec2, int, IVec2Hash> fScore;
    fScore[start] = heuristic(start, goal);
    
    openSet.put(Node(start, start, 0, fScore[start]), fScore[start]);
    
    const std::vector<ivec2> directions = {
        ivec2(0, -1), // up
        ivec2(1, 0),  // right
        ivec2(0, 1),  // down
        ivec2(-1, 0)  // left
    };
    
    while (!openSet.empty()) {
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
        
        // For each neighbor.
        for (const auto& dir : directions) {
            ivec2 neighbor = current.position + dir;
            // Check bounds using MapSystem.
            if (neighbor.x < 0 || neighbor.x >= MapSystem::map_width ||
                neighbor.y < 0 || neighbor.y >= MapSystem::map_height)
                continue;
            
            // Check if neighbor tile is walkable.
            if (!MapSystem::walkable_tile(MapSystem::get_tile_type_by_indices(neighbor.x, neighbor.y)))
                continue;
            
            int tentativeG = current.G + 1; // Assuming cost 1 for each move.
            if (gScore.find(neighbor) == gScore.end() || tentativeG < gScore[neighbor]) {
                cameFrom[neighbor] = current.position;
                gScore[neighbor] = tentativeG;
                int f = tentativeG + heuristic(neighbor, goal);
                fScore[neighbor] = f;
                openSet.put(Node(neighbor, current.position, tentativeG, heuristic(neighbor, goal)), f);
            }
        }
    }
    
    // no path was found.
    return path;
}