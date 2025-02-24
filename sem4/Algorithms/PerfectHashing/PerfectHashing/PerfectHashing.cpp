#include"PerfectHashing.h"


ComplexNumber ComplexNumber:: operator == (const ComplexNumber& other) const {
	return (real == other.real) && (img == other.img);
}
int ComplexNumber::toInt() const{
	return (real * 31 + 17 * img);
}
void ComplexNumber::printComplex()const {
	if (img >= 0)
		std::cout << real << "+" << img << "i";
	else
		std::cout << real << img << "i";
}

int HashFunction::isPrime(int num) {
	for (int i = 2; i*i<= num; i++) {
		if (num % i == 0) return 0;
	}
	return 1;
}

int HashFunction::getRandomPrime(int min, int max){ 
	while (true){
		int randomPrime = rand() % (max - min+1) + min;
		if (isPrime(randomPrime)) return randomPrime;
	}
}
HashFunction::HashFunction(int m) {
	this->p = getRandomPrime(200, 700);
	this->m = m;
	generateHashFunction;

}
int HashFunction::generateHashFunction() {
	a = rand() % (p - 1) + 1;
	b = rand() % p;
}
int HashFunction::hash(ComplexNumber z) {
	return((a * z.toInt() + b) % p) % m;
}

