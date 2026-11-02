# Castaway Capital: Island of Ruin 🏝️

An interactive, turn-based, console-driven survival-monopoly simulation engine built in **Modern C++20**.

---

## 🏛️ System Architecture & Object-Oriented Design

- **`Tile.hpp` / `Tile.cpp`**: Abstract `Tile` base class with polymorphic `onLand(Player&, GameEngine&)` method.
  - `BaseCampTile`: Safe havens (Go / Outpost Bravo) distributing emergency supplies.
  - `PropertyTile`: Island sectors with dynamic states (`RUINED`, `PRISTINE`, `FORTIFIED`).
  - `UtilityTile` ("Max Autos"): Transit routes enabling Fixed Toll collection and tactical 1d6 dice sabotage.
  - `LicenseTile` ("Ministry of Finance"): Allows acquiring the coveted Finance Manager License.
  - `EventTile` ("Island Calamity"): Triggers randomized disasters and salvage events from the Calamity Deck.
- **`Player.hpp` / `Player.cpp`**: Encapsulates survivor state, cash capital, owned sectors, mortgages, and AI heuristic routines.
- **`Board.hpp` / `Board.cpp`**: 24-sector circular island board managing `std::unique_ptr<Tile>` instances with zero raw pointer leaks.
- **`GameEngine.hpp` / `GameEngine.cpp`**: Complete game state orchestration, dice rolling, Max Autos tampering, 5% Ministry cuts, calamity card resolution, trading, bankruptcy, and victory verification.
- **`main.cpp`**: Interactive CLI with ANSI color support (Windows virtual terminal processing).

---

## ⚙️ Custom Mechanics Implemented

1. **The Finance Manager License:**
   - Landing on the Ministry of Finance allows purchasing the license for $250.
   - Automatically skims a **5% cut** on all rents paid between other players and on all asset loans (mortgages).
2. **Max Autos / Transit Monopoly:**
   - Owning a Max Autos route grants **Fixed Tolls** when opponents pass through.
   - Unlocks **Dice Tampering**: Once per round per utility owned, the owner can sabotage an approaching opponent's vehicle, restricting their roll to a **single 1d6** instead of 2d6 to strand them in high-hazard or fortified zones.
3. **Pristine Assets vs. Ruined Assets:**
   - Properties hold 3 distinct states:
     - `RUINED`: Generates zero rent; costs a repair fee to restore.
     - `PRISTINE`: Generates base sector rent; can be upgraded.
     - `FORTIFIED`: Upgraded with permanent strongholds for high rent.
   - **Island Calamity Deck**: Calamity hazards degrade sectors into `RUINED` status or demand emergency survival taxes.
4. **Interactive CLI & AI Bots:**
   - Supports 2 to 4 players (any mix of Humans and heuristic AI Bots).
   - Menu actions: Roll & Move, Fortify, Repair, Take Asset Loan, Repay Loan, Propose Trade, View Map, Surrender.

---

## 🚀 How to Build & Run

### Method 1: Using the Batch Scripts (Windows)
- To compile: run `build.bat`
- To launch: run `run.bat` or double-click it.

### Method 2: Manual Command Line
```cmd
cl /EHsc /std:c++20 /W4 /O2 /Fe:CastawayCapital.exe main.cpp GameEngine.cpp Board.cpp Player.cpp Tile.cpp
CastawayCapital.exe
```
