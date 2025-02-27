#include"OrderStatisticTree(RBT).h"

//////PRIVATE//////////
void OST::updateSize(Node* node) {
	if (node != NIL) {
		node->size = node->left->size + node->right->size + 1;
	}
}

void OST::leftRotate(Node* x) {
	Node* y = x->right; //new Node
	x->right = y->left; 
	if (y->left != NIL) {
		y->left->parent = x;
	}
	y->parent = x->parent;
	if (x->parent == NIL) {
		root = y;
	}
	else if (x == x->parent->left) {
		x->parent->left = y;
	}
	else {
		x->parent->right = y;
	}
	
	y->left = x;
	x->parent = y;

	updateSize(x);
	updateSize(y);

}

void OST::rightRotate(Node* x) {
	Node* y = x->left;
	x->left = y->right;

	if (y->right != NIL) {
		y->right->parent = x;
	}

	y->parent = x->parent;

	if (x->parent == NIL) {
		root=y;
	}
	else if (x == x->parent->left) {
		x->parent->left = y;
	}
	else {
		x->parent->right = y;
	}

	y->right = x;
	x->parent = y;

	updateSize(x);
	updateSize(y);
}

///////PUBLIC//////

void OST::insert(int key) {
	Node* newNode = new Node(key);
	newNode->left = NIL;
	newNode->right = NIL;
	newNode->color = false; // new node always red

	Node* parent = NIL;
	Node* current = root;
	while (current != NIL) {
		parent = current;
		parent->size++;
		if (key < current->key) {
			current=current->left;
		}
		else {
			current = current->right;
		}
	}

	newNode->parent = parent;
	if (parent == NIL) root = newNode;
	else if (key < parent->key) parent->left = newNode;
	else parent->right = newNode;

	fixInsert(newNode);
}