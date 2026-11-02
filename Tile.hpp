#pragma once
#include <string>
#include <memory>
#include <vector>
#include "Enums.hpp"

class Player;
class GameEngine;

class Tile {
protected:
    int m_index;
    std::string m_name;
    TileType m_type;
    SectorGroup m_sector;

public:
    Tile(int index, std::string name, TileType type, SectorGroup sector);
    virtual ~Tile() = default;

    int getIndex() const { return m_index; }
    const std::string& getName() const { return m_name; }
    TileType getType() const { return m_type; }
    SectorGroup getSector() const { return m_sector; }

    virtual void onLand(Player& player, GameEngine& engine) = 0;
    virtual std::string getDisplayInfo() const;
};

// Base Camp / Safe Haven Tile (Passing or landing grants capital supplies)
class BaseCampTile : public Tile {
private:
    int m_reward;
    bool m_isSafeHaven;

public:
    BaseCampTile(int index, std::string name, int reward, bool isSafeHaven = false);
    void onLand(Player& player, GameEngine& engine) override;
    int getReward() const { return m_reward; }
    bool isSafeHaven() const { return m_isSafeHaven; }
    std::string getDisplayInfo() const override;
};

// Property Tile (Pristine vs Ruined vs Fortified)
class PropertyTile : public Tile {
private:
    int m_cost;
    int m_baseRent;
    int m_fortifiedRent;
    int m_repairCost;
    int m_upgradeCost;
    int m_mortgageValue;
    PropertyState m_state;
    int m_ownerId; // -1 if unowned
    bool m_isMortgaged;

public:
    PropertyTile(int index, std::string name, SectorGroup sector,
                 int cost, int baseRent, int fortifiedRent,
                 int repairCost, int upgradeCost);

    void onLand(Player& player, GameEngine& engine) override;
    std::string getDisplayInfo() const override;

    int getCost() const { return m_cost; }
    int getBaseRent() const { return m_baseRent; }
    int getFortifiedRent() const { return m_fortifiedRent; }
    int getRepairCost() const { return m_repairCost; }
    int getUpgradeCost() const { return m_upgradeCost; }
    int getMortgageValue() const { return m_mortgageValue; }

    PropertyState getState() const { return m_state; }
    void setState(PropertyState state) { m_state = state; }

    int getOwnerId() const { return m_ownerId; }
    void setOwnerId(int ownerId) { m_ownerId = ownerId; }

    bool isMortgaged() const { return m_isMortgaged; }
    void setMortgaged(bool mortgaged) { m_isMortgaged = mortgaged; }

    int calculateRent() const;
    void ruin();
    bool repair();
    bool upgrade();
};

// Utility Tile ("Max Autos" Transit Routes)
class UtilityTile : public Tile {
private:
    int m_cost;
    int m_fixedToll;
    int m_ownerId; // -1 if unowned

public:
    UtilityTile(int index, std::string name, int cost, int fixedToll);
    void onLand(Player& player, GameEngine& engine) override;
    std::string getDisplayInfo() const override;

    int getCost() const { return m_cost; }
    int getFixedToll() const { return m_fixedToll; }
    int getOwnerId() const { return m_ownerId; }
    void setOwnerId(int id) { m_ownerId = id; }
};

// License Tile ("Ministry of Finance")
class LicenseTile : public Tile {
private:
    int m_licenseCost;

public:
    LicenseTile(int index, std::string name, int licenseCost);
    void onLand(Player& player, GameEngine& engine) override;
    std::string getDisplayInfo() const override;

    int getLicenseCost() const { return m_licenseCost; }
};

// Event Tile ("Island Calamity" Hazard / Salvage Tile)
class EventTile : public Tile {
private:
    std::string m_flavorText;

public:
    EventTile(int index, std::string name, std::string flavorText);
    void onLand(Player& player, GameEngine& engine) override;
    std::string getDisplayInfo() const override;
};
