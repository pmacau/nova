#pragma once
#include "../tinyECS/components.hpp"
#include "../collision/hitbox.hpp"	
#include <vector>
#include <array>


class Quad {
public: 
	float x, y, width, height; 

	Quad(float x, float y, float width, float height) : x(x), y(y), width(width), height(height) {}

	bool contains(float px, float py) const {
		return (px >= x && px <= x + width && py >= y && py <= y + height);
	}

	bool intersects(const Quad& other) const {
		return !(x + width <= other.x || x >= other.x + other.width ||
			y + height <= other.y || y >= other.y + other.height);
	}

	// likely don't need these. 
	// Getter methods
	float getX() const { return x; }
	float getY() const { return y; }
	float getWidth() const { return width; }
	float getHeight() const { return height; }

	// Setter methods
	void setX(float newX) { x = newX; }
	void setY(float newY) { y = newY; }
	void setWidth(float newWidth) { width = newWidth; }
	void setHeight(float newHeight) { height = newHeight; }



};