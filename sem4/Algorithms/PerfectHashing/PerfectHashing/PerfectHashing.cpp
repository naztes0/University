#include"PerfectHashing.h"


bool ComplexNumber:: operator == (const ComplexNumber& other) const {
	return (real == other.real) && (img == other.img);
}

bool ComplexNumber:: operator != (const ComplexNumber& other) const {
	return (real != other.real) || (img != other.img);
}
int ComplexNumber::toInt() const{
	return (real * 31 * 17 * img);
}
void ComplexNumber::printComplex() const {
	std::cout << real << (img >= 0 ? "+" : "") << img << "i";
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
	if(p!=0){
		a = rand() % (p - 1) + 1;
		b = rand() % p;
	}
}
int HashFunction::hash(ComplexNumber z) const {
	if (m == 0||p==0) {
		std::cerr << "Error: m or p is zero in hash function!" << std::endl;
		return 0;
	}
	
	int zInt = z.toInt();
	std::cerr << "\n\nHashing: ";
	z.printComplex();
	std::cerr << " -> int value: " << zInt << std::endl;
	std::cerr << "Hash params: a=" << a << ", b=" << b << ", p=" << p << ", m=" << m << std::endl;
	int rawHash = ((a *zInt + b) % p) % m;
	if (rawHash < 0) rawHash += m; 
	std::cerr << "Hash result: " << rawHash << std::endl;
	return rawHash;
}
void HashFunction::setTableSize(int tableSize) {
	m = tableSize;
}


/////////////

const ComplexNumber PerfectHashing::EMPTY_CELL(0, 0);

PerfectHashing::PerfectHashing(int size) :primarySize(size), primaryHashFunc(size) {
	hashTable.resize(size);
	secondaryHashFuncs.resize(size);
	secondaryTableSizes.resize(size,0);
	secondaryTableInitialized.resize(size, false);
	//temo init
	for (int i = 0; i < size; i++) {
		secondaryHashFuncs[i] = HashFunction(1);
	}
}

void PerfectHashing::insert(const vector<ComplexNumber>& elements) {
	vector<vector<ComplexNumber>> tempBuckets(primarySize);
	//First-level
	for (const auto& element : elements) {
		int primaryIndex = primaryHashFunc.hash(element);
		if (primaryIndex < 0 || primaryIndex >= primarySize) {
			std::cerr << "Error: primaryIndex out of bounds: " << primaryIndex << std::endl;
		}
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
		std::cerr << "Bucket " << i << " elements: " << elementsInBucket << "\n\n";
		if(elementsInBucket>1) {
			int secondarySize = elementsInBucket * elementsInBucket;
			std::cerr << "	Bucket " << i << " -> secondarySize: " << secondarySize << "\n\n";
			if (secondarySize == 0) {
				std::cerr << "Error: secondarySize is 0 for bucket " << i << std::endl;
				continue;
			}
			secondaryTableSizes[i] = secondarySize;
			secondaryHashFuncs[i].setTableSize(secondarySize);

			secondaryHashFuncs[i] = HashFunction(secondarySize);

			bool noCollision = false;
			int attempts = 0;
			const int MAX_ATTEMPTS = 100;

			while (!noCollision && attempts < MAX_ATTEMPTS) {
				secondaryHashFuncs[i].generateHashFunction();
				hashTable[i].clear();
				hashTable[i].resize(secondarySize, EMPTY_CELL);
				

				noCollision = true;
				for (const auto& element : tempBuckets[i]) {
					int secondaryIndex = secondaryHashFuncs[i].hash(element);
					if (secondaryIndex < 0 || secondaryIndex >= secondaryTableSizes[i]) {
						std::cerr << "Error: secondaryIndex out of bounds: " << secondaryIndex
							<< " (secondaryTableSizes[i] = " << secondaryTableSizes[i] << ")\n";
					}
					std::cerr << "Hashed ";
					element.printComplex();
					if (hashTable[i][secondaryIndex] != EMPTY_CELL) {
						noCollision = false;
						break;
					}
					hashTable[i][secondaryIndex] = element;
					
				}
				attempts++;


			}
			/*if (!noCollision) {
				std::cout << "Warning: Could not find perfect hash function for bucket "
					<< i << " after " << MAX_ATTEMPTS << " attempts.\n";
				return;
			}*/
			secondaryTableInitialized[i] = true;
		}

	}
	

}

bool PerfectHashing::search(const ComplexNumber& element) const {

	int primaryIndex = primaryHashFunc.hash(element);
	if (primaryIndex < 0 || primaryIndex >= primarySize) {
		return false;
	}

	if (!secondaryTableInitialized[primaryIndex] || secondaryTableSizes[primaryIndex] == 0) {
		return false;
	}
	if (secondaryTableSizes[primaryIndex] == 1 && hashTable[primaryIndex].size() == 1) {
		return hashTable[primaryIndex][0] == element;
	}
	int secondaryIndex = secondaryHashFuncs[primaryIndex].hash(element);
	if (secondaryIndex < hashTable[primaryIndex].size()) {
		return hashTable[primaryIndex][secondaryIndex] == element;
	}
	return false;
}

	void PerfectHashing::print()const {

		std::cout << "=== Perfect Hash Table Structure ===\n";
		std::cout << "Primary table size: " << primarySize << "\n\n";

		for (int i = 0; i < primarySize; i++) {
			if (secondaryTableInitialized[i]) {
				std::cout << "Bucket " << i << " (size: " << secondaryTableSizes[i] << "):\n";

				if (secondaryTableSizes[i] == 1 && hashTable[i].size() == 1) {
					std::cout << "  Single element: ";
					hashTable[i][0].printComplex();
					std::cout << "\n";

				}
				else {
					for (int j = 0; j < hashTable[i].size(); j++) {
						std::cout << "  [" << j << "]: ";
						hashTable[i][j].printComplex();
						std::cout << "\n";

					}
				}
				std::cout << "\n";
			}
		}
		std::cout << "==================================\n";
	}