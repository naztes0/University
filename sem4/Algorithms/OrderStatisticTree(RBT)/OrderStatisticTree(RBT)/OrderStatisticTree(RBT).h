#pragma once


#include <iostream>

class OST {
private:
	struct Node {
		int key, size;
		bool color;
		Node* left,*right,*parent;

		Node(int key):key(key),size(1),color(true), left(nullptr), right(nullptr), parent(nullptr){}
	};
	Node* root;
	Node* NIL;

	void updateSize(Node*node);
	void leftRotate(Node* node);
	void rightRotate(Node* node);

public:
	OST() {
		NIL = new Node(0);
		NIL->color = false;
		NIL->size = 0;
		root = NIL;
	}

	void insert(int key);
	void fixInsert(Node*node);
	

};

