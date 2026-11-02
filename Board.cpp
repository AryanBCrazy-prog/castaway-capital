#include "Board.hpp"
#include <iostream>

Board::Board() {
    initializeBoard();
}

void Board::initializeBoard() {
    m_tiles.clear();
    m_tiles.reserve(24);

    // 0: Base Camp Alpha (GO) - 200 Credits Salary
    m_tiles.push_back(std::make_unique<BaseCampTile>(0, "Base Camp Alpha", 200, false));

    // 1: Coastal Property (Tier 1 - Pristine Rent: 15)
    m_tiles.push_back(std::make_unique<PropertyTile>(1, "Shipwreck Cove", SectorGroup::COASTAL, 100, 15, 45, 30, 50));

    // 2: Event Hazard
    m_tiles.push_back(std::make_unique<EventTile>(2, "Tide Pool Mystery", "Shifting tides and sudden rogue waves crash ashore!"));

    // 3: Coastal Property (Tier 1 - Pristine Rent: 20)
    m_tiles.push_back(std::make_unique<PropertyTile>(3, "Flotsam Reef", SectorGroup::COASTAL, 120, 20, 55, 35, 60));

    // 4: Max Autos Transit
    m_tiles.push_back(std::make_unique<UtilityTile>(4, "Max Autos: Coastal Buggy", 150, 40));

    // 5: Jungle Property (Tier 1 - Pristine Rent: 22)
    m_tiles.push_back(std::make_unique<PropertyTile>(5, "Bamboo Thicket", SectorGroup::JUNGLE, 130, 22, 65, 40, 65));

    // 6: Ministry of Finance
    m_tiles.push_back(std::make_unique<LicenseTile>(6, "Ministry of Finance", 250));

    // 7: Jungle Property (Tier 1 - Pristine Rent: 28)
    m_tiles.push_back(std::make_unique<PropertyTile>(7, "Canopy Heights", SectorGroup::JUNGLE, 150, 28, 80, 45, 75));

    // 8: Event Hazard
    m_tiles.push_back(std::make_unique<EventTile>(8, "Monsoon Surge", "Monsoonal tempest batters the inland forestry!"));

    // 9: Jungle Property (Tier 1 - Pristine Rent: 32)
    m_tiles.push_back(std::make_unique<PropertyTile>(9, "Jaguar Den", SectorGroup::JUNGLE, 170, 32, 95, 50, 85));

    // 10: Max Autos Transit
    m_tiles.push_back(std::make_unique<UtilityTile>(10, "Max Autos: Jungle Tram", 180, 50));

    // 11: Geothermal Property (Tier 1 - Pristine Rent: 35)
    m_tiles.push_back(std::make_unique<PropertyTile>(11, "Sulfur Springs", SectorGroup::GEOTHERMAL, 190, 35, 105, 55, 95));

    // 12: Survival Outpost Bravo (Safe Haven) - 100 Credits
    m_tiles.push_back(std::make_unique<BaseCampTile>(12, "Outpost Bravo", 100, true));

    // 13: Geothermal Property (Tier 1 - Pristine Rent: 40)
    m_tiles.push_back(std::make_unique<PropertyTile>(13, "Obsidian Ridge", SectorGroup::GEOTHERMAL, 210, 40, 120, 60, 105));

    // 14: Event Hazard
    m_tiles.push_back(std::make_unique<EventTile>(14, "Volcanic Tremor", "Magma shifts subterranean vaults; toxic fumes surge!"));

    // 15: Geothermal Property (Tier 1 - Pristine Rent: 45)
    m_tiles.push_back(std::make_unique<PropertyTile>(15, "Lava Cascades", SectorGroup::GEOTHERMAL, 230, 45, 135, 65, 115));

    // 16: Max Autos Transit
    m_tiles.push_back(std::make_unique<UtilityTile>(16, "Max Autos: Magma Rover", 200, 60));

    // 17: Ancient Property (Tier 2 - Pristine Rent: 55)
    m_tiles.push_back(std::make_unique<PropertyTile>(17, "Ruins of Eldoria", SectorGroup::ANCIENT, 260, 55, 160, 75, 130));

    // 18: Ministry of Finance (Treasury Vault)
    m_tiles.push_back(std::make_unique<LicenseTile>(18, "Treasury Vault", 250));

    // 19: Ancient Property (Tier 2 - Pristine Rent: 65)
    m_tiles.push_back(std::make_unique<PropertyTile>(19, "Sunken Citadel", SectorGroup::ANCIENT, 280, 65, 185, 85, 145));

    // 20: Event Hazard
    m_tiles.push_back(std::make_unique<EventTile>(20, "Hurricane Cataclysm", "Category 5 gale sweeps across all island structures!"));

    // 21: Ancient Property (Tier 2 - Pristine Rent: 75)
    m_tiles.push_back(std::make_unique<PropertyTile>(21, "Temple of the Ruin King", SectorGroup::ANCIENT, 320, 75, 220, 95, 165));

    // 22: Max Autos Transit
    m_tiles.push_back(std::make_unique<UtilityTile>(22, "Max Autos: Aerial Monorail", 220, 70));

    // 23: Event Hazard
    m_tiles.push_back(std::make_unique<EventTile>(23, "Smuggler's Rift", "Dangerous salvage chasm where survivors gamble for loot!"));
}

Tile* Board::getTile(int index) {
    if (index >= 0 && index < static_cast<int>(m_tiles.size())) {
        return m_tiles[index].get();
    }
    return nullptr;
}

const Tile* Board::getTile(int index) const {
    if (index >= 0 && index < static_cast<int>(m_tiles.size())) {
        return m_tiles[index].get();
    }
    return nullptr;
}

PropertyTile* Board::getProperty(int index) {
    Tile* t = getTile(index);
    if (t && t->getType() == TileType::PROPERTY) {
        return dynamic_cast<PropertyTile*>(t);
    }
    return nullptr;
}

const PropertyTile* Board::getProperty(int index) const {
    const Tile* t = getTile(index);
    if (t && t->getType() == TileType::PROPERTY) {
        return dynamic_cast<const PropertyTile*>(t);
    }
    return nullptr;
}

UtilityTile* Board::getUtility(int index) {
    Tile* t = getTile(index);
    if (t && t->getType() == TileType::UTILITY) {
        return dynamic_cast<UtilityTile*>(t);
    }
    return nullptr;
}

const UtilityTile* Board::getUtility(int index) const {
    const Tile* t = getTile(index);
    if (t && t->getType() == TileType::UTILITY) {
        return dynamic_cast<const UtilityTile*>(t);
    }
    return nullptr;
}

std::vector<PropertyTile*> Board::getPropertiesByOwner(int ownerId) {
    std::vector<PropertyTile*> result;
    for (const auto& tile : m_tiles) {
        if (tile->getType() == TileType::PROPERTY) {
            auto* prop = dynamic_cast<PropertyTile*>(tile.get());
            if (prop && prop->getOwnerId() == ownerId) {
                result.push_back(prop);
            }
        }
    }
    return result;
}

std::vector<UtilityTile*> Board::getUtilitiesByOwner(int ownerId) {
    std::vector<UtilityTile*> result;
    for (const auto& tile : m_tiles) {
        if (tile->getType() == TileType::UTILITY) {
            auto* util = dynamic_cast<UtilityTile*>(tile.get());
            if (util && util->getOwnerId() == ownerId) {
                result.push_back(util);
            }
        }
    }
    return result;
}
