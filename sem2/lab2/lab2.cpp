//Linear lists.Realization of addition, subtraction, result and derivative 
#include <iostream>
#include<vector>
using namespace std;

struct Terms {
	float coefficient;
	int expA;
	int expB;
	int expC;
	Terms* next;
};

struct Polynomial {
	Terms* head;
};

void initTerm(Terms* term, float coef, int expA, int expB, int expC) {
	term->coefficient = coef;
	term->expA = expA;
	term->expB = expB;
	term->expC = expC;
	term->next = nullptr;
}

Polynomial termsAdd(Polynomial& poly,int coefficient, int expA, int expB, int expC) {
	Terms* newTerm = new Terms();
	initTerm(newTerm, coefficient, expA, expB, expC);

	if (!poly.head) {
		poly.head = newTerm;
	}
	else {
		Terms* current = poly.head;
		while (current->next) {
			current = current->next;
		}
		current->next = newTerm;
	}
	return poly;
}

//Для всього многочлену
Polynomial inputPolynomial() {
	Polynomial poly;
	poly.head = nullptr;
	int choice;
	do {
		float coefficient;
		int expA, expB, expC;
		cout << "Enter coefficient: ";
		cin >> coefficient;
		cout << "Enter exponent A: ";
		cin >> expA;
		cout << "Enter exponent B: ";
		cin >> expB;
		cout << "Enter exponent C: ";
		cin >> expC;
		termsAdd(poly, coefficient, expA, expB, expC);
		cout << "Add another term? (1-Yes / 0-No): ";
		cin >> choice;
	} while (choice == 1);
	return poly;
}
void printPolynomial(const Polynomial& poly) {
	Terms* current = poly.head;
	bool isFirstTerm = true;

	while (current) {
		if (current->coefficient != 0) {
			if (!isFirstTerm) {
				cout << " + ";
			}
			cout << current->coefficient;
			if (current->expA != 0) {
				cout << "x^" << current->expA;
			}
			if (current->expB != 0) {
				cout << "y^" << current->expB;
			}
			if (current->expC != 0) {
				cout << "z^" << current->expC;
			}
			isFirstTerm = false;
		}
		current = current->next;
	}
	cout << endl;
}
Polynomial polyAdder(const Polynomial& poly1, const Polynomial& poly2) {
	Polynomial result;
	result.head = nullptr;

	Terms* current1 = poly1.head;
	Terms* current2 = poly2.head;

	if (!current1 && !current2) {
		cout << "Polynomials are empty";
		return result;
	}

	while (current1 && current2) {
		if (current1->expA == current2->expA && current1->expB == current2->expB && current1->expC == current2->expC) {
			float coefficient = current1->coefficient + current2->coefficient;
			termsAdd(result, coefficient, current1->expA, current1->expB, current1->expC);
			current1 = current1->next;
			current2 = current2->next;
		}
		else if (current1->expA < current2->expA || (current1->expA == current2->expA && current1->expB < current2->expB) || (current1->expA == current2->expA && current1->expB == current2->expB && current1->expC < current2->expC)) {
			termsAdd(result, current1->coefficient, current1->expA, current1->expB, current1->expC);
			current1 = current1->next;
		}
		else {
			termsAdd(result, current2->coefficient, current2->expA, current2->expB, current2->expC);
			current2 = current2->next;
		}
	}

	while (current1) {
		termsAdd(result, current1->coefficient, current1->expA, current1->expB, current1->expC);
		current1 = current1->next;
	}

	while (current2) {
		termsAdd(result, current2->coefficient, current2->expA, current2->expB, current2->expC);
		current2 = current2->next;
	}

	return result;
}

Polynomial polyMultiplier(const Polynomial& poly1, const Polynomial& poly2) {
	Polynomial result;
	result.head=NULL;

	
	

	if (!poly1.head && !poly2.head) {
		cout << "Polynomials are empty";
		return result;
	}

	Terms* current1 = poly1.head;
	while (current1) {
		Terms* current2 = poly2.head;
		while (current2) {
			float coefficient = current1->coefficient * current2->coefficient;
			int expA = current1->expA + current2->expA;
			int expB = current1->expB + current2->expB;
			int expC = current1->expC + current2->expC;

			termsAdd(result, coefficient, expA, expB, expC);
			current2 = current2->next;
		}
		current1 = current1->next;
	}
	return result;
}

Polynomial polysDerivative(const Polynomial& poly, char variable) {
    Polynomial result;
    result.head = nullptr;

    if (!poly.head) {
        cout << "Polynomial is empty";
        return result;
    }

    Terms* current = poly.head;
    while (current) {
        float coefficient = 0;
        int expA = current->expA, expB = current->expB, expC = current->expC;

        switch(variable) {
            case 'x':
                if (expA > 0) {
                    coefficient = current->coefficient * expA;
                    expA--;
                }
                break;
            case 'y':
                if (expB > 0) {
                    coefficient = current->coefficient * expB;
                    expB--;
                }
                break;
            case 'z':
                if (expC > 0) {
                    coefficient = current->coefficient * expC;
                    expC--;
                }
                break;
            default:
                cout << "Invalid variable choice";
                return result;
        }

        if (coefficient != 0) {
            termsAdd(result, coefficient, expA, expB, expC);
        }

        current = current->next;
    }

    return result;
}

	int main() {
		
		cout << "Enter the first polynomial:\n";
		Polynomial poly1 = inputPolynomial();

		
		cout << "Enter the second polynomial:\n";
		Polynomial poly2 = inputPolynomial();


		Polynomial resultOfAdd = polyAdder(poly1, poly2);
		Polynomial resultOfMult = polyMultiplier(poly1, poly2);
		

		cout << "\nFirst polynomial: ";
		printPolynomial(poly1);
		cout << "Second polynomial: ";
		printPolynomial(poly2);

		
		cout << "\nResult of addition:\n";
		printPolynomial(resultOfAdd);

		cout << "\nResult of multiplication:\n";
		printPolynomial(resultOfMult);

		cout << "\nResult of derivative:\n";

		
		Polynomial resultOfDerivativeX1 = polysDerivative(poly1, 'x');
		Polynomial resultOfDerivativeX2 = polysDerivative(poly2, 'x');
		cout << "For the first polynomial (derivative with respect to x): ";
		printPolynomial(resultOfDerivativeX1);
		cout << "For the second polynomial (derivative with respect to x): ";
		printPolynomial(resultOfDerivativeX2);
		cout << "\n";
		
		Polynomial resultOfDerivativeY1 = polysDerivative(poly1, 'y');
		Polynomial resultOfDerivativeY2 = polysDerivative(poly2, 'y');
		cout << "For the first polynomial (derivative with respect to y): ";
		printPolynomial(resultOfDerivativeY1);
		cout << "For the second polynomial (derivative with respect to y):";
		printPolynomial(resultOfDerivativeY2);
		cout << "\n";
		
		Polynomial resultOfDerivativeZ1 = polysDerivative(poly1, 'z');
		Polynomial resultOfDerivativeZ2 = polysDerivative(poly2, 'z');
		cout << "For the first polynomial (derivative with respect to z): ";
		printPolynomial(resultOfDerivativeZ1);
		cout << "For the second polynomial (derivative with respect to z): ";
		printPolynomial(resultOfDerivativeZ2);
		return 0;
	}

/*
4
5
2
3
1
8
6
1
2
0
8
5
2
3
1
2
4
5
8
1
7
0
2
3
0
*/
