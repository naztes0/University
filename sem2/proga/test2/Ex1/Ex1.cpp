#include<iostream>
using namespace std;

struct TreeNode {
	int data;
	TreeNode* left;
	TreeNode* right;
};

TreeNode* createNode(int data) {
	TreeNode* newNode = new TreeNode();
	newNode->data=data;
	newNode->left = NULL;
	newNode->right = NULL;
	return newNode;
}

int heightTree(TreeNode* root) {
	if (!root)return 0;

	int leftHeight = heightTree(root->left);
	int rightHeight = heightTree(root->right);

	return max(leftHeight,rightHeight)+1;
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
	TreeNode* root= createNode(1);
	root->left = createNode(2);
	root->right = createNode(3);
	root->left->right = createNode(5);
	root->left->left = createNode(4);
	root->right->right = createNode(7);
	root->right->left = createNode(6);
	root->right->right->right = createNode(8);

	printTree(root);

	int res = heightTree(root);
	cout << "The height of bin tree: " << res;

}