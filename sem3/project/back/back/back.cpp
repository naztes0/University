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
		std::string dayString = (day < 10 ? "0" : "") + std::to_string(day);
		std::string monthString = (month < 10 ? "0" : "") + std::to_string(month);
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

class IncomeTransaction :public Transaction {
private:
	std::string source;
public:
	IncomeTransaction(double amount, const Date& dt, const std::string& descrip, const std::string& source)
		:Transaction(amount, dt, descrip, "Income"), source(source) {

	}
	void printDetails() const override {
		Transaction::printDetails();
		std::cout << "Source: " << source << std::endl;
	}
	std::string getSource()const { return source; }
	void setSource(const std::string& newSource) { source = newSource; }

};

class ExpenseTransaction : public Transaction {
private:
	std::string category;
public:
	ExpenseTransaction(double amount, const Date& dt, const std::string& descrip, const std::string& category)
		: Transaction(amount, dt, descrip, category) {
		
	}
	void printDetails() const override {
		Transaction::printDetails(); 
		std::cout << "Category: " << category << std::endl; 
	}

	
	std::string getCategory() const { return category; }
	void setCategory(const std::string& newCategory) { category = newCategory; }
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
	/*~TransactionManager() {
		for (Transaction* trans : transactions) {
			delete trans;
		}
	}*/
	
	
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
				transactions.erase(transactions.begin() + index); // Remove from vector
		}

		else {
			std::cerr << "Error: Invalid index!" << std::endl;
		}
	}
	Transaction* getTransaction(int index) {
		if (indexChecker(index)) {
			if (transactions[index] != nullptr) {
				return transactions[index];
			}
			else {
				std::cerr << "Error: Attempted to access a null transaction." << std::endl;
				return nullptr;
			}
		}
		return nullptr;
	}

	size_t getTransactionCount() const {
		return transactions.size();
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
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>

// Assume the classes Date, Transaction, TransactionManager, and CategoryManager are already implemented as discussed above.

int main() {
	try {
		// =======================
		// Testing the Date class
		// =======================
		std::cout << "Testing Date class:\n";
		Date date1(5, 10, 2023);
		Date date2(15, 11, 2023);
		std::cout << "Date 1: " << date1.toString() << "\n";
		std::cout << "Date 2: " << date2.toString() << "\n";
		std::cout << "Is Date 1 earlier than Date 2? " << (date1.isEarlier(date1, date2) ? "Yes" : "No") << "\n\n";

		// ==============================
		// Testing the Transaction class
		// ==============================
		std::cout << "Testing Transaction class:\n";
		Transaction t1(1500.0, Date(5, 10, 2023), "Rent payment", "Expenses");
		Transaction t2(2500.0, Date(10, 10, 2023), "Freelance work", "Income");
		Transaction t3(350.0, Date(15, 10, 2023), "Groceries", "Expenses");

		t1.printDetails();
		t2.printDetails();
		t3.printDetails();
		std::cout << "\n";

		// =====================================
		// Testing the TransactionManager class
		// =====================================
		std::cout << "Testing TransactionManager class:\n";
		TransactionManager manager;

		manager.addTransaction(&t1);
		manager.addTransaction(&t2);
		manager.addTransaction(&t3);

		std::cout << "All transactions:\n";
		for (size_t i = 0; i < manager.getTransactionCount(); ++i) {
			manager.getTransaction(i)->printDetails();
		}

		// Remove a transaction and test
		std::cout << "\nRemoving transaction at index 1 (Freelance work):\n";
		manager.removeTransaction(0);

		std::cout << "Transactions after removal:\n";
		for (size_t i = 0; i < manager.getTransactionCount(); ++i) {
			manager.getTransaction(i)->printDetails();
		}
		std::cout << "\n";

		// =====================================
		// Testing the CategoryManager class
		// =====================================
		std::cout << "Testing CategoryManager class:\n";
		CategoryManager categoryManager;
		categoryManager.addCategory("Expenses");
		categoryManager.addCategory("Income");
		categoryManager.addCategory("Savings");

		std::cout << categoryManager.getCategories() << "\n";

		// Remove a category and test
		std::string categoryToRemove = "Savings";
		std::cout << "\nRemoving category '" << categoryToRemove << "'...\n";
		categoryManager.removeCategory(categoryToRemove);

		std::cout << categoryManager.getCategories() << "\n";

	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}

	return 0;
}



