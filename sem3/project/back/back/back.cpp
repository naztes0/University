#include<iostream>
#include<string>
#include<stdexcept> // for reporting about exceptions

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