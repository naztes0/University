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

	return query(node->leftChild, l, r) + query(node->rightChild, l, r);
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

