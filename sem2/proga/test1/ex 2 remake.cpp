#include<iostream>
using namespace std;
#define SIZE 5

struct Q {
	int items[SIZE];
	int front, tail;
	Q() {
		front = -1;
		tail = -1;
	}
};

bool isFull(Q q) {
	return(q.front == 0 && q.tail == SIZE - 1) || (q.front == q.tail + 1);
}
bool isEmpty(Q q) {
	return (q.front == -1);
}

void enQueue(Q &q, int el) {
	if (isFull(q)) {
		cout << "Q is full";
		return;
	}
	if (q.front == -1)q.front = 0;
	q.tail = (q.tail + 1) % SIZE;
	q.items[q.tail] = el;
	

}
void display(Q q) {
	if (isEmpty(q)) {
		cout << "Q is empty";
		return;
	}
	else {
		int i = q.front;
		do {
			cout << q.items[i] << " ";
			i = (i + 1) % SIZE;
		} while (i != (q.tail + 1) % SIZE);
	}
}


int main() {
	Q q;
	enQueue(q, 1);
	enQueue(q, 2);
	enQueue(q, 3);
	enQueue(q, 4);
	enQueue(q, 5);

	display(q);
	

	return 0;
}
