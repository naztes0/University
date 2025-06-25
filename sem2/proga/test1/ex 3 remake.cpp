#include<iostream>
using namespace std;

#define SIZE 100
#define numBuckets 10

struct Node {
	int data;
	Node* next;
};


Node* newNode(int data) {
	Node* newNode = new Node();
	newNode->data = data;
	newNode->next = NULL;
	return newNode;
}

void addNode(Node*&head,int data){
	if (!head) {
		head = newNode(data);
	}
	else {
		Node* temp = head;
		while (temp->next) {
			temp = temp->next;
		}
		temp->next = newNode(data);
	}
}

void deleteNodewithValue(Node*& head, int v) {
	Node* prev = NULL;
	Node* current = head;
	
	while (current && current->data != v) {
		prev = current;
		current = current->next;
	}
	if (!current) {
		return;
	}
	if (!prev) {
		head = current->next;
	}
	else {
		prev->next = current -> next;
		
	}
	delete current;
	
}

void printL(Node* head) {
	if (!head) {
		cout << "List is empty";
		return;
	}
	else {
		Node* temp = head;
		while (temp) {
			cout << temp->data << " ";
			temp = temp->next;
		}
	}
}

int main() {


	Node* buckets[numBuckets] = { nullptr };
	int elements[SIZE] = { 12,23,56,43,78,98,101,508,604 };
	int arrSize = sizeof(elements) / sizeof(elements[0]);
	//Прохід по масиву там розподіляєомо по спискам за останньою цифрою
	for (int i = 0; i < arrSize; i++) {
		int lastdigit = elements[i] % 10;
		if (elements[i] == 0) continue;
		addNode(buckets[lastdigit], elements[i]);

	}
	cout << "Base list: ";
	for (int i = 0; i < numBuckets; i++) {
		if (!buckets[i]) {
			continue;
		}
		else {
			printL(buckets[i]);
		}
	}
	int valToDel;
	cout << "\n\nEnter the value to delete: ";
	cin >> valToDel;
	int lastDig = valToDel % 10;
	deleteNodewithValue(buckets[lastDig], valToDel);
	


	cout << "\nNew List: ";
	for (int i = 0; i < numBuckets; i++) {
		if (!buckets[i]) {
			continue;
		}
		else {
			printL(buckets[i]);
		}
		cout << " ";
	}

	return 0;
}