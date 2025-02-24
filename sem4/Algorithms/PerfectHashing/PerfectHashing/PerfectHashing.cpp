#include"PerfectHashing.h"


bool ComplexNumber:: operator == (const ComplexNumber& other) const {
	return (real == other.real) && (img == other.img);
}

bool ComplexNumber:: operator != (const ComplexNumber& other) const {
	return (real != other.real) && (img != other.img);
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

//////////////////////////

HashFunction::HashFunction(int tableSize):m(tableSize) {
	std::srand(static_cast<unsigned int>(std::time(nullptr)));
	this->p = getRandomPrime(200, 700);
	generateHashFunction();

}

int HashFunction::isPrime(int num) {
	for (int i = 2; i*i<= num; i++) {
		if (num % i == 0) return 0;
	}
	return 1;
}

int HashFunction::getRandomPrime(int min, int max){ 
	int attempts = 0;
	const int MAX_ATTEMPTS = 1000;  // Обмеження кількості спроб

	while (attempts < MAX_ATTEMPTS) {
		int randomNum = rand() % (max - min + 1) + min;
		if (isPrime(randomNum)) return randomNum;
		attempts++;
	}
	return 13;//If for 1000 attempt primeNumb wont be found
}

int HashFunction::generateHashFunction() {
	a = rand() % (p - 1) + 1;
	b = rand() % p;
}
int HashFunction::hash(ComplexNumber z) {
	if (m == 0) return 0;
	return((a * z.toInt() + b) % p) % m;
}
void HashFunction::setTableSize(int tableSize) {
	m = tableSize;
}


/////////////

const ComplexNumber PerfectHashing::EMPTY_CELL(-1, -1);

PerfectHashing::PerfectHashing(int size) :primarySize(size), primaryHashFunc(size) {
	hashTable.resize(size);
	secondaryHashFuncs.resize(size);
	secondaryTableSizes.resize(size, 0);
	secondaryTableInitialized.resize(size, false);
}

void PerfectHashing::insert(const vector<ComplexNumber>& elements) {
	vector<vector<ComplexNumber>> tempBuckets(primarySize);
	//First-level
	for (const auto& element : elements) {
		int primaryIndex = primaryHashFunc.hash(element);
		tempBuckets[primaryIndex].push_back(element);
	}
	

}