#ifndef AASTAR_HPP
#define AASTAR_HPP

#include <queue>
#include <vector>
#include <map>
#include <iterator>

#include "ANode.hpp"

class AAStar {
	private:
		/* nodes visited during pathfinding */
		std::vector<ANode*> visited;

		/* priority queue of the open list */
		std::priority_queue<ANode*, std::vector<ANode*, std::allocator<ANode*> >, ANode> open;

		/* successors stack */
		std::queue<ANode*> succs;

		/* traces the path from the goal node through its parents */
		void tracePath(std::vector<ANode*> &path);


	protected:
		void init();
		virtual ~AAStar() {};

		virtual void successors(ANode *n, std::queue<ANode*> &succ) = 0;
		virtual float heuristic(ANode *n1, ANode *n2) = 0;

	public:
		std::vector<ANode*> history;
		void findPath(std::vector<ANode*> &path);
		ANode* start;
		ANode* goal;

};

#endif
