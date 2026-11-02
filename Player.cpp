#include "Player.hpp"
#include "Tile.hpp"
#include "GameEngine.hpp"
#include <iostream>

Player::Player(int id, std::string name, std::string colorName, std::string hexColor, bool isBot)
    : m_id(id), m_name(std::move(name)), m_colorName(std::move(colorName)), m_hexColor(std::move(hexColor)),
      m_position(0), m_isBot(isBot), m_isBankrupt(false),
      m_hasFinanceLicense(false), m_tamperCharges(0) {
    m_wallet.initializeStarterKit();
}

void Player::addCapital(int amount) {
    m_wallet.addCash(amount);
}

void Player::collectSalary() {
    m_wallet.collectBaseCampSalary();
}

bool Player::deductCapital(int amount) {
    return m_wallet.spendCash(amount);
}

int Player::advancePosition(int steps, int boardSize) {
    int oldPos = m_position;
    m_position = (m_position + steps) % boardSize;
    return (m_position < oldPos) ? 1 : 0; // returns 1 if wrapped around Go/BaseCamp
}

void Player::addProperty(int tileIndex) {
    if (std::find(m_ownedPropertyIndices.begin(), m_ownedPropertyIndices.end(), tileIndex) == m_ownedPropertyIndices.end()) {
        m_ownedPropertyIndices.push_back(tileIndex);
    }
}

void Player::removeProperty(int tileIndex) {
    m_ownedPropertyIndices.erase(
        std::remove(m_ownedPropertyIndices.begin(), m_ownedPropertyIndices.end(), tileIndex),
        m_ownedPropertyIndices.end()
    );
}

void Player::addUtility(int tileIndex) {
    if (std::find(m_ownedUtilityIndices.begin(), m_ownedUtilityIndices.end(), tileIndex) == m_ownedUtilityIndices.end()) {
        m_ownedUtilityIndices.push_back(tileIndex);
    }
}

void Player::removeUtility(int tileIndex) {
    m_ownedUtilityIndices.erase(
        std::remove(m_ownedUtilityIndices.begin(), m_ownedUtilityIndices.end(), tileIndex),
        m_ownedUtilityIndices.end()
    );
}

// Bot AI Decision Heuristics
bool Player::botShouldBuyProperty(const PropertyTile& property) const {
    // Keep a buffer of at least $100 for survival
    return getCapital() >= (property.getCost() + 100);
}

bool Player::botShouldBuyUtility(const UtilityTile& utility) const {
    // Transit routes give monopoly power
    return getCapital() >= (utility.getCost() + 80);
}

bool Player::botShouldBuyLicense(int licenseCost) const {
    // 5% cut on all island rents and loans is huge long-term
    return getCapital() >= (licenseCost + 120);
}

bool Player::botShouldRepair(const PropertyTile& property) const {
    // Repair ruined properties so they start generating rent again
    return getCapital() >= (property.getRepairCost() + 80);
}

bool Player::botShouldUpgrade(const PropertyTile& property) const {
    // Upgrades increase rent, do it when capital buffer is safe
    return getCapital() >= (property.getUpgradeCost() + 180);
}

bool Player::botShouldTamperOpponent(const Player& target, [[maybe_unused]] int projectedPosition, GameEngine& engine) const {
    if (m_tamperCharges <= 0) return false;

    // Check if within 1 to 6 steps from target there is a high-hazard sector or bot-owned property
    int targetPos = target.getPosition();
    const Board& board = engine.getBoard();

    for (int step = 1; step <= 6; ++step) {
        int checkPos = (targetPos + step) % board.size();
        const Tile* tile = board.getTile(checkPos);
        if (!tile) continue;

        if (tile->getType() == TileType::PROPERTY) {
            const auto* prop = dynamic_cast<const PropertyTile*>(tile);
            if (prop && prop->getOwnerId() == m_id && prop->getState() != PropertyState::RUINED) {
                return true;
            }
        } else if (tile->getType() == TileType::EVENT) {
            return true;
        }
    }

    return false;
}
