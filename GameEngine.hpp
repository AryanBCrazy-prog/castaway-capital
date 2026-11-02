#pragma once
#include <vector>
#include <memory>
#include <string>
#include <random>
#include <utility>
#include <deque>
#include "Board.hpp"
#include "Player.hpp"

struct CalamityCard {
    std::string title;
    std::string description;
    enum class EffectType {
        RUIN_SECTOR,
        RUIN_HIGHEST_PROPERTY,
        SALVAGE_CASH,
        CALAMITY_TAX,
        FREE_REPAIR,
        VOLCANIC_FALLOUT
    } effect;
    SectorGroup targetSector;
    int amount;
};

class GameEngine {
private:
    Board m_board;
    std::vector<std::shared_ptr<Player>> m_players; // Exactly 4 players
    int m_currentTurnIndex;
    int m_roundCount;
    std::mt19937 m_rng;
    int m_financeManagerId; // -1 if unheld
    bool m_gameOver;
    int m_winnerId;

    std::deque<CalamityCard> m_calamityDeck;
    std::vector<std::string> m_recentEvents;

    void initializeCalamityDeck();
    void shuffleCalamityDeck();

public:
    GameEngine();
    ~GameEngine() = default;

    // Strict 4-Player engine setup (Red, Blue, Green, Gold)
    void initializeStrict4PlayerGame(const std::vector<std::pair<std::string, bool>>& playerConfigs);
    void runGameLoop();

    void executeTurn(Player& player);
    std::pair<int, int> rollDice(Player& player, bool isTampered);

    // Core Custom Mechanics
    void checkAndOfferMaxAutosTampering(Player& activePlayer, bool& outTampered);
    void payRent(Player& payer, Player& receiver, int rentAmount);
    void payToll(Player& payer, Player& receiver, int tollAmount);
    void triggerCalamity(Player& triggerPlayer);
    void purchaseFinanceLicense(Player& buyer, int cost);
    void processAssetLoan(Player& player, int propertyIndex);
    void repayAssetLoan(Player& player, int propertyIndex);
    void upgradeProperty(Player& player, int propertyIndex);
    void repairProperty(Player& player, int propertyIndex);
    void executeTrade(Player& initiator);
    void handleBankruptcy(Player& bankruptPlayer, Player* creditor);
    bool checkWinCondition();

    // JSON Bridge / State Exporter for 3D Browser Visualization
    void exportGameState(const std::string& filepath = "export_state.json") const;

    // UI & Rendering
    void renderHeader() const;
    void renderBoard() const;
    void renderPlayerDashboard(const Player& player) const;
    void renderAllPlayersSummary() const;
    void logEvent(const std::string& event);
    void renderEventLog() const;

    // Accessors
    Board& getBoard() { return m_board; }
    const Board& getBoard() const { return m_board; }
    Player* getPlayer(int id);
    const Player* getPlayer(int id) const;
    const std::vector<std::shared_ptr<Player>>& getPlayers() const { return m_players; }

    int getFinanceManagerId() const { return m_financeManagerId; }
    void setFinanceManagerId(int id) { m_financeManagerId = id; }
    int getRoundCount() const { return m_roundCount; }
    bool isGameOver() const { return m_gameOver; }
};
