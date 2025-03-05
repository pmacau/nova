#pragma once

#include "common.hpp"
#include "tinyECS/components.hpp"

// TODO: handle circles (should be relatively trivial)
// optimizations:
// - precompute normals and store 1 "mesh" for each shape type
// - remove collinear axes (this might be slower due to O(n^2) check

constexpr int EPSILON = 10;

struct Hitbox {
    bool isRigid = false;
    float depth = EPSILON;
    std::vector<vec2> pts;
};

vec2 edge_normal(const vec2& edge);
std::vector<vec2> get_normals(const std::vector<vec2>& pts);
void project(
    const std::vector<vec2>& pts, const vec2& centre, const vec2& axis,
    float& minProj, float& maxProj
);
bool overlaps(float minA, float maxA, float minB, float maxB);
bool collides(
    const Hitbox& h_a, const Motion& m_a,
    const Hitbox& h_b, const Motion& m_b
);