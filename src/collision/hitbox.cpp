#include <vector>
#include "hitbox.hpp"

vec2 edge_normal(const vec2& edge) {
    return vec2(-edge.y, edge.x);
}

std::vector<vec2> get_normals(const std::vector<vec2>& pts) {
    std::vector<vec2> normals;
    int size = pts.size();
    for (int i = 0; i < size; ++i) {
        vec2 edge = pts[(i + 1) % size] - pts[i];
        normals.push_back(edge_normal(edge));
    }
    return normals;
}

void project(
    const std::vector<vec2>& pts, const vec2& centre, const vec2& axis,
    float& minProj, float& maxProj
) {
    minProj = std::numeric_limits<float>::infinity();
    maxProj = -std::numeric_limits<float>::infinity();

    for (const vec2& pt : pts) {
        float projection = dot(pt + centre, axis);
        if (projection < minProj) minProj = projection;
        if (projection > maxProj) maxProj = projection;
    }
}

bool overlaps(float minA, float maxA, float minB, float maxB) {
    return !(maxA < minB || maxB < minA);
}


// for each (unique) axis
// project each point of shape onto the axis
// find min and max value for each shape (if s1 max > s2 min)
// if we can find 1 axis to separate objects, they are not touching
bool collides(
    const Hitbox& h_a, const Motion& m_a,
    const Hitbox& h_b, const Motion& m_b
) {
    float a_baseY = (m_a.position + m_a.offset_to_ground).y;
    float b_baseY = (m_b.position + m_b.offset_to_ground).y;

    if (
        !(a_baseY >= (b_baseY - h_b.depth) && a_baseY <= b_baseY) && // A is not within B's bounds
        !(b_baseY >= (a_baseY - h_a.depth) && b_baseY <= a_baseY)    // B is not within A's bounds
    ) return false;

    const std::vector<vec2>& ptsA = h_a.pts;
    const std::vector<vec2>& ptsB = h_b.pts;

    std::vector<vec2> normalsA = get_normals(ptsA);
    std::vector<vec2> normalsB = get_normals(ptsB);
    float minA, maxA, minB, maxB;

    for (const vec2& axis : normalsA) {
        project(ptsA, m_a.position, axis, minA, maxA);
        project(ptsB, m_b.position, axis, minB, maxB);
        if (!overlaps(minA, maxA, minB, maxB)) {
            return false;
        }
    }

    for (const vec2& axis : normalsB) {
        project(ptsA, m_a.position, axis, minA, maxA);
        project(ptsB, m_b.position, axis, minB, maxB);
        if (!overlaps(minA, maxA, minB, maxB)) {
            return false;
        }
    }

    return true;
}