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
	Node* NIL; //equ to the nullptr in RBT

	
	void rightRotate(Node* x); 
	void leftRotate(Node* x);
	void fixInsert(Node* node); //fixing the tree due to RBT rules after the insertion
	int queryPrivate(Node* node, int l, int r); //Get the sum number on some range
	void updatePrivate(Node* node, int l, int r, int newValue);
	
	//Remove methods
	void removeNode(Node* node, int l, int r);
	void transplant(Node* u, Node* v);
	Node* minimum(Node* node); 
	void fixDelete(Node* node);

public:
	SegmentTree() { 
		NIL = new Node(0,0,0);
		NIL->color = false;
		root = NIL;
	}
	void insert(int l, int r, int sum); //insertion of node 
	int query(int l, int r); //the call of private method querry 
	void update(int l, int r, int newValue); //change the sum on some range and update sum in other nodes
	void remove(int l, int r);
};

