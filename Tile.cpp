#include "Tile.hpp"
#include "Player.hpp"
#include "GameEngine.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>

// Base Tile Implementation
Tile::Tile(int index, std::string name, TileType type, SectorGroup sector)
    : m_index(index), m_name(std::move(name)), m_type(type), m_sector(sector) {}

std::string Tile::getDisplayInfo() const {
    std::ostringstream oss;
    oss << "#" << std::setw(2) << std::setfill('0') << m_index << " "
        << std::setfill(' ') << std::left << std::setw(24) << m_name
        << " [" << sectorGroupToString(m_sector) << "]";
    return oss.str();
}

// BaseCampTile Implementation
BaseCampTile::BaseCampTile(int index, std::string name, int reward, bool isSafeHaven)
    : Tile(index, std::move(name), TileType::BASE_CAMP, SectorGroup::SPECIAL),
      m_reward(reward), m_isSafeHaven(isSafeHaven) {}

void BaseCampTile::onLand(Player& player, GameEngine& engine) {
    if (m_index == 0) {
        player.collectSalary();
        std::string msg = "🏕️ " + player.getName() + " landed at Base Camp Alpha and collected $200 Salary in structured notes (1x 100, 1x 50, 2x 20, 1x 10)!";
        engine.logEvent(msg);
    } else {
        player.addCapital(m_reward);
        std::string msg = "🏕️ " + player.getName() + " reached " + m_name +
                          " and collected $" + std::to_string(m_reward) + " in emergency survival supplies!";
        engine.logEvent(msg);
    }
}

std::string BaseCampTile::getDisplayInfo() const {
    std::ostringstream oss;
    oss << Tile::getDisplayInfo() << " | Supply Drop: $" << m_reward;
    if (m_isSafeHaven) oss << " (Safe Haven)";
    return oss.str();
}

// PropertyTile Implementation
PropertyTile::PropertyTile(int index, std::string name, SectorGroup sector,
                           int cost, int baseRent, int fortifiedRent,
                           int repairCost, int upgradeCost)
    : Tile(index, std::move(name), TileType::PROPERTY, sector),
      m_cost(cost), m_baseRent(baseRent), m_fortifiedRent(fortifiedRent),
      m_repairCost(repairCost), m_upgradeCost(upgradeCost),
      m_mortgageValue(cost / 2),
      m_state(PropertyState::PRISTINE),
      m_ownerId(-1),
      m_isMortgaged(false) {}

int PropertyTile::calculateRent() const {
    if (m_isMortgaged) return 0;
    switch (m_state) {
        case PropertyState::RUINED:    return 0;
        case PropertyState::PRISTINE:  return m_baseRent;
        case PropertyState::FORTIFIED: return m_fortifiedRent;
        default:                       return 0;
    }
}

void PropertyTile::ruin() {
    m_state = PropertyState::RUINED;
}

bool PropertyTile::repair() {
    if (m_state == PropertyState::RUINED) {
        m_state = PropertyState::PRISTINE;
        return true;
    }
    return false;
}

bool PropertyTile::upgrade() {
    if (m_state == PropertyState::PRISTINE) {
        m_state = PropertyState::FORTIFIED;
        return true;
    }
    return false;
}

void PropertyTile::onLand(Player& player, GameEngine& engine) {
    if (m_ownerId == -1) {
        // Unowned Property
        std::cout << UI::CYAN << "\n>>> " << player.getName() << " discovered unmapped territory: " 
                  << m_name << " (" << sectorGroupToString(m_sector) << " Sector)!" << UI::RESET << "\n";
        std::cout << "Claim Price: $" << m_cost 
                  << " | Pristine Rent: $" << m_baseRent 
                  << " | Fortified Rent: $" << m_fortifiedRent << "\n";

        bool wantToBuy = false;
        if (player.getCapital() >= m_cost) {
            if (player.isBot()) {
                wantToBuy = player.botShouldBuyProperty(*this);
            } else {
                std::cout << "Your Capital: $" << player.getCapital() << ". Claim this sector? (y/n): ";
                char choice;
                std::cin >> choice;
                wantToBuy = (choice == 'y' || choice == 'Y');
            }
        } else {
            std::cout << UI::YELLOW << player.getName() << " does not have enough capital ($" 
                      << player.getCapital() << "/$" << m_cost << ") to claim this sector.\n" << UI::RESET;
        }

        if (wantToBuy) {
            player.deductCapital(m_cost);
            m_ownerId = player.getId();
            player.addProperty(m_index);
            std::string log = "🏝️ " + player.getName() + " acquired " + m_name + " for $" + std::to_string(m_cost) + "!";
            engine.logEvent(log);
            std::cout << UI::GREEN << log << UI::RESET << "\n";
        }
    } else if (m_ownerId == player.getId()) {
        std::cout << UI::GREEN << "\n>>> " << player.getName() << " visited their own sector: " 
                  << m_name << " " << propertyStateToString(m_state) << UI::RESET << "\n";
    } else {
        // Owned by opponent
        Player* owner = engine.getPlayer(m_ownerId);
        if (owner && !owner->isBankrupt()) {
            std::cout << UI::YELLOW << "\n>>> " << player.getName() << " stumbled onto " 
                      << owner->getName() << "'s territory: " << m_name << "! Status: " 
                      << propertyStateToString(m_state) << UI::RESET << "\n";

            if (m_isMortgaged) {
                std::cout << UI::YELLOW << "This sector is under an active asset loan (mortgaged). Zero rent owed!\n" << UI::RESET;
            } else if (m_state == PropertyState::RUINED) {
                std::cout << UI::RED << "This sector is [RUINED] from past calamities! Zero rent collected.\n" << UI::RESET;
            } else {
                int rent = calculateRent();
                engine.payRent(player, *owner, rent);
            }
        }
    }
}

std::string PropertyTile::getDisplayInfo() const {
    std::ostringstream oss;
    oss << Tile::getDisplayInfo() << " | Cost: $" << std::setw(3) << m_cost
        << " | Base: $" << std::setw(2) << m_baseRent
        << " | Fortified: $" << std::setw(3) << m_fortifiedRent
        << " | " << propertyStateToString(m_state);
    if (m_isMortgaged) oss << " " << UI::YELLOW << "[LOAN]" << UI::RESET;
    return oss.str();
}

// UtilityTile ("Max Autos" Transit Route)
UtilityTile::UtilityTile(int index, std::string name, int cost, int fixedToll)
    : Tile(index, std::move(name), TileType::UTILITY, SectorGroup::TRANSIT),
      m_cost(cost), m_fixedToll(fixedToll), m_ownerId(-1) {}

void UtilityTile::onLand(Player& player, GameEngine& engine) {
    if (m_ownerId == -1) {
        std::cout << UI::CYAN << "\n>>> " << player.getName() << " reached an unclaimed Transit Route: " 
                  << m_name << "!" << UI::RESET << "\n";
        std::cout << "Price: $" << m_cost << " | Fixed Toll: $" << m_fixedToll << "\n";
        std::cout << UI::MAGENTA << "★ Max Autos Monopoly Perk: Unlocks Fixed Tolls and Dice Tampering (1d6 sabotage)!" << UI::RESET << "\n";

        bool wantToBuy = false;
        if (player.getCapital() >= m_cost) {
            if (player.isBot()) {
                wantToBuy = player.botShouldBuyUtility(*this);
            } else {
                std::cout << "Your Capital: $" << player.getCapital() << ". Buy transit route? (y/n): ";
                char choice;
                std::cin >> choice;
                wantToBuy = (choice == 'y' || choice == 'Y');
            }
        } else {
            std::cout << UI::YELLOW << "Insufficient funds to acquire " << m_name << ".\n" << UI::RESET;
        }

        if (wantToBuy) {
            player.deductCapital(m_cost);
            m_ownerId = player.getId();
            player.addUtility(m_index);
            std::string log = "🏎️ " + player.getName() + " purchased Transit Route " + m_name + " for $" + std::to_string(m_cost) + "!";
            engine.logEvent(log);
            std::cout << UI::GREEN << log << UI::RESET << "\n";
        }
    } else if (m_ownerId == player.getId()) {
        std::cout << UI::GREEN << "\n>>> " << player.getName() << " inspected their transit depot: " 
                  << m_name << UI::RESET << "\n";
    } else {
        Player* owner = engine.getPlayer(m_ownerId);
        if (owner && !owner->isBankrupt()) {
            std::cout << UI::YELLOW << "\n>>> " << player.getName() << " passed through " 
                      << owner->getName() << "'s Max Autos Transit Route: " << m_name << "!" << UI::RESET << "\n";
            engine.payToll(player, *owner, m_fixedToll);
        }
    }
}

std::string UtilityTile::getDisplayInfo() const {
    std::ostringstream oss;
    oss << Tile::getDisplayInfo() << " | Cost: $" << std::setw(3) << m_cost
        << " | Fixed Toll: $" << std::setw(3) << m_fixedToll;
    return oss.str();
}

// LicenseTile ("Ministry of Finance")
LicenseTile::LicenseTile(int index, std::string name, int licenseCost)
    : Tile(index, std::move(name), TileType::LICENSE, SectorGroup::GOVERNMENT),
      m_licenseCost(licenseCost) {}

void LicenseTile::onLand(Player& player, GameEngine& engine) {
    std::cout << UI::MAGENTA << UI::BOLD << "\n======================================================\n"
              << "🏛️  MINISTRY OF FINANCE: " << m_name << "\n"
              << "======================================================" << UI::RESET << "\n";

    int currentLicenseeId = engine.getFinanceManagerId();
    if (currentLicenseeId == player.getId()) {
        std::cout << UI::GREEN << "Welcome, Finance Minister " << player.getName() 
                  << "! You currently hold the Finance Manager License and are earning 5% on all island rents and loans.\n" 
                  << UI::RESET;
        return;
    }

    std::cout << "The Finance Manager License grants:\n"
              << UI::YELLOW << "★ Automatic 5% cut on EVERY rent payment made between other players!\n"
              << "★ Automatic 5% administrative fee on all asset loans (mortgages)!\n" << UI::RESET;

    if (currentLicenseeId != -1) {
        Player* cur = engine.getPlayer(currentLicenseeId);
        if (cur) {
            std::cout << "Current Licensee: " << cur->getName() << ". You can challenge and buy out the license for $" 
                      << m_licenseCost << "!\n";
        }
    } else {
        std::cout << "License is currently UNHELD. Acquisition cost: $" << m_licenseCost << ".\n";
    }

    bool wantToBuy = false;
    if (player.getCapital() >= m_licenseCost) {
        if (player.isBot()) {
            wantToBuy = player.botShouldBuyLicense(m_licenseCost);
        } else {
            std::cout << "Your Capital: $" << player.getCapital() << ". Acquire Finance Manager License? (y/n): ";
            char choice;
            std::cin >> choice;
            wantToBuy = (choice == 'y' || choice == 'Y');
        }
    } else {
        std::cout << UI::YELLOW << player.getName() << " does not have enough capital ($" 
                  << player.getCapital() << "/$" << m_licenseCost << ") to obtain the license.\n" << UI::RESET;
    }

    if (wantToBuy) {
        engine.purchaseFinanceLicense(player, m_licenseCost);
    }
}

std::string LicenseTile::getDisplayInfo() const {
    std::ostringstream oss;
    oss << Tile::getDisplayInfo() << " | License Cost: $" << m_licenseCost
        << " (5% cut on all island rents & loans)";
    return oss.str();
}

// EventTile ("Island Calamity" Hazard)
EventTile::EventTile(int index, std::string name, std::string flavorText)
    : Tile(index, std::move(name), TileType::EVENT, SectorGroup::WILDERNESS),
      m_flavorText(std::move(flavorText)) {}

void EventTile::onLand(Player& player, GameEngine& engine) {
    std::cout << UI::RED << UI::BOLD << "\n⚡ HAZARD TRIGGERED at " << m_name << "! ⚡\n" 
              << m_flavorText << UI::RESET << "\n";
    engine.triggerCalamity(player);
}

std::string EventTile::getDisplayInfo() const {
    std::ostringstream oss;
    oss << Tile::getDisplayInfo() << " | " << UI::RED << "Island Calamity Risk" << UI::RESET;
    return oss.str();
}
