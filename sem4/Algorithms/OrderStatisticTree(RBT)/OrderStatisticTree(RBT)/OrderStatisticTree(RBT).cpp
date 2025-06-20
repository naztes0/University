#include "OrderStatisticTree(RBT).h"

//////PRIVATE/////////
void OST::updateSize(Node *node)
{
	if (node == NIL)
		return;
	node->size = ((node->left != NIL) ? node->left->size : 0) +
				 ((node->right != NIL) ? node->right->size : 0) + 1;
}

void OST::leftRotate(Node *x)
{
	Node *y = x->right; // new Node
	x->right = y->left;
	if (y->left != NIL)
	{
		y->left->parent = x;
	}
	y->parent = x->parent;
	if (x->parent == NIL)
	{
		root = y;
	}
	else if (x == x->parent->left)
	{
		x->parent->left = y;
	}
	else
	{
		x->parent->right = y;
	}

	y->left = x;
	x->parent = y;

	updateSize(x);
	updateSize(y);
}

void OST::rightRotate(Node *x)
{
	Node *y = x->left;
	x->left = y->right;

	if (y->right != NIL)
	{
		y->right->parent = x;
	}

	y->parent = x->parent;

	if (x->parent == NIL)
	{
		root = y;
	}
	else if (x == x->parent->left)
	{
		x->parent->left = y;
	}
	else
	{
		x->parent->right = y;
	}

	y->right = x;
	x->parent = y;

	updateSize(x);
	updateSize(y);
}

void OST::updateSizeAfterDelete(Node *node)
{
	while (node != NIL)
	{
		node->size = node->left->size + node->right->size + 1;
		node = node->parent;
	}
}
void OST::fixInsert(Node *node)
{
	while (node != root && node->parent->color == false)
	{ // till parent red
		if (node->parent == node->parent->parent->left)
		{ // parent- left son of grand
			Node *uncle = node->parent->parent->right;

			if (uncle->color == false)
			{										 // uncle is red
				node->parent->color = true;			 // parent->black
				uncle->color = true;				 // uncle->black
				node->parent->parent->color = false; // graand->red
				node = node->parent->parent;		 // move to grand
			}
			else
			{ // uncle is black
				if (node == node->parent->right)
				{ // node is right son
					node = node->parent;
					leftRotate(node);
				}
				// node is left son
				node->parent->color = true;			 // parent->black
				node->parent->parent->color = false; // grand->red
				rightRotate(node->parent->parent);
			}
		}
		else
		{ // symetrical case
			Node *uncle = node->parent->parent->left;
			if (uncle->color == false)
			{
				node->parent->color = true;
				uncle->color = true;
				node->parent->parent->color = false;
				node = node->parent->parent;
			}
			else
			{
				if (node == node->parent->left)
				{
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

void OST::fixDelete(Node *node)
{
	while (node != root && node->color == true)
	{
		if (node == node->parent->left)
		{ // node - left son
			Node *sibling = node->parent->right;

			if (sibling->color == false)
			{								 // Case 1: sibling is red
				sibling->color = true;		 // sibling ->balck
				node->parent->color = false; // parent -> red
				leftRotate(node->parent);
				sibling = node->parent->right;
			}

			if (sibling->left->color == true && sibling->right->color == true)
			{ // sibling`s children are black
				sibling->color = false;
				node = node->parent;
			}
			else
			{
				if (sibling->right->color == true)
				{ // right son of sibling is black
					sibling->left->color = true;
					sibling->color = false;
					rightRotate(sibling);
					sibling = node->parent->right;
				}

				// right son of sibling is red
				sibling->color = node->parent->color;
				node->parent->color = true;
				sibling->right->color = true;
				leftRotate(node->parent);
				node = root; // Виходимо з циклу
			}
		}
		else
		{ // symetrical case
			Node *sibling = node->parent->left;

			if (sibling->color == false)
			{
				sibling->color = true;
				node->parent->color = false;
				rightRotate(node->parent);
				sibling = node->parent->left;
			}

			if (sibling->right->color == true && sibling->left->color == true)
			{
				sibling->color = false;
				node = node->parent;
			}
			else
			{
				if (sibling->left->color == true)
				{
					sibling->right->color = true;
					sibling->color = false;
					leftRotate(sibling);
					sibling = node->parent->left;
				}

				sibling->color = node->parent->color;
				node->parent->color = true;
				sibling->left->color = true;
				rightRotate(node->parent);
				node = root;
			}
		}
	}
	node->color = true; // root always black
}

void OST::callPrintTree(Node *node, int depth)
{
	if (node == NIL)
		return;

	callPrintTree(node->right, depth + 1); // right braanch

	for (int i = 0; i < depth; i++)
	{
		std::cout << "\t";
	}
	std::cout << node->key << " (" << node->size << ")"
			  << (node->color ? " (B)" : " (R)") << std::endl; // black or red node

	callPrintTree(node->left, depth + 1); // left branch
}

OST::Node *OST::findByOrderInternal(Node *node, int k)
{
	if (node == NIL || k <= 0 || k > node->size)
		return NIL;

	int leftsize = (node->left != NIL) ? node->left->size : 0;

	if (leftsize + 1 == k)
	{
		return node;
	}
	else if (k <= leftsize)
	{
		return findByOrderInternal(node->left, k);
	}
	else
	{
		return findByOrderInternal(node->right, k - (leftsize + 1));
	}
}

int OST::orderOfKeyInternal(Node *node, int x)
{
	if (node == NIL)
		return 0;

	if (x == node->key)
		return node->left->size;

	if (x < node->key)
		return orderOfKeyInternal(node->left, x);
	else
	{
		return node->left->size + 1 + orderOfKeyInternal(node->right, x);
	}
}
///////PUBLIC//////

void OST::insert(int key)
{
	Node *newNode = new Node(key);
	newNode->left = NIL;
	newNode->right = NIL;
	newNode->color = false; // new node always red

	Node *parent = NIL;
	Node *current = root;
	while (current != NIL)
	{
		parent = current;
		parent->size++;
		if (key < current->key)
		{
			current = current->left;
		}
		else
		{
			current = current->right;
		}
	}

	newNode->parent = parent;
	if (parent == NIL)
		root = newNode;
	else if (key < parent->key)
		parent->left = newNode;
	else
		parent->right = newNode;

	fixInsert(newNode);
}

int OST::findByOrder(int k)
{
	Node *result = findByOrderInternal(root, k);
	if (result == NIL)
		std::cout << "Res in find by order = NIL";
	return result->key;
}
int OST::orderOfKey(int x)
{
	return orderOfKeyInternal(root, x);
}

void OST::removeNode(Node *node, int key)
{
	if (node == NIL)
		return; // Ключа немає в дереві

	if (key < node->key)
	{
		removeNode(node->left, key);
		return;
	}
	if (key > node->key)
	{
		removeNode(node->right, key);
		return;
	}

	// Знайшли вузол для видалення
	Node *removable = node;
	Node *child; // Вузол, який стане на місце видаленого

	if (removable->left == NIL || removable->right == NIL)
	{
		// Випадок 1 та 2: 0 або 1 дитина
		child = (removable->left != NIL) ? removable->left : removable->right;
	}
	else
	{
		// Випадок 3: 2 дитини → знаходимо наступника (мінімальний у правому піддереві)
		Node *successor = removable->right;
		while (successor->left != NIL)
		{
			successor = successor->left;
		}
		// Копіюємо значення наступника та видаляємо його
		removable->key = successor->key;
		removeNode(successor, successor->key);
		return;
	}

	// Підключаємо child до батька
	if (child != NIL)
	{
		child->parent = removable->parent;
	}

	if (removable->parent == NIL)
	{
		root = child;
	}
	else if (removable == removable->parent->left)
	{
		removable->parent->left = child;
	}
	else
	{
		removable->parent->right = child;
	}

	// Оновлюємо size після видалення
	updateSizeAfterDelete(removable->parent);

	// Якщо видалений вузол був чорним, виправляємо баланс
	if (removable->color == false)
	{
		fixDelete(child);
	}

	delete removable; // Звільняємо пам'ять
}

void OST::remove(int key)
{
	removeNode(root, key);
}

void OST::printTree()
{
	if (root == NIL)
	{
		std::cout << "The tree is empty";
		return;
	}
	callPrintTree(root, 0);
	std::cout << "\n\n\n";
}
