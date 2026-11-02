# Castaway Capital: Island of Ruin 🏝️

[![C++ Standard](https://img.shields.io/badge/Language-Modern%20C%2B%2B20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![Three.js](https://img.shields.io/badge/3D%20Engine-Three.js%20r128-black.svg)](https://threejs.org/)
[![Multiplayer](https://img.shields.io/badge/Networking-WebSockets%20Realtime-green.svg)](https://www.tornadoweb.org/)
[![Deploy on Railway](https://railway.app/button.svg)](https://railway.app/template)

An interactive, turn-based survival-monopoly simulation engine and online multiplayer game. Features an authoritative real-time WebSocket game server, room-code matchmaking, realistic physical note currency mechanics, and a 3D Three.js WebGL island board.

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

## 🚆 Deploying to Railway (Public Online Server)

Deploy this project on [Railway](https://railway.app) for free to get a public `https://...up.railway.app` URL with live WebSocket support.

### Step-by-Step Deployment:

1. **Sign in to Railway:**
   - Go to [railway.app](https://railway.app) and sign in using your GitHub account.

2. **Create New Project from GitHub:**
   - Click **New Project** ➡️ **Deploy from GitHub repo**.
   - Select your repository: **`AryanBCrazy-prog/castaway-capital`**.

3. **Automatic Configuration:**
   - Railway will automatically detect [`railway.json`](railway.json) and [`Procfile`](Procfile).
   - It will install dependencies from [`requirements.txt`](requirements.txt) (`tornado>=6.2`) and launch `python server.py`.

4. **Generate Public Domain:**
   - Once the service deploys, click on the **Service box** in your Railway canvas.
   - Go to **Settings** ➡️ **Networking** ➡️ Click **Generate Domain**.
   - Railway will issue your public live HTTPS URL (e.g., `https://castaway-capital-production.up.railway.app`).

5. **Play Online with Friends:**
   - Open your Railway domain in any browser.
   - Click **Create Room**, copy the shareable room link, and send it to your friends to play!

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
