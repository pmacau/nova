#pragma once
#include <queue>
#include <vector>
#include <functional>

template<typename T, typename PriorityT>
class PriorityQueue {
public:
    using PQElement = std::pair<PriorityT, T>;

    bool empty() const { return elements.empty(); }
    
    void put(const T& item, PriorityT priority) {
        elements.emplace(priority, item);
    }
    
    T get() {
        T bestItem = elements.top().second;
        elements.pop();
        return bestItem;
    }
    
private:
    std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>> elements;
};