import os
import sys
import json
import random
import string
import socket
import webbrowser
from pathlib import Path
from dotenv import load_dotenv

# Force UTF-8 stdout on Windows
if sys.platform == "win32":
    try:
        sys.stdout.reconfigure(encoding="utf-8")
    except Exception:
        pass

import tornado.ioloop
import tornado.web
import tornado.websocket

PROJECT_DIR = Path(__file__).parent.resolve()

# Load environment variables (.env)
load_dotenv(PROJECT_DIR / ".env")
load_dotenv()

# Sarvam AI Initialization
SARVAM_API_KEY = os.environ.get("SARVAM_API_KEY", "").strip()
sarvam_client = None
if SARVAM_API_KEY:
    try:
        from sarvamai import SarvamAI
        sarvam_client = SarvamAI(api_subscription_key=SARVAM_API_KEY)
        print(">> Sarvam AI Voice Engine initialized successfully.")
    except Exception as e:
        print(f">> Warning: Failed to initialize SarvamAI client: {e}")
else:
    print(">> Note: SARVAM_API_KEY not found in environment. TTS will run in mock/silent mode.")

# In-memory audio cache: (text, language_code, speaker) -> base64 audio
TTS_CACHE = {}

def synthesize_voice(text, language_code="en-IN", speaker="shubh"):
    """Synthesize voice using Sarvam AI Bulbul TTS with memory caching."""
    if not sarvam_client or not text:
        return None
    cache_key = (text.strip(), language_code, speaker)
    if cache_key in TTS_CACHE:
        return TTS_CACHE[cache_key]

    try:
        # Try bulbul:v4-flash first
        res = sarvam_client.text_to_speech.convert(
            text=text.strip(),
            language_code=language_code,
            speaker=speaker,
            model="bulbul:v4-flash"
        )
        if res.audios and len(res.audios) > 0:
            TTS_CACHE[cache_key] = res.audios[0]
            return res.audios[0]
    except Exception as e:
        # Fallback to bulbul:v3 with compatible speaker
        try:
            fallback_speaker = "shubh" if language_code == "en-IN" else "ritu"
            res = sarvam_client.text_to_speech.convert(
                text=text.strip(),
                language_code=language_code,
                speaker=fallback_speaker,
                model="bulbul:v3"
            )
            if res.audios and len(res.audios) > 0:
                TTS_CACHE[cache_key] = res.audios[0]
                return res.audios[0]
        except Exception as e2:
            print(f">> Sarvam TTS Error: {e2}")
            return None
    return None

def find_available_port(start_port=8080, max_attempts=10):
    for port in range(start_port, start_port + max_attempts):
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            if s.connect_ex(('127.0.0.1', port)) != 0:
                return port
    return start_port

DEFAULT_PORT = int(os.environ.get("PORT", 0))
if DEFAULT_PORT == 0:
    PORT = find_available_port(8080)
else:
    PORT = DEFAULT_PORT

TILE_DATA = [
    {"id": 0, "name": "Base Camp Alpha", "sector": "Special", "type": "BASE_CAMP", "cost": 0, "rent": 200, "color": 0x06b6d4},
    {"id": 1, "name": "Shipwreck Cove", "sector": "Coastal", "type": "PROPERTY", "cost": 100, "rent": 15, "fortifiedRent": 45, "repairCost": 30, "upgradeCost": 50, "color": 0x06b6d4},
    {"id": 2, "name": "Tide Pool Mystery", "sector": "Wilderness", "type": "EVENT", "cost": 0, "rent": 0, "color": 0xef4444},
    {"id": 3, "name": "Flotsam Reef", "sector": "Coastal", "type": "PROPERTY", "cost": 120, "rent": 20, "fortifiedRent": 55, "repairCost": 35, "upgradeCost": 60, "color": 0x06b6d4},
    {"id": 4, "name": "Max Autos: Coastal", "sector": "Transit", "type": "UTILITY", "cost": 150, "rent": 40, "color": 0x3b82f6},
    {"id": 5, "name": "Bamboo Thicket", "sector": "Jungle", "type": "PROPERTY", "cost": 130, "rent": 22, "fortifiedRent": 65, "repairCost": 40, "upgradeCost": 65, "color": 0x10b981},
    {"id": 6, "name": "Ministry of Finance", "sector": "Government", "type": "LICENSE", "cost": 250, "rent": 0, "color": 0xeab308},
    {"id": 7, "name": "Canopy Heights", "sector": "Jungle", "type": "PROPERTY", "cost": 150, "rent": 28, "fortifiedRent": 80, "repairCost": 45, "upgradeCost": 75, "color": 0x10b981},
    {"id": 8, "name": "Monsoon Surge", "sector": "Wilderness", "type": "EVENT", "cost": 0, "rent": 0, "color": 0xef4444},
    {"id": 9, "name": "Jaguar Den", "sector": "Jungle", "type": "PROPERTY", "cost": 170, "rent": 32, "fortifiedRent": 95, "repairCost": 50, "upgradeCost": 85, "color": 0x10b981},
    {"id": 10, "name": "Max Autos: Jungle", "sector": "Transit", "type": "UTILITY", "cost": 180, "rent": 50, "color": 0x3b82f6},
    {"id": 11, "name": "Sulfur Springs", "sector": "Geothermal", "type": "PROPERTY", "cost": 190, "rent": 35, "fortifiedRent": 105, "repairCost": 55, "upgradeCost": 95, "color": 0xf97316},
    {"id": 12, "name": "Outpost Bravo", "sector": "Special", "type": "BASE_CAMP", "cost": 0, "rent": 100, "color": 0x06b6d4},
    {"id": 13, "name": "Obsidian Ridge", "sector": "Geothermal", "type": "PROPERTY", "cost": 210, "rent": 40, "fortifiedRent": 120, "repairCost": 60, "upgradeCost": 105, "color": 0xf97316},
    {"id": 14, "name": "Volcanic Tremor", "sector": "Wilderness", "type": "EVENT", "cost": 0, "rent": 0, "color": 0xef4444},
    {"id": 15, "name": "Lava Cascades", "sector": "Geothermal", "type": "PROPERTY", "cost": 230, "rent": 45, "fortifiedRent": 135, "repairCost": 65, "upgradeCost": 115, "color": 0xf97316},
    {"id": 16, "name": "Max Autos: Magma", "sector": "Transit", "type": "UTILITY", "cost": 200, "rent": 60, "color": 0x3b82f6},
    {"id": 17, "name": "Ruins of Eldoria", "sector": "Ancient", "type": "PROPERTY", "cost": 260, "rent": 55, "fortifiedRent": 160, "repairCost": 75, "upgradeCost": 130, "color": 0xa855f7},
    {"id": 18, "name": "Treasury Vault", "sector": "Government", "type": "LICENSE", "cost": 250, "rent": 0, "color": 0xeab308},
    {"id": 19, "name": "Sunken Citadel", "sector": "Ancient", "type": "PROPERTY", "cost": 280, "rent": 65, "fortifiedRent": 185, "repairCost": 85, "upgradeCost": 145, "color": 0xa855f7},
    {"id": 20, "name": "Hurricane Cataclysm", "sector": "Wilderness", "type": "EVENT", "cost": 0, "rent": 0, "color": 0xef4444},
    {"id": 21, "name": "Temple of Ruin King", "sector": "Ancient", "type": "PROPERTY", "cost": 320, "rent": 75, "fortifiedRent": 220, "repairCost": 95, "upgradeCost": 165, "color": 0xa855f7},
    {"id": 22, "name": "Max Autos: Monorail", "sector": "Transit", "type": "UTILITY", "cost": 220, "rent": 70, "color": 0x3b82f6},
    {"id": 23, "name": "Smuggler's Rift", "sector": "Wilderness", "type": "EVENT", "cost": 0, "rent": 0, "color": 0xef4444}
]

ROOMS = {}

class GameRoom:
    def __init__(self, code, host_name):
        self.code = code
        self.host_name = host_name
        self.game_started = False
        self.round = 1
        self.current_turn_index = 0
        self.finance_manager_id = -1
        self.game_over = False
        self.winner_id = -1

        self.slots = [
            {"id": 0, "name": host_name, "color": "Red",   "hex": "#EF4444", "is_bot": False, "ws": None, "pos": 0, "capital": 1500, "peak_capital": 1500, "lowest_capital": 1500, "total_rent_paid": 0, "total_rent_earned": 0, "is_bankrupt": False, "tamper": 0, "notes": {500:2, 100:2, 50:2, 20:6, 10:5, 5:5, 1:5}, "properties": []},
            {"id": 1, "name": "Waiting...", "color": "Blue",  "hex": "#3B82F6", "is_bot": True,  "ws": None, "pos": 0, "capital": 1500, "peak_capital": 1500, "lowest_capital": 1500, "total_rent_paid": 0, "total_rent_earned": 0, "is_bankrupt": False, "tamper": 0, "notes": {500:2, 100:2, 50:2, 20:6, 10:5, 5:5, 1:5}, "properties": []},
            {"id": 2, "name": "Waiting...", "color": "Green", "hex": "#10B981", "is_bot": True,  "ws": None, "pos": 0, "capital": 1500, "peak_capital": 1500, "lowest_capital": 1500, "total_rent_paid": 0, "total_rent_earned": 0, "is_bankrupt": False, "tamper": 0, "notes": {500:2, 100:2, 50:2, 20:6, 10:5, 5:5, 1:5}, "properties": []},
            {"id": 3, "name": "Waiting...", "color": "Gold",  "hex": "#F59E0B", "is_bot": True,  "ws": None, "pos": 0, "capital": 1500, "peak_capital": 1500, "lowest_capital": 1500, "total_rent_paid": 0, "total_rent_earned": 0, "is_bankrupt": False, "tamper": 0, "notes": {500:2, 100:2, 50:2, 20:6, 10:5, 5:5, 1:5}, "properties": []}
        ]

        self.tiles = [{
            "id": t["id"],
            "name": t["name"],
            "sector": t["sector"],
            "type": t["type"],
            "cost": t["cost"],
            "rent": t["rent"],
            "fortifiedRent": t.get("fortifiedRent", t["rent"]),
            "repairCost": t.get("repairCost", 40),
            "upgradeCost": t.get("upgradeCost", 60),
            "ownerId": -1,
            "state": "PRISTINE",
            "isMortgaged": False
        } for t in TILE_DATA]

        self.recent_events = [f"Room {code} initialized by {host_name}."]
        self.action_history = []
        self.chat_history = []
        self.calamity_deck = [
            {"title": "Tsunami Surge", "desc": "Tidal wave sweeps Coastal sectors into RUINS!", "type": "RUIN_COASTAL"},
            {"title": "Pyroclastic Tremor", "desc": "Subterranean vents burst! Geothermal sectors are RUINED!", "type": "RUIN_GEO"},
            {"title": "Jungle Rot Swarm", "desc": "Termite swarm devours Jungle sectors into RUINS!", "type": "RUIN_JUNGLE"},
            {"title": "Island Super-Cyclone", "desc": "Devastates highest-rent sector into RUINS!", "type": "RUIN_HIGHEST"},
            {"title": "Cargo Crate Salvage", "desc": "Floating shipping crate salvaged (+150 Credits)!", "type": "SALVAGE_150"},
            {"title": "Ration Spoilage Tax", "desc": "Medical emergency! Each survivor pays $50.", "type": "TAX_50"}
        ]
        random.shuffle(self.calamity_deck)

    def log_event(self, text):
        self.recent_events.append(text)
        if len(self.recent_events) > 15:
            self.recent_events.pop(0)

    def change_capital(self, player, delta):
        player["capital"] += delta
        if player["capital"] > player.get("peak_capital", player["capital"]):
            player["peak_capital"] = player["capital"]
        if player["capital"] < player.get("lowest_capital", player["capital"]):
            player["lowest_capital"] = player["capital"]
        if delta < 0:
            player["total_rent_paid"] = player.get("total_rent_paid", 0) + abs(delta)
        elif delta > 0:
            player["total_rent_earned"] = player.get("total_rent_earned", 0) + delta

    def record_action(self, player, action_title, movement, cause, effect, financial="", category="INFO"):
        entry = {
            "id": len(self.action_history) + 1,
            "round": self.round,
            "player_id": player["id"],
            "player_name": player["name"],
            "player_hex": player["hex"],
            "action": action_title,
            "movement": movement,
            "cause": cause,
            "effect": effect,
            "financial": financial,
            "category": category
        }
        self.action_history.append(entry)
        if len(self.action_history) > 30:
            self.action_history.pop(0)
        self.broadcast({"type": "action_recorded", "entry": entry})

    def broadcast(self, message_dict):
        msg_str = json.dumps(message_dict)
        for slot in self.slots:
            ws = slot["ws"]
            if ws and not ws.ws_connection.is_closing():
                try:
                    ws.write_message(msg_str)
                except Exception:
                    pass

    def get_public_state(self):
        return {
            "type": "state_update",
            "room_code": self.code,
            "game_started": self.game_started,
            "round": self.round,
            "current_turn_index": self.current_turn_index,
            "finance_manager_id": self.finance_manager_id,
            "game_over": self.game_over,
            "winner_id": self.winner_id,
            "players": [{
                "id": p["id"],
                "name": p["name"],
                "color": p["color"],
                "hex": p["hex"],
                "is_bot": p["is_bot"],
                "connected": p["ws"] is not None,
                "pos": p["pos"],
                "capital": p["capital"],
                "peak_capital": p.get("peak_capital", p["capital"]),
                "lowest_capital": p.get("lowest_capital", p["capital"]),
                "total_rent_paid": p.get("total_rent_paid", 0),
                "total_rent_earned": p.get("total_rent_earned", 0),
                "is_bankrupt": p["is_bankrupt"],
                "tamper": p["tamper"],
                "notes": p["notes"],
                "properties": p["properties"]
            } for p in self.slots],
            "tiles": self.tiles,
            "recent_events": self.recent_events,
            "action_history": self.action_history[-20:],
            "chat_history": self.chat_history[-10:]
        }

    def start_game(self):
        self.game_started = True
        bot_names = ["Navigator", "Engineer", "Scavenger", "Sentinel"]
        for p in self.slots:
            if p["ws"] is None:
                p["is_bot"] = True
                p["name"] = f"Bot_{bot_names[p['id']]}"
        self.log_event("The match has begun! 4 survivors deployed to Base Camp Alpha.")
        self.broadcast(self.get_public_state())

        if self.slots[0]["is_bot"]:
            tornado.ioloop.IOLoop.current().call_later(1.2, self.step_bot_turn)

    def advance_turn(self):
        self.current_turn_index = (self.current_turn_index + 1) % 4
        if self.current_turn_index == 0:
            self.round += 1
            self.log_event(f">>> === LAP #{self.round} COMMENCED === <<<")

        self.broadcast(self.get_public_state())

        cur = self.slots[self.current_turn_index]
        if cur["is_bot"] and not cur["is_bankrupt"] and not self.game_over:
            tornado.ioloop.IOLoop.current().call_later(1.2, self.step_bot_turn)

    def execute_roll(self, player_id):
        player = self.slots[player_id]
        if player["is_bankrupt"] or self.game_over:
            return

        d1 = random.randint(1, 6)
        d2 = random.randint(1, 6)
        steps = d1 + d2

        old_pos = player["pos"]
        player["pos"] = (player["pos"] + steps) % 24

        if player["pos"] < old_pos:
            self.change_capital(player, 200)
            player["notes"][100] = player["notes"].get(100, 0) + 1
            player["notes"][50]  = player["notes"].get(50, 0) + 1
            player["notes"][20]  = player["notes"].get(20, 0) + 2
            player["notes"][10]  = player["notes"].get(10, 0) + 1
            self.log_event(f"🏕️ {player['name']} completed a lap and collected $200 Base Camp Salary (1x 100, 1x 50, 2x 20, 1x 10)!")
            self.record_action(
                player=player,
                action_title=f"Rolled {steps} ({d1} + {d2})",
                movement=f"Completed lap circuit ➔ Tile #{player['pos']}",
                cause="Completed a full lap across the island circuit",
                effect="Collected $200 Base Camp Salary in structured notes",
                financial="+$200",
                category="SALARY"
            )

        landed_tile = self.tiles[player["pos"]]
        self.broadcast({
            "type": "dice_rolled",
            "player_id": player_id,
            "d1": d1, "d2": d2, "steps": steps,
            "new_pos": player["pos"],
            "tile_name": landed_tile["name"]
        })

        if landed_tile["type"] == "PROPERTY":
            if landed_tile["ownerId"] == -1:
                if player["capital"] >= landed_tile["cost"]:
                    self.change_capital(player, -landed_tile["cost"])
                    self.spend_notes(player, landed_tile["cost"])
                    landed_tile["ownerId"] = player["id"]
                    player["properties"].append(landed_tile["id"])
                    self.log_event(f"🏝️ {player['name']} acquired {landed_tile['name']} for ${landed_tile['cost']}!")
                    self.record_action(
                        player=player,
                        action_title=f"Acquired {landed_tile['name']}",
                        movement=f"Landed on #{landed_tile['id']} ({landed_tile['name']})",
                        cause=f"Sector was unowned. Player had sufficient funds (${landed_tile['cost']})",
                        effect=f"Purchased ownership. Future visitors must pay ${landed_tile['rent']} rent.",
                        financial=f"-${landed_tile['cost']}",
                        category="BUY"
                    )
                    self.broadcast({
                        "type": "transaction_alert",
                        "category": "BUY",
                        "title": "SECTOR PURCHASED",
                        "buyer_id": player["id"],
                        "buyer_name": player["name"],
                        "buyer_hex": player["hex"],
                        "tile_name": landed_tile["name"],
                        "cost": landed_tile["cost"],
                        "rent": landed_tile["rent"]
                    })
            elif landed_tile["ownerId"] != player["id"]:
                owner = self.slots[landed_tile["ownerId"]]
                rent = landed_tile["fortifiedRent"] if landed_tile["state"] == "FORTIFIED" else landed_tile["rent"]
                if landed_tile["state"] != "RUINED" and not landed_tile["isMortgaged"]:
                    fee = 0
                    minister_name = ""
                    if self.finance_manager_id != -1 and self.finance_manager_id != player["id"]:
                        fee = max(1, int(rent * 0.05))
                        minister = self.slots[self.finance_manager_id]
                        minister_name = minister["name"]
                        self.change_capital(minister, fee)
                        self.add_notes(minister, fee)

                    net_rent = rent - fee
                    self.change_capital(player, -rent)
                    self.spend_notes(player, rent)
                    self.change_capital(owner, net_rent)
                    self.add_notes(owner, net_rent)

                    msg = f"💰 {player['name']} paid ${net_rent} rent to {owner['name']}"
                    if fee > 0:
                        msg += f" [🏛️ ${fee} cut to Minister]"
                    self.log_event(msg)

                    self.record_action(
                        player=player,
                        action_title="House Call: Paid Rent",
                        movement=f"Entered {owner['name']}'s #{landed_tile['id']} ({landed_tile['name']})",
                        cause=f"Trespassed onto rival {landed_tile['state']} sector",
                        effect=f"Paid ${net_rent} to {owner['name']}" + (f" and ${fee} royalty to Minister {minister_name}" if fee > 0 else ""),
                        financial=f"-${rent}",
                        category="RENT"
                    )

                    # Active Transaction Alert Broadcast
                    self.broadcast({
                        "type": "transaction_alert",
                        "category": "RENT",
                        "title": "HOUSE CALL: RENT SURRENDERED",
                        "tenant_id": player["id"],
                        "tenant_name": player["name"],
                        "tenant_hex": player["hex"],
                        "landlord_id": owner["id"],
                        "landlord_name": owner["name"],
                        "landlord_hex": owner["hex"],
                        "tile_name": landed_tile["name"],
                        "tile_state": landed_tile["state"],
                        "base_rent": rent,
                        "minister_fee": fee,
                        "minister_name": minister_name,
                        "net_rent": net_rent
                    })

                    if player["capital"] < 0:
                        player["is_bankrupt"] = True
                        self.log_event(f"💀 {player['name']} went BANKRUPT and was eliminated!")
                        self.record_action(
                            player=player,
                            action_title="Survivor Bankrupt",
                            movement=f"Eliminated at #{landed_tile['id']} ({landed_tile['name']})",
                            cause=f"Unable to afford ${rent} rent payment to {owner['name']}",
                            effect="All assets surrendered to the bank. Player eliminated from the island.",
                            financial=f"-${abs(player['capital'])} deficit",
                            category="BANKRUPT"
                        )
                        voice_text = f"Survivor eliminated! {player['name']} has declared bankruptcy and leaves the island."
                        audio = synthesize_voice(voice_text, "en-IN", "shubh")
                        self.broadcast({
                            "type": "voice_announcement",
                            "text": voice_text,
                            "category": "bankruptcy",
                            "audio": audio
                        })
                        self.check_win()

        elif landed_tile["type"] == "LICENSE":
            if player["capital"] >= landed_tile["cost"] and self.finance_manager_id != player["id"]:
                self.change_capital(player, -landed_tile["cost"])
                self.spend_notes(player, landed_tile["cost"])
                self.finance_manager_id = player["id"]
                self.log_event(f"🏛️ {player['name']} acquired the FINANCE MANAGER LICENSE (5% royalties on all rents & loans)!")
                self.record_action(
                    player=player,
                    action_title="Minister of Finance Appointed",
                    movement=f"Landed on #{landed_tile['id']} ({landed_tile['name']})",
                    cause=f"Acquired the government license for ${landed_tile['cost']}",
                    effect="Will automatically receive a 5% royalty whenever another player pays rent or borrows funds",
                    financial=f"-${landed_tile['cost']}",
                    category="LICENSE"
                )
                self.broadcast({
                    "type": "transaction_alert",
                    "category": "LICENSE",
                    "title": "FINANCE MINISTER APPOINTED",
                    "minister_id": player["id"],
                    "minister_name": player["name"],
                    "minister_hex": player["hex"],
                    "tile_name": landed_tile["name"],
                    "cost": landed_tile["cost"]
                })
                # Voice announcement
                voice_text = f"Notice! {player['name']} has acquired the Ministry of Finance license. A five percent toll is now active across the island."
                audio = synthesize_voice(voice_text, "en-IN", "aditya")
                self.broadcast({
                    "type": "voice_announcement",
                    "text": voice_text,
                    "category": "license",
                    "audio": audio
                })

        elif landed_tile["type"] == "EVENT":
            self.trigger_calamity(player)

        self.broadcast(self.get_public_state())

    def trigger_calamity(self, player):
        card = self.calamity_deck.pop(0)
        self.calamity_deck.append(card)
        self.log_event(f"⚡ Calamity: {card['title']} - {card['desc']}")

        self.record_action(
            player=player,
            action_title=f"Calamity: {card['title']}",
            movement=f"Triggered at Tile #{player['pos']}",
            cause="Landed on Island Wilderness Hazard Sector",
            effect=card["desc"],
            financial="-$50 tax" if card["type"] == "TAX_50" else "+$150 salvage" if card["type"] == "SALVAGE_150" else "Sectors Damaged",
            category="CALAMITY"
        )

        # Voice broadcast of island calamity
        voice_text = f"Emergency Alert! {card['title']}. {card['desc']}"
        audio = synthesize_voice(voice_text, "en-IN", "shubh")
        self.broadcast({
            "type": "voice_announcement",
            "text": voice_text,
            "category": "calamity",
            "audio": audio
        })

        if card["type"] == "RUIN_COASTAL":
            for t in self.tiles:
                if t["sector"] == "Coastal" and t["state"] != "RUINED":
                    t["state"] = "RUINED"
        elif card["type"] == "RUIN_GEO":
            for t in self.tiles:
                if t["sector"] == "Geothermal" and t["state"] != "RUINED":
                    t["state"] = "RUINED"
        elif card["type"] == "RUIN_JUNGLE":
            for t in self.tiles:
                if t["sector"] == "Jungle" and t["state"] != "RUINED":
                    t["state"] = "RUINED"
        elif card["type"] == "SALVAGE_150":
            self.change_capital(player, 150)
            self.add_notes(player, 150)
        elif card["type"] == "TAX_50":
            for p in self.slots:
                if not p["is_bankrupt"]:
                    self.change_capital(p, -50)
                    self.spend_notes(p, 50)

    def generate_radio_broadcast(self, lang="en-IN", speaker="ashwin_en_sports_energetic"):
        active = [p for p in self.slots if not p["is_bankrupt"]]
        if not active:
            return None
        leader = max(active, key=lambda p: p["capital"])
        ruined_count = sum(1 for t in self.tiles if t["state"] == "RUINED")

        if lang == "hi-IN":
            text = f"कास्टअवे कैपिटल रेडियो बुलेटिन! राउंड {self.round} जारी है। {leader['name']} {leader['capital']} क्रेडिट्स के साथ आगे हैं। द्वीप पर {ruined_count} क्षेत्र खंडहर बन चुके हैं।"
            voice_spk = "shubh" if speaker == "shubh" else "ritu"
        else:
            text = f"Castaway Capital radio bulletin! Lap #{self.round} underway. {leader['name']} leads the island with {leader['capital']} credits. {ruined_count} sectors are currently ruined."
            voice_spk = speaker

        audio = synthesize_voice(text, lang, voice_spk)
        return {"text": text, "audio": audio, "lang": lang, "speaker": voice_spk}

    def spend_notes(self, player, amount):
        rem = amount
        for d in [500, 100, 50, 20, 10, 5, 1]:
            take = min(player["notes"].get(d, 0), rem // d)
            player["notes"][d] -= take
            rem -= take * d
        if rem > 0:
            for d in [5, 10, 20, 50, 100, 500]:
                if player["notes"].get(d, 0) > 0:
                    player["notes"][d] -= 1
                    change = d - rem
                    self.add_notes(player, change)
                    break

    def add_notes(self, player, amount):
        rem = amount
        for d in [500, 100, 50, 20, 10, 5, 1]:
            count = rem // d
            player["notes"][d] = player["notes"].get(d, 0) + count
            rem %= d

    def check_win(self):
        active = [p for p in self.slots if not p["is_bankrupt"]]
        if len(active) <= 1:
            self.game_over = True
            self.winner_id = active[0]["id"] if active else -1
            winner_name = active[0]["name"] if active else "Nobody"
            self.log_event(f"🏆 VICTORY! {winner_name} is the last survivor standing!")

            # Detailed survival dossier & high/low scorecard
            standings = []
            for p in self.slots:
                prop_val = sum(self.tiles[tid]["cost"] for tid in p["properties"])
                net_worth = p["capital"] + prop_val
                standings.append({
                    "id": p["id"],
                    "name": p["name"],
                    "color": p["color"],
                    "hex": p["hex"],
                    "final_capital": p["capital"],
                    "peak_capital": p.get("peak_capital", p["capital"]),
                    "lowest_capital": p.get("lowest_capital", p["capital"]),
                    "net_worth": net_worth,
                    "properties_count": len(p["properties"]),
                    "total_rent_paid": p.get("total_rent_paid", 0),
                    "total_rent_earned": p.get("total_rent_earned", 0),
                    "is_bankrupt": p["is_bankrupt"]
                })
            standings.sort(key=lambda s: (not s["is_bankrupt"], s["net_worth"]), reverse=True)
            for idx, s in enumerate(standings, 1):
                s["rank"] = idx

            self.broadcast({
                "type": "game_over_celebration",
                "winner_id": self.winner_id,
                "winner_name": winner_name,
                "standings": standings
            })

            voice_text = f"Game Over! Victory belongs to {winner_name}, the supreme survivor of Castaway Capital!"
            audio = synthesize_voice(voice_text, "en-IN", "ashwin_en_sports_energetic")
            self.broadcast({
                "type": "voice_announcement",
                "text": voice_text,
                "category": "victory",
                "audio": audio
            })

    def step_bot_turn(self):
        if self.game_over:
            return
        cur = self.slots[self.current_turn_index]
        if not cur["is_bot"] or cur["is_bankrupt"]:
            return

        for p_id in cur["properties"]:
            tile = self.tiles[p_id]
            if tile["state"] == "RUINED" and cur["capital"] >= tile["repairCost"] + 100:
                cur["capital"] -= tile["repairCost"]
                self.spend_notes(cur, tile["repairCost"])
                tile["state"] = "PRISTINE"
                self.log_event(f"🔧 {cur['name']} repaired {tile['name']} to [PRISTINE] for ${tile['repairCost']}!")
                break

        for p_id in cur["properties"]:
            tile = self.tiles[p_id]
            if tile["state"] == "PRISTINE" and cur["capital"] >= tile["upgradeCost"] + 200:
                cur["capital"] -= tile["upgradeCost"]
                self.spend_notes(cur, tile["upgradeCost"])
                tile["state"] = "FORTIFIED"
                self.log_event(f"🏰 {cur['name']} FORTIFIED {tile['name']}! Rent is now ${tile['fortifiedRent']}.")
                break

        self.execute_roll(self.current_turn_index)
        tornado.ioloop.IOLoop.current().call_later(1.5, self.advance_turn)


class MainHandler(tornado.web.RequestHandler):
    def get(self):
        self.set_header("Cache-Control", "no-cache, no-store, must-revalidate")
        self.render("index.html")

class HealthHandler(tornado.web.RequestHandler):
    def get(self):
        self.write({"status": "healthy", "service": "Castaway Capital Multiplayer Engine"})

class StateExportHandler(tornado.web.RequestHandler):
    def get(self):
        export_file = PROJECT_DIR / "export_state.json"
        if export_file.exists():
            self.set_header("Content-Type", "application/json")
            self.write(export_file.read_text(encoding="utf-8"))
        else:
            self.write({"error": "No export state yet."})


class GameWebSocketHandler(tornado.websocket.WebSocketHandler):
    def check_origin(self, origin):
        return True

    def open(self):
        self.room_code = None
        self.player_id = None

    def on_message(self, message):
        try:
            data = json.loads(message)
            msg_type = data.get("type")

            if msg_type == "create_room":
                code = ''.join(random.choices(string.ascii_uppercase + string.digits, k=6))
                host_name = data.get("name", "Host")[:16]
                room = GameRoom(code, host_name)
                room.slots[0]["name"] = host_name
                room.slots[0]["ws"] = self
                room.slots[0]["is_bot"] = False

                self.room_code = code
                self.player_id = 0
                ROOMS[code] = room

                self.write_message(json.dumps({
                    "type": "room_created",
                    "room_code": code,
                    "slot": 0,
                    "state": room.get_public_state()
                }))

            elif msg_type == "join_room":
                code = data.get("room_code", "").upper().strip()
                name = data.get("name", "Survivor")[:16]
                target_slot = data.get("slot")

                if code not in ROOMS:
                    self.write_message(json.dumps({"type": "error", "message": f"Room '{code}' not found."}))
                    return

                room = ROOMS[code]
                if room.game_started:
                    self.write_message(json.dumps({"type": "error", "message": "Game already in progress in this room."}))
                    return

                chosen_slot = None
                if target_slot is not None and 0 <= target_slot < 4 and room.slots[target_slot]["ws"] is None:
                    chosen_slot = target_slot
                else:
                    for i in range(4):
                        if room.slots[i]["ws"] is None:
                            chosen_slot = i
                            break

                if chosen_slot is None:
                    self.write_message(json.dumps({"type": "error", "message": "Room is full (max 4 survivors)."}))
                    return

                room.slots[chosen_slot]["name"] = name
                room.slots[chosen_slot]["ws"] = self
                room.slots[chosen_slot]["is_bot"] = False
                self.room_code = code
                self.player_id = chosen_slot

                room.log_event(f"👋 {name} joined as [{room.slots[chosen_slot]['color']}].")
                self.write_message(json.dumps({
                    "type": "room_joined",
                    "room_code": code,
                    "slot": chosen_slot,
                    "state": room.get_public_state()
                }))
                room.broadcast(room.get_public_state())

            elif msg_type == "start_game":
                if self.room_code in ROOMS:
                    room = ROOMS[self.room_code]
                    if self.player_id == 0:
                        room.start_game()

            elif msg_type == "roll_dice":
                if self.room_code in ROOMS:
                    room = ROOMS[self.room_code]
                    if room.game_started and room.current_turn_index == self.player_id:
                        room.execute_roll(self.player_id)

            elif msg_type == "end_turn":
                if self.room_code in ROOMS:
                    room = ROOMS[self.room_code]
                    if room.game_started and room.current_turn_index == self.player_id:
                        room.advance_turn()

            elif msg_type == "fortify":
                if self.room_code in ROOMS:
                    room = ROOMS[self.room_code]
                    tile_id = data.get("tile_id")
                    player = room.slots[self.player_id]
                    if tile_id in player["properties"]:
                        tile = room.tiles[tile_id]
                        if tile["state"] == "PRISTINE" and player["capital"] >= tile["upgradeCost"]:
                            player["capital"] -= tile["upgradeCost"]
                            room.spend_notes(player, tile["upgradeCost"])
                            tile["state"] = "FORTIFIED"
                            room.log_event(f"🏰 {player['name']} FORTIFIED {tile['name']}! Rent is now ${tile['fortifiedRent']}.")
                            room.broadcast(room.get_public_state())

            elif msg_type == "repair":
                if self.room_code in ROOMS:
                    room = ROOMS[self.room_code]
                    tile_id = data.get("tile_id")
                    player = room.slots[self.player_id]
                    if tile_id in player["properties"]:
                        tile = room.tiles[tile_id]
                        if tile["state"] == "RUINED" and player["capital"] >= tile["repairCost"]:
                            player["capital"] -= tile["repairCost"]
                            room.spend_notes(player, tile["repairCost"])
                            tile["state"] = "PRISTINE"
                            room.log_event(f"🔧 {player['name']} repaired {tile['name']} to [PRISTINE] for ${tile['repairCost']}!")
                            room.broadcast(room.get_public_state())

            elif msg_type == "request_broadcast":
                if self.room_code in ROOMS:
                    room = ROOMS[self.room_code]
                    lang = data.get("language_code", "en-IN")
                    speaker = data.get("speaker", "ashwin_en_sports_energetic")
                    bulletin = room.generate_radio_broadcast(lang=lang, speaker=speaker)
                    if bulletin:
                        room.broadcast({
                            "type": "voice_announcement",
                            "text": bulletin["text"],
                            "category": "radio_bulletin",
                            "audio": bulletin["audio"]
                        })

            elif msg_type == "send_chat":
                if self.room_code in ROOMS:
                    room = ROOMS[self.room_code]
                    player = room.slots[self.player_id]
                    text = data.get("text", "").strip()[:100]
                    if text:
                        chat_obj = {
                            "sender": player["name"],
                            "color": player["hex"],
                            "text": text
                        }
                        room.chat_history.append(chat_obj)
                        room.broadcast({"type": "chat_message", "chat": chat_obj})

        except Exception as e:
            self.write_message(json.dumps({"type": "error", "message": str(e)}))

    def on_close(self):
        if self.room_code and self.room_code in ROOMS:
            room = ROOMS[self.room_code]
            if self.player_id is not None and self.player_id < len(room.slots):
                room.slots[self.player_id]["ws"] = None
                room.log_event(f"⚠️ {room.slots[self.player_id]['name']} disconnected.")
                room.broadcast(room.get_public_state())


class TTSHandler(tornado.web.RequestHandler):
    def set_default_headers(self):
        self.set_header("Access-Control-Allow-Origin", "*")
        self.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS")
        self.set_header("Access-Control-Allow-Headers", "Content-Type")

    def options(self):
        self.set_status(204)
        self.finish()

    def post(self):
        try:
            data = json.loads(self.request.body) if self.request.body else {}
        except Exception:
            data = {}

        text = data.get("text", "").strip()
        lang = data.get("language_code", "en-IN")
        speaker = data.get("speaker", "shubh")

        if not text:
            self.set_status(400)
            self.write({"status": "error", "message": "Missing text parameter"})
            return

        audio = synthesize_voice(text, language_code=lang, speaker=speaker)
        if audio:
            self.write({
                "status": "ok",
                "audio": audio,
                "text": text,
                "speaker": speaker,
                "language_code": lang
            })
        else:
            self.write({
                "status": "unavailable",
                "message": "Sarvam AI TTS is unavailable or failed to generate audio.",
                "text": text
            })

    def get(self):
        text = self.get_argument("text", "").strip()
        lang = self.get_argument("lang", "en-IN")
        speaker = self.get_argument("speaker", "shubh")

        if not text:
            self.set_status(400)
            self.write({"status": "error", "message": "Missing text parameter"})
            return

        audio = synthesize_voice(text, language_code=lang, speaker=speaker)
        if audio:
            self.write({
                "status": "ok",
                "audio": audio,
                "text": text,
                "speaker": speaker,
                "language_code": lang
            })
        else:
            self.write({
                "status": "unavailable",
                "message": "Sarvam AI TTS is unavailable or failed to generate audio.",
                "text": text
            })


class TTSStatusHandler(tornado.web.RequestHandler):
    def get(self):
        self.set_header("Access-Control-Allow-Origin", "*")
        voices = [
            {"id": "shubh", "name": "🎙️ Shubh (Dramatic Radio Host)", "lang": "en-IN"},
            {"id": "ashwin_en_sports_energetic", "name": "⚡ Ashwin (Energetic Sportscaster)", "lang": "en-IN"},
            {"id": "ritu", "name": "🌺 Ritu (Lively Island Guide)", "lang": "en-IN"},
            {"id": "aditya", "name": "🏛️ Aditya (Ministry Overseer)", "lang": "en-IN"},
            {"id": "priya", "name": "🧭 Priya (Calm Island Dispatch)", "lang": "en-IN"},
            {"id": "shubh_hi", "name": "🎙️ Shubh (Hindi Commentary हिंदी)", "lang": "hi-IN", "speaker": "shubh"},
            {"id": "ritu_hi", "name": "🌺 Ritu (Hindi Radio हिंदी)", "lang": "hi-IN", "speaker": "ritu"}
        ]
        self.write({
            "status": "ok",
            "available": bool(sarvam_client),
            "engine": "Sarvam AI Bulbul (v4-flash / v3)",
            "voices": voices
        })


def make_app():
    return tornado.web.Application([
        (r"/", MainHandler),
        (r"/health", HealthHandler),
        (r"/api/tts", TTSHandler),
        (r"/api/tts/status", TTSStatusHandler),
        (r"/export_state.json", StateExportHandler),
        (r"/ws", GameWebSocketHandler),
        (r"/(.*)", tornado.web.StaticFileHandler, {"path": str(PROJECT_DIR)})
    ], template_path=str(PROJECT_DIR), static_path=str(PROJECT_DIR), debug=False)

if __name__ == "__main__":
    local_url = f"http://localhost:{PORT}"
    print("=" * 60)
    print("  CASTAWAY CAPITAL: MULTIPLAYER WEBSOCKET GAME SERVER")
    print(f"  Live Server URL: {local_url}")
    print("=" * 60)

    app = make_app()
    app.listen(PORT, address="0.0.0.0")

    # In local mode, automatically open the default web browser!
    if "PORT" not in os.environ:
        try:
            webbrowser.open(local_url)
        except Exception:
            pass

    try:
        tornado.ioloop.IOLoop.current().start()
    except KeyboardInterrupt:
        print("\nServer terminated.")
