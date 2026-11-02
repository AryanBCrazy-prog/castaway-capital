#pragma once
#include <string>

// ANSI Terminal Colors & Styling
namespace UI {
    inline const std::string RESET   = "\033[0m";
    inline const std::string BOLD    = "\033[1m";
    inline const std::string RED     = "\033[31m";
    inline const std::string GREEN   = "\033[32m";
    inline const std::string YELLOW  = "\033[33m";
    inline const std::string BLUE    = "\033[34m";
    inline const std::string MAGENTA = "\033[35m";
    inline const std::string CYAN    = "\033[36m";
    inline const std::string WHITE   = "\033[37m";

    inline const std::string BG_BLUE   = "\033[44m";
    inline const std::string BG_RED    = "\033[41m";
    inline const std::string BG_DARK   = "\033[40m";
    inline const std::string BG_ORANGE = "\033[48;5;208m";
}

enum class TileType {
    BASE_CAMP,
    PROPERTY,
    UTILITY,
    LICENSE,
    EVENT
};

enum class PropertyState {
    RUINED,
    PRISTINE,
    FORTIFIED
};

enum class SectorGroup {
    COASTAL,
    JUNGLE,
    GEOTHERMAL,
    ANCIENT,
    TRANSIT,
    GOVERNMENT,
    WILDERNESS,
    SPECIAL
};

inline std::string sectorGroupToString(SectorGroup sector) {
    switch (sector) {
        case SectorGroup::COASTAL:    return "Coastal";
        case SectorGroup::JUNGLE:     return "Jungle";
        case SectorGroup::GEOTHERMAL: return "Geothermal";
        case SectorGroup::ANCIENT:    return "Ancient";
        case SectorGroup::TRANSIT:    return "Transit (Max Autos)";
        case SectorGroup::GOVERNMENT: return "Government (Ministry)";
        case SectorGroup::WILDERNESS: return "Wilderness";
        default:                      return "General";
    }
}

inline std::string propertyStateToString(PropertyState state) {
    switch (state) {
        case PropertyState::RUINED:    return UI::RED + "[RUINED]" + UI::RESET;
        case PropertyState::PRISTINE:  return UI::GREEN + "[PRISTINE]" + UI::RESET;
        case PropertyState::FORTIFIED: return UI::CYAN + UI::BOLD + "[FORTIFIED ★]" + UI::RESET;
        default:                       return "[UNKNOWN]";
    }
}
