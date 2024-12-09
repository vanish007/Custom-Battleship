#include <string>
#include <iostream>
#include <cstdint>

int main() {
    // Welcome to the game
    while(true) {
        std::string cmd;
        std::getline(std::cin, cmd);

        if(cmd == "exit") {
            std::cout << "ok" << std::endl;
            break;
        } else if(cmd == "ping") {
            std::cout << "pong" << std::endl;
        } else {
            std::cout << "'" << cmd << "' is an unknown command!" 
                      << " Please enter correct command." << std::endl;
        }
    }

    return EXIT_SUCCESS;
}
