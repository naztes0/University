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

void SegmentTree::updateSum(Node* node) {
	while (node != NIL && node != nullptr) {
		int leftSum = (node->leftChild != NIL) ? node->leftChild->sum : 0;
		int rightSum = (node->rightChild != NIL) ? node->rightChild->sum : 0;

		if (node->leftChild == NIL && node->rightChild == NIL) {
		}
		else {
			node->sum = leftSum + rightSum;
		}
		node = node->parent;
	}
}
void SegmentTree::fixInsert(Node* node) {
	while (node->parent != NIL && node->parent->color == false) {

		Node* grandparent = node->parent->parent;
		if (grandparent == NIL) break; // If its the position of the root, then rotation won`t be done

		// Define if parent is right or left son 
		bool isParentLeft = (node->parent == grandparent->leftChild);
		Node* uncle = isParentLeft ? grandparent->rightChild : grandparent->leftChild;

		// Case 1: Uncle is RED(false)
		if (uncle != NIL && uncle->color == false) {
			node->parent->color = true;
			uncle->color = true;
			grandparent->color = false;
			node = grandparent;
			continue;
		}

		// Main node check of not strict belonging to the parent range
		if (node->parent->left <= node->left && node->right <= node->parent->right) {
			break; // No rotation for nested ranges
		}

		//   Case 2: Uncle is BLACK(true), but we arent on the "correct" side
		if (isParentLeft) {
			if (node == node->parent->rightChild) {
				node = node->parent;
				leftRotate(node);
			}
			node->parent->color = true;
			grandparent->color = false;
			rightRotate(grandparent);
		}
		else {
			if (node == node->parent->leftChild) {
				node = node->parent;
				rightRotate(node);
			}
			node->parent->color = true;
			grandparent->color = false;
			leftRotate(grandparent);
		}
	}
	root->color = true;
}



int SegmentTree::queryPrivate(Node* node, int l, int r) {
	if (node == NIL) return 0;

	//full overlap
	if (l <= node->left && node->right <= r) {
		return node->sum;
	}
	//No overlap
	if (r < node->left || l > node->right) {
		return 0;
	}

	// Partial overlap
	int totalSum = 0;
	if (node->leftChild != NIL)
		totalSum += queryPrivate(node->leftChild, l, r);
	if (node->rightChild != NIL)
		totalSum += queryPrivate(node->rightChild, l, r);

	return totalSum;
}

void SegmentTree::printTreePriv(Node* node, int depth) {
	// Check for empty or not correct tree
	if (node == NIL || node == nullptr) {
		return;
	}

	const int INDENT_SIZE = 4; // Size if indentation(number of spaces)

	// Print of the right subtree
	if (node->rightChild != NIL) {
		printTreePriv(node->rightChild, depth + 1);
	}

	// Print of indentations for hierarchic output
	std::cout << std::string(depth * INDENT_SIZE, ' ');

	// Print of current node inf
	std::string colorStr = node->color ? "Black" : "Red";
	std::cout << "[" << node->left << ", " << node->right
		<< "] sum: " << node->sum << " (" << colorStr << ")\n";

	// Print of left subtree 
	if (node->leftChild != NIL) {
		printTreePriv(node->leftChild, depth + 1);
	}
}





// Putting of v subtree at hte place of u subtree
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

// Find the min value node in subtree
SegmentTree::Node* SegmentTree::minimum(Node* node) {
	while (node->leftChild != NIL) {
		node = node->leftChild;
	}
	return node;
}


// Correction of RBT properties
void SegmentTree::fixDelete(Node* x) {
	while (x != root && x->color == true) {
		if (x == x->parent->leftChild) {
			Node* w = x->parent->rightChild;
			// Case 1: x brother is RED
			if (w->color == false) {
				w->color = true;
				x->parent->color = false;
				leftRotate(x->parent);
				w = x->parent->rightChild;
			}

			// Case 2: both x sons are black
			if (w->leftChild->color == true && w->rightChild->color == true) {
				w->color = false;
				x = x->parent;
			}
			else {
				// Case 3:brother`s left son is black and right one - red
				if (w->rightChild->color == true) {
					w->leftChild->color = true;
					w->color = false;
					rightRotate(w);
					w = x->parent->rightChild;
				}

				// Case 4:brother`s left son is red
				w->color = x->parent->color;
				x->parent->color = true;
				w->rightChild->color = true;
				leftRotate(x->parent);
				x = root;
			}
		}
		else {
			// Symetrical case for the right son
			Node* w = x->parent->leftChild;
			// Case1: brother x is red 
			if (w->color == false) {
				w->color = true;
				x->parent->color = false;
				rightRotate(x->parent);
				w = x->parent->leftChild;
			}

			// Case 2: both brother`s son are red
			if (w->rightChild->color == true && w->leftChild->color == true) {
				w->color = false;
				x = x->parent;
			}
			else {
				// Case 3: x-brother`s right son is black and left one - red
				if (w->leftChild->color == true) {
					w->rightChild->color = true;
					w->color = false;
					leftRotate(w);
					w = x->parent->leftChild;
				}

				//Case 4: x-brother`s right son - red
				w->color = x->parent->color;
				x->parent->color = true;
				w->leftChild->color = true;
				rightRotate(x->parent);
				x = root;
			}
		}
	}
	x->color = true; // Node x becoming black
}

// Deleting of node from the range including his children
void SegmentTree::removeNode(Node* node, int l, int r) {
	if (node == NIL) return;

	// Check for ranges intersection
	if (r < node->left || l > node->right) {
		//No intersactions => searching
		if (l < node->left) {
			removeNode(node->leftChild, l, r);
		}
		else {
			removeNode(node->rightChild, l, r);
		}
		return;
	}

	//If the node range fully included to the range of deleting
	if (l <= node->left && r >= node->right) {
		//Firstly, recusive deletion of children included to the deleting range
		if (node->leftChild != NIL) {
			removeNode(node->leftChild, l, r);
		}
		if (node->rightChild != NIL) {
			removeNode(node->rightChild, l, r);
		}

		// Now deletin the node itself
		Node* y = node;
		Node* x;
		bool yOriginalColor = y->color;

		if (node->leftChild == NIL) {
			// Case 1: no left son
			x = node->rightChild;
			transplant(node, node->rightChild);
		}
		else if (node->rightChild == NIL) {
			//Case 2: no right son
			x = node->leftChild;
			transplant(node, node->leftChild);
		}
		else {
			// Case 3: has both son 
			y = minimum(node->rightChild);  // Search for heir
			yOriginalColor = y->color;
			x = y->rightChild;

			if (y->parent == node) {
				// Heir is right node`s son
				x->parent = y;
			}
			else {
				// Heir is deeper in the right sunbtree
				transplant(y, y->rightChild);
				y->rightChild = node->rightChild;
				y->rightChild->parent = y;
			}

			// Replacing the node wit hheir
			transplant(node, y);
			y->leftChild = node->leftChild;
			y->leftChild->parent = y;
			y->color = node->color;

			// Copy of data
			y->left = node->left;
			y->right = node->right;
		}

		// Defining of the node for sum calc
		Node* updateNode = NIL;
		if (y != NIL && y != node) {
			updateNode = y;
		}
		else if (x != NIL) {
			updateNode = x->parent;
		}

		// sum update
		if (updateNode != NIL) {
			updateSum(updateNode);
		}

		// RBT prop fix
		if (yOriginalColor == true) { 
			fixDelete(x);
		}

		delete node;
	}
	else {
	
		//Node range partial intersects with deletion range 

		//Recursive deletion from the left subtree, if its intersects with the deletion range 
		if (l <= node->right && node->leftChild != NIL) {
			removeNode(node->leftChild, l, r);
		}

		//Recursive deletion from the left subtree, if its intersects with the deletion range 
		if (r >= node->left && node->rightChild != NIL) {
			removeNode(node->rightChild, l, r);
		}

		//Updating current node sum after the deletion
		updateSum(node);
	}
}


////////////PUBLIC//////////////////////


void SegmentTree::insert(int l, int r, int sum) {
	// New node creation
	Node* newNode = new Node(l, r, sum);
	newNode->leftChild = NIL;
	newNode->rightChild = NIL;
	newNode->color = false;  // New node has RED color (false)
	newNode->parent = NIL;

	// If the tree is empty
	if (root == NIL) {
		root = newNode;
		newNode->color = true;  // Root always black
		return;
	}

	// Finding the place for insert 
	Node* parent = NIL;
	Node* current = root;
	while (current != NIL) {
		parent = current;

		// Check if new range is included
		if (l >= current->left && r <= current->right) {
			
			//Included => go on the right/left. Depends on the intersaction of mid of the range
			int mid = (current->left + current->right) / 2;
			if (r <= mid) {
				current = current->leftChild;
			}
			else if (l > mid) {
				current = current->rightChild;
			}
			else {
				//If intersects the mid => choose the direction bases on the limit comparison
				if (l < current->left || (l == current->left && r < current->right)) {
					current = current->leftChild;
				}
				else {
					current = current->rightChild;
				}
			}
		}
		else {
			// If the range wasn`t included => use lexicographic comparison
			if (l < current->left || (l == current->left && r < current->right)) {
				current = current->leftChild;
			}
			else {
				current = current->rightChild;
			}
		}
	}

	// Insert the node
	newNode->parent = parent;

	// Define which side node will be
	int mid = (parent->left + parent->right) / 2;
	if (parent->left <= l && r <= parent->right) {
		//Range included
		if (r <= mid) {
			parent->leftChild = newNode;
		}
		else if (l > mid) {
			parent->rightChild = newNode;
		}
		else {
			// intersects mid =>lexicographic comparison
			if (l < parent->left || (l == parent->left && r < parent->right)) {
				parent->leftChild = newNode;
			}
			else {
				parent->rightChild = newNode;
			}
		}
	}
	else {
		// Not included, use lexicographic compariosn
		if (l < parent->left || (l == parent->left && r < parent->right)) {
			parent->leftChild = newNode;
		}
		else {
			parent->rightChild = newNode;
		}
	}
	std::cout << "Node range: [" << l << ";" << r << "]" << "with parent" << "[" <<newNode->parent->left<<";"<<newNode->parent->right<<"]"<<"\n";
	// Correct the violation of properties and update the sum
	fixInsert(newNode);
	updateSum(newNode);
}

//Public function callers
int SegmentTree::query(int l, int r) {
	return queryPrivate(root, l, r);
}


void SegmentTree::remove(int l, int r) {
	removeNode(root, l, r);
}

void SegmentTree::printTree() {
	printTreePriv(root, 0);
}