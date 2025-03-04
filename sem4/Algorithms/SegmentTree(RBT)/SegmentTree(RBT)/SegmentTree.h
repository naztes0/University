#pragma once

#include<iostream>



class SegmentTree
{
private:
	struct Node {
		int left, right;  
		int sum;      
		bool color;       
		Node* parent, * leftChild, * rightChild;

		Node(int l, int r, int s) : left(l), right(r), sum(s),
			color(false), parent(nullptr),
			leftChild(nullptr), rightChild(nullptr) {}
	};

	Node* root;
	Node* NIL;

	
	void rightRotate(Node* x);
	void leftRotate(Node* x);
	void fixInsert(Node* node);
	int queryPrivate(Node* node, int l, int r);
		

public:
	SegmentTree() {
		NIL = new Node(0,0,0);
		NIL->color = false;
		root = NIL;
	}
	void insert(int l, int r, int sum);
	int query(Node*node,int l, int r);
	int query(int l, int r);

};

