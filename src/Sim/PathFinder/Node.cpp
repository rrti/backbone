#include "Node.hpp"
#include <iostream>

Node::Node(int x, int y, int z, unsigned int id, float w): ANode(id, w) {
	this->x = x;
	this->y = y;
	this->z = z;
	bType = NORMAL;
	radius = 0.0f;
}

vec3& Node::GetColor() {
	switch(bType) {
		case START:		color = vec3(0.0f, 1.0f, 0.0f); break;
		case GOAL:		color = vec3(1.0f, 0.0f, 0.0f); break;
		case BLOCKED:	color = vec3(0.0f, 0.0f, 0.4f); break;
		case NORMAL:	color = vec3(1.0f, 1.0f, 1.0f); break;
		break;
	}
	return color;
}

bool Node::blocked() { return bType == BLOCKED; }
void Node::toggleBlocked() { bType = ((bType == BLOCKED)? NORMAL: BLOCKED); }
void Node::setStart() { bType = START; }
void Node::setGoal() { bType = GOAL; }
vec3 Node::GetVector() { return vec3(x, y, z); }
