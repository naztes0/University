#include<iostream>
using namespace std;
const int N = 50;
struct TreeNode {
	int data;
	TreeNode* left;
	TreeNode* right;
};

TreeNode* createNode(int data) {
	TreeNode* newNode = new TreeNode();
	newNode->data = data;
	newNode->left = NULL;
	newNode->right = NULL;
	return newNode;

}
TreeNode* sortedArrayToBST(const int nums[N], int start, int end) {
	if (start > end) {
		return nullptr;
	}

	int mid = (start + end) / 2;
	TreeNode* root = new TreeNode();
	root->data = nums[mid];
	root->left = sortedArrayToBST(nums, start, mid - 1);
	root->right = sortedArrayToBST(nums, mid + 1, end);
	return root;
}

bool obhid(TreeNode* root, int v, int u) {
	if (!root) return false;

	if (root->data >= v && root->data <= u)
		return true;

	if (root->data > u)
		return obhid(root->left, v, u);

	if (root->data < v)
		return obhid(root->right, v, u);

	return false;
}

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
	TreeNode* root;
	int nums[N], size;
	cout << "Enter size: ";
	cin >> size;
	cout << "Enter array: ";
	for (int i = 0; i < size; i++) {
		cin >> nums[i];
	}
	root = sortedArrayToBST(nums, 0, size - 1);
	printTree(root);

	int startp, endp;
	cout << "Enter interval (use space): ";
	cin >> startp >> endp;

	cout << (obhid(root, startp, endp) ? "Tree has the number from interval" : "Tree hasn`t the number from interval");

}
/*
7
0 1 2 3 4 5 6

*/