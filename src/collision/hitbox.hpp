#pragma once

#include <vector>
#include <limits>
#include <cmath>
#include <type_traits>

#include "common.hpp"

struct Hitbox {
    Vector centre;
    bool isRigid;
    float baseY;
    float depth;

    virtual ~Hitbox() = default;
};

struct CircleHitbox : public Hitbox {
    float radius;
};
struct PolygonHitbox : public Hitbox {
    Points pts;
    std::vector<Vector> normals = getNormals(pts);
};
using Points = std::vector<Vector>;

struct Vector {
    float x, y;

    Vector operator-(const Vector& other) const {
        return {x - other.x, y - other.y};
    }

    Vector normal() const {
        return {-y, x};
    }

    float dot(const Vector& other) const {
        return x * other.x + y * other.y;
    }
};

std::vector<Vector> getNormals(const Points& poly) {
    std::vector<Vector> normals;
    int size = poly.size();
    for (int i = 0; i < size; ++i) {
        Vector edge = poly[(i + 1) % size] - poly[i];
        normals.push_back(edge.normal());
    }
    return normals;
}

void projectPolygon(const Points& poly, const Vector& axis, float& minProj, float& maxProj) {
    minProj = std::numeric_limits<float>::infinity();
    maxProj = -std::numeric_limits<float>::infinity();

    for (const Vector& point : poly) {
        float projection = point.dot(axis);
        if (projection < minProj) minProj = projection;
        if (projection > maxProj) maxProj = projection;
    }
}

bool isOverlapping(float minA, float maxA, float minB, float maxB) {
    return !(maxA < minB || maxB < minA);
}

// template <typename S1, typename S2>
// typename std::enable_if<std::is_same<S1, Polygon>::value && std::is_same<S2, Polygon>::value, bool>::type
// collides(const S1& poly1, const S2& poly2) {
//     std::cout << "Polygon-Polygon SAT collision check\n";
//     // Implement SAT for Polygon vs. Polygon
//     return true; // Placeholder
// }

bool collides(const Points& polyA, const Points& polyB) {
    std::vector<Vector> normalsA = getNormals(polyA);
    std::vector<Vector> normalsB = getNormals(polyB);
    float minA, maxA, minB, maxB;

    for (const Vector& axis : normalsA) {
        projectPolygon(polyA, axis, minA, maxA);
        projectPolygon(polyB, axis, minB, maxB);
        if (!isOverlapping(minA, maxA, minB, maxB)) {
            return false;
        }
    }

    for (const Vector& axis : normalsB) {
        projectPolygon(polyA, axis, minA, maxA);
        projectPolygon(polyB, axis, minB, maxB);
        if (!isOverlapping(minA, maxA, minB, maxB)) {
            return false;
        }
    }

    return true;
}

// TODO: handle circles (should be relatively trivial)
//       - optimization, remove collinear axes (not really important rn)

// b x ((a dot b) / (b dot b))

// for each (unique) axis
// project each point of shape onto the axis
// find min and max value for each shape (if s1 max > s2 min)
// if we can find 1 axis to separate objects,
// they are not touching


// Registry.sort<Transform>([](const auto& lhs, const auto& ths) { return lhs.z < rhs.z; } // sort by Z-axis
// auto view = Registry.view<Transform, Sprite>().use<Transform>(); // I didn't know about .use()!
// for (auto [entity, transform, sprite] : view)
// {
//     RenderSprite(transform, sprite); // entities with lower z-coord are rendered first (below) entities with greater z-coord.
// }