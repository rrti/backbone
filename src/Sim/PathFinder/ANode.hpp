#ifndef ANODE_HPP
#define ANODE_HPP

class ANode {
	public:
		ANode() {id = 0; g = 0; h = w = 0.0f; open = closed = false; }
		ANode(unsigned int id, float w) {
			this->id		= id;
			this->w			= w;
			this->g			= 0;
			this->h			= 0.0f;
			this->open		= false;
			this->closed	= false;
		}

		virtual ~ANode() {}

		bool open;
		bool closed;

		unsigned int id;

		unsigned int g;
		float h;
		float w;

		ANode* parent;

		bool operator < (const ANode* n) const {
			return (g + h) > (n->g + n->h);
		}

		bool operator () (const ANode* a, const ANode* b) const {
			return (float(a->g + a->h) > float(b->g + b->h));
		}

		bool operator == (const ANode* n) const {
			return (id == n->id);
		}

		bool operator != (const ANode* n) const {
			return !(this == n);
		}
};

#endif
