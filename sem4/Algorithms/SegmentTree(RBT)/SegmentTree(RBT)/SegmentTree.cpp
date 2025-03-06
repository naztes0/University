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
		if (grandparent == NIL) break; // Якщо ми на рівні кореня, то ніяких обертів не робимо

		// Визначаємо, чи наш батько є лівим або правим сином
		bool isParentLeft = (node->parent == grandparent->leftChild);
		Node* uncle = isParentLeft ? grandparent->rightChild : grandparent->leftChild;

		// Випадок 1: Дядько червоний
		if (uncle != NIL && uncle->color == false) {
			node->parent->color = true;
			uncle->color = true;
			grandparent->color = false;
			node = grandparent;
			continue;
		}

		// **Головна перевірка для вкладених відрізків**
		if (node->parent->left <= node->left && node->right <= node->parent->right) {
			break; // **Вкладені відрізки не повинні спричиняти поворот!**
		}

		// Випадок 2: Дядько чорний, а ми з «неправильного» боку
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

	// Повне перекриття
	if (l <= node->left && node->right <= r) {
		node->sum += (node->right - node->left + 1) * val;
		return;
	}

	// Часткове перекриття
	if (node->leftChild != NIL)
		updatePrivate(node->leftChild, l, r, val);
	if (node->rightChild != NIL)
		updatePrivate(node->rightChild, l, r, val);

	// Оновлення суми вузла
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
////Methods for removing nodes
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
	Node* sibling;
	while (node != root && node->color == true) {  // true = BLACK
		if (node == node->parent->leftChild) {
			sibling = node->parent->rightChild;

			// Випадок 1: червоний брат
			if (sibling->color == false) {  // false = RED
				sibling->color = true;      // BLACK
				node->parent->color = false; // RED
				leftRotate(node->parent);
				sibling = node->parent->rightChild;
			}

			// Випадок 2: обидва дочірні вузли брата чорні
			if (sibling->leftChild->color == true && sibling->rightChild->color == true) {
				sibling->color = false;  // RED
				node = node->parent;
			}
			else {
				// Випадок 3: правий дочірній вузол брата чорний
				if (sibling->rightChild->color == true) {
					sibling->leftChild->color = true;  // BLACK
					sibling->color = false;  // RED
					rightRotate(sibling);
					sibling = node->parent->rightChild;
				}

				// Випадок 4: лівий дочірній вузол брата чорний
				sibling->color = node->parent->color;
				node->parent->color = true;  // BLACK
				sibling->rightChild->color = true;  // BLACK
				leftRotate(node->parent);
				node = root;
			}
		}
		else {  // Дзеркальний випадок
			sibling = node->parent->leftChild;

			// Випадок 1: червоний брат
			if (sibling->color == false) {  // false = RED
				sibling->color = true;      // BLACK
				node->parent->color = false; // RED
				rightRotate(node->parent);
				sibling = node->parent->leftChild;
			}

			// Випадок 2: обидва дочірні вузли брата чорні
			if (sibling->leftChild->color == true && sibling->rightChild->color == true) {
				sibling->color = false;  // RED
				node = node->parent;
			}
			else {
				// Випадок 3: лівий дочірній вузол брата чорний
				if (sibling->leftChild->color == true) {
					sibling->rightChild->color = true;  // BLACK
					sibling->color = false;  // RED
					leftRotate(sibling);
					sibling = node->parent->leftChild;
				}

				// Випадок 4: правий дочірній вузол брата чорний
				sibling->color = node->parent->color;
				node->parent->color = true;  // BLACK
				sibling->leftChild->color = true;  // BLACK
				rightRotate(node->parent);
				node = root;
			}
		}
	}
	node->color = true;  // BLACK
}

//// Оновлення суми в дереві відрізків
//void SegmentTree::updateSum(Node* node) {
//	if (node == NIL) return;
//
//	// Оновлення суми для поточного вузла на основі його дочірніх вузлів
//	if (node->leftChild != NIL && node->rightChild != NIL) {
//		node->sum = node->leftChild->sum + node->rightChild->sum;
//	}
//	else if (node->leftChild != NIL) {
//		node->sum = node->leftChild->sum;
//	}
//	else if (node->rightChild != NIL) {
//		node->sum = node->rightChild->sum;
//	}
//	else {
//		node->sum = node->value;  // Листовий вузол
//	}
//
//	// Рекурсивно оновлюємо суми вгору по дереву
//	if (node->parent != NIL) {
//		updateSum(node->parent);
//	}
//}

void SegmentTree::removeNode(Node* node, int l, int r) {
	if (node == NIL) return;

	// Перевірка перетину діапазонів
	if (r < node->left || l > node->right) {
		// Діапазони не перетинаються, шукаємо далі
		if (l < node->left) {
			removeNode(node->leftChild, l, r);
		}
		else {
			removeNode(node->rightChild, l, r);
		}
		return;
	}

	// Якщо діапазон вузла повністю входить в діапазон видалення
	if (l <= node->left && r >= node->right) {
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
			y = minimum(node->rightChild);  // Знаходимо наступника
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

			// Оновлюємо діапазон та інші дані вузла
			y->left = node->left;
			y->right = node->right;
		}

		// Оновлюємо суми у дереві
		Node* updateNode = (y != NIL) ? y : x->parent;
		if (updateNode != NIL) {
			updateSum(updateNode);
		}

		if (yOriginalColor == true) {  // Якщо видалявся чорний вузол (true = BLACK)
			fixDelete(x);
		}

		delete node;
	}
	else {
		// Діапазон вузла частково перетинається з діапазоном видалення
		// Рекурсивно видаляємо з обох піддерев, якщо потрібно
		if (l < node->left || (node->left <= l && l <= node->right)) {
			removeNode(node->leftChild, l, r);
		}
		if (r > node->right || (node->left <= r && r <= node->right)) {
			removeNode(node->rightChild, l, r);
		}

		// Оновлюємо суму поточного вузла після видалення з піддерев
		updateSum(node);
	}
}

void SegmentTree::printTreePriv(Node* node, int depth) {
	// Перевірка на порожнє або некоректне дерево
	if (node == NIL || node == nullptr) {
		return;
	}

	const int INDENT_SIZE = 4; // Розмір відступу (кількість пробілів)

	// Друк правого піддерева (верхня частина)
	if (node->rightChild != NIL) {
		printTreePriv(node->rightChild, depth + 1);
	}

	// Друк відступів для ієрархічного виведення
	std::cout << std::string(depth * INDENT_SIZE, ' ');

	// Друк інформації про поточний вузол
	std::string colorStr = node->color ? "Black" : "Red";
	std::cout << "[" << node->left << ", " << node->right
		<< "] sum: " << node->sum << " (" << colorStr << ")\n";

	// Друк лівого піддерева (нижня частина)
	if (node->leftChild != NIL) {
		printTreePriv(node->leftChild, depth + 1);
	}
}


////////////PUBLIC//////////////////////
void SegmentTree::insert(int l, int r, int sum) {
	// Створюємо новий вузол
	Node* newNode = new Node(l, r, sum);
	newNode->leftChild = NIL;
	newNode->rightChild = NIL;
	newNode->color = false;  // Червоний колір (false у вашому випадку)
	newNode->parent = NIL;

	// Якщо дерево порожнє
	if (root == NIL) {
		root = newNode;
		newNode->color = true;  // Корінь завжди чорний
		return;
	}

	// Знаходимо місце для вставки з урахуванням вкладеності відрізків
	Node* parent = NIL;
	Node* current = root;
	while (current != NIL) {
		parent = current;

		// Перевіряємо, чи вкладений новий відрізок у поточний
		if (l >= current->left && r <= current->right) {
			// Відрізок вкладений, йдемо вліво або вправо в залежності
			// від того, чи перетинає новий відрізок центр поточного
			int mid = (current->left + current->right) / 2;
			if (r <= mid) {
				current = current->leftChild;
			}
			else if (l > mid) {
				current = current->rightChild;
			}
			else {
				// Якщо відрізок перетинає центр, вибираємо напрямок
				// на основі порівняння меж
				if (l < current->left || (l == current->left && r < current->right)) {
					current = current->leftChild;
				}
				else {
					current = current->rightChild;
				}
			}
		}
		else {
			// Якщо відрізки не вкладені, використовуємо лексикографічне порівняння
			if (l < current->left || (l == current->left && r < current->right)) {
				current = current->leftChild;
			}
			else {
				current = current->rightChild;
			}
		}
	}

	// Вставляємо вузол
	newNode->parent = parent;

	// Визначаємо, лівим чи правим сином буде новий вузол
	int mid = (parent->left + parent->right) / 2;
	if (parent->left <= l && r <= parent->right) {
		// Відрізок вкладений
		if (r <= mid) {
			parent->leftChild = newNode;
		}
		else if (l > mid) {
			parent->rightChild = newNode;
		}
		else {
			// Перетинає центр, використовуємо лексикографічне порівняння
			if (l < parent->left || (l == parent->left && r < parent->right)) {
				parent->leftChild = newNode;
			}
			else {
				parent->rightChild = newNode;
			}
		}
	}
	else {
		// Не вкладений, використовуємо лексикографічне порівняння
		if (l < parent->left || (l == parent->left && r < parent->right)) {
			parent->leftChild = newNode;
		}
		else {
			parent->rightChild = newNode;
		}
	}
	std::cout << "Node range: [" << l << ";" << r << "]" << "with parent" << "[" <<newNode->parent->left<<";"<<newNode->parent->right<<"]"<<"\n";
	// Виправляємо порушення властивостей червоно-чорного дерева
	fixInsert(newNode);
	updateSum(newNode);
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