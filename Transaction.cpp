#include "Transaction.h"

Transaction::Transaction(std::string d, std::string c, double a, std::string n)
    : date(d), category(c), amount(a), note(n) {}

Transaction::~Transaction() {}

// ==========================================
// Income 實作
// ==========================================
Income::Income(std::string d, std::string c, double a, std::string n)
    : Transaction(d, c, a, n) {}

std::string Income::getType() const {
    return "Income";
}

void Income::display() const {
    std::cout << "\033[32m[收入]\033[0m "
              << std::setw(12) << std::left << date
              << " | 分類: " << std::setw(12) << std::left << category
              << " | 金額: " << std::setw(10) << std::left << ("+" + std::to_string(amount))
              << " | 備註: " << note << std::endl;
}

std::string Income::serialize() const {
    return "INCOME|" + date + "|" + category + "|" + std::to_string(amount) + "|" + note;
}

// ==========================================
// Expense 實作
// ==========================================
Expense::Expense(std::string d, std::string c, double a, std::string n)
    : Transaction(d, c, a, n) {}

std::string Expense::getType() const {
    return "Expense";
}

void Expense::display() const {
    std::cout << "\033[31m[支出]\033[0m "
              << std::setw(12) << std::left << date
              << " | 分類: " << std::setw(12) << std::left << category
              << " | 金額: " << std::setw(10) << std::left << ("-" + std::to_string(amount))
              << " | 備註: " << note << std::endl;
}

std::string Expense::serialize() const {
    return "EXPENSE|" + date + "|" + category + "|" + std::to_string(amount) + "|" + note;
}
