#include "GameEngine.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <sstream>

GameEngine::GameEngine()
    : m_currentTurnIndex(0), m_roundCount(1),
      m_rng(std::random_device{}()),
      m_financeManagerId(-1),
      m_gameOver(false), m_winnerId(-1) {
    initializeCalamityDeck();
}

void GameEngine::initializeCalamityDeck() {
    m_calamityDeck.clear();

    m_calamityDeck.push_back({
        "🌊 Tsunami Surge",
        "A sudden tidal swell submerges low-lying shorelines! All Coastal sectors are reduced to RUINS!",
        CalamityCard::EffectType::RUIN_SECTOR,
        SectorGroup::COASTAL, 0
    });

    m_calamityDeck.push_back({
        "🌋 Pyroclastic Tremor",
        "Subterranean vents burst with magma and toxic ash! All Geothermal sectors are RUINED!",
        CalamityCard::EffectType::RUIN_SECTOR,
        SectorGroup::GEOTHERMAL, 0
    });

    m_calamityDeck.push_back({
        "🌿 Jungle Rot & Termite Swarm",
        "Voracious parasites consume wooden structures! All Jungle sectors are RUINED!",
        CalamityCard::EffectType::RUIN_SECTOR,
        SectorGroup::JUNGLE, 0
    });

    m_calamityDeck.push_back({
        "🌀 Ruin King's Wrath",
        "Ancient curses awaken across the island! All Ancient Temple sectors are RUINED!",
        CalamityCard::EffectType::RUIN_SECTOR,
        SectorGroup::ANCIENT, 0
    });

    m_calamityDeck.push_back({
        "🌪️ Island Super-Cyclone",
        "Fierce 150mph winds tear through the island, devastating the highest-value fortified sector into RUINS!",
        CalamityCard::EffectType::RUIN_HIGHEST_PROPERTY,
        SectorGroup::WILDERNESS, 0
    });

    m_calamityDeck.push_back({
        "📦 Drifting Cargo Crate",
        "You spot an intact waterproof shipping crate floating near the shallows! Salvaged +$150 Capital.",
        CalamityCard::EffectType::SALVAGE_CASH,
        SectorGroup::SPECIAL, 150
    });

    m_calamityDeck.push_back({
        "🛠️ Engineer's Salvage Toolkit",
        "Washed-up industrial tools allow you to restore one RUINED property to PRISTINE state for free!",
        CalamityCard::EffectType::FREE_REPAIR,
        SectorGroup::SPECIAL, 0
    });

    m_calamityDeck.push_back({
        "🦟 Severe Malaria & Ration Spoilage",
        "Harsh weather spoils island rations! Every survivor must pay $50 in medical & food supplies.",
        CalamityCard::EffectType::CALAMITY_TAX,
        SectorGroup::SPECIAL, 50
    });

    shuffleCalamityDeck();
}

void GameEngine::shuffleCalamityDeck() {
    std::shuffle(m_calamityDeck.begin(), m_calamityDeck.end(), m_rng);
}

void GameEngine::initializeStrict4PlayerGame(const std::vector<std::pair<std::string, bool>>& playerConfigs) {
    m_players.clear();

    const std::vector<std::pair<std::string, std::string>> colors = {
        {"Red",   "#EF4444"},
        {"Blue",  "#3B82F6"},
        {"Green", "#10B981"},
        {"Gold",  "#F59E0B"}
    };

    for (int i = 0; i < 4; ++i) {
        std::string name = (i < static_cast<int>(playerConfigs.size())) ? playerConfigs[i].first : ("Player " + std::to_string(i + 1));
        bool isBot = (i < static_cast<int>(playerConfigs.size())) ? playerConfigs[i].second : false;
        m_players.push_back(std::make_shared<Player>(i, name, colors[i].first, colors[i].second, isBot));
    }

    m_currentTurnIndex = 0;
    m_roundCount = 1;
    m_gameOver = false;
    m_winnerId = -1;
    m_financeManagerId = -1;

    for (auto& p : m_players) {
        p->setTamperCharges(0);
    }

    logEvent("Rigid 4-Player engine initialized: Red, Blue, Green, Gold. Starting bankroll: $1,500 in physical notes.");
    exportGameState();
}

Player* GameEngine::getPlayer(int id) {
    for (auto& p : m_players) {
        if (p->getId() == id) return p.get();
    }
    return nullptr;
}

const Player* GameEngine::getPlayer(int id) const {
    for (const auto& p : m_players) {
        if (p->getId() == id) return p.get();
    }
    return nullptr;
}

void GameEngine::logEvent(const std::string& event) {
    m_recentEvents.push_back(event);
    if (m_recentEvents.size() > 10) {
        m_recentEvents.erase(m_recentEvents.begin());
    }
}

void GameEngine::renderEventLog() const {
    std::cout << UI::CYAN << "--- Recent Island Events ---" << UI::RESET << "\n";
    if (m_recentEvents.empty()) {
        std::cout << " (No events recorded yet)\n";
    } else {
        for (const auto& ev : m_recentEvents) {
            std::cout << " • " << ev << "\n";
        }
    }
    std::cout << "----------------------------\n";
}

void GameEngine::renderHeader() const {
    std::cout << "\n" << UI::CYAN << UI::BOLD
              << "=========================================================================================\n"
              << "                  🏝️  CASTAWAY CAPITAL: ISLAND OF RUIN  🏝️\n"
              << "                 Strict 4-Player Engine | Lap " << m_roundCount << "\n"
              << "========================================================================================="
              << UI::RESET << "\n";
}

void GameEngine::renderAllPlayersSummary() const {
    std::cout << "\n" << UI::YELLOW << UI::BOLD << "Survivor Standings & Physical Wallets:" << UI::RESET << "\n";
    std::cout << "Slot | Color | Name                 | Capital  | Pos | Properties | Max Autos | Licensee\n";
    std::cout << "-----+-------+----------------------+----------+-----+------------+-----------+----------\n";

    for (const auto& p : m_players) {
        std::string colorAnsi = UI::WHITE;
        if (p->getColorName() == "Red")   colorAnsi = UI::RED;
        if (p->getColorName() == "Blue")  colorAnsi = UI::BLUE;
        if (p->getColorName() == "Green") colorAnsi = UI::GREEN;
        if (p->getColorName() == "Gold")  colorAnsi = UI::YELLOW;

        std::string license = (p->getId() == m_financeManagerId) ? (UI::MAGENTA + "MINISTER" + UI::RESET) : "None";
        std::string botTag = p->isBot() ? " [BOT]" : "";

        std::cout << " #" << p->getId() << "   | "
                  << colorAnsi << std::left << std::setw(5) << p->getColorName() << UI::RESET << " | "
                  << std::left << std::setw(20) << (p->getName() + botTag) << " | "
                  << "$" << std::setw(7) << p->getCapital() << " | "
                  << "T#" << std::setw(2) << p->getPosition() << " | "
                  << std::setw(10) << p->getOwnedPropertyIndices().size() << " | "
                  << std::setw(9) << p->getOwnedUtilityIndices().size() << " | "
                  << license << "\n";

        // Show note breakdown per player
        std::cout << "     └─ Notes: " << p->getWallet().getColoredSummary() << "\n";
    }
    std::cout << "-----------------------------------------------------------------------------------------\n";
}

void GameEngine::renderPlayerDashboard(const Player& player) const {
    std::string colorAnsi = UI::WHITE;
    if (player.getColorName() == "Red")   colorAnsi = UI::RED;
    if (player.getColorName() == "Blue")  colorAnsi = UI::BLUE;
    if (player.getColorName() == "Green") colorAnsi = UI::GREEN;
    if (player.getColorName() == "Gold")  colorAnsi = UI::YELLOW;

    std::cout << "\n" << colorAnsi << "================= ACTIVE TURN: [" << player.getColorName() << "] " 
              << player.getName() << (player.isBot() ? " [BOT]" : " [HUMAN]") << " =================" << UI::RESET << "\n";
    std::cout << "• Bankroll Notes   : " << player.getWallet().getColoredSummary() << "\n";

    const Tile* curTile = m_board.getTile(player.getPosition());
    std::cout << "• Board Position   : #" << player.getPosition() << " - " 
              << (curTile ? curTile->getName() : "Unknown") << "\n";

    std::cout << "• Finance License  : " 
              << (player.getId() == m_financeManagerId ? (UI::MAGENTA + "HELD (Earning 5% cuts on rents & loans)" + UI::RESET) : "No") << "\n";
    std::cout << "• Max Autos Tamper : " << UI::YELLOW << player.getTamperCharges() << " charges available" << UI::RESET << "\n";

    // Show owned properties
    std::cout << "• Owned Sectors    : ";
    const auto& props = player.getOwnedPropertyIndices();
    if (props.empty() && player.getOwnedUtilityIndices().empty()) {
        std::cout << "None\n";
    } else {
        std::cout << "\n";
        for (int idx : props) {
            const PropertyTile* prop = m_board.getProperty(idx);
            if (prop) {
                std::cout << "   - #" << std::setw(2) << idx << " " << std::left << std::setw(22) << prop->getName()
                          << " " << propertyStateToString(prop->getState())
                          << " Rent: $" << prop->calculateRent()
                          << (prop->isMortgaged() ? " (MORTGAGED)" : "") << "\n";
            }
        }
        for (int idx : player.getOwnedUtilityIndices()) {
            const UtilityTile* util = m_board.getUtility(idx);
            if (util) {
                std::cout << "   - #" << std::setw(2) << idx << " " << std::left << std::setw(22) << util->getName()
                          << " " << UI::MAGENTA << "[MAX AUTOS TRANSIT]" << UI::RESET
                          << " Fixed Toll: $" << util->getFixedToll() << "\n";
            }
        }
    }
}

void GameEngine::renderBoard() const {
    std::cout << "\n" << UI::BOLD << "🏝️  ISLAND MAP (24 Sectors):" << UI::RESET << "\n";
    for (int i = 0; i < m_board.size(); ++i) {
        const Tile* tile = m_board.getTile(i);
        if (!tile) continue;

        std::cout << std::right << std::setw(2) << i << ": ";

        std::string markers = "";
        for (const auto& p : m_players) {
            if (!p->isBankrupt() && p->getPosition() == i) {
                std::string col = (p->getColorName() == "Red") ? UI::RED :
                                  (p->getColorName() == "Blue") ? UI::BLUE :
                                  (p->getColorName() == "Green") ? UI::GREEN : UI::YELLOW;
                markers += col + "[" + p->getName().substr(0, 2) + "]" + UI::RESET;
            }
        }
        if (!markers.empty()) {
            std::cout << std::left << std::setw(18) << markers << " ";
        } else {
            std::cout << "           ";
        }

        std::cout << tile->getDisplayInfo();

        if (tile->getType() == TileType::PROPERTY) {
            const auto* prop = dynamic_cast<const PropertyTile*>(tile);
            if (prop && prop->getOwnerId() != -1) {
                const Player* owner = getPlayer(prop->getOwnerId());
                std::cout << " (Owner: " << (owner ? owner->getName() : "None") << ")";
            }
        } else if (tile->getType() == TileType::UTILITY) {
            const auto* util = dynamic_cast<const UtilityTile*>(tile);
            if (util && util->getOwnerId() != -1) {
                const Player* owner = getPlayer(util->getOwnerId());
                std::cout << " (Owner: " << (owner ? owner->getName() : "None") << ")";
            }
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

// Check and offer Max Autos Dice Tampering
void GameEngine::checkAndOfferMaxAutosTampering(Player& activePlayer, bool& outTampered) {
    outTampered = false;

    for (auto& owner : m_players) {
        if (owner->isBankrupt() || owner->getId() == activePlayer.getId()) continue;
        if (!owner->ownsMaxAutos() || owner->getTamperCharges() <= 0) continue;

        bool shouldTamper = false;
        if (owner->isBot()) {
            shouldTamper = owner->botShouldTamperOpponent(activePlayer, activePlayer.getPosition(), *this);
        } else {
            std::cout << UI::MAGENTA << UI::BOLD << "\n⚙️  MAX AUTOS OPPORTUNITY FOR " 
                      << owner->getName() << " [" << owner->getColorName() << "]! ⚙️" << UI::RESET << "\n";
            std::cout << activePlayer.getName() << " is about to roll 2d6 at position #" << activePlayer.getPosition() << ".\n";
            std::cout << "You own Max Autos with " << owner->getTamperCharges() 
                      << " tamper charges remaining this round.\n";
            std::cout << "Do you want to sabotage their vehicle to force a SINGLE 1d6 roll? (y/n): ";
            char ans;
            std::cin >> ans;
            shouldTamper = (ans == 'y' || ans == 'Y');
        }

        if (shouldTamper) {
            owner->decrementTamperCharge();
            outTampered = true;
            std::string log = "⚙️ MAX AUTOS SABOTAGE! " + owner->getName() + 
                              " tampered with " + activePlayer.getName() + "'s transport! Roll forced to 1d6!";
            logEvent(log);
            std::cout << UI::RED << UI::BOLD << "\n" << log << UI::RESET << "\n";
            break;
        }
    }
}

std::pair<int, int> GameEngine::rollDice(Player& player, bool isTampered) {
    std::uniform_int_distribution<int> d6(1, 6);

    if (isTampered) {
        int d1 = d6(m_rng);
        std::cout << UI::YELLOW << ">>> " << player.getName() << " rolls a single tampered die [1d6]: (" 
                  << d1 << ") -> Total: " << d1 << UI::RESET << "\n";
        return {d1, 0};
    } else {
        int d1 = d6(m_rng);
        int d2 = d6(m_rng);
        std::cout << UI::GREEN << ">>> " << player.getName() << " rolls [2d6]: (" 
                  << d1 << " + " << d2 << ") -> Total: " << (d1 + d2) << UI::RESET << "\n";
        return {d1, d2};
    }
}

// Rent payment with change-making wallet and 5% cut to Finance Manager
void GameEngine::payRent(Player& payer, Player& receiver, int rentAmount) {
    if (rentAmount <= 0) return;

    int financeCut = 0;
    if (m_financeManagerId != -1 && m_financeManagerId != payer.getId()) {
        financeCut = std::max(1, (rentAmount * 5) / 100);
    }

    int netRent = rentAmount - financeCut;

    std::cout << UI::YELLOW << payer.getName() << " owes $" << rentAmount 
              << " in sector rent to " << receiver.getName() << "." << UI::RESET << "\n";

    if (payer.getCapital() < rentAmount) {
        std::cout << UI::RED << payer.getName() << " has insufficient cash ($" 
                  << payer.getCapital() << ") to cover rent ($" << rentAmount << ")!\n" << UI::RESET;
        handleBankruptcy(payer, &receiver);
        exportGameState();
        return;
    }

    // Change-maker algorithm executes
    payer.deductCapital(rentAmount);
    receiver.addCapital(netRent);

    std::string log = "💰 " + payer.getName() + " paid $" + std::to_string(netRent) + 
                      " rent to " + receiver.getName() + ".";

    if (financeCut > 0) {
        Player* finMgr = getPlayer(m_financeManagerId);
        if (finMgr && !finMgr->isBankrupt()) {
            finMgr->addCapital(financeCut);
            log += " [🏛️ Ministry Cut: $" + std::to_string(financeCut) + " to " + finMgr->getName() + "]";
            std::cout << UI::MAGENTA << "🏛️ FINANCE MANAGER CUT: " << finMgr->getName() 
                      << " automatically collected a 5% cut ($" << financeCut << ")!" << UI::RESET << "\n";
        }
    }

    logEvent(log);
    exportGameState();
}

void GameEngine::payToll(Player& payer, Player& receiver, int tollAmount) {
    if (tollAmount <= 0) return;

    std::cout << UI::YELLOW << payer.getName() << " owes $" << tollAmount 
              << " transit toll to Max Autos operator " << receiver.getName() << "." << UI::RESET << "\n";

    if (payer.getCapital() < tollAmount) {
        handleBankruptcy(payer, &receiver);
        exportGameState();
        return;
    }

    payer.deductCapital(tollAmount);
    receiver.addCapital(tollAmount);

    std::string log = "🏎️ " + payer.getName() + " paid $" + std::to_string(tollAmount) + 
                      " toll to " + receiver.getName() + ".";
    logEvent(log);
    exportGameState();
}

void GameEngine::purchaseFinanceLicense(Player& buyer, int cost) {
    if (!buyer.deductCapital(cost)) {
        std::cout << UI::RED << "Insufficient capital to purchase license.\n" << UI::RESET;
        return;
    }

    if (m_financeManagerId != -1) {
        Player* old = getPlayer(m_financeManagerId);
        if (old) old->setFinanceLicense(false);
    }

    m_financeManagerId = buyer.getId();
    buyer.setFinanceLicense(true);

    std::string log = "🏛️ " + buyer.getName() + " acquired the FINANCE MANAGER LICENSE! Earning 5% on all island rents & loans.";
    logEvent(log);
    std::cout << UI::MAGENTA << UI::BOLD << log << UI::RESET << "\n";
    exportGameState();
}

void GameEngine::triggerCalamity(Player& triggerPlayer) {
    if (m_calamityDeck.empty()) {
        initializeCalamityDeck();
    }

    CalamityCard card = m_calamityDeck.front();
    m_calamityDeck.pop_front();
    m_calamityDeck.push_back(card);

    std::cout << "\n" << UI::RED << UI::BOLD << "══════════════════════════════════════════════════════\n"
              << "⚡ ISLAND CALAMITY EVENT: " << card.title << "\n"
              << "══════════════════════════════════════════════════════" << UI::RESET << "\n";
    std::cout << card.description << "\n\n";

    switch (card.effect) {
        case CalamityCard::EffectType::RUIN_SECTOR: {
            int count = 0;
            for (int i = 0; i < m_board.size(); ++i) {
                PropertyTile* prop = m_board.getProperty(i);
                if (prop && prop->getSector() == card.targetSector) {
                    if (prop->getState() != PropertyState::RUINED) {
                        prop->ruin();
                        count++;
                        std::cout << UI::RED << "💥 " << prop->getName() << " degraded to RUINS!\n" << UI::RESET;
                    }
                }
            }
            logEvent("⚡ Calamity: " + card.title + " ruined " + std::to_string(count) + " sectors.");
            break;
        }

        case CalamityCard::EffectType::RUIN_HIGHEST_PROPERTY: {
            PropertyTile* highestProp = nullptr;
            int maxRent = -1;

            for (int i = 0; i < m_board.size(); ++i) {
                PropertyTile* prop = m_board.getProperty(i);
                if (prop && prop->getOwnerId() != -1 && prop->getState() != PropertyState::RUINED) {
                    if (prop->calculateRent() > maxRent) {
                        maxRent = prop->calculateRent();
                        highestProp = prop;
                    }
                }
            }

            if (highestProp) {
                highestProp->ruin();
                std::cout << UI::RED << "💥 Super-cyclone obliterated " << highestProp->getName() 
                          << "! Status reduced to [RUINED].\n" << UI::RESET;
                logEvent("⚡ Calamity ruined prime sector: " + highestProp->getName());
            } else {
                std::cout << "No fortified sectors found to destroy.\n";
            }
            break;
        }

        case CalamityCard::EffectType::SALVAGE_CASH: {
            triggerPlayer.addCapital(card.amount);
            std::cout << UI::GREEN << "💰 " << triggerPlayer.getName() << " gained $" 
                      << card.amount << " in emergency capital notes!\n" << UI::RESET;
            logEvent("📦 " + triggerPlayer.getName() + " salvaged $" + std::to_string(card.amount));
            break;
        }

        case CalamityCard::EffectType::CALAMITY_TAX: {
            for (auto& p : m_players) {
                if (!p->isBankrupt()) {
                    p->deductCapital(card.amount);
                    std::cout << " • " << p->getName() << " spent $" << card.amount << " in survival notes.\n";
                }
            }
            logEvent("🦟 Calamity Tax: Each survivor paid $" + std::to_string(card.amount));
            break;
        }

        case CalamityCard::EffectType::FREE_REPAIR: {
            bool repaired = false;
            for (int idx : triggerPlayer.getOwnedPropertyIndices()) {
                PropertyTile* prop = m_board.getProperty(idx);
                if (prop && prop->getState() == PropertyState::RUINED) {
                    prop->repair();
                    repaired = true;
                    std::cout << UI::GREEN << "🔧 Free emergency repair applied to " 
                              << prop->getName() << "! Now [PRISTINE].\n" << UI::RESET;
                    logEvent("🛠️ Free repair restored " + prop->getName());
                    break;
                }
            }
            if (!repaired) {
                std::cout << triggerPlayer.getName() << " has no ruined properties. Toolkit salvaged for $50.\n";
                triggerPlayer.addCapital(50);
            }
            break;
        }

        default:
            break;
    }

    exportGameState();
}

void GameEngine::processAssetLoan(Player& player, int propertyIndex) {
    PropertyTile* prop = m_board.getProperty(propertyIndex);
    if (!prop || prop->getOwnerId() != player.getId()) {
        std::cout << UI::RED << "You do not own this sector!\n" << UI::RESET;
        return;
    }
    if (prop->isMortgaged()) {
        std::cout << UI::RED << "This sector is already under an active asset loan!\n" << UI::RESET;
        return;
    }

    int loanAmount = prop->getCost() / 2;
    int fee = 0;

    if (m_financeManagerId != -1 && m_financeManagerId != player.getId()) {
        fee = std::max(1, (loanAmount * 5) / 100);
        Player* finMgr = getPlayer(m_financeManagerId);
        if (finMgr && !finMgr->isBankrupt()) {
            finMgr->addCapital(fee);
            std::cout << UI::MAGENTA << "🏛️ FINANCE MANAGER LOAN CUT: " << finMgr->getName() 
                      << " took a 5% administrative loan fee ($" << fee << ")!\n" << UI::RESET;
        }
    }

    int netLoan = loanAmount - fee;
    player.addCapital(netLoan);
    prop->setMortgaged(true);

    std::string log = "🏦 " + player.getName() + " took an asset loan on " + prop->getName() + 
                      " for net $" + std::to_string(netLoan) + "!";
    logEvent(log);
    std::cout << UI::GREEN << log << UI::RESET << "\n";
    exportGameState();
}

void GameEngine::repayAssetLoan(Player& player, int propertyIndex) {
    PropertyTile* prop = m_board.getProperty(propertyIndex);
    if (!prop || prop->getOwnerId() != player.getId()) {
        std::cout << UI::RED << "You do not own this sector!\n" << UI::RESET;
        return;
    }
    if (!prop->isMortgaged()) {
        std::cout << UI::RED << "This sector is not mortgaged.\n" << UI::RESET;
        return;
    }

    int payoff = (prop->getCost() / 2) + 20;
    if (player.getCapital() < payoff) {
        std::cout << UI::RED << "Insufficient capital to lift mortgage!\n" << UI::RESET;
        return;
    }

    player.deductCapital(payoff);
    prop->setMortgaged(false);

    std::string log = "✅ " + player.getName() + " repaid the asset loan on " + prop->getName() + " for $" + std::to_string(payoff);
    logEvent(log);
    std::cout << UI::GREEN << log << UI::RESET << "\n";
    exportGameState();
}

void GameEngine::repairProperty(Player& player, int propertyIndex) {
    PropertyTile* prop = m_board.getProperty(propertyIndex);
    if (!prop || prop->getOwnerId() != player.getId()) {
        std::cout << UI::RED << "You do not own this sector!\n" << UI::RESET;
        return;
    }
    if (prop->getState() != PropertyState::RUINED) {
        std::cout << UI::YELLOW << "This sector is not ruined.\n" << UI::RESET;
        return;
    }

    int cost = prop->getRepairCost();
    if (player.getCapital() < cost) {
        std::cout << UI::RED << "Insufficient capital notes to repair sector!\n" << UI::RESET;
        return;
    }

    player.deductCapital(cost);
    prop->repair();

    std::string log = "🔧 " + player.getName() + " repaired " + prop->getName() + " to [PRISTINE] for $" + std::to_string(cost);
    logEvent(log);
    std::cout << UI::GREEN << log << UI::RESET << "\n";
    exportGameState();
}

void GameEngine::upgradeProperty(Player& player, int propertyIndex) {
    PropertyTile* prop = m_board.getProperty(propertyIndex);
    if (!prop || prop->getOwnerId() != player.getId()) {
        std::cout << UI::RED << "You do not own this sector!\n" << UI::RESET;
        return;
    }
    if (prop->getState() == PropertyState::RUINED) {
        std::cout << UI::RED << "Sector is RUINED! Repair before upgrading.\n" << UI::RESET;
        return;
    }
    if (prop->getState() == PropertyState::FORTIFIED) {
        std::cout << UI::YELLOW << "Sector is already at maximum fortification!\n" << UI::RESET;
        return;
    }

    int cost = prop->getUpgradeCost();
    if (player.getCapital() < cost) {
        std::cout << UI::RED << "Insufficient capital to fortify this sector!\n" << UI::RESET;
        return;
    }

    player.deductCapital(cost);
    prop->upgrade();

    std::string log = "🏰 " + player.getName() + " FORTIFIED " + prop->getName() + " for $" + std::to_string(cost) + 
                      "! New rent: $" + std::to_string(prop->getFortifiedRent());
    logEvent(log);
    std::cout << UI::CYAN << UI::BOLD << log << UI::RESET << "\n";
    exportGameState();
}

void GameEngine::executeTrade(Player& initiator) {
    std::cout << "\n--- TRADE NEGOTIATIONS ---\n";
    std::cout << "Select partner to trade with:\n";

    std::vector<Player*> partners;
    for (auto& p : m_players) {
        if (!p->isBankrupt() && p->getId() != initiator.getId()) {
            partners.push_back(p.get());
            std::cout << " [" << partners.size() << "] " << p->getName() 
                      << " [" << p->getColorName() << "] (Capital: $" << p->getCapital() << ")\n";
        }
    }

    if (partners.empty()) {
        std::cout << "No other survivors available to trade.\n";
        return;
    }

    std::cout << "Choice (0 to cancel): ";
    int pChoice;
    std::cin >> pChoice;
    if (pChoice <= 0 || pChoice > static_cast<int>(partners.size())) return;

    Player* partner = partners[pChoice - 1];

    std::cout << "Your owned properties:\n";
    for (int idx : initiator.getOwnedPropertyIndices()) {
        PropertyTile* p = m_board.getProperty(idx);
        if (p) std::cout << " #" << idx << " - " << p->getName() << "\n";
    }
    std::cout << "Enter property ID to offer (-1 for none): ";
    int offerPropId;
    std::cin >> offerPropId;

    std::cout << "Enter cash to offer: $";
    int offerCash;
    std::cin >> offerCash;
    if (offerCash > initiator.getCapital()) offerCash = initiator.getCapital();

    std::cout << "\n" << partner->getName() << "'s owned properties:\n";
    for (int idx : partner->getOwnedPropertyIndices()) {
        PropertyTile* p = m_board.getProperty(idx);
        if (p) std::cout << " #" << idx << " - " << p->getName() << "\n";
    }
    std::cout << "Enter property ID you request (-1 for none): ";
    int reqPropId;
    std::cin >> reqPropId;

    std::cout << "Enter cash you request: $";
    int reqCash;
    std::cin >> reqCash;

    bool accepted = false;
    if (partner->isBot()) {
        int valOffered = offerCash + (offerPropId != -1 ? 150 : 0);
        int valRequested = reqCash + (reqPropId != -1 ? 150 : 0);
        accepted = (valOffered >= valRequested) && (partner->getCapital() >= reqCash);
    } else {
        std::cout << "\n" << partner->getName() << ", do you accept this trade deal? (y/n): ";
        char ans;
        std::cin >> ans;
        accepted = (ans == 'y' || ans == 'Y');
    }

    if (accepted) {
        if (offerPropId != -1) {
            PropertyTile* p = m_board.getProperty(offerPropId);
            if (p && p->getOwnerId() == initiator.getId()) {
                p->setOwnerId(partner->getId());
                initiator.removeProperty(offerPropId);
                partner->addProperty(offerPropId);
            }
        }
        if (reqPropId != -1) {
            PropertyTile* p = m_board.getProperty(reqPropId);
            if (p && p->getOwnerId() == partner->getId()) {
                p->setOwnerId(initiator.getId());
                partner->removeProperty(reqPropId);
                initiator.addProperty(reqPropId);
            }
        }
        initiator.deductCapital(offerCash);
        partner->addCapital(offerCash);
        partner->deductCapital(reqCash);
        initiator.addCapital(reqCash);

        std::string log = "🤝 Trade executed between " + initiator.getName() + " and " + partner->getName() + "!";
        logEvent(log);
        std::cout << UI::GREEN << log << UI::RESET << "\n";
        exportGameState();
    } else {
        std::cout << UI::YELLOW << "Trade proposal declined.\n" << UI::RESET;
    }
}

void GameEngine::handleBankruptcy(Player& bankruptPlayer, Player* creditor) {
    bankruptPlayer.setBankrupt(true);
    std::string log = "💀 " + bankruptPlayer.getName() + " [" + bankruptPlayer.getColorName() + 
                      "] has gone BANKRUPT and was eliminated from the island!";
    logEvent(log);
    std::cout << UI::RED << UI::BOLD << "\n" << log << UI::RESET << "\n";

    for (int idx : bankruptPlayer.getOwnedPropertyIndices()) {
        PropertyTile* prop = m_board.getProperty(idx);
        if (prop) {
            if (creditor) {
                prop->setOwnerId(creditor->getId());
                creditor->addProperty(idx);
            } else {
                prop->setOwnerId(-1);
                prop->ruin();
            }
        }
    }
    for (int idx : bankruptPlayer.getOwnedUtilityIndices()) {
        UtilityTile* util = m_board.getUtility(idx);
        if (util) {
            if (creditor) {
                util->setOwnerId(creditor->getId());
                creditor->addUtility(idx);
            } else {
                util->setOwnerId(-1);
            }
        }
    }

    if (m_financeManagerId == bankruptPlayer.getId()) {
        m_financeManagerId = -1;
        bankruptPlayer.setFinanceLicense(false);
        logEvent("🏛️ Finance Manager License is now vacant.");
    }

    exportGameState();
}

bool GameEngine::checkWinCondition() {
    int activeCount = 0;
    Player* lastActive = nullptr;

    for (auto& p : m_players) {
        if (!p->isBankrupt()) {
            activeCount++;
            lastActive = p.get();

            int fortifiedCount = 0;
            for (int idx : p->getOwnedPropertyIndices()) {
                PropertyTile* prop = m_board.getProperty(idx);
                if (prop && prop->getState() == PropertyState::FORTIFIED) {
                    fortifiedCount++;
                }
            }

            if (p->getCapital() >= 4000 && fortifiedCount >= 3) {
                m_gameOver = true;
                m_winnerId = p->getId();
                std::cout << UI::CYAN << UI::BOLD
                          << "\n🏆 DOMINION VICTORY! " << p->getName() << " [" << p->getColorName()
                          << "] achieved Island Dominion ($4000+ Capital and 3+ Fortified Strongholds)!\n"
                          << UI::RESET;
                exportGameState();
                return true;
            }
        }
    }

    if (activeCount <= 1 && lastActive) {
        m_gameOver = true;
        m_winnerId = lastActive->getId();
        std::cout << UI::GREEN << UI::BOLD
                  << "\n🏆 LAST SURVIVOR VICTORY! " << lastActive->getName() << " [" << lastActive->getColorName()
                  << "] outlasted all other castaways on the Island of Ruin!\n"
                  << UI::RESET;
        exportGameState();
        return true;
    }

    return false;
}

// JSON Bridge / State Exporter
void GameEngine::exportGameState(const std::string& filepath) const {
    std::ofstream out(filepath);
    if (!out.is_open()) return;

    out << "{\n";
    out << "  \"round\": " << m_roundCount << ",\n";
    out << "  \"currentTurnIndex\": " << m_currentTurnIndex << ",\n";
    out << "  \"gameOver\": " << (m_gameOver ? "true" : "false") << ",\n";
    out << "  \"winnerId\": " << m_winnerId << ",\n";
    out << "  \"financeManagerId\": " << m_financeManagerId << ",\n";

    // Players
    out << "  \"players\": [\n";
    for (size_t i = 0; i < m_players.size(); ++i) {
        const auto& p = m_players[i];
        out << "    {\n";
        out << "      \"id\": " << p->getId() << ",\n";
        out << "      \"name\": \"" << p->getName() << "\",\n";
        out << "      \"color\": \"" << p->getColorName() << "\",\n";
        out << "      \"hexColor\": \"" << p->getHexColor() << "\",\n";
        out << "      \"position\": " << p->getPosition() << ",\n";
        out << "      \"capital\": " << p->getCapital() << ",\n";
        out << "      \"isBot\": " << (p->isBot() ? "true" : "false") << ",\n";
        out << "      \"isBankrupt\": " << (p->isBankrupt() ? "true" : "false") << ",\n";
        out << "      \"hasFinanceLicense\": " << (p->hasFinanceLicense() ? "true" : "false") << ",\n";
        out << "      \"tamperCharges\": " << p->getTamperCharges() << ",\n";
        out << "      \"notes\": " << p->getWallet().toJSON() << ",\n";

        out << "      \"ownedProperties\": [";
        const auto& props = p->getOwnedPropertyIndices();
        for (size_t j = 0; j < props.size(); ++j) {
            out << props[j] << (j + 1 < props.size() ? "," : "");
        }
        out << "],\n";

        out << "      \"ownedUtilities\": [";
        const auto& utils = p->getOwnedUtilityIndices();
        for (size_t j = 0; j < utils.size(); ++j) {
            out << utils[j] << (j + 1 < utils.size() ? "," : "");
        }
        out << "]\n";

        out << "    }" << (i + 1 < m_players.size() ? ",\n" : "\n");
    }
    out << "  ],\n";

    // Tiles (all 24)
    out << "  \"tiles\": [\n";
    for (int i = 0; i < m_board.size(); ++i) {
        const Tile* t = m_board.getTile(i);
        out << "    {\n";
        out << "      \"index\": " << i << ",\n";
        out << "      \"name\": \"" << t->getName() << "\",\n";
        out << "      \"sector\": \"" << sectorGroupToString(t->getSector()) << "\",\n";

        std::string typeStr = "EVENT";
        if (t->getType() == TileType::BASE_CAMP) typeStr = "BASE_CAMP";
        else if (t->getType() == TileType::PROPERTY) typeStr = "PROPERTY";
        else if (t->getType() == TileType::UTILITY) typeStr = "UTILITY";
        else if (t->getType() == TileType::LICENSE) typeStr = "LICENSE";
        out << "      \"type\": \"" << typeStr << "\",\n";

        int ownerId = -1;
        std::string stateStr = "NONE";
        int rent = 0;
        int cost = 0;
        bool isMortgaged = false;

        if (t->getType() == TileType::PROPERTY) {
            const auto* prop = dynamic_cast<const PropertyTile*>(t);
            if (prop) {
                ownerId = prop->getOwnerId();
                stateStr = (prop->getState() == PropertyState::RUINED) ? "RUINED" :
                           (prop->getState() == PropertyState::FORTIFIED) ? "FORTIFIED" : "PRISTINE";
                rent = prop->calculateRent();
                cost = prop->getCost();
                isMortgaged = prop->isMortgaged();
            }
        } else if (t->getType() == TileType::UTILITY) {
            const auto* util = dynamic_cast<const UtilityTile*>(t);
            if (util) {
                ownerId = util->getOwnerId();
                cost = util->getCost();
                rent = util->getFixedToll();
            }
        }

        out << "      \"ownerId\": " << ownerId << ",\n";
        out << "      \"state\": \"" << stateStr << "\",\n";
        out << "      \"rent\": " << rent << ",\n";
        out << "      \"cost\": " << cost << ",\n";
        out << "      \"isMortgaged\": " << (isMortgaged ? "true" : "false") << "\n";
        out << "    }" << (i + 1 < m_board.size() ? ",\n" : "\n");
    }
    out << "  ],\n";

    // Recent events
    out << "  \"recentEvents\": [\n";
    for (size_t i = 0; i < m_recentEvents.size(); ++i) {
        // Escape quotes
        std::string ev = m_recentEvents[i];
        std::string clean = "";
        for (char c : ev) {
            if (c == '"') clean += "\\\"";
            else clean += c;
        }
        out << "    \"" << clean << "\"" << (i + 1 < m_recentEvents.size() ? ",\n" : "\n");
    }
    out << "  ]\n";
    out << "}\n";

    out.close();
}

void GameEngine::executeTurn(Player& player) {
    if (player.isBankrupt()) return;

    renderHeader();
    renderAllPlayersSummary();
    renderEventLog();
    renderPlayerDashboard(player);

    if (player.isBot()) {
        std::cout << UI::YELLOW << "\n🤖 " << player.getName() << " [" << player.getColorName() 
                  << "] is planning moves...\n" << UI::RESET;

        // Bot heuristic: Auto repair ruined sectors if funds allow
        for (int idx : player.getOwnedPropertyIndices()) {
            PropertyTile* prop = m_board.getProperty(idx);
            if (prop && prop->getState() == PropertyState::RUINED && player.botShouldRepair(*prop)) {
                repairProperty(player, idx);
            }
        }

        // Bot heuristic: Auto upgrade pristine sectors if funds allow
        for (int idx : player.getOwnedPropertyIndices()) {
            PropertyTile* prop = m_board.getProperty(idx);
            if (prop && prop->getState() == PropertyState::PRISTINE && player.botShouldUpgrade(*prop)) {
                upgradeProperty(player, idx);
            }
        }

        // Check Max Autos tampering from opponents
        bool isTampered = false;
        checkAndOfferMaxAutosTampering(player, isTampered);

        // Roll dice
        auto [d1, d2] = rollDice(player, isTampered);
        int steps = d1 + d2;

        // Move player
        int crossedGo = player.advancePosition(steps, m_board.size());
        if (crossedGo) {
            player.collectSalary();
            std::string log = "🏕️ " + player.getName() + " completed an island lap and collected $200 Base Camp Salary (1x 100, 1x 50, 2x 20, 1x 10)!";
            logEvent(log);
            std::cout << UI::GREEN << log << UI::RESET << "\n";
        }

        Tile* landedTile = m_board.getTile(player.getPosition());
        if (landedTile) {
            landedTile->onLand(player, *this);
        }

    } else {
        // Human player interactive turn
        bool hasRolled = false;

        while (!hasRolled && !player.isBankrupt()) {
            std::cout << "\n" << UI::BOLD << "Turn Menu Options:" << UI::RESET << "\n"
                      << " [1] Roll Dice & Advance\n"
                      << " [2] Fortify a Pristine Sector (Upgrade)\n"
                      << " [3] Repair a Ruined Sector\n"
                      << " [4] Take Asset Loan (Mortgage)\n"
                      << " [5] Repay Asset Loan (Unmortgage)\n"
                      << " [6] Propose Trade\n"
                      << " [7] View Full Island Map\n"
                      << " [8] Surrender / Forfeit\n"
                      << "Select an action (1-8): ";

            int choice;
            if (!(std::cin >> choice)) {
                std::cin.clear();
                std::cin.ignore(1000, '\n');
                continue;
            }

            switch (choice) {
                case 1: {
                    hasRolled = true;

                    bool isTampered = false;
                    checkAndOfferMaxAutosTampering(player, isTampered);

                    auto [d1, d2] = rollDice(player, isTampered);
                    int steps = d1 + d2;

                    int crossedGo = player.advancePosition(steps, m_board.size());
                    if (crossedGo) {
                        player.collectSalary();
                        std::string log = "🏕️ " + player.getName() + " completed an island lap and collected $200 Base Camp Salary (1x 100, 1x 50, 2x 20, 1x 10)!";
                        logEvent(log);
                        std::cout << UI::GREEN << log << UI::RESET << "\n";
                    }

                    Tile* landedTile = m_board.getTile(player.getPosition());
                    if (landedTile) {
                        landedTile->onLand(player, *this);
                    }
                    break;
                }
                case 2: {
                    std::cout << "\nSelect property to FORTIFY:\n";
                    const auto& pIndices = player.getOwnedPropertyIndices();
                    for (size_t i = 0; i < pIndices.size(); ++i) {
                        PropertyTile* p = m_board.getProperty(pIndices[i]);
                        if (p && p->getState() == PropertyState::PRISTINE) {
                            std::cout << " [" << (i + 1) << "] " << p->getName() 
                                      << " | Upgrade Cost: $" << p->getUpgradeCost()
                                      << " | New Fortified Rent: $" << p->getFortifiedRent() << "\n";
                        }
                    }
                    std::cout << "Choice (0 to back): ";
                    int pChoice;
                    std::cin >> pChoice;
                    if (pChoice > 0 && pChoice <= static_cast<int>(pIndices.size())) {
                        upgradeProperty(player, pIndices[pChoice - 1]);
                    }
                    break;
                }
                case 3: {
                    std::cout << "\nSelect property to REPAIR:\n";
                    const auto& pIndices = player.getOwnedPropertyIndices();
                    for (size_t i = 0; i < pIndices.size(); ++i) {
                        PropertyTile* p = m_board.getProperty(pIndices[i]);
                        if (p && p->getState() == PropertyState::RUINED) {
                            std::cout << " [" << (i + 1) << "] " << p->getName() 
                                      << " | Repair Cost: $" << p->getRepairCost()
                                      << " | Base Rent after repair: $" << p->getBaseRent() << "\n";
                        }
                    }
                    std::cout << "Choice (0 to back): ";
                    int pChoice;
                    std::cin >> pChoice;
                    if (pChoice > 0 && pChoice <= static_cast<int>(pIndices.size())) {
                        repairProperty(player, pIndices[pChoice - 1]);
                    }
                    break;
                }
                case 4: {
                    std::cout << "\nSelect property for ASSET LOAN (Mortgage):\n";
                    const auto& pIndices = player.getOwnedPropertyIndices();
                    for (size_t i = 0; i < pIndices.size(); ++i) {
                        PropertyTile* p = m_board.getProperty(pIndices[i]);
                        if (p && !p->isMortgaged()) {
                            std::cout << " [" << (i + 1) << "] " << p->getName() 
                                      << " | Loan Amount: $" << (p->getCost() / 2)
                                      << " (5% fee to Finance Minister if held)\n";
                        }
                    }
                    std::cout << "Choice (0 to back): ";
                    int pChoice;
                    std::cin >> pChoice;
                    if (pChoice > 0 && pChoice <= static_cast<int>(pIndices.size())) {
                        processAssetLoan(player, pIndices[pChoice - 1]);
                    }
                    break;
                }
                case 5: {
                    std::cout << "\nSelect property to REPAY ASSET LOAN:\n";
                    const auto& pIndices = player.getOwnedPropertyIndices();
                    for (size_t i = 0; i < pIndices.size(); ++i) {
                        PropertyTile* p = m_board.getProperty(pIndices[i]);
                        if (p && p->isMortgaged()) {
                            std::cout << " [" << (i + 1) << "] " << p->getName() 
                                      << " | Payoff: $" << ((p->getCost() / 2) + 20) << "\n";
                        }
                    }
                    std::cout << "Choice (0 to back): ";
                    int pChoice;
                    std::cin >> pChoice;
                    if (pChoice > 0 && pChoice <= static_cast<int>(pIndices.size())) {
                        repayAssetLoan(player, pIndices[pChoice - 1]);
                    }
                    break;
                }
                case 6:
                    executeTrade(player);
                    break;
                case 7:
                    renderBoard();
                    break;
                case 8: {
                    std::cout << UI::RED << "Are you sure you want to surrender? (y/n): " << UI::RESET;
                    char c;
                    std::cin >> c;
                    if (c == 'y' || c == 'Y') {
                        handleBankruptcy(player, nullptr);
                        return;
                    }
                    break;
                }
                default:
                    std::cout << "Invalid selection. Please choose 1-8.\n";
                    break;
            }
        }
    }

    exportGameState();

    std::cout << "\nPress Enter to proceed to the next survivor in queue...";
    std::cin.ignore(1000, '\n');
    std::string dummy;
    std::getline(std::cin, dummy);
}

void GameEngine::runGameLoop() {
    exportGameState();

    while (!m_gameOver) {
        // Lap start check: when cycle restarts at Player 0 (Red)
        if (m_currentTurnIndex == 0) {
            for (auto& p : m_players) {
                if (!p->isBankrupt()) {
                    p->setTamperCharges(static_cast<int>(p->getOwnedUtilityIndices().size()));
                }
            }
        }

        Player& current = *m_players[m_currentTurnIndex];
        if (!current.isBankrupt()) {
            executeTurn(current);
        }

        if (checkWinCondition()) {
            break;
        }

        // Strict deterministic 4-player turn cycle:
        // Player 0 (Red) -> Player 1 (Blue) -> Player 2 (Green) -> Player 3 (Gold) -> Lap Increment
        m_currentTurnIndex = (m_currentTurnIndex + 1) % 4;
        if (m_currentTurnIndex == 0) {
            m_roundCount++;
            std::cout << UI::CYAN << UI::BOLD << "\n>>> === LAP " << m_roundCount << " COMMENCES! === <<<\n" << UI::RESET;
        }

        exportGameState();
    }
}
