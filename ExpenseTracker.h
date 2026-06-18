#ifndef EXPENSETRACKER_H
#define EXPENSETRACKER_H

#include "Transaction.h"
#include <vector>
#include <memory>
#include <string>

class ExpenseTracker {
private:
    std::vector<std::unique_ptr<Transaction>> transactions;
    std::string dataFile;

public:
    ExpenseTracker(std::string filename);

    void addTransaction(std::unique_ptr<Transaction> t);
    bool deleteTransaction(size_t index);
    size_t getTransactionsSize() const;
    void viewTransactions() const;
    void calculateSummary(double& totalIncome, double& totalExpense) const;
    bool saveToFile() const;
    bool loadFromFile();
};

#endif // EXPENSETRACKER_H
