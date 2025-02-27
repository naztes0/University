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

public:

};