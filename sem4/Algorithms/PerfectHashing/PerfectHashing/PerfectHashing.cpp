#include"PerfectHashing.h"




bool ComplexNumber::operator==(const ComplexNumber& other) const {
	return (real == other.real) && (img == other.img);
}

bool ComplexNumber::operator!=(const ComplexNumber& other) const {
	return (real != other.real) || (img != other.img);
}

//inhereted
bool ComplexNumber:: operator==(const Hashable& other) const  {
	try {
		const ComplexNumber& otherComplex = dynamic_cast<const ComplexNumber&>(other);
		return *this == otherComplex;
	}
	catch (const std::bad_cast&) {
		return false;
	}
}

bool ComplexNumber:: operator!=(const Hashable& other) const  {
	return !(*this == other);
}

int ComplexNumber::toInt() const{
	return (real * 31 )+( 17 * img);
}
void ComplexNumber::printComplex() const {
	std::cout << real << (img >= 0 ? "+" : "") << img << "i";
}
/////////////////////////////


 ComplexVector::ComplexVector(const vector<ComplexNumber>& comps) : values(comps) {}

 ComplexVector::ComplexVector(const vector<int>& ints) {
		for (const auto& val : ints) {
			values.push_back(ComplexNumber(val, 0));
		}
	}



bool ComplexVector::operator==(const ComplexVector& other) const {
		if (values.size() != other.values.size()) return false;
		for (size_t i = 0; i < values.size(); ++i) {
			if (values[i] != other.values[i]) return false;
		}
		return true;
}

bool ComplexVector:: operator!=(const ComplexVector& other) const {
		return !(*this == other);
}

	// Реалізація базового класу
bool ComplexVector::operator==(const Hashable& other) const {
	try {
		const ComplexVector& otherVector = dynamic_cast<const ComplexVector&>(other);
		if (values.size() != otherVector.values.size())
			return false;
		for (size_t i = 0; i < values.size(); ++i) {
			if (values[i] != otherVector.values[i])
				return false;
		}
		return true;
	}
	catch (const std::bad_cast&) {
		return false;
	}
}

bool ComplexVector::operator!=(const Hashable& other) const  {
		return !(*this == other);
	}

int ComplexVector::toInt() const {
	// Use a better hash combining function
	int result = 17;
	for (const auto& val : values) {
		// Use a more consistent hash combining method
		result = result * 31 + val.toInt();
	}
	return result;
}

void ComplexVector::print() const  {
		std::cout << "(";
		for (size_t i = 0; i < values.size(); ++i) {
			if (i > 0) std::cout << ",";
			values[i].printComplex();
		}
		std::cout << ")";
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
int HashFunction::hash(const Hashable& element) const {
	if (m == 0 || p == 0) {
		std::cerr << "Error: m or p is zero in hash function!" << std::endl;
		return 0;
	}
	
	int elementInt = element.toInt();
	/*std::cerr << "\n\nHashing element with int value: " << elementInt << std::endl;
	std::cerr << "Hash params: a=" << a << ", b=" << b << ", p=" << p << ", m=" << m << std::endl;*/
	int rawHash = ((a * elementInt + b) % p) % m;
	if (rawHash < 0) rawHash += m;
	//std::cerr << "Hash result: " << rawHash << std::endl;
	return rawHash;
}
void HashFunction::setTableSize(int tableSize) {
	m = tableSize;
}


/////////////

const ComplexNumber PerfectHashing::EMPTY_CELL(0, 0);

PerfectHashing::PerfectHashing(int size) :primarySize(size), primaryHashFunc(size) {
	hashTable.resize(size);
	secondaryHashFuncs.resize(size, HashFunction(1));
	secondaryTableSizes.resize(size,0);
	secondaryTableInitialized.resize(size, false);
	
}
PerfectHashing::~PerfectHashing() {
	for (auto& bucket : hashTable) {
		for (auto& element : bucket) {
			delete element;
		}
	}
}

void PerfectHashing::insert(const vector<Hashable*>& elements) {
    vector<vector<Hashable*>> tempBuckets(primarySize);
	
	//First level
    for (const auto& element : elements) {
        // skip null
        if (!element) continue;
        

        int primaryIndex = primaryHashFunc.hash(*element);

        // Створюємо глибоку копію елемента для зберігання в таблиці
        Hashable* elementCopy = nullptr;

        // Копіюємо ComplexNumber
        const ComplexNumber* complexElement = dynamic_cast<const ComplexNumber*>(element);
        if (complexElement) {
            elementCopy = new ComplexNumber(*complexElement);
        }
        else {
            // Копіюємо ComplexVector
            const ComplexVector* vectorElement = dynamic_cast<const ComplexVector*>(element);
			if (vectorElement) {
				elementCopy = new ComplexVector(*vectorElement);
			}
        }

        if(elementCopy) tempBuckets[primaryIndex].push_back(elementCopy);
    }

    // Другий рівень хешування
    for (int i = 0; i < primarySize; i++) {
        // Очищаємо попередні елементи бакета (якщо вони є)
        for (auto& element : hashTable[i]) {
            delete element;
        }
        hashTable[i].clear();

        int elementsInBucket = tempBuckets[i].size();
        if (elementsInBucket == 0) continue;

        // Випадок з одним елементом у бакеті
        if (elementsInBucket == 1) {
            hashTable[i].resize(1);
            hashTable[i][0] = tempBuckets[i][0];
            secondaryTableSizes[i] = 1;
            secondaryTableInitialized[i] = true;
            tempBuckets[i].clear();  // Очищаємо тимчасовий вектор (без видалення пам'яті)
            continue;
        }

        std::cerr << "Bucket " << i << " elements: " << elementsInBucket << "\n\n";

        // Випадок з кількома елементами в бакеті
        if (elementsInBucket > 1) {
            int secondarySize = elementsInBucket * elementsInBucket;
            std::cerr << "    Bucket " << i << " -> secondarySize: " << secondarySize << "\n\n";

            secondaryTableSizes[i] = secondarySize;
			secondaryHashFuncs[i].setTableSize(secondarySize);

            bool noCollision = false;
            int attempts = 0;
            const int MAX_ATTEMPTS = 100;

            while (!noCollision && attempts < MAX_ATTEMPTS) {
				secondaryHashFuncs[i] = HashFunction(secondarySize);

                hashTable[i].clear();
                hashTable[i].resize(secondarySize, nullptr);

                noCollision = true;
                for (const auto& element : tempBuckets[i]) {
       

                    int secondaryIndex = secondaryHashFuncs[i].hash(*element);
                    std::cerr << "Hashed ";
                    element->print();
                    std::cerr << std::endl;

                    if (hashTable[i][secondaryIndex] != nullptr) {
                        noCollision = false;
                        break;
						std::cout << "\ncolision!\n";
                    }

                    hashTable[i][secondaryIndex] = element;
                }
                attempts++;
            }

			if (!noCollision) {
				// Clean up the table for this bucket
				hashTable[i].clear();

				// Simple list to store elements (not perfect hash)
				hashTable[i].resize(elementsInBucket);
				for (size_t j = 0; j < tempBuckets[i].size(); ++j) {
					hashTable[i][j] = tempBuckets[i][j];
				}
			}

            secondaryTableInitialized[i] = true;
            tempBuckets[i].clear(); 
        }
    }
}

bool PerfectHashing::search(const Hashable& element) const {
	int primaryIndex = primaryHashFunc.hash(element);

	if (primaryIndex >= primarySize || !secondaryTableInitialized[primaryIndex]) {
		return false;
	}

	// Case with a single element in bucket
	if (secondaryTableSizes[primaryIndex] == 1 && hashTable[primaryIndex].size() == 1) {
		return hashTable[primaryIndex][0] && *hashTable[primaryIndex][0] == element;
	}

	// Case with multiple elements in bucket
	int secondarySize = secondaryTableSizes[primaryIndex];
	int secondaryIndex = secondaryHashFuncs[primaryIndex].hash(element);

	if (secondaryIndex >= hashTable[primaryIndex].size() ||
		!hashTable[primaryIndex][secondaryIndex]) {
		// Check if the element is stored in a simple list (fallback case)
		for (const auto& item : hashTable[primaryIndex]) {
			if (item && *item == element) {
				return true;
			}
		}
		return false;
	}

	return *hashTable[primaryIndex][secondaryIndex] == element;
}

void PerfectHashing::print() const {
	std::cout << "=== Perfect Hash Table Structure ===\n";
	std::cout << "Primary table size: " << primarySize << "\n\n";

	for (int i = 0; i < primarySize; i++) {
		if (secondaryTableInitialized[i]) {
			std::cout << "Bucket " << i << " (size: " << secondaryTableSizes[i] << "):\n";

			if (secondaryTableSizes[i] == 1 && hashTable[i].size() == 1 && hashTable[i][0] != nullptr) {
				std::cout << "  Single element: ";
				hashTable[i][0]->print();
				std::cout << "\n";
			}
			else {
				for (int j = 0; j < hashTable[i].size(); j++) {
					std::cout << "  [" << j << "]: ";
					if (hashTable[i][j] != nullptr) {
						hashTable[i][j]->print();
					}
					else {
						std::cout << "Empty";
					}
					std::cout << "\n";
				}
			}
			std::cout << "\n";
		}
		else {
			std::cout << "Bucket " << i << ": Empty\n\n";
		}
	}
	std::cout << "==================================\n";
}