#pragma once
#include "../tinyECS/components.hpp"
#include "../collision/hitbox.hpp"	
#include <vector>
#include <array>


//class Quad {
//public: 
//	float x, y, width, height; 
//
//	Quad(float x, float y, float width, float height) : x(x), y(y), width(width), height(height) {}
//
//	bool contains(float px, float py) const {
//		return (px >= x && px <= x + width && py >= y && py <= y + height);
//	}
//
//	bool intersects(const Quad& other) const {
//		return !(x + width <= other.x || x >= other.x + other.width ||
//			y + height <= other.y || y >= other.y + other.height);
//	}
//
//	// likely don't need these. 
//	// Getter methods
//	float getX() const { return x; }
//	float getY() const { return y; }
//	float getWidth() const { return width; }
//	float getHeight() const { return height; }
//
//	// Setter methods
//	void setX(float newX) { x = newX; }
//	void setY(float newY) { y = newY; }
//	void setWidth(float newWidth) { width = newWidth; }
//	void setHeight(float newHeight) { height = newHeight; }
//
//
//
//};

class Quad {
public:
    float x, y;          // Center position
    float width, height; // Dimensions

    Quad(float centerX, float centerY, float width, float height)
        : x(centerX), y(centerY), width(width), height(height) {
    }

    // Check if a point (px, py) is inside this quad
    bool contains(float px, float py) const {
        return (px >= x - width / 2.f && px <= x + width / 2.f &&
            py >= y - height / 2.f && py <= y + height / 2.f);
    }

    // Check intersection with another quad
    bool intersects(const Quad& other) const {
        return !(x + width / 2.f <= other.x - other.width / 2.f ||
            x - width / 2.f >= other.x + other.width / 2.f ||
            y + height / 2.f <= other.y - other.height / 2.f ||
            y - height / 2.f >= other.y + other.height / 2.f);
    }

    // Getter methods (optional)
    float getCenterX() const { return x; }
    float getCenterY() const { return y; }
    float getWidth() const { return width; }
    float getHeight() const { return height; }

    // Setter methods (optional)
    void setCenterX(float newX) { x = newX; }
    void setCenterY(float newY) { y = newY; }
    void setWidth(float newWidth) { width = newWidth; }
    void setHeight(float newHeight) { height = newHeight; }

};
