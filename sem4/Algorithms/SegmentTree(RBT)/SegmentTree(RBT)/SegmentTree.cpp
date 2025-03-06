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
	while (node->parent != NIL && node->parent->color == false) {

		Node* grandparent = node->parent->parent;
		if (grandparent == NIL) break; // ���� �� �� ��� ������, �� ����� ������ �� ������

		// ���������, �� ��� ������ � ���� ��� ������ �����
		bool isParentLeft = (node->parent == grandparent->leftChild);
		Node* uncle = isParentLeft ? grandparent->rightChild : grandparent->leftChild;

		// ������� 1: ������ ��������
		if (uncle != NIL && uncle->color == false) {
			node->parent->color = true;
			uncle->color = true;
			grandparent->color = false;
			node = grandparent;
			continue;
		}

		// **������� �������� ��� ��������� ������**
		if (node->parent->left <= node->left && node->right <= node->parent->right) {
			break; // **������� ������ �� ������ ���������� �������!**
		}

		// ������� 2: ������ ������, � �� � �������������� ����
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



void SegmentTree:: updatePrivate(Node* node, int l, int r, int val) {
	if (node == NIL) return;

	if (r < node->left || l > node->right) {
		return;
	}

	// ����� ����������
	if (l <= node->left && node->right <= r) {
		node->sum += (node->right - node->left + 1) * val;
		return;
	}

	// �������� ����������
	if (node->leftChild != NIL)
		updatePrivate(node->leftChild, l, r, val);
	if (node->rightChild != NIL)
		updatePrivate(node->rightChild, l, r, val);

	// ��������� ���� �����
	node->sum = (node->leftChild ? node->leftChild->sum : 0) +
		(node->rightChild ? node->rightChild->sum : 0);
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
	else if (l > node->right) {
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

		if (yOriginalColor == true) { // ���� ��������� ������ �����
			fixDelete(x);
		}
	}
}


void SegmentTree::printTreePriv(Node* node, int depth) {
	// �������� �� ������ ��� ���������� ������
	if (node == NIL || node == nullptr) {
		return;
	}

	const int INDENT_SIZE = 4; // ����� ������� (������� ������)

	// ���� ������� �������� (������ �������)
	if (node->rightChild != NIL) {
		printTreePriv(node->rightChild, depth + 1);
	}

	// ���� ������� ��� ������������ ���������
	std::cout << std::string(depth * INDENT_SIZE, ' ');

	// ���� ���������� ��� �������� �����
	std::string colorStr = node->color ? "Black" : "Red";
	std::cout << "[" << node->left << ", " << node->right
		<< "] sum: " << node->sum << " (" << colorStr << ")\n";

	// ���� ����� �������� (����� �������)
	if (node->leftChild != NIL) {
		printTreePriv(node->leftChild, depth + 1);
	}
}


////////////PUBLIC//////////////////////
void SegmentTree::insert(int l, int r, int sum) {
	// ��������� ����� �����
	Node* newNode = new Node(l, r, sum);
	newNode->leftChild = NIL;
	newNode->rightChild = NIL;
	newNode->color = false;  // �������� ���� (false � ������ �������)
	newNode->parent = NIL;

	// ���� ������ ������
	if (root == NIL) {
		root = newNode;
		newNode->color = true;  // ����� ������ ������
		return;
	}

	// ��������� ���� ��� ������� � ����������� ���������� ������
	Node* parent = NIL;
	Node* current = root;
	while (current != NIL) {
		parent = current;

		// ����������, �� ��������� ����� ������ � ��������
		if (l >= current->left && r <= current->right) {
			// ³����� ���������, ����� ���� ��� ������ � ���������
			// �� ����, �� �������� ����� ������ ����� ���������
			int mid = (current->left + current->right) / 2;
			if (r <= mid) {
				current = current->leftChild;
			}
			else if (l > mid) {
				current = current->rightChild;
			}
			else {
				// ���� ������ �������� �����, �������� ��������
				// �� ����� ��������� ���
				if (l < current->left || (l == current->left && r < current->right)) {
					current = current->leftChild;
				}
				else {
					current = current->rightChild;
				}
			}
		}
		else {
			// ���� ������ �� �������, ������������� ��������������� ���������
			if (l < current->left || (l == current->left && r < current->right)) {
				current = current->leftChild;
			}
			else {
				current = current->rightChild;
			}
		}
	}

	// ���������� �����
	newNode->parent = parent;

	// ���������, ���� �� ������ ����� ���� ����� �����
	int mid = (parent->left + parent->right) / 2;
	if (parent->left <= l && r <= parent->right) {
		// ³����� ���������
		if (r <= mid) {
			parent->leftChild = newNode;
		}
		else if (l > mid) {
			parent->rightChild = newNode;
		}
		else {
			// �������� �����, ������������� ��������������� ���������
			if (l < parent->left || (l == parent->left && r < parent->right)) {
				parent->leftChild = newNode;
			}
			else {
				parent->rightChild = newNode;
			}
		}
	}
	else {
		// �� ���������, ������������� ��������������� ���������
		if (l < parent->left || (l == parent->left && r < parent->right)) {
			parent->leftChild = newNode;
		}
		else {
			parent->rightChild = newNode;
		}
	}
	std::cout << "Node range: [" << l << ";" << r << "]" << "with parent" << "[" <<newNode->parent->left<<";"<<newNode->parent->right<<"]"<<"\n";
	// ����������� ��������� ������������ �������-������� ������
	fixInsert(newNode);
}

int SegmentTree::query(int l, int r) {
	return queryPrivate(root, l, r);
}

void SegmentTree::update(int l, int r, int newValue) {
	updatePrivate(root, l, r, newValue);
}

void SegmentTree::remove(int l, int r) {
	removeNode(root, l, r);
}

void SegmentTree::printTree() {
	printTreePriv(root, 0);
}