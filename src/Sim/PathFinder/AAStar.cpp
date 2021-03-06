#include "AAStar.hpp"

void AAStar::init() {
	/* Reset the open and closed "lists" */
	for (unsigned int i = 0; i < visited.size(); i++)
		visited[i]->closed = visited[i]-> open = false;

	visited.clear();
	history.clear();

	/* Pop the queue */
	while (!open.empty()) {
		open.pop();
	}
}

void AAStar::findPath(std::vector<ANode*>& path) {
	float c;
	ANode *x, *y;

	init();

	printf("Pathfinding...");
	open.push(start);
	start->open = true;
	visited.push_back(start);

	while (!open.empty()) {
		x = open.top(); open.pop();
		x->open = false;

		if (x == goal) {
			tracePath(path);
			printf("[done]\n");
			return;
		}

		x->closed = true;

		successors(x, succs);
		while (!succs.empty()) {
			y = succs.front(); succs.pop();
			c = x->g + (y->w * heuristic(x, y));

			if (y->open && c < y->g)
				y->open = false;

			/* Only happens with an admissable heuristic */
			if (y->closed && c < y->g)
				y->closed = false;
			
			if (!y->open && !y->closed) {
				y->g = (unsigned int) c;
				y->parent = x;
				y->h = (y->w * heuristic(y, goal));
				open.push(y);
				y->open = true;

				visited.push_back(y);
				history.push_back(y);
				history.push_back(x);
			}
		}
	}

	printf("[failed]\n");
}

void AAStar::tracePath(std::vector<ANode*>& path) {
	ANode* n = goal;

	while (n != start) {
		path.push_back(n);
		history.push_back(n);
		n = n->parent;
	}
}
