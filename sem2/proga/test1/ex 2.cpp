#include<iostream>
using namespace std;


struct Node {
	int data;
	Node* next;

};

Node* front = NULL;
Node* tail = NULL;

void addToQEnd(int data) {
	Node* newNode = new Node;
	newNode->data = data;
	newNode->next = NULL;

	if (!tail) {
		front = tail = newNode;
		tail->next = front;// умовна циклічнітсь
	}
	else {
		tail->next = newNode;
		tail = newNode;
		tail->next = front;
	}
}

void displayQ() {
	if (!front) {
		cout << "Q is empty";
		return;
	}
	Node* temp = front;
	do {
		cout << temp->data << " ";
		temp = temp->next;
	} while (temp != front);

}


int main() {

	addToQEnd(4);
	addToQEnd(5);
	addToQEnd(9);
	addToQEnd(8);
	addToQEnd(6);


	cout << "Q: ";
	displayQ();
}