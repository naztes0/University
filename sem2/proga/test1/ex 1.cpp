#include<iostream>

using namespace std;

struct Node {
	int data;
	Node* next;
};

void addToEnd(Node*& head, int data) {
	Node* newNode = new Node;
	newNode->data = data;
	newNode->next = NULL;
	if (!head) {
		head = newNode;
	}
	else {
		Node* temp = head;
		while (temp->next) {
			temp = temp->next;
		}
		temp->next = newNode;
	}
}


void mirroredList(Node*& head) {
	if (!head) {
		cout << "List is empty";
		return;
	}
	else {
		Node* reversehead = NULL;
		Node* current = head;
		while (current) {
			Node* newNode = new Node;
			newNode->data = current->data;
			newNode->next = reversehead;
			reversehead = newNode;
			current = current->next;
		}
		current = head;
		while (current->next) {
			current = current->next;
		}
		current->next = reversehead;

	}
}

void outputList(Node* head) {
	Node* temp = head;
	while (temp) {
		cout << temp->data << " ";
		temp = temp->next;
	}
}

int main() {
	Node* head=NULL;
	addToEnd(head,1 );
	addToEnd(head,2 );
	addToEnd(head,3 );
	addToEnd(head,6 );
	addToEnd(head,5 );
	addToEnd(head,4 );

	cout << "Original list: ";
	outputList(head);
	cout << endl;

	mirroredList(head);

	cout << "Mirrored list: ";
	outputList(head);
	cout << endl;

	return 0;


}