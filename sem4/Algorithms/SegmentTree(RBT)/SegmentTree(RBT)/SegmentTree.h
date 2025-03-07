#pragma once

#include<iostream>
#include<vector>


class SegmentTree
{
private:
	struct Node {
		double left, right;  
		double sum;
		bool color;       
		Node* parent, * leftChild, * rightChild;

		Node(double l, double r, double s) : left(l), right(r), sum(s),
			color(false), parent(nullptr),
			leftChild(nullptr), rightChild(nullptr) {}
	};

	Node* root;
	Node* NIL; //special nullptr in RBT

	
	void rightRotate(Node* x); 
	void leftRotate(Node* x);
	void updateSum(Node* node);
	void fixInsert(Node* node); //fixing the tree due to RBT rules after the insertion
	double queryPrivate(Node* node, double l, double r); //Get the sum number on some range
	
	
	//Remove methods
	void removeNode(Node* node, double l, double r);
	void transplant(Node* u, Node* v);
	Node* minimum(Node* node); 
	void fixDelete(Node* node);
	void printTreePriv(Node* node, int depth );

public:
	SegmentTree() { 
		NIL = new Node(0,0,0);
		NIL->color = false;
		root = NIL;
	}
	void insert(double l, double r, double sum); //insertion of node 
	double query(double l, double r); //the call of private method querry 
	void remove(double l, double r); //deletion of the node
	void printTree(); //tree print
};

