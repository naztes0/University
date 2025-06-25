//Лінінйний список F цілих чисел збеіргається як послідовно зв'язний індексний список так що числа які мають однакову останню цифру розміщаюьбся в один підсписок 
//Написати фнукцію яка вилучає з списску елемент зі значеннням v якщо такий елемент списку присутній

#include<iostream>

using namespace std;

struct Node {
	int data;
	Node* next;
};

void addToEnd(Node*& head, int value) {
	Node* newNode = new Node;
	newNode->data = value;
	newNode->next = nullptr;

	if (head == nullptr) {
		head = newNode;
		return;
	}

	Node* current = head;
	while (current->next != nullptr) {
		current = current->next;
	}
	current->next = newNode;
}
bool valuePresent(Node* head, int v) {
	Node* current = head;

	while (current != NULL) {
		if (current->data % 10 == v % 10) {
			return true;
		}
		current = current->next;
	}
	return false;
}

void delV(Node*& head, int v) {
	Node* current = head;
	Node* prev = NULL;

	while (current && current->data % 10 != v % 10) {
		prev = current;
		current = current->next;
	}
	if (!prev) {
		head = current->next;
	}
	else {
		prev->next = current->next;
	}
	delete current;
}


void printL(Node* head) {
	Node* temp = head;
	while (temp){
		cout << temp->data<<" ";
		temp = temp->next;
	}
}



int main(){
	Node* head = NULL;
	addToEnd(head, 27);
	addToEnd(head, 37);
	addToEnd(head, 45);
	addToEnd(head, 66);
	addToEnd(head, 57);
	addToEnd(head, 49);

	cout << "Original list: ";
	printL(head);
	cout << endl;

	int v;
	cout << "Enter v to delete: ";
	cin >> v;

	if (valuePresent(head, v)) {
		delV(head, v);
		cout << "New list ";
		printL(head);
	}
	else {
		cout << "\nNo such values";
	}



}