#pragma once
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include "Enums.hpp"
#include "Wallet.hpp"

class PropertyTile;
class UtilityTile;
class GameEngine;

class Player {
private:
    int m_id;
    std::string m_name;
    std::string m_colorName; // "Red", "Blue", "Green", "Gold"
    std::string m_hexColor;  // e.g. "#EF4444"
    Wallet m_wallet;
    int m_position;
    bool m_isBot;
    bool m_isBankrupt;
    bool m_hasFinanceLicense;
    int m_tamperCharges; // Max Autos dice tampering charges for the current round
    std::vector<int> m_ownedPropertyIndices;
    std::vector<int> m_ownedUtilityIndices;

public:
    Player(int id, std::string name, std::string colorName, std::string hexColor, bool isBot = false);

    int getId() const { return m_id; }
    const std::string& getName() const { return m_name; }
    const std::string& getColorName() const { return m_colorName; }
    const std::string& getHexColor() const { return m_hexColor; }

    int getCapital() const { return m_wallet.getTotalCapital(); }
    const Wallet& getWallet() const { return m_wallet; }
    Wallet& getWallet() { return m_wallet; }

    int getPosition() const { return m_position; }
    bool isBot() const { return m_isBot; }
    bool isBankrupt() const { return m_isBankrupt; }
    bool hasFinanceLicense() const { return m_hasFinanceLicense; }
    int getTamperCharges() const { return m_tamperCharges; }

    void setBankrupt(bool bankrupt) { m_isBankrupt = bankrupt; }
    void setFinanceLicense(bool hasLicense) { m_hasFinanceLicense = hasLicense; }
    void setTamperCharges(int charges) { m_tamperCharges = charges; }
    void decrementTamperCharge() { if (m_tamperCharges > 0) m_tamperCharges--; }

    void addCapital(int amount);
    void collectSalary(); // Collects structured note salary: 1x100, 1x50, 2x20, 1x10 = 200
    bool deductCapital(int amount); // returns true if covered, false if insufficient
    void setPosition(int pos) { m_position = pos; }
    int advancePosition(int steps, int boardSize);

    const std::vector<int>& getOwnedPropertyIndices() const { return m_ownedPropertyIndices; }
    const std::vector<int>& getOwnedUtilityIndices() const { return m_ownedUtilityIndices; }

    void addProperty(int tileIndex);
    void removeProperty(int tileIndex);
    void addUtility(int tileIndex);
    void removeUtility(int tileIndex);

    bool ownsMaxAutos() const { return !m_ownedUtilityIndices.empty(); }

    // Bot AI heuristic decision helpers
    bool botShouldBuyProperty(const PropertyTile& property) const;
    bool botShouldBuyUtility(const UtilityTile& utility) const;
    bool botShouldBuyLicense(int licenseCost) const;
    bool botShouldRepair(const PropertyTile& property) const;
    bool botShouldUpgrade(const PropertyTile& property) const;
    bool botShouldTamperOpponent(const Player& target, int projectedPosition, GameEngine& engine) const;
};
