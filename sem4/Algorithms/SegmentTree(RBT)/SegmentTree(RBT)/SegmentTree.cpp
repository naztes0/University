#include "SegmentTree.h"

//////////////PRIVATE////////////

void SegmentTree::rightRotate(Node* x) {
	Node* y = x->leftChild;
	x->leftChild = y->rightChild;

	if (y->rightChild != NIL) {
		y->rightChild->parent = x;
	}

	y->parent = x->parent;

	if (x->parent == NIL) {
		root = y;
	}
	else if (x == x->parent->leftChild) {
		x->parent->leftChild = y;
	}
	else {
		x->parent->rightChild = y;
	}

	y->rightChild = x;
	x->parent = y;

	
}

void SegmentTree::leftRotate(Node* x) {
	Node* y = x->rightChild; //new Node
	x->rightChild = y->leftChild;
	if (y->leftChild != NIL) {
		y->leftChild->parent = x;
	}
	y->parent = x->parent;
	if (x->parent == NIL) {
		root = y;
	}
	else if (x == x->parent->leftChild) {
		x->parent->leftChild = y;
	}
	else {
		x->parent->rightChild = y;
	}

	y->leftChild = x;
	x->parent = y;

}

void SegmentTree::fixInsert(Node* node) {
	while (node != root && node->parent->color == false) { //till parent red
		if (node->parent == node->parent->parent->leftChild) { // parent- left son of grand
			Node* uncle = node->parent->parent->rightChild;

			if (uncle->color == false) { // uncle is red
				node->parent->color = true; // parent->black
				uncle->color = true; // uncle->black
				node->parent->parent->color = false; // graand->red
				node = node->parent->parent; // move to grand
			}
			else { // uncle is black
				if (node == node->parent->rightChild) { // node is right son
					node = node->parent;
					leftRotate(node);
				}
				// node is left son
				node->parent->color = true; // parent->black
				node->parent->parent->color = false; // grand->red
				rightRotate(node->parent->parent);
			}
		}
		else { // symetrical case
			Node* uncle = node->parent->parent->leftChild;
			if (uncle->color == false) {
				node->parent->color = true;
				uncle->color = true;
				node->parent->parent->color = false;
				node = node->parent->parent;
			}
			else {
				if (node == node->parent->leftChild) {
					node = node->parent;
					rightRotate(node);
				}
				node->parent->color = true;
				node->parent->parent->color = false;
				leftRotate(node->parent->parent);
			}
		}
	}
	root->color = true;
}

int SegmentTree::queryPrivate(Node* node, int l, int r) {
	if (node == NIL || l > node->right || r < node->left)return 0;

	if (l <= node->left && node->right <= r)	return node->sum;

	return queryPrivate(node->leftChild, l, r) + queryPrivate(node->rightChild, l, r);
}

void SegmentTree::updatePrivate(Node* node, int l, int r, int newValue) {
	if (node == NIL || l > node->right || r < node->left) {
		return; // Відрізок взагалі не перетинається
	}

	if (l <= node->left && node->right <= r) {
		node->sum += (node->right - node->left + 1) * newValue; // Оновлюємо суму
	}
	else {
		// Часткове перекриття - оновлюємо дітей
		updatePrivate(node->leftChild, l, r, newValue);
		updatePrivate(node->rightChild, l, r, newValue);

		// Після оновлення дітей оновлюємо поточну суму
		node->sum = node->leftChild->sum + node->rightChild->sum;
	}
}

//Methods for removing nodes
void SegmentTree::transplant(Node* u, Node* v) {
	if (u->parent == NIL) {
		root = v;
	}
	else if (u == u->parent->leftChild) {
		u->parent->leftChild = v;
	}
	else {
		u->parent->rightChild = v;
	}
	v->parent = u->parent;
}

SegmentTree::Node* SegmentTree::minimum(Node* node) {
	while (node->leftChild != NIL) {
		node = node->leftChild;
	}
	return node;
}
void SegmentTree::fixDelete(Node* node) {
	while (node != root && node->color == true) {
		if (node == node->parent->leftChild) {
			Node* sibling = node->parent->rightChild;

			if (sibling->color == false) {
				sibling->color = true;
				node->parent->color = false;
				leftRotate(node->parent);
				sibling = node->parent->rightChild;
			}

			if (sibling->leftChild->color == true && sibling->rightChild->color == true) {
				sibling->color = false;
				node = node->parent;
			}
			else {
				if (sibling->rightChild->color == true) {
					sibling->leftChild->color = true;
					sibling->color = false;
					rightRotate(sibling);
					sibling = node->parent->rightChild;
				}

				sibling->color = node->parent->color;
				node->parent->color = true;
				sibling->rightChild->color = true;
				leftRotate(node->parent);
				node = root;
			}
		}
		else {
			Node* sibling = node->parent->leftChild;

			if (sibling->color == false) {
				sibling->color = true;
				node->parent->color = false;
				rightRotate(node->parent);
				sibling = node->parent->leftChild;
			}

			if (sibling->leftChild->color == true && sibling->rightChild->color == true) {
				sibling->color = false;
				node = node->parent;
			}
			else {
				if (sibling->leftChild->color == true) {
					sibling->rightChild->color = true;
					sibling->color = false;
					leftRotate(sibling);
					sibling = node->parent->leftChild;
				}

				sibling->color = node->parent->color;
				node->parent->color = true;
				sibling->leftChild->color = true;
				rightRotate(node->parent);
				node = root;
			}
		}
	}
	node->color = true;
}
void SegmentTree::removeNode(Node* node, int l, int r) {
	if (node == NIL) return;

	if (l < node->left) {
		removeNode(node->leftChild, l, r);
	}
	else if (l > node->left) {
		removeNode(node->rightChild, l, r);
	}
	else {
		Node* y = node;
		Node* x;
		bool yOriginalColor = y->color;

		if (node->leftChild == NIL) {
			x = node->rightChild;
			transplant(node, node->rightChild);
		}
		else if (node->rightChild == NIL) {
			x = node->leftChild;
			transplant(node, node->leftChild);
		}
		else {
			y = minimum(node->rightChild);
			yOriginalColor = y->color;
			x = y->rightChild;

			if (y->parent == node) {
				x->parent = y;
			}
			else {
				transplant(y, y->rightChild);
				y->rightChild = node->rightChild;
				y->rightChild->parent = y;
			}

			transplant(node, y);
			y->leftChild = node->leftChild;
			y->leftChild->parent = y;
			y->color = node->color;
		}

		delete node;

		if (yOriginalColor == true) { // Якщо видалявся чорний вузол
			fixDelete(x);
		}
	}
}


////////////PUBLIC//////////////////////
void SegmentTree::insert(int l, int r, int sum) {
	Node* newNode = new Node(l, r, sum);
	newNode->leftChild = NIL;
	newNode->rightChild = NIL;
	newNode->color = false;

	Node* parent = NIL;
	Node* current = root;

	while (current != NIL) {
		parent = current;
		if (l < current->left) {
			current = current->leftChild;
		}
		else {
			current = current->rightChild;
		}
	}
	newNode->parent = parent;
	if (parent == NIL) {
		root = newNode;
	}
	else if (l < parent->left) {
		parent->leftChild = newNode;
	}
	else {
		parent->rightChild = newNode;
	}
	
	
	fixInsert(newNode);
}

int SegmentTree::query(int l, int r) {
	queryPrivate(root, l, r);
}

void SegmentTree::update(int l, int r, int newValue) {
	updatePrivate(root, l, r, newValue);
}

void SegmentTree::remove(int l, int r) {
	removeNode(root, l, r);
}