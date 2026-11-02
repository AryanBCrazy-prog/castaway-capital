#include "Wallet.hpp"
#include <iostream>
#include <algorithm>

Wallet::Wallet() {
    m_notes[500] = 0;
    m_notes[100] = 0;
    m_notes[50]  = 0;
    m_notes[20]  = 0;
    m_notes[10]  = 0;
    m_notes[5]   = 0;
    m_notes[1]   = 0;
}

void Wallet::initializeStarterKit() {
    // Exact starter distribution:
    // 2 x 500 (Purple)  = 1000
    // 2 x 100 (Teal)    =  200
    // 2 x  50 (Orange)  =  100
    // 6 x  20 (Green)   =  120
    // 5 x  10 (Yellow)  =   50
    // 5 x   5 (Pink)    =   25
    // 5 x   1 (White)   =    5
    // Total = 1,500 Credits
    m_notes[500] = 2;
    m_notes[100] = 2;
    m_notes[50]  = 2;
    m_notes[20]  = 6;
    m_notes[10]  = 5;
    m_notes[5]   = 5;
    m_notes[1]   = 5;
}

void Wallet::collectBaseCampSalary() {
    // Base Camp Salary Distribution: 200 Credits
    // 1x 100, 1x 50, 2x 20, 1x 10 = 200
    m_notes[100] += 1;
    m_notes[50]  += 1;
    m_notes[20]  += 2;
    m_notes[10]  += 1;
}

int Wallet::getTotalCapital() const {
    int total = 0;
    for (const auto& [denom, count] : m_notes) {
        total += denom * count;
    }
    return total;
}

int Wallet::getNoteCount(int denom) const {
    auto it = m_notes.find(denom);
    return (it != m_notes.end()) ? it->second : 0;
}

void Wallet::addNotes(int denom, int count) {
    if (count > 0 && m_notes.find(denom) != m_notes.end()) {
        m_notes[denom] += count;
    }
}

void Wallet::addCash(int amount) {
    if (amount <= 0) return;
    const std::vector<int> denoms = {500, 100, 50, 20, 10, 5, 1};
    int rem = amount;
    for (int d : denoms) {
        int count = rem / d;
        if (count > 0) {
            m_notes[d] += count;
            rem %= d;
        }
    }
}

void Wallet::breakNoteAtBank(int denom) {
    if (m_notes[denom] <= 0) return;
    m_notes[denom]--;

    switch (denom) {
        case 500:
            m_notes[100] += 3;
            m_notes[50]  += 2;
            m_notes[20]  += 4;
            m_notes[10]  += 1;
            m_notes[5]   += 1;
            m_notes[1]   += 5; // 300 + 100 + 80 + 10 + 5 + 5 = 500
            break;
        case 100:
            m_notes[50] += 1;
            m_notes[20] += 2;
            m_notes[10] += 1; // 50 + 40 + 10 = 100
            break;
        case 50:
            m_notes[20] += 2;
            m_notes[10] += 1; // 40 + 10 = 50
            break;
        case 20:
            m_notes[10] += 1;
            m_notes[5]  += 2; // 10 + 10 = 20
            break;
        case 10:
            m_notes[5] += 1;
            m_notes[1] += 5; // 5 + 5 = 10
            break;
        case 5:
            m_notes[1] += 5; // 5 x 1 = 5
            break;
        default:
            m_notes[denom]++;
            break;
    }
}

bool Wallet::spendCash(int amount) {
    if (getTotalCapital() < amount) return false;
    if (amount <= 0) return true;

    const std::vector<int> denoms = {500, 100, 50, 20, 10, 5, 1};

    int iterations = 40;
    while (iterations-- > 0) {
        // Attempt greedy exact deduction
        int rem = amount;
        std::map<int, int> tempNotes = m_notes;
        for (int d : denoms) {
            int take = std::min(tempNotes[d], rem / d);
            rem -= take * d;
            tempNotes[d] -= take;
        }

        if (rem == 0) {
            m_notes = tempNotes;
            return true;
        }

        // Automatic Bank note breakdown: find smallest note > 1 to break
        int targetNote = -1;
        for (auto it = denoms.rbegin(); it != denoms.rend(); ++it) {
            int d = *it;
            if (d > 1 && m_notes[d] > 0) {
                targetNote = d;
                if (d >= rem) break; // best match
            }
        }

        if (targetNote != -1) {
            breakNoteAtBank(targetNote);
        } else {
            break;
        }
    }

    // Direct fallback reconstruct if needed
    int remainingBalance = getTotalCapital() - amount;
    for (int d : denoms) m_notes[d] = 0;
    addCash(remainingBalance);
    return true;
}

std::string Wallet::getColoredSummary() const {
    std::ostringstream oss;
    oss << DenomColor::C_500 << "[500: Purple x" << getNoteCount(500) << "] "
        << DenomColor::C_100 << "[100: Teal x" << getNoteCount(100) << "] "
        << DenomColor::C_50  << "[50: Orange x" << getNoteCount(50) << "] "
        << DenomColor::C_20  << "[20: Green x" << getNoteCount(20) << "] "
        << DenomColor::C_10  << "[10: Yellow x" << getNoteCount(10) << "] "
        << DenomColor::C_5   << "[5: Pink x" << getNoteCount(5) << "] "
        << DenomColor::C_1   << "[1: White x" << getNoteCount(1) << "]"
        << UI::RESET << " = " << UI::GREEN << UI::BOLD << "$" << getTotalCapital() << " Credits" << UI::RESET;
    return oss.str();
}

std::string Wallet::toJSON() const {
    std::ostringstream oss;
    oss << "{"
        << "\"500\":" << getNoteCount(500) << ","
        << "\"100\":" << getNoteCount(100) << ","
        << "\"50\":"  << getNoteCount(50)  << ","
        << "\"20\":"  << getNoteCount(20)  << ","
        << "\"10\":"  << getNoteCount(10)  << ","
        << "\"5\":"   << getNoteCount(5)   << ","
        << "\"1\":"   << getNoteCount(1)   << ","
        << "\"total\":" << getTotalCapital()
        << "}";
    return oss.str();
}
