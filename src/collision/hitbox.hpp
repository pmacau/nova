#pragma once

#include <vector>
#include <limits>
#include <cmath>

#include "common.hpp"


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

using Polygon = std::vector<Vector>;

std::vector<Vector> getNormals(const Polygon& poly) {
    std::vector<Vector> normals;
    int size = poly.size();
    for (int i = 0; i < size; ++i) {
        Vector edge = poly[(i + 1) % size] - poly[i];
        normals.push_back(edge.normal());
    }
    return normals;
}

void projectPolygon(const Polygon& poly, const Vector& axis, float& minProj, float& maxProj) {
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

bool collides(const Polygon& polyA, const Polygon& polyB) {
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