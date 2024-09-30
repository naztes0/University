#include<iostream>
#include<string>
#include<stdexcept> // for reporting about exceptions
#include<vector>

class Date {
protected:
	int day;
	int month;
	int year;

public:
	Date(int d, int m, int y) :day(d), month(m), year(y) {
		if (!isValid()) {
			throw std::invalid_argument("Invalid date!");
		}
	}


	bool isValid() const {
		if (year < 0 || month < 1 || month>12 || day < 1 || day>31) {
			return false;
		}
		//For valid days in february
		if (month == 2) {
			bool isLeapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
			return isLeapYear ? day <= 29 : day <= 28;
		}
		//For valid days in months with 30 days
		if (month == 4 || month == 6 || month == 9 || month == 11) {
			return day <= 30;
		}
		return true;
	}

	//for comparing two dates
	bool isEarlier(const Date& date1, const Date& date2) {
		if (date1.year < date2.year) {
			return true;
		}
		else if (date1.year == date2.year) {
			if (date1.month < date2.month) {
				return true;
			}
			else if (date1.month == date2.month) {
				return date1.day < date2.day;
			}
		}
		return false;
	}
	std::string toString() const {
		std::string dayString = (day < 10 ? 0 : "") + std::to_string(day);
		std::string monthString = (month < 10 ? 0 : "") + std::to_string(month);
		std::string yearString = std::to_string(year);
		return  dayString + "." + monthString + "." + yearString;
	}
	int getDay() const { return day; }
	int getMonth()const { return month; }
	int getYear()const { return year; }

	void setDay(int d) {
		day = d;
		if (!isValid()) {
			throw std::invalid_argument("Invalid date after setting day!");
		}
	}
	void setMonth(int m) {
		month = m;
		if (!isValid()) {
			throw std::invalid_argument("Invalid date after setting month!");
		}
	}
	void setYear(int y) {
		year = y;
		if (!isValid()) {
			throw std::invalid_argument("Invalid date after setting year!");
		}
	}
};

//Basic class for financial operations
class Transaction {
protected:
		double amount;
		Date date;
		std::string description;
		std::string category;
public:
	//Constructor for init
	Transaction(double amount, const Date& dt, const std::string& descrip, const std::string& categ)
		:amount(amount), date(dt), description(descrip), category(categ) {
		if (!isValid()) {
			throw std::invalid_argument("Invalid transaction data");
		}
	}
	//Destructor
	virtual ~Transaction() = default;

	double getAmount() const { return amount;}
	Date getDate() const { return date; }
	std::string getDescription() const { return description;}
	std::string getCategory() const { return category; }

	void setAmount(double amt) { amount = amt; }
	void setDate(const Date& dt) { date = dt; }
	void setDecription(const std::string& desc) { description = desc; }
	void setCategory(const std::string& categ) { category = categ; }

	virtual void printDetails()const {
		std::cout << "Transaction Details:\n";
		std::cout << "Amount: " << amount
			<< ", Date: " << date.toString() << ", Description: " << description
			<< ", Category: " << category << std::endl;

	}

	
	bool isValid() const {
		return !category.empty() && amount != 0;
	}

};




class TransactionManager {
private:
	std::vector<Transaction*> transactions;
	std::string databasePath;

public:
	/*
	* add loading transactions from the database for future operation with them
	*  TransactionManager(const std::string& dbPath) : databasePath(dbPath) {}
	*/
    // Destructor
    ~TransactionManager() {
        for (auto trans : transactions) {
            delete trans; // Free memory allocated for each transaction
        }
    }

	
	
	bool indexChecker(int index) {
		if (index < 0 || index >= transactions.size()) {
			throw std::out_of_range("Index out of range.");
		}
		else {
			return true;
		}
	}

	void addTransaction(Transaction* trans) {
		if (trans->isValid()) transactions.push_back(trans);
		else {
			throw std::invalid_argument("Invalid transaction can not be added");
		}
	}
	void removeTransaction(int index) {
		if (indexChecker(index)) {
			delete transactions[index]; // Free memory allocated for the transaction
			transactions.erase(transactions.begin() + index); // Remove from vector
		}
	}
	Transaction* getTransaction(int index) {
		if (indexChecker(index)) {
			return transactions[index]; 
		}
		return nullptr;
	}

	//ADD FUNC FOR LOADING TRANSACTIONS FROM DB
	//ADD FUNC FOR SAVING  TRANSACTIONS FROM DB

};


class CategoryManager {
private:
	std::vector<std::string> categories;
public:
	CategoryManager() = default;
	void addCategory(const std::string& category) {
		if (!isValidCategory(category)) {
			throw std::invalid_argument("Invalid category!");
		}
		if(categoryExists(category)) {
			throw std::invalid_argument("Category already exists");
		}
		categories.push_back(category);
		std::cout << "Category '"<<category<< "' added successfully.\n";
	}

	void removeCategory(std::string& category) {
		int index = findCategoryIndex(category);
		if (index==-1) {
			throw std::invalid_argument("Category not found!");
		}
		categories.erase(categories.begin() + index);
		std::cout << "Category '" << category << "'removed successfully.\n";
	}

	bool categoryExists(const std::string& cateegory) {
		for (size_t i = 0; i < categories.size(); i++) {
			if (categories[i] == cateegory) return true;
		}
		return false;
	}
	
	bool isValidCategory(const std::string& category) const {
		return !category.empty();
	}
	std::string getCategories()const {
		std::string result = "Categories: ";
		for (size_t i = 0; i < categories.size(); ++i) {
			result += categories[i];
			if (i < categories.size() - 1) result += ", ";
		}
		return result;
	}

private: 
	int findCategoryIndex(const std::string& category) const {
		for (size_t i = 0; i < categories.size(); ++i) {
			if (categories[i] == category) {
				return static_cast<int>(i);
			}
		}
		return -1;
	}
};
int main() {
	try {
		// Create a transaction manager
		TransactionManager manager;

		// Create several transactions (without using new)
		Transaction t1(1500.0, Date(5, 10, 2023), "Rent payment", "Expenses");
		Transaction t2(2500.0, Date(10, 10, 2023), "Freelance work", "Income");
		Transaction t3(350.0, Date(15, 10, 2023), "Groceries", "Expenses");

		// Add transactions to the manager
		manager.addTransaction(&t1);
		manager.addTransaction(&t2);
		manager.addTransaction(&t3);

		// Print all transactions before any removal
		std::cout << "Printing all transactions:" << std::endl;
		for (int i = 0; i < 2; i++) {
			Transaction* trans = manager.getTransaction(i);
			trans->printDetails();
		}

		// Remove one transaction
		std::cout << "\nRemoving transaction at index 1 (Freelance work):" << std::endl;
		manager.removeTransaction(1);

		// Print remaining transactions after removal
		std::cout << "\nTransactions after removal:" << std::endl;
		for (int i = 0; i < 2; i++) {
			Transaction* trans = manager.getTransaction(i);
			trans->printDetails();
		}

	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}

	return 0;
}

