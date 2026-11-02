#pragma once
#include <vector>
#include <memory>
#include <string>
#include "Tile.hpp"

class Board {
private:
    std::vector<std::unique_ptr<Tile>> m_tiles;

public:
    Board();
    ~Board() = default;

    // Prevent copying because of std::unique_ptr
    Board(const Board&) = delete;
    Board& operator=(const Board&) = delete;
    Board(Board&&) = default;
    Board& operator=(Board&&) = default;

    void initializeBoard();

    Tile* getTile(int index);
    const Tile* getTile(int index) const;

    PropertyTile* getProperty(int index);
    const PropertyTile* getProperty(int index) const;

    UtilityTile* getUtility(int index);
    const UtilityTile* getUtility(int index) const;

    int size() const { return static_cast<int>(m_tiles.size()); }

    std::vector<PropertyTile*> getPropertiesByOwner(int ownerId);
    std::vector<UtilityTile*> getUtilitiesByOwner(int ownerId);
};
