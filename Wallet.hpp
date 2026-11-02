#pragma once
#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include "Enums.hpp"

// Physical Note Denominations
namespace DenomColor {
    inline const std::string C_500 = "\033[38;5;129m"; // Purple
    inline const std::string C_100 = "\033[38;5;38m";  // Teal / Cyan
    inline const std::string C_50  = "\033[38;5;208m"; // Orange
    inline const std::string C_20  = "\033[38;5;34m";  // Green
    inline const std::string C_10  = "\033[38;5;220m"; // Yellow
    inline const std::string C_5   = "\033[38;5;205m"; // Pink
    inline const std::string C_1   = "\033[38;5;255m"; // White
}

class Wallet {
private:
    // Key: Denomination (500, 100, 50, 20, 10, 5, 1), Value: count
    std::map<int, int> m_notes;

    // Helper: break down note into smaller denominations via Bank
    void breakNoteAtBank(int denom);

public:
    Wallet();

    // Starter Bankroll: 1,500 Credits in physical notes
    // 2x 500 (Purple), 2x 100 (Teal), 2x 50 (Orange), 6x 20 (Green), 5x 10 (Yellow), 5x 5 (Pink), 5x 1 (White)
    void initializeStarterKit();

    // Base Camp Salary Distribution: 200 Credits
    // 1x 100, 1x 50, 2x 20, 1x 10 = 200
    void collectBaseCampSalary();

    int getTotalCapital() const;
    int getNoteCount(int denom) const;
    const std::map<int, int>& getAllNotes() const { return m_notes; }

    void addNotes(int denom, int count);
    void addCash(int amount);

    // Greedy change-maker algorithm with automatic Bank note breakdown
    bool spendCash(int amount);

    // ANSI Color-coded Wallet ledger
    std::string getColoredSummary() const;

    // JSON serialization for 3D web synchronization
    std::string toJSON() const;
};
