//Non-recursive ways of circuting trees using bool indicator +different useful func for trees  
#include<iostream>
#include<stack>
#include<queue>
using namespace std;

struct TreeNode {
	int data;
	TreeNode* left;
	TreeNode* right;
};
//Для використання у обходах з індикаторами (bool)
struct TreeNodeIndicator {
	TreeNode* node;
	bool leftDone;
};

//Створеня нового вузла
TreeNode* createTreeNode(int data) {
	TreeNode* newNode = new TreeNode();
	newNode->data = data;
	newNode->left = NULL;
	newNode->right = NULL;
	return newNode;

}

//Для розподілення у праву та ліву вітку дерева
void insertNode(TreeNode*& root, int data) {
	if (!root) {
		root = createTreeNode(data);
		return;
	}
	if (data < root->data) insertNode(root->left, data);
	else insertNode(root->right, data);
}
//Для створення дерева з масиву
TreeNode* createTreeFromArray(int arr[], int size) {
	TreeNode* root = NULL;
	for (int i = 0; i < size; i++) {
		insertNode(root, arr[i]);
	}
	return root;
}
///
///
/// 
/// 
//Не рекурсивна функція для обходу в ПРЯМОМУ порядку
void strOrder(TreeNode* root) {
	if (!root)return;
	
	stack<TreeNode*>s;
	s.push(root);

	while (!s.empty()) {
		TreeNode* current = s.top();
		s.pop();
		cout << current->data << " ";
		
		if (current->right) s.push(current->right);
		if (current->left) s.push(current->left);
		
	}
}
//Не рекурсивна функція для обходу в ОБЕРНЕНОМУ порядку
void obernOrder(TreeNode* root) {
	stack<TreeNode*> s;
	while (root || !s.empty()) {
		if (root) {
			if (root->right) s.push(root->right);
			s.push(root);
			root = root->left;
		}
		else {
			root = s.top(); s.pop();
			if (!s.empty() && root->right == s.top()) {
				s.pop();
				s.push(root);
				root = root->right;
			}
			else {
				cout << root->data << " ";
				root = nullptr;
			}
		}
	}
}
//Нерекурсивна функція для обходу в СИМЕТРИЧНОМУ порядку (з викорситанням bool)
void symetricOrder(TreeNode* root) {
	if (root == nullptr)
		return ;

	stack<TreeNodeIndicator> s;
	s.push({ root,false });
	

	while (!s.empty()) {
		TreeNodeIndicator current = s.top();
		s.pop();

		if (current.leftDone) {
			cout << current.node->data << " ";
		}
		else {
			if (current.node->right)
				s.push({ current.node->right, false });
			s.push({ current.node, true });
			if (current.node->left)
				s.push({ current.node->left, false });
		}
	}
}
//Нерекурсивна функція для обходу в ПОРІВНЕВОМУ порядку
void porivnOrder(TreeNode* root) {
	if (!root) return;

	queue<TreeNode*>q;
	q.push(root);
	while (!q.empty()) {
		TreeNode* curr = q.front();
		q.pop();
		cout << curr->data << " ";

		if (curr->left) q.push(curr->left);
		if (curr->right) q.push(curr->right);
	}
}
//Підрахунок кількості вузлів з обходом у СИМЕТРИЧНОМУ порядку
int symetricOrderCounter(TreeNode* root) {
	if (root == nullptr)
		return NULL;

	stack<TreeNodeIndicator> s;
	s.push({ root,false });
	int counter = 0;

	while (!s.empty()) {
		TreeNodeIndicator current = s.top();
		s.pop();

		if (current.leftDone) {
			counter++;
		}
		else {
			if (current.node->right)
				s.push({ current.node->right, false });
			s.push({ current.node, true });
			if (current.node->left)
				s.push({ current.node->left, false });
		}
	}
	return counter;
}
//Знаходження висоти дерева з обходом у СИМЕТРИЧНОМУ порядку
int symetricOrderHeight(TreeNode* root) {
	if (root == nullptr)
		return NULL;

	stack<TreeNodeIndicator> s;
	s.push({ root,false });
	int maxHeight = 0;
	int currentHeight = 0;


	while (!s.empty()) {
		TreeNodeIndicator current = s.top();
		s.pop();

		if (!current.leftDone) {
			currentHeight++;
			if (current.node->right)
				s.push({ current.node->right, false });
			s.push({ current.node, true });
			
			if (current.node->left)
				s.push({ current.node->left, false });
			
		}
		else {
			maxHeight = max(maxHeight, currentHeight);
			currentHeight--;
		}
	}
	return maxHeight;
}
//Кількість лситків дерева  з обходом у СИМЕТРИЧНОМУ порядку
int symetricOrderLeavesNumber(TreeNode* root) {
	if (root == nullptr)
		return NULL;

	stack<TreeNodeIndicator> s;
	s.push({ root,false });
	int leavesNumb = 0;


	while (!s.empty()) {
		TreeNodeIndicator current = s.top();
		s.pop();
		

		if (!current.leftDone) {
			if (current.node->right)
				s.push({ current.node->right, false });
			s.push({ current.node, true });

			if (current.node->left)
				s.push({ current.node->left, false });

		}
		else {
			if (current.node->left == NULL && current.node->right == NULL)
				leavesNumb++;
		}
		
	}
	return leavesNumb;
}
//Максимальний та мінімальний елемент дерева  з обходом у СИМЕТРИЧНОМУ порядку
void symetricOrderMaxAndMinData(TreeNode* root) {
	if (root == nullptr)
		return ;

	stack<TreeNodeIndicator> s;
	s.push({ root,false });
	int maxData = root->data;
	int minData = root->data;


	while (!s.empty()) {
		TreeNodeIndicator current = s.top();
		s.pop();

		if (current.leftDone) {
			maxData=max(maxData,current.node->data);
			minData = min(minData, current.node->data);
		}
		else {
			if (current.node->right)
				s.push({ current.node->right, false });
			maxData = max(maxData,current.node->data);
			s.push({ current.node, true });
			if (current.node->left)
				s.push({ current.node->left, false });
		}
		

	}
	cout << "\nMax digit: " << maxData;
	cout << "\nMin digit: " << minData;
}

/// 
/// 
/// 
//Для вивведення дерева
void printTree(TreeNode* root, double depth = 0) {
	if (root == nullptr)
		return;
	printTree(root->right, depth + 1);
	for (int i = 0; i < depth; ++i)
		cout << "  ";
	cout << root->data << endl;
	printTree(root->left, depth + 1);
}


int main() {
	int arr[] = { 5, 3, 8, 2,0, 4, 7, 9};
	int size = sizeof(arr) / sizeof(arr[0]);
	TreeNode* root = createTreeFromArray(arr, size);

	printTree(root);
	cout << "\n Pryamiy : ";
	strOrder(root);
	cout << "\n Obern : ";
	obernOrder(root);
	cout << "\n Symetric order: ";
	symetricOrder(root);
	cout << "\n Porivneviy: ";
	porivnOrder(root);

	int numbOfNodes = symetricOrderCounter(root);
	int heightOfTree = symetricOrderHeight(root);
	int leavesNumb = symetricOrderLeavesNumber(root);

	cout << "\n" <<"\nNumber of nodes in tree:"<< numbOfNodes;
	cout << "\n" <<"Height of the tree: "<< heightOfTree;
	cout << "\n" <<"Number of tree`s leaves: "<< leavesNumb;
	symetricOrderMaxAndMinData(root);
	cout << '\n';

}



//Індикатори,універсальний обхід дерева
//Кількість вузлів у дереві
//Глибина дерева
//Кількість листків у дереві
//Максимальне та мінімальне значення у дереві
