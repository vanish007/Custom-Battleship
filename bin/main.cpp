#include "main.hpp"

#define _DEV_

CGameStats game_info;

std::vector<std::string> SplitString(const std::string& str) {
    std::stringstream ss(str);

    return {std::istream_iterator<std::string>(ss),
            std::istream_iterator<std::string>()};
}

int main() {
    #ifdef _DEV_
    std::cout << "program has started!" << std::endl;
    #endif


    sf::Music music;
    std::vector<std::string> playlist = {
        "resources/heartbeat.wav",
        "resources/stressful_sound_1.wav",
        "resources/faded.wav",
        "resources/hello.wav",
        "resources/blank.wav",
        "resources/alone.wav",
        "resources/levels.wav"
    };
    

    int current_track = 0;
    music.openFromFile("resources/starting_melody.wav");
    music.setLoop(true);
    double previous_volume = 30.0f;
    music.setVolume(previous_volume);
    music.play();


    CMyField my_field;
    CEnemyField enemy_field;
    CStrategy strategy;
    bool is_master = false;
    bool print_field = false;


    while(true) {
        if (music.getStatus() == sf::SoundSource::Stopped) {
            current_track = (current_track + 1) % playlist.size();
            music.openFromFile(playlist[current_track]);
            music.setVolume(previous_volume);
            music.play();
        }


        std::string str;
        std::getline(std::cin, str);
        std::vector<std::string> cmd_list = SplitString(str);


        if (cmd_list.empty()) {
            std::cout << "please enter a command!" << std::endl;
            continue;
        } 
        std::string cmd = cmd_list[0];


        // basic commands
        if (cmd == "ping") {                     // ping
            std::cout << "pong" << std::endl;
            continue;
        }
        if (cmd == "exit") {                     // exit
            std::cout << "ok" << std::endl;
            
            if (game_info.IsGameInProcess()) {
                return EXIT_FAILURE;
            } else {
                return EXIT_SUCCESS;
            }
            break;
        } 
        if (cmd == "create") {                   // create
            if (cmd_list[1] == "master") {
                is_master = true;
                std::cout << "ok" << std::endl;
            } else if (cmd_list[1] == "slave") {
                std::cout << "ok" << std::endl;
            }
            continue;
        }
        if (cmd == "start") {                    // start
            music.stop();
            music.setLoop(false);
            music.openFromFile(playlist[current_track]);
            music.play();

            if (!is_master) {
                my_field.CreateDefaultGame();
            } else {
                my_field.CreateMasterGame();
                enemy_field.CopyValues(my_field);
            }
            enemy_field.CreateDefaultGame();

            if (print_field) {
                std::cout << "My field:" << std::endl;
                my_field.PrintField();
                std::cout << "Enemy field:" << std::endl;
                enemy_field.PrintField();
            }

            std::cout << "ok" << std::endl;

            continue;
        }
        if (cmd == "stop") {                     // stop
            std::cout << "ok" << std::endl;
            continue;
        }
        if (cmd == "set") {                      // set
            if (cmd_list[1] == "strategy") {
                if (cmd_list[2] == "ordered") {
                    strategy.SetOrderedStrategy();
                    std::cout << "ok" << std::endl;
                } else if (cmd_list[2] == "custom") {
                    strategy.SetCustomStrategy();
                    std::cout << "ok" << std::endl;
                }
            } else if(cmd_list[1] == "result") {
                strategy.ShotResult(cmd_list[2], enemy_field);

                if (print_field) {
                    std::cout << "My field:" << std::endl;
                    my_field.PrintField();
                    std::cout << "Enemy field:" << std::endl;
                    enemy_field.PrintField();
                }

                if (enemy_field.GetTotalShipCnt() == 0) {
                    if (print_field) {
                        std::cout << "Game won!" << std::endl;
                    }
                    game_info.EndGame();
                    game_info.SetGameWon();
                }
            } else if(cmd_list[1] == "width") {
                my_field.SetWidth(std::stoull(cmd_list[2]));
                enemy_field.SetWidth(std::stoull(cmd_list[2]));
                std::cout << "ok" << std::endl;
            } else if(cmd_list[1] == "height") {
                my_field.SetHeight(std::stoull(cmd_list[2]));
                enemy_field.SetHeight(std::stoull(cmd_list[2]));
                std::cout << "ok" << std::endl;
             } else if (cmd_list[1] == "count") {
                my_field.SetShipAmount(std::stoi(cmd_list[2]), 
                                                 std::stoull(cmd_list[3]));
                enemy_field.SetShipAmount(std::stoi(cmd_list[2]), 
                                                    std::stoull(cmd_list[3]));
                std::cout << "ok" << std::endl;
            }
            continue;
        }
        if (cmd == "get") {                      // get
            if (cmd_list[1] == "width") {
                std::cout << my_field.GetWidth() << std::endl;
            } else if(cmd_list[1] == "height") {
                std::cout << my_field.GetHeight() << std::endl;
            } else if (cmd_list[1] == "count") {
                std::cout << my_field.GetShipAmount(std::stoi(cmd_list[2])) 
                          << std::endl;
            }
            continue;
        }         
        if (cmd == "shot") {                     // shot
            if (cmd_list.size() == 1) {
                std::pair<int64_t, int64_t> coords = strategy.GetNextShot(enemy_field);
                std::cout << coords.first << ' ' << coords.second << std::endl;
            } else {
                sf::Music sound;
                sound.setVolume(50.f);

                std::string result = my_field.Hit(std::stoull(cmd_list[1]), 
                                                std::stoull(cmd_list[2]));
                if (result == "kill") {
                    sound.openFromFile("resources/kill.wav");
                } else if (result == "miss") {
                    sound.openFromFile("resources/miss.wav");
                }  else if (result == "hit") {
                    sound.openFromFile("resources/hit.wav");
                }            
                sound.play();

                if (print_field) {
                    std::cout << "My field:" << std::endl;
                    my_field.PrintField();
                    std::cout << "Enemy field:" << std::endl;
                    enemy_field.PrintField();
                }

                std::cout << result << std::endl;

                if (my_field.GetTotalShipCnt() == 0) {
                    if (print_field) {
                        std::cout << "game over!" << std::endl;
                    }
                    game_info.EndGame();
                    game_info.SetGameLost();
                }


                while (sound.getStatus() == sf::SoundSource::Playing) {
                    sf::sleep(sf::milliseconds(50));
                }
            } 
            continue;
        }            
        if (cmd == "finished") {                 // finished
            if (game_info.IsGameInProcess()) {
                std::cout << "no" << std::endl;
            } else {
                std::cout << "yes" << std::endl;
            }
            continue;
        }        
        if (cmd == "win") {                      // win
            if (game_info.IsGameInProcess() || game_info.IsGameLost()) {
                std::cout << "no" << std::endl;
            } else {
                std::cout << game_info.IsGameLost() << std::endl;
                std::cout << "yes" << std::endl;
            }
            continue;
        }       
        if (cmd == "lose") {                     // lose
            if (game_info.IsGameInProcess() || game_info.IsGameWon()) {
                std::cout << "no" << std::endl;
            } else {
                std::cout << game_info.IsGameWon() << std::endl;
                std::cout << "yes" << std::endl;
            }
            continue;
        }        
        if (cmd == "dump") {                     // dump
            std::string file_name = cmd_list[1];

            if (!std::fstream(file_name)) {
                std::cout << "failed" << std::endl;
            } else {
                std::ofstream fout(file_name);
                my_field.Dump(fout);
                std::cout << "ok" << std::endl;
            }
            continue;
        } 
        if (cmd == "load") {                     // load
            std::string file_name = cmd_list[1];

            std::ifstream fin(file_name);
            if (!fin.is_open()) {
                std::cout << "failed" << std::endl;
            } else {
                my_field.Load(fin);
                is_master = true;
                std::cout << "ok"  << std::endl;
            }
            continue;
        }

        // custom commands
        if (cmd == "sound") {                    // sound
            if (cmd_list.size() == 1) {
                std::cout << "sound control commands:" << std::endl;
                std::cout << "mute   - turn off the sound" << std::endl;
                std::cout << "unmute - restore the previous sound volume" 
                        << std::endl;
                std::cout << "pause  - pause the sound playback" << std::endl;
                std::cout << "resume - resume sound playback" << std::endl;
                std::cout << "down   - decrease volume" << std::endl;
                std::cout << "up     - increase volume" << std::endl;
                std::cout << "next   - change melody" << std::endl;
            } else {
                if (cmd_list[1] == "mute") {
                    previous_volume = music.getVolume();
                    music.setVolume(0);
                    std::cout << "sound muted" << std::endl;
                } else if (cmd_list[1] == "unmute") {
                    music.setVolume(previous_volume);
                    std::cout << "sound unmuted" << std::endl;
                } else if (cmd_list[1] == "pause") {
                    music.pause();
                    std::cout << "sound paused" << std::endl;
                } else if (cmd_list[1] == "resume") {
                    music.play();
                    std::cout << "sound resumed" << std::endl;
                } else if (cmd_list[1] == "down") {
                    float current_volume = music.getVolume();
                    current_volume = std::max(0.0f, current_volume - 5);
                    music.setVolume(current_volume);
                    std::cout << "sound volume decreased to " << current_volume 
                            << "%" << std::endl;
                } else if (cmd_list[1] == "up") {
                    float current_volume = music.getVolume();
                    current_volume = std::min(100.0f, current_volume + 5);
                    music.setVolume(current_volume);
                    std::cout << "sound volume increased to " << current_volume 
                            << "%" << std::endl;
                } else if (cmd_list[1] == "next") {
                    music.stop();
                    current_track = (current_track + 1) % playlist.size();
                    music.openFromFile(playlist[current_track]);
                    music.play(); 
                    std::cout << "melody changed" << std::endl;
                } else {
                    std::cout << "unknown sound command: " << cmd_list[1] 
                            << std::endl;
                }
            }
            continue;
        } 
        if (cmd == "show") {
            if (cmd_list.size() == 2) {
                if (cmd_list[1] == "field") {
                    std::cout << "My field:" << std::endl;
                    my_field.PrintField();
                    std::cout << "Enemy field:" << std::endl;
                    enemy_field.PrintField();
                }
            } else {
                if (cmd_list[2] == "true") {
                    print_field = true;
                } else if (cmd_list[2] == "false") {
                    print_field = false;
                }
            }
            continue;
        }
        if (cmd == "interface") {
            
        }
        // unknown commands
        std::cerr << "'";
        for (std::string& s : cmd_list) {
            std::cerr << s << ' ';
        }
        std::cerr << "\b' is an unknown command!" 
                    << " Please enter a correct command." << std::endl;
    }

    return EXIT_SUCCESS;
}