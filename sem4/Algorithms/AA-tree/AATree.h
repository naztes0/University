#pragma once

#include<iostream>
#include<vector>
#include<iomanip>
using std::vector;

class AATree
{
private:
	struct Node {
		double val;
		int level;
		Node* right;
		Node* left;
		Node* parent;

		Node(double value) : val(value), right(nullptr), left(nullptr), level(1){}

	};
	Node* root;
	Node* nullNode;

	void skew(Node*& node);
	void split(Node*& node);
	void insert(double val, Node*& node, Node* parent);
	void remove(double val, Node*& node);
	Node* find(double val, Node* node);
	void print(Node* node, int space);
	void deleteTree(Node* node);
	Node* findMin(Node* node);
	void rebalance(Node*& node);

public:

	AATree() {
		nullNode = new Node(0);
		nullNode->level = 0;
		nullNode->left = nullNode->right = nullNode;
		root = nullNode;
	}

	~AATree() {
		deleteTree(root);
		delete nullNode;
	}



	void insert(double val);
	void remove(double val);
	bool find(double val);

	void print();


};

