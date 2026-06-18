#include "ExpenseTracker.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

ExpenseTracker::ExpenseTracker(std::string filename) : dataFile(filename) {}

void ExpenseTracker::addTransaction(std::unique_ptr<Transaction> t) {
    transactions.push_back(std::move(t));
}

bool ExpenseTracker::deleteTransaction(size_t index) {
    if (index >= transactions.size()) {
        return false;
    }
    transactions.erase(transactions.begin() + index);
    return true;
}

size_t ExpenseTracker::getTransactionsSize() const {
    return transactions.size();
}

void ExpenseTracker::viewTransactions() const {
    if (transactions.empty()) {
        std::cout << "\033[33m目前沒有任何記帳紀錄！\033[0m" << std::endl;
        return;
    }

    std::cout << std::string(70, '-') << std::endl;
    std::cout << std::setw(6) << std::left << "編號" << " | 類型   | 日期         | 分類         | 金額       | 備註" << std::endl;
    std::cout << std::string(70, '-') << std::endl;

    for (size_t i = 0; i < transactions.size(); ++i) {
        std::cout << std::setw(6) << std::left << (i + 1) << " | ";
        transactions[i]->display();
    }
    std::cout << std::string(70, '-') << std::endl;
}

void ExpenseTracker::calculateSummary(double& totalIncome, double& totalExpense) const {
    totalIncome = 0.0;
    totalExpense = 0.0;
    for (const auto& t : transactions) {
        if (t->getType() == "Income") {
            totalIncome += t->getAmount();
        } else if (t->getType() == "Expense") {
            totalExpense += t->getAmount();
        }
    }
}

bool ExpenseTracker::saveToFile() const {
    std::ofstream outFile(dataFile);
    if (!outFile) {
        return false;
    }
    for (const auto& t : transactions) {
        outFile << t->serialize() << "\n";
    }
    return true;
}

bool ExpenseTracker::loadFromFile() {
    std::ifstream inFile(dataFile);
    if (!inFile) {
        return false;
    }

    transactions.clear();
    std::string line;
    while (std::getline(inFile, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string type, date, category, amountStr, note;

        std::getline(ss, type, '|');
        std::getline(ss, date, '|');
        std::getline(ss, category, '|');
        std::getline(ss, amountStr, '|');
        std::getline(ss, note, '|');

        double amount = 0.0;
        try {
            amount = std::stod(amountStr);
        } catch (...) {
            continue;
        }

        if (type == "INCOME") {
            transactions.push_back(std::unique_ptr<Transaction>(new Income(date, category, amount, note)));
        } else if (type == "EXPENSE") {
            transactions.push_back(std::unique_ptr<Transaction>(new Expense(date, category, amount, note)));
        }
    }
    return true;
}
