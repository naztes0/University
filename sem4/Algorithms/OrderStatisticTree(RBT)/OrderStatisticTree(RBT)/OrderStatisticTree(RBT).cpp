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

void OST::updateSizeAfterDelete(Node* node) {
	while (node != NIL) {
		node->size = node->left->size + node->right->size + 1;
		node = node->parent;
	}
}
void OST::fixInsert(Node* node) {
	while (node->parent->color == false) { //till the parent is red
		if (node->parent == node->parent->parent->left) { // if parent is left son of grand
			Node* uncle = node->parent->parent->right;

			if (uncle->color == false) {
				node->parent->color = true;
				uncle->color = true;
				node->parent->parent->color = false;
				node = node->parent->parent;
			}
			else {
				if (node == node->parent->right) {
					node = node->parent;
					leftRotate(node);
				}
				node->parent->color = true;
				node->parent->parent->color = true;
				rightRotate(node->parent->parent);
			}
		}
		else {
			Node* uncle = node->parent->parent->left;
			if (uncle->color == false) {
				node->parent->color = true;
				uncle->color = true;
				node->parent->parent->color = false;
				node = node->parent->parent;
			}
			else {
				if (node == node->parent->left) {
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



OST::Node* OST::findByOrder(Node* node, int k) {
	if (node == NIL || k <= 0 || k > node->size) return NIL;

	int leftsize = node->left->size;

	if (leftsize + 1 == k) {
		return node;
	}
	else if (k <= leftsize) {
		return  findByOrder(node->left,k);
	}
	else {
		return findByOrder(node->right, k - (leftsize + 1));
	}

}

int OST::orderOfKey(Node* node, int x) {
	if (node == NIL) return 0;

	if (x == node->key) return node->left->size;

	if (x < node->key) return orderOfKey(node->left, x);
	else {
		return node->left->size + 1 + orderOfKey(node->right, x);
	}

}

void OST::remove(Node*node,int key) {
	if (node == NIL) return; // No key in tree
	if (key < node->key) {
		remove(node->left, key);
		return;
	}
	if (key > node->key) {
		remove(node->right, key);
		return;
	}
	
	Node* removable = node;
	Node* child;
	if (removable->left == NIL || removable->right == NIL) {
		child = (removable->left != NIL) ? removable->left : removable->right;

	}
	else {
		Node* succesor = removable->right;
		while (succesor->left != NIL){
			succesor = succesor->left;
		}
		removable->key = succesor->key;
		remove(succesor, succesor->key);
		return;
	}

	//Connect child to parent
	if (child != NIL) {
		child->parent = removable->parent;
	}

	if (removable->parent == NIL) {
		root = child;
	}
	else if (removable == removable->parent->left) {
		removable->parent->left = child;
	}
	else {
		removable->parent->right = child;
	}

	updateSizeAfterDelete(removable->parent);
	if (removable->color == false) {
		fixDelete(child);
	}

	delete removable;
}




