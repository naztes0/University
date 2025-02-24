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

void HashFunction::generateHashFunction() {
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
	//Second-level
	for (int i = 0; i < primarySize; i++) {
		int elementsInBucket = tempBuckets[i].size();

		if (elementsInBucket == 0)continue;
		else if (elementsInBucket == 1) {
			hashTable[i].resize(1);
			hashTable[i][0] = tempBuckets[i][0];
			secondaryTableSizes[i] = 1;
			secondaryTableInitialized[i] = true;
		}
		else {
			int secondarySize = elementsInBucket * elementsInBucket;
			secondaryTableSizes[i] = secondarySize;
			secondaryHashFuncs[i].setTableSize(secondarySize);

			bool noCollision = false;
			int attempts = 0;
			const int MAX_ATTEMPTS = 100;

			while (!noCollision && attempts < MAX_ATTEMPTS) {
				secondaryHashFuncs[i].generateHashFunction();
				hashTable[i].clear();
				hashTable[i].resize(secondarySize, 0);

				noCollision = true;
				for (const auto& element : tempBuckets[i]) {
					int secondaryIndex = secondaryHashFuncs[i].hash(element);
					if (hashTable[i][secondaryIndex] != EMPTY_CELL) {
						noCollision = false;
						break;
					}
					hashTable[i][secondaryIndex] = element;
				}
				attempts++;


			}
			if (!noCollision) {
				std::cout << "Warning: Could not find perfect hash function for bucket "
					<< i << " after " << MAX_ATTEMPTS << " attempts.\n";
				return;
			}
			secondaryTableInitialized[i] = true;
		}

	}
	

}