# Castaway Capital: Island of Ruin 🏝️

An interactive, turn-based survival-monopoly simulation engine built in **Modern C++20**, paired with a **real-time 3D Three.js browser visualization**.

---

## 🏛️ System Architecture & Object-Oriented Design

- **`Tile.hpp` / `Tile.cpp`**: Abstract `Tile` base class with polymorphic `onLand(Player&, GameEngine&)` method.
  - `BaseCampTile`: Safe havens (Go / Outpost Bravo) distributing structured note salaries ($200 per lap).
  - `PropertyTile`: Island sectors with dynamic states (`RUINED`, `PRISTINE`, `FORTIFIED`).
  - `UtilityTile` ("Max Autos"): Transit routes enabling Fixed Toll collection and tactical 1d6 dice sabotage.
  - `LicenseTile` ("Ministry of Finance"): Allows acquiring the coveted Finance Manager License.
  - `EventTile` ("Island Calamity"): Triggers randomized disasters and salvage events from the Calamity Deck.
- **`Wallet.hpp` / `Wallet.cpp`**: Realistic physical currency wallet tracking 7 colorful note tiers (500, 100, 50, 20, 10, 5, 1) with an automated change-making algorithm and bank note breakdowns.
- **`Player.hpp` / `Player.cpp`**: Encapsulates survivor state, physical wallet note counts, owned sectors, mortgages, and AI heuristic routines.
- **`Board.hpp` / `Board.cpp`**: 24-sector circular island board managing `std::unique_ptr<Tile>` instances with zero raw pointer leaks.
- **`GameEngine.hpp` / `GameEngine.cpp`**: Strict 4-player turn controller, dice rolling, Max Autos tampering, 5% Ministry cuts, calamity card resolution, trading, bankruptcy, and real-time JSON state export (`export_state.json`).
- **`index.html`**: Interactive 3D WebGL Island Board using Three.js and OrbitControls. Displays 3D terrain, modular tile meshes, animated hopping pawns, physical note wallets, and interactive action controls.
- **`server.py`**: Lightweight local server providing instant state synchronization without CORS limitations.
- **`main.cpp`**: Terminal entry point supporting Human and AI survivors with ANSI color formatting.

---

## ⚙️ Core Mechanics

1. **Strict 4-Player Turn Cycle:**
   - Player 1 (Red) -> Player 2 (Blue) -> Player 3 (Green) -> Player 4 (Gold) -> Lap Increment.
   - Starting Bankroll: 1,500 Credits in physical notes.
   - Base Camp Salary: Passing or landing on Base Camp Alpha awards 200 Credits in structured notes (1x 100, 1x 50, 2x 20, 1x 10).
2. **Physical Note Denominations & Change-Making:**
   - 7 colorful note tiers: 500 (Purple), 100 (Teal), 50 (Orange), 20 (Green), 10 (Yellow), 5 (Pink), 1 (White).
   - Automatic bank note breakdowns when exact change is unavailable.
3. **The Finance Manager License:**
   - Landing on the Ministry of Finance allows purchasing the license for $250.
   - Automatically skims a **5% cut** on all rents paid between other players and on all asset loans (mortgages).
4. **Max Autos / Transit Monopoly:**
   - Owning a Max Autos route grants **Fixed Tolls** when opponents pass through.
   - Unlocks **Dice Tampering**: Once per round per utility owned, the owner can sabotage an approaching opponent's vehicle, restricting their roll to a **single 1d6** instead of 2d6.
5. **Pristine Assets vs. Ruined Assets:**
   - Properties hold 3 distinct states:
     - `RUINED`: Generates zero rent; costs a repair fee to restore.
     - `PRISTINE`: Generates base sector rent; can be upgraded.
     - `FORTIFIED`: Upgraded with permanent strongholds for high rent.
   - **Island Calamity Deck**: Calamity hazards degrade sectors into `RUINED` status or demand emergency survival taxes.

---

## 🚀 How to Run

### 🌐 Method 1: Interactive 3D Browser App (Recommended)
Double-click `launch_3d_app.bat` or run:
```powershell
python server.py
```
Open **http://localhost:8080** in your web browser.

### 💻 Method 2: C++ Engine (Console Simulation)
- To compile: run `build.bat`
- To launch: run `run.bat` or double-click `CastawayCapital.exe`.
```cmd
cl /EHsc /std:c++20 /W4 /O2 /Fe:CastawayCapital.exe main.cpp GameEngine.cpp Board.cpp Player.cpp Tile.cpp Wallet.cpp
CastawayCapital.exe
```
