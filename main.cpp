#include <iostream>
#include <vector>
#include <string>
#include "GameEngine.hpp"

#ifdef _WIN32
#include <windows.h>
void enableVirtualTerminalProcessing() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }
}
#else
void enableVirtualTerminalProcessing() {}
#endif

void printBanner() {
    std::cout << UI::CYAN << UI::BOLD << R"(
  ██████╗ █████╗ ███████╗████████╗ █████╗ ██╗    ██╗ █████╗ ██╗   ██╗
 ██╔════╝██╔══██╗██╔════╝╚══██╔══╝██╔══██╗██║    ██║██╔══██╗╚██╗ ██╔╝
 ██║     ███████║███████╗   ██║   ███████║██║ █╗ ██║███████║ ╚████╔╝ 
 ██║     ██╔══██║╚════██║   ██║   ██╔══██║██║███╗██║██╔══██║  ╚██╔╝  
 ╚██████╗██║  ██║███████║   ██║   ██║  ██║╚███╔███╔╝██║  ██║   ██║   
  ╚═════╝╚═╝  ╚═╝╚══════╝   ╚═╝   ╚═╝  ╚═╝ ╚══╝╚══╝ ╚═╝  ╚═╝   ╚═╝   
             ██████╗ █████╗ ██████╗ ██╗████████╗ █████╗ ██╗          
            ██╔════╝██╔══██╗██╔══██╗██║╚══██╔══╝██╔══██╗██║          
            ██║     ███████║██████╔╝██║   ██║   ███████║██║          
            ██║     ██╔══██║██╔═══╝ ██║   ██║   ██╔══██║██║          
            ╚██████╗██║  ██║██║     ██║   ██║   ██║  ██║███████╗     
             ╚═════╝╚═╝  ╚═╝╚═╝     ╚═╝   ╚═╝   ╚═╝  ╚═╝╚══════╝     
                   🏝️  ISLAND OF RUIN: SURVIVAL MONOPOLY 🏝️
)" << UI::RESET << "\n";

    std::cout << UI::YELLOW << UI::BOLD
              << "   Rigid 4-Player Engine: Red -> Blue -> Green -> Gold\n"
              << "   Physical Currency Wallet: 500, 100, 50, 20, 10, 5, 1 Notes with Automatic Bank Change\n"
              << "   Interactive 3D Web Canvas: Open index.html to view live synchronized 3D board\n"
              << UI::RESET << "\n";
}

int main() {
    enableVirtualTerminalProcessing();
    printBanner();

    bool keepPlaying = true;
    while (keepPlaying) {
        std::cout << "\n" << UI::BOLD << "Configure the 4 Survivors:" << UI::RESET << "\n"
                  << " [1] Standard: You (Player 1 Red) vs 3 Heuristic AI Bots (Blue, Green, Gold)\n"
                  << " [2] Spectator / Simulation: 4 AI Bots\n"
                  << " [3] Custom 4-Player Roster (Enter names & Human/Bot for each)\n"
                  << "Select mode (1-3): ";

        int modeChoice = 1;
        std::cin >> modeChoice;

        std::vector<std::pair<std::string, bool>> playerConfigs;

        if (modeChoice == 2) {
            playerConfigs = {
                {"Bot_Red", true},
                {"Bot_Blue", true},
                {"Bot_Green", true},
                {"Bot_Gold", true}
            };
        } else if (modeChoice == 3) {
            const std::vector<std::string> colors = {"Red", "Blue", "Green", "Gold"};
            for (int i = 0; i < 4; ++i) {
                std::string name;
                std::cout << "\nEnter Name for Slot #" << (i + 1) << " [" << colors[i] << "] (Default: Survivor_" << colors[i] << "): ";
                std::cin.ignore();
                std::getline(std::cin, name);
                if (name.empty()) name = "Survivor_" + colors[i];

                char botChar;
                std::cout << "Is " << name << " controlled by AI Bot? (y/n): ";
                std::cin >> botChar;
                playerConfigs.push_back({name, (botChar == 'y' || botChar == 'Y')});
            }
        } else {
            // Default Mode 1: 1 Human + 3 Bots
            std::cout << "\nEnter your name [Default: Commander]: ";
            std::string humanName;
            std::cin.ignore();
            std::getline(std::cin, humanName);
            if (humanName.empty()) humanName = "Commander";

            playerConfigs = {
                {humanName, false},
                {"Navigator_Blue", true},
                {"Engineer_Green", true},
                {"Scavenger_Gold", true}
            };
        }

        std::cout << "\n" << UI::GREEN << "Booting strict 4-Player Island Engine & Exporting State..." << UI::RESET << "\n";
        GameEngine engine;
        engine.initializeStrict4PlayerGame(playerConfigs);
        engine.runGameLoop();

        std::cout << "\nGame session concluded. Would you like to run another simulation? (y/n): ";
        char playAgain;
        std::cin >> playAgain;
        keepPlaying = (playAgain == 'y' || playAgain == 'Y');
    }

    std::cout << UI::CYAN << "\nThank you for playing Castaway Capital: Island of Ruin!\n" << UI::RESET;
    return 0;
}
