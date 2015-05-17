#ifndef NODE_HPP
#define NODE_HPP

#include "ANode.hpp"
#include "../../Math/vec3.hpp"

enum blockType {BLOCKED, START, GOAL, NORMAL};

class Node: public ANode {
	public:
		Node(int x, int y, int z, unsigned int id, float w);

		blockType bType;
		int x, y, z;
		float radius;

		vec3 color;
		vec3 normal;

		vec3& GetColor();
		bool blocked();
		void toggleBlocked();
		void setStart();
		void setGoal();
		vec3 GetVector();
};

#endif
