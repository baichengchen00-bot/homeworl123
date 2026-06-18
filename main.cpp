#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <ctime>

// ==========================================
// 輔助函式：取得今日日期與清理輸入緩衝區
// ==========================================
std::string getTodayDate() {
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);
    std::stringstream ss;
    ss << (now->tm_year + 1900) << "-"
       << std::setw(2) << std::setfill('0') << (now->tm_mon + 1) << "-"
       << std::setw(2) << std::setfill('0') << now->tm_mday;
    return ss.str();
}

void clearInputBuffer() {
    std::cin.clear();
    // 忽略緩衝區中的所有字元，直到換行符
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// 簡單的日期格式檢查 (YYYY-MM-DD)
bool isValidDate(const std::string& dateStr) {
    if (dateStr.length() != 10) return false;
    if (dateStr[4] != '-' || dateStr[7] != '-') return false;
    for (int i = 0; i < 10; ++i) {
        if (i == 4 || i == 7) continue;
        if (!std::isdigit(dateStr[i])) return false;
    }
    return true;
}

// 清理字串，避免包含 '|' 分隔符號影響存檔
std::string sanitizeString(std::string str) {
    std::replace(str.begin(), str.end(), '|', ' ');
    return str;
}

// ==========================================
// 迭代一：建立資料模型 (C++ 類別繼承與多型)
// ==========================================

// 抽象基礎類別 (Base Class)
class Transaction {
protected:
    std::string date;
    std::string category;
    double amount;
    std::string note;

public:
    Transaction(std::string d, std::string c, double a, std::string n)
        : date(d), category(c), amount(a), note(n) {}

    virtual ~Transaction() {} // 虛擬解構子，確保衍生類別記憶體安全釋放

    // 純虛擬函式 (Pure Virtual Functions) 實現多型
    virtual std::string getType() const = 0;
    virtual void display() const = 0;
    virtual std::string serialize() const = 0;

    // Getter 函式
    std::string getDate() const { return date; }
    std::string getCategory() const { return category; }
    double getAmount() const { return amount; }
    std::string getNote() const { return note; }
};

// 衍生類別：收入 (Derived Class: Income)
class Income : public Transaction {
public:
    Income(std::string d, std::string c, double a, std::string n)
        : Transaction(d, c, a, n) {}

    std::string getType() const override {
        return "Income";
    }

    void display() const override {
        // 綠色高亮顯示收入字樣
        std::cout << "\033[32m[收入]\033[0m "
                  << std::setw(12) << std::left << date
                  << " | 分類: " << std::setw(12) << std::left << category
                  << " | 金額: " << std::setw(10) << std::left << ("+" + std::to_string(amount))
                  << " | 備註: " << note << std::endl;
    }

    std::string serialize() const override {
        return "INCOME|" + date + "|" + category + "|" + std::to_string(amount) + "|" + note;
    }
};

// 衍生類別：支出 (Derived Class: Expense)
class Expense : public Transaction {
public:
    Expense(std::string d, std::string c, double a, std::string n)
        : Transaction(d, c, a, n) {}

    std::string getType() const override {
        return "Expense";
    }

    void display() const override {
        // 紅色高亮顯示支出字樣
        std::cout << "\033[31m[支出]\033[0m "
                  << std::setw(12) << std::left << date
                  << " | 分類: " << std::setw(12) << std::left << category
                  << " | 金額: " << std::setw(10) << std::left << ("-" + std::to_string(amount))
                  << " | 備註: " << note << std::endl;
    }

    std::string serialize() const override {
        return "EXPENSE|" + date + "|" + category + "|" + std::to_string(amount) + "|" + note;
    }
};

// ==========================================
// 迭代二：實作 `ExpenseTracker` 核心管理器與檔案 I/O
// ==========================================
class ExpenseTracker {
private:
    // 使用 STL std::vector 與 std::unique_ptr 進行記憶體自動管理
    std::vector<std::unique_ptr<Transaction>> transactions;
    std::string dataFile;

public:
    ExpenseTracker(std::string filename) : dataFile(filename) {}

    // 新增帳目
    void addTransaction(std::unique_ptr<Transaction> t) {
        transactions.push_back(std::move(t)); // 移轉 unique_ptr 所有權
    }

    // 刪除帳目 (以 0-based 索引刪除)
    bool deleteTransaction(size_t index) {
        if (index >= transactions.size()) {
            return false;
        }
        transactions.erase(transactions.begin() + index);
        return true;
    }

    // 取得帳目總數
    size_t getTransactionsSize() const {
        return transactions.size();
    }

    // 查看所有紀錄
    void viewTransactions() const {
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

    // 計算財務摘要
    void calculateSummary(double& totalIncome, double& totalExpense) const {
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

    // 寫檔功能 (Save)
    bool saveToFile() const {
        std::ofstream outFile(dataFile);
        if (!outFile) {
            return false;
        }
        for (const auto& t : transactions) {
            outFile << t->serialize() << "\n";
        }
        return true;
    }

    // 讀檔功能 (Load)
    bool loadFromFile() {
        std::ifstream inFile(dataFile);
        if (!inFile) {
            // 檔案不存在不視為錯誤，只代表是新啟動無舊資料
            return false;
        }

        transactions.clear(); // 清空舊資料
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
                continue; // 金額解析錯誤則跳過該行
            }

            if (type == "INCOME") {
                transactions.push_back(std::unique_ptr<Transaction>(new Income(date, category, amount, note)));
            } else if (type == "EXPENSE") {
                transactions.push_back(std::unique_ptr<Transaction>(new Expense(date, category, amount, note)));
            }
        }
        return true;
    }
};

// ==========================================
// 迭代三：打造終端機 UI 互動選單 (防呆輸入)
// ==========================================
void showMenu() {
    std::cout << "\n\033[36m==================================================\033[0m" << std::endl;
    std::cout << "\033[36m          個人簡易記帳本 (C++ 專題版)            \033[0m" << std::endl;
    std::cout << "\033[36m==================================================\033[0m" << std::endl;
    std::cout << " 1. 新增「收入」項目" << std::endl;
    std::cout << " 2. 新增「支出」項目" << std::endl;
    std::cout << " 3. 查看所有收支紀錄" << std::endl;
    std::cout << " 4. 刪除指定編號項目" << std::endl;
    std::cout << " 5. 顯示財務摘要與餘額" << std::endl;
    std::cout << " 6. 儲存變更並結束程式" << std::endl;
    std::cout << "\033[36m==================================================\033[0m" << std::endl;
    std::cout << "請輸入操作選項 (1-6): ";
}

int main() {
    // 初始化記帳本，存檔檔案名稱設為 records.txt
    ExpenseTracker tracker("records.txt");
    
    std::cout << "正在載入過往紀錄..." << std::endl;
    if (tracker.loadFromFile()) {
        std::cout << "\033[32m成功載入歷史資料！\033[0m" << std::endl;
    } else {
        std::cout << "\033[33m未偵測到過往檔案，將為您建立新的記帳本。\033[0m" << std::endl;
    }

    int choice = 0;
    while (true) {
        showMenu();
        if (!(std::cin >> choice)) {
            std::cout << "\033[31m[錯誤] 請輸入有效的選項數字！\033[0m" << std::endl;
            clearInputBuffer();
            continue;
        }
        clearInputBuffer(); // 清理換行符號

        if (choice == 6) {
            std::cout << "\n正在將資料儲存至 records.txt..." << std::endl;
            if (tracker.saveToFile()) {
                std::cout << "\033[32m資料儲存成功！感謝您的使用。\033[0m" << std::endl;
            } else {
                std::cout << "\033[31m[錯誤] 資料儲存失敗！請檢查檔案寫入權限。\033[0m" << std::endl;
            }
            break;
        }

        std::string date, category, note;
        double amount = 0.0;

        switch (choice) {
            case 1: // 新增收入
            case 2: // 新增支出
                {
                    std::string actionName = (choice == 1) ? "收入" : "支出";
                    std::cout << "\n--- 新增" << actionName << "項目 ---" << std::endl;

                    // 1. 輸入日期
                    while (true) {
                        std::cout << "請輸入日期 (YYYY-MM-DD，按 Enter 鍵預設為今日 " << getTodayDate() << "): ";
                        std::getline(std::cin, date);
                        if (date.empty()) {
                            date = getTodayDate();
                            break;
                        }
                        if (isValidDate(date)) {
                            break;
                        }
                        std::cout << "\033[31m[錯誤] 日期格式不符，請使用 YYYY-MM-DD 格式！\033[0m" << std::endl;
                    }

                    // 2. 輸入分類
                    std::cout << "請輸入分類 (例如: " << (choice == 1 ? "薪資/獎金/投資" : "餐飲/交通/娛樂") << "): ";
                    std::getline(std::cin, category);
                    if (category.empty()) category = "未分類";
                    category = sanitizeString(category);

                    // 3. 輸入金額
                    while (true) {
                        std::cout << "請輸入金額 (正數): ";
                        if (std::cin >> amount && amount > 0) {
                            clearInputBuffer();
                            break;
                        }
                        std::cout << "\033[31m[錯誤] 金額必須大於 0 且為有效數字！\033[0m" << std::endl;
                        clearInputBuffer();
                    }

                    // 4. 輸入備註
                    std::cout << "請輸入備註 (選填): ";
                    std::getline(std::cin, note);
                    note = sanitizeString(note);
                    if (note.empty()) note = "無";

                    // 5. 使用多型建立對應的子類別物件，並加入 tracker
                    if (choice == 1) {
                        tracker.addTransaction(std::unique_ptr<Transaction>(new Income(date, category, amount, note)));
                    } else {
                        tracker.addTransaction(std::unique_ptr<Transaction>(new Expense(date, category, amount, note)));
                    }

                    std::cout << "\033[32m[成功] 已成功新增" << actionName << "紀錄！\033[0m" << std::endl;
                }
                break;

            case 3: // 查看所有紀錄
                std::cout << "\n--- 所有收支紀錄清單 ---" << std::endl;
                tracker.viewTransactions();
                break;

            case 4: // 刪除指定項目
                {
                    std::cout << "\n--- 刪除收支項目 ---" << std::endl;
                    tracker.viewTransactions();
                    if (tracker.getTransactionsSize() == 0) {
                        break;
                    }

                    size_t deleteIdx = 0;
                    while (true) {
                        std::cout << "請輸入要刪除的項目編號 (1 - " << tracker.getTransactionsSize() << "): ";
                        if (std::cin >> deleteIdx && deleteIdx >= 1 && deleteIdx <= tracker.getTransactionsSize()) {
                            clearInputBuffer();
                            break;
                        }
                        std::cout << "\033[31m[錯誤] 輸入編號超出範圍或格式無效！\033[0m" << std::endl;
                        clearInputBuffer();
                    }

                    if (tracker.deleteTransaction(deleteIdx - 1)) {
                        std::cout << "\033[32m[成功] 已成功刪除編號 " << deleteIdx << " 的紀錄！\033[0m" << std::endl;
                    } else {
                        std::cout << "\033[31m[錯誤] 刪除失敗！\033[0m" << std::endl;
                    }
                }
                break;

            case 5: // 顯示財務摘要與餘額
                {
                    double totalIncome = 0.0;
                    double totalExpense = 0.0;
                    tracker.calculateSummary(totalIncome, totalExpense);
                    double balance = totalIncome - totalExpense;

                    std::cout << "\n\033[33m--- 財務摘要與統計 ---\033[0m" << std::endl;
                    std::cout << "總收入: \033[32m+" << totalIncome << " 元\033[0m" << std::endl;
                    std::cout << "總支出: \033[31m-" << totalExpense << " 元\033[0m" << std::endl;
                    std::cout << std::string(30, '-') << std::endl;
                    
                    if (balance >= 0) {
                        std::cout << "目前餘額: \033[32m+" << balance << " 元\033[0m (財務狀態良好)" << std::endl;
                    } else {
                        std::cout << "目前餘額: \033[31m" << balance << " 元\033[0m (警告: 收支失衡！)" << std::endl;
                    }
                }
                break;

            default:
                std::cout << "\033[31m[錯誤] 無效的選項，請輸入 1 至 6 之間的數字！\033[0m" << std::endl;
                break;
        }
    }

    return 0;
}
