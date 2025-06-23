//Realization of quadratic sort 
#include<iostream>
#include<vector>
#include<cmath>
using namespace std;

struct Node {
	int data;
	Node* next;
};


void addEndNode(Node*& head, int value) {
	Node* newNode = new Node();
	newNode->data = value;
	newNode->next = NULL;
	if (head == NULL) {
		head = newNode;
		return;
	}
	Node* temp = head;
	while (temp->next != NULL) {
		temp = temp->next;
	}
	temp->next = newNode;

}

void printL(Node* head) {
	if (head == NULL) {
		cout << "List is empty";
	}
	else {
		Node* temp = head;
		while (temp != NULL) {
			cout << temp->data << " ";
			temp = temp->next;
		}
	}
	cout << endl;
}


void swap(Node* a, Node* b) {
	int temp = a->data;
	a->data = b->data;
	b->data = temp;
}

int lengthL(Node* head) {
	int length = 0;
	Node* temp = head;
	while (temp != NULL) {
		length++;
		temp = temp->next;
	}
	return length;
}

void quadraticSort(Node* head) {

	int length = lengthL(head);
	int blockSize = sqrt(length);
	for (int i = 0; i <= length; i += blockSize) {
		Node* blockStart = head;
		Node* blockEnd = head;
		for (int j = 0; j < i; j++) {
			blockStart = blockStart->next;
		}
		for (int j = 0; j < blockSize - 1; j++) {
			blockEnd = blockEnd->next;
		}

		Node* current = blockStart;
		while (current&&current!=blockEnd->next) {
			Node* minVal = current;
			Node* temp = current->next;
			while (temp) {
				if (temp->data < minVal->data) {
					minVal = temp;
				}
				temp = temp->next;
			}
			swap(current, minVal);
			current = current->next;
		}
	}

}


int main() {
	Node* head = NULL;
	addEndNode(head, 64); // Додавання вузла в кінець списку
	addEndNode(head, 34); // Додавання вузла в кінець списку
	addEndNode(head, 25); // Додавання вузла в кінець списку
	addEndNode(head, 12); // Додавання вузла в кінець списку
	addEndNode(head, 22); // Додавання вузла в кінець списку
	addEndNode(head, 11); // Додавання вузла в кінець списку
	addEndNode(head, 90); // Додавання вузла в кінець списку
	addEndNode(head, 77); // Додавання вузла в кінець списку
	addEndNode(head, 88);
	addEndNode(head, 45);

	cout << "Original list: ";
	printL(head);

	quadraticSort(head);
	cout << "Sorted list: ";
	printL(head);
	return 0;
}
