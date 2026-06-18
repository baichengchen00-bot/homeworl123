#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include <iostream>
#include <iomanip>

// ==========================================
// 抽象基礎類別 (Base Class)
// ==========================================
class Transaction {
protected:
    std::string date;
    std::string category;
    double amount;
    std::string note;

public:
    Transaction(std::string d, std::string c, double a, std::string n);
    virtual ~Transaction(); // 虛擬解構子

    virtual std::string getType() const = 0;
    virtual void display() const = 0;
    virtual std::string serialize() const = 0;

    std::string getDate() const { return date; }
    std::string getCategory() const { return category; }
    double getAmount() const { return amount; }
    std::string getNote() const { return note; }
};

// ==========================================
// 衍生類別：收入 (Derived Class: Income)
// ==========================================
class Income : public Transaction {
public:
    Income(std::string d, std::string c, double a, std::string n);
    std::string getType() const override;
    void display() const override;
    std::string serialize() const override;
};

// ==========================================
// 衍生類別：支出 (Derived Class: Expense)
// ==========================================
class Expense : public Transaction {
public:
    Expense(std::string d, std::string c, double a, std::string n);
    std::string getType() const override;
    void display() const override;
    std::string serialize() const override;
};

#endif // TRANSACTION_H
