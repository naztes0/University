#include<iostream>
#include<string>
#include<stdexcept> // for reporting about exceptions
#include<vector>


//Basic class for financial operations
class Transaction {
protected:
		double amount;
		std::string date;
		std::string description;
		std::string category;
public:
	//Constructor for init
	Transaction(double amount, const std::string& dt, const std::string& descrip, const std::string& categ)
		:amount(amount), date(dt), description(descrip), category(categ) {
		if (!isValid()) {
			throw std::invalid_argument("Invalid transaction data");
		}
	}
	//Destructor
	virtual ~Transaction() = default;

	double getAmount() const { return amount;}
	std::string getDate() const { return date; }
	std::string getDescription() const { return description;}
	std::string getCategory() const { return category; }

	void setAmount(double amt) { amount = amt; }
	void setDate(const std::string& dt) { date = dt; }
	void setDecription(const std::string& desc) { description = desc; }
	void setCategory(const std::string& categ) { category = categ; }

	virtual void printDetails()const {
		std::cout << "Transaction Details:\n";
		std::cout << "Amount: " << amount
			<< ", Date" << date << ", Description: " << description
			<< ", Category: " << category << std::endl;

	}


	bool isValid() const {
		return !date.empty() && !category.empty() && amount != 0;
	}

};


class Date {
private:
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

class TransactionManager {
private:
	std::vector<Transaction*> transactions;
	std::string databasePath;

public:
	/*
	* add loading transactions from the database for future operation with them
	*/
	

};

int main() {
	try {
		Transaction t(1894.34, "29/09/24", "Salary", "Income");
		t.printDetails();
		t.setDecription("Monthly salary");
		t.printDetails();
	}
	catch(std::exception& e){
		std::cerr << "Error: " << e.what() << std::endl;
	}

	return 0;
}