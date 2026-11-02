# Castaway Capital: Island of Ruin 🏝️

[![C++ Standard](https://img.shields.io/badge/Language-Modern%20C%2B%2B20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![Three.js](https://img.shields.io/badge/3D%20Engine-Three.js%20r128-black.svg)](https://threejs.org/)
[![Multiplayer](https://img.shields.io/badge/Networking-WebSockets%20Realtime-green.svg)](https://www.tornadoweb.org/)
[![Deploy on Railway](https://railway.app/button.svg)](https://railway.app/template)
[![Sarvam AI Voice Edition](https://img.shields.io/badge/AI%20Voice%20Branch-sarvam--ai-purple.svg)](https://github.com/AryanBCrazy-prog/castaway-capital/tree/sarvam-ai)

> 🎙️ **NEW: Sarvam AI Voice Announcer Edition Available!**  
> Check out the dedicated **[`sarvam-ai`](https://github.com/AryanBCrazy-prog/castaway-capital/tree/sarvam-ai)** branch for real-time Text-to-Speech narration powered by **Sarvam AI Bulbul**, multilingual Indian commentary (English & Hindi), synchronized calamity radio alerts, and dynamic leaderboard analysis!  
> Switch to it locally with: `git checkout sarvam-ai`

An interactive, turn-based survival-monopoly simulation engine and online multiplayer game. Features an authoritative real-time WebSocket game server, room-code matchmaking, realistic physical note currency mechanics, and a 3D Three.js WebGL island board.

---

## 🌿 Available Branches

| Branch | Description | Key Capabilities |
| :--- | :--- | :--- |
| **[`main`](https://github.com/AryanBCrazy-prog/castaway-capital)** | Core 3D multiplayer WebSocket game engine | C++20 engine, Three.js 3D board, 7 physical denominations, AI heuristic bots, room matchmaking |
| **[`sarvam-ai`](https://github.com/AryanBCrazy-prog/castaway-capital/tree/sarvam-ai)** | Sarvam AI Bulbul TTS voice integration | Real-time Indian English & Hindi voice announcer, radio bulletin, audio caching (<1ms), soundwave visualizer |

---

## 🌟 Key Features

- **🌐 Live Online Multiplayer:** Play with friends anywhere in the world. Create a private room, copy a 1-click invite link (`?room=CODE`), and pick your survivor color (Red, Blue, Green, Gold).
- **🤖 Autonomous Heuristic AI:** Any unclaimed player slot is seamlessly piloted by AI survivor bots that balance capital reserves, repair ruined sectors, upgrade strongholds, and sabotage opponents.
- **🎮 3D WebGL Island World:** Built with Three.js and OrbitControls. Features procedural tropical terrain, beaches, jungle foliage, and a smoking volcano caldera with glowing lava.
- **♟️ Animated 3D Pawns:** Real-time parabolic jumping arcs as dice are rolled across the 24 circular island sectors.
- **💵 Physical Currency Wallet:** 7 colorful note denominations (500, 100, 50, 20, 10, 5, 1) with an automatic bank change-making algorithm.
- **🏛️ Ministry of Finance:** Land on government sectors to acquire the Finance Manager License and automatically skim a 5% royalty on all rents and loans across the island.
- **🏎️ Max Autos Monopoly:** Transit routes grant fixed tolls and unlock tactical **Dice Tampering** (force an opponent to roll a single 1d6 instead of 2d6).
- **⚡ Island Calamity Deck:** Randomized natural disasters (Tsunamis, Ashfall, Cyclones, Malaria) that degrade upgraded sectors into `RUINED` status.

---

## 🚀 Deploying Online (Free Public Server)

If Railway is down or requiring credit card verification, **Render.com** provides a 100% free cloud host that supports live WebSockets out of the box.

### Option A: Render.com (100% Free, No Credit Card Required)
1. Go to [dashboard.render.com](https://dashboard.render.com) and sign in with your GitHub account.
2. Click **New +** ➡️ **Web Service**.
3. Select your repository: **`AryanBCrazy-prog/castaway-capital`**.
4. Render automatically reads [`render.yaml`](render.yaml):
   - **Environment:** `Python`
   - **Build Command:** `pip install -r requirements.txt`
   - **Start Command:** `python server.py`
5. Select the **Free** tier and click **Create Web Service**.
6. Render will assign you a live HTTPS domain (e.g., `https://castaway-capital.onrender.com`).

### Option B: Railway (When Active)
1. Go to [railway.app](https://railway.app) and sign in with GitHub.
2. Click **New Project** ➡️ **Deploy from GitHub repo** ➡️ Choose `AryanBCrazy-prog/castaway-capital`.
3. In **Settings** ➡️ **Networking** ➡️ Click **Generate Domain** to get your public `https://...up.railway.app` URL.

---

## 💻 Local Quickstart

### Option 1: 3D Multiplayer Web App (Local)
Run the Python multiplayer WebSocket server:
```powershell
python server.py
```
Open your browser at **http://localhost:8080**.

### Option 2: C++ Engine (Console Simulation)
Compile and run the native C++20 engine:
```cmd
call build.bat
CastawayCapital.exe
```
*(Compiled with MSVC `cl.exe /EHsc /std:c++20 /W4 /O2` with zero raw pointer leaks).*

---

## 📁 Repository Structure

```
├── server.py           <- Python asynchronous WebSocket multiplayer game server
├── index.html          <- 3D WebGL Three.js client with OrbitControls & HUD
├── railway.json        <- Railway deployment configuration
├── Procfile            <- Process runner for cloud hosts (Railway / Render / Heroku)
├── requirements.txt    <- Python cloud dependencies (tornado)
├── Dockerfile          <- Universal container deployment
├── render.yaml         <- Render.com cloud deployment blueprint
├── launch_3d_app.bat   <- Windows one-click local server launcher
│
├── Enums.hpp           <- Shared sector groups, property states, ANSI styles
├── Tile.hpp / .cpp     <- Polymorphic Tile hierarchy (Property, Utility, License, Event, BaseCamp)
├── Wallet.hpp / .cpp   <- Physical currency wallet with bank change-maker
├── Player.hpp / .cpp   <- Survivor state, note counts, bot heuristics
├── Board.hpp / .cpp    <- 24-sector circular island board layout
├── GameEngine.hpp/.cpp <- Turn controller, dice mechanics, 5% cuts, state exporter
├── main.cpp            <- C++ native terminal entry point
├── build.bat           <- MSVC C++20 compiler script
└── export_state.json   <- Real-time engine state bridge
```

---

## 📜 License

Distributed under the MIT License. See `LICENSE` for more information.
