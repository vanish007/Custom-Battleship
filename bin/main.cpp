#include "main.hpp"


std::vector<std::string> SplitString(const std::string& str) {
    std::stringstream ss(str);

    return {std::istream_iterator<std::string>(ss),
            std::istream_iterator<std::string>()};
}

bool isButtonPressed(sf::RectangleShape &button, sf::Vector2i mousePosition) {
    return button.getGlobalBounds().contains(sf::Vector2f(mousePosition));
}

bool isButtonPressed(sf::Sprite &button, sf::Vector2i mousePosition) {
    return button.getGlobalBounds().contains(sf::Vector2f(mousePosition));
}

struct Counter {
    sf::Sprite minusButton;
    sf::Sprite plusButton;
    sf::Text labelText;
    int value = 0;
};

class CComand {
public:
    CComand(sf::Music& music_,
            CGameStats& game_info_,
            CMyField& my_field_,
            CEnemyField& enemy_field_,
            CStrategy& strategy_,
            std::vector<std::string>& playlist_,
            short& current_track_,
            double& previous_volume_,
            bool& is_master_,
            bool& print_field_,
            bool& show_menu_,
            bool& is_sound_on_,
            bool& is_sound_play_) :
        music(music_),
        game_info(game_info_),
        my_field(my_field_),
        enemy_field(enemy_field_),
        strategy(strategy_),
        playlist(playlist_),
        current_track(current_track_),
        previous_volume(previous_volume_),
        is_master(is_master_),
        print_field(print_field_),
        show_menu(show_menu_),
        is_sound_on(is_sound_on_),
        is_sound_play(is_sound_play_)
    {}

    void SetCmdList(std::vector<std::string> cmd_list) {
        this->cmd_list = cmd_list;
    }

    void Set() {
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
    }

    void Start() {
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
    }

    void Load() {
        std::string file_name = cmd_list[1];

        std::ifstream fin(file_name);
        if (!fin.is_open()) {
            std::cout << "failed" << std::endl;
        } else {
            my_field.Load(fin);
            is_master = true;
            std::cout << "ok"  << std::endl;
        }
    }

    void Sound() {
        if (cmd_list.size() == 1) {
            std::cout << "sound control commands:" << std::endl;
            std::cout << "mute      - turn off the sound" << std::endl;
            std::cout << "unmute    - restore the previous sound volume" 
                    << std::endl;
            std::cout << "pause     - pause the sound playback" << std::endl;
            std::cout << "resume    - resume sound playback" << std::endl;
            std::cout << "down      - decrease volume" << std::endl;
            std::cout << "up        - increase volume" << std::endl;
            std::cout << "next      - change melody" << std::endl;
            std::cout << "previous  - return melody" << std::endl;
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
            } else if (cmd_list[1] == "previous") {
                music.stop();
                if (current_track == 0) {
                    current_track = playlist.size();
                }
                current_track = (current_track - 1) % playlist.size();
                music.openFromFile(playlist[current_track]);
                music.play(); 
                std::cout << "melody changed" << std::endl;
            } else {
                std::cout << "unknown sound command: " << cmd_list[1] 
                        << std::endl;
            }
        }
    }
private:
    std::vector<std::string> cmd_list;

    sf::Music& music;
    CGameStats& game_info;
    CMyField& my_field;
    CEnemyField& enemy_field;
    CStrategy& strategy;
    std::vector<std::string>& playlist;
    short& current_track;
    double& previous_volume;
    bool& is_master;
    bool& print_field;
    bool& show_menu;
    bool& is_sound_on;
    bool& is_sound_play;
};

int main() {
    CGameStats game_info;
    CMyField my_field;
    CEnemyField enemy_field;
    CStrategy strategy;
    bool is_master = false;
    bool print_field = false;

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
    

    short current_track = 0;
    music.openFromFile("resources/starting_melody.wav");
    music.setLoop(true);
    double previous_volume = 30.0f;
    music.setVolume(previous_volume);
    music.play();

    sf::RenderWindow window(sf::VideoMode(1200, 750), "labwork5 by Ivan Erofeev");
    window.setVisible(false);

    sf::Font font;
    font.loadFromFile("resources/NK123.ttf");

    sf::Texture backgroundTexture;
    sf::Texture backgroundTexture2;
    backgroundTexture.loadFromFile("resources/background.jpg");
    backgroundTexture2.loadFromFile("resources/background3.png");
    sf::Sprite background(backgroundTexture);

    sf::Text menuTitle("Main Menu", font, 50);
    menuTitle.setPosition(275, 100);

    sf::Texture start_button_texture;
    start_button_texture.loadFromFile("resources/rectangle_tile.png");
    sf::Texture sound_on;
    sound_on.loadFromFile("resources/sound_on.png");
    sf::Texture sound_off;
    sound_off.loadFromFile("resources/sound_off.png");
    sf::Texture sound_pause;
    sound_pause.loadFromFile("resources/pause.png");
    sf::Texture sound_play;
    sound_play.loadFromFile("resources/play.png");
    sf::Texture cross;
    cross.loadFromFile("resources/cross.png");
    sf::Texture exit_ok;
    exit_ok.loadFromFile("resources/exit_ok.png");
    sf::Texture minus;
    minus.loadFromFile("resources/minus.png");
    sf::Texture plus;
    plus.loadFromFile("resources/plus.png");
    sf::Texture ok;
    ok.loadFromFile("resources/ok.png");
    sf::Texture create_tile;
    create_tile.loadFromFile("resources/backgorund_create_tile.png");
    sf::Texture previous;
    previous.loadFromFile("resources/previous.png");
    sf::Texture next;
    next.loadFromFile("resources/next.png");
    sf::Texture pirate1;
    pirate1.loadFromFile("resources/pirate1.png");
    sf::Texture pirate2;
    pirate2.loadFromFile("resources/pirate2.png");

    std::vector<std::string> ans = {"miss", "hit", "kill"};
    int ans_int = 0;
    sf::Text pirate1_text;
    pirate1_text.setFont(font);
    pirate1_text.setString(ans[ans_int]);
    pirate1_text.setCharacterSize(20);
    pirate1_text.setPosition(150, 500);

    sf::Text pirate2_text;
    pirate2_text.setFont(font);
    pirate2_text.setString(ans[ans_int]);
    pirate2_text.setCharacterSize(20);
    pirate2_text.setPosition(500, 500);

    sf::Sprite startButton(start_button_texture);
    startButton.setPosition(475, 220);
    sf::Text startText("Start", font, 25);
    startText.setPosition(550, 230);

    sf::Sprite createButton(start_button_texture);
    createButton.setPosition(475, 350);
    sf::Text createText("Create field", font, 25);
    createText.setPosition(520, 360);

    sf::Sprite volumeUp(plus);
    volumeUp.setPosition(1100, 5);
    sf::Sprite Next(next);
    Next.setPosition(1100, 65);

    sf::Sprite soundOn(sound_on);
    soundOn.setPosition(1030, 5);
    sf::Sprite soundPlay(sound_pause);
    soundPlay.setPosition(1030, 65);
    

    sf::Sprite volumeDown(minus);
    volumeDown.setPosition(960, 5);
    sf::Sprite Previous(previous);
    Previous.setPosition(960, 65);

    sf::Sprite exitGame(cross);
    exitGame.setPosition(20, 5);
    
    sf::Sprite createConfirm(exit_ok);
    createConfirm.setPosition(700, 530);

    sf::Sprite createBackground(create_tile);
    createBackground.setPosition(300, 75);


    sf::Sprite Pirate1(pirate1);
    Pirate1.setPosition(10, 300);

    sf::Sprite Pirate2(pirate2);
    Pirate2.setPosition(700, 250);


    bool show_menu = false;
    bool is_sound_on = true;
    bool is_sound_play = true;
    bool isInCreateMenu = false;
    bool isInStartMenu = false;

    int width_value = 1;
    int height_value = 1;
    int pirate1x = -1;
    int pirate2x = -1;

    std::vector<Counter> counters(4);

    int x_create = 20;
    int y_create = 20;

    for (size_t i = 0; i < counters.size(); ++i) {
        counters[i].minusButton.setTexture(minus);
        counters[i].minusButton.setPosition(x_create, y_create + i * 100);

        counters[i].plusButton.setTexture(plus);
        counters[i].plusButton.setPosition(x_create + 100, y_create + i * 100);

        counters[i].labelText.setFont(font);
        counters[i].labelText.setString("Counter " + std::to_string(i + 1) + ": 0");
        counters[i].labelText.setCharacterSize(20);
        counters[i].labelText.setPosition(x_create + 150, y_create + i * 100);
    }

    Counter widthCounter = {sf::Sprite(minus), sf::Sprite(plus), sf::Text(), width_value};
    widthCounter.minusButton.setPosition(x_create, y_create + 400);
    widthCounter.plusButton.setPosition(x_create + 100, y_create + 400);
    widthCounter.labelText.setFont(font);
    widthCounter.labelText.setString("Field width: " + std::to_string(width_value));
    widthCounter.labelText.setCharacterSize(20);
    widthCounter.labelText.setPosition(x_create + 150, y_create + 400);

    Counter heightCounter = {sf::Sprite(minus), sf::Sprite(plus), sf::Text(), height_value};
    heightCounter.minusButton.setPosition(x_create, y_create + 500);
    heightCounter.plusButton.setPosition(x_create + 100, y_create + 500);
    heightCounter.labelText.setFont(font);
    heightCounter.labelText.setString("Field height: " + std::to_string(height_value));
    heightCounter.labelText.setCharacterSize(20);
    heightCounter.labelText.setPosition(x_create + 150, y_create + 500);


    CComand command_processor(music,
                              game_info,
                              my_field,
                              enemy_field,
                              strategy,
                              playlist,
                              current_track,
                              previous_volume,
                              is_master,
                              print_field,
                              show_menu,
                              is_sound_on,
                              is_sound_play);
    
    std::cout << "program has started!" << std::endl;
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

        command_processor.SetCmdList(cmd_list);

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
            command_processor.Start();
            continue;
        }
        if (cmd == "stop") {                     // stop
            std::cout << "ok" << std::endl;
            continue;
        }
        if (cmd == "set") {                      // set
            command_processor.Set();
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
            command_processor.Load();
            continue;
        }


        // custom commands
        if (cmd == "sound") {                    // sound
            command_processor.Sound();
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
            if (cmd_list[1] == "on") {
                command_processor.SetCmdList({"set", "count", "1", std::to_string(counters[0].value)});
                command_processor.Set();
                command_processor.SetCmdList({"set", "count", "2", std::to_string(counters[1].value)});
                command_processor.Set();
                command_processor.SetCmdList({"set", "count", "3", std::to_string(counters[2].value)});
                command_processor.Set();
                command_processor.SetCmdList({"set", "count", "4", std::to_string(counters[3].value)});
                command_processor.Set();
                command_processor.SetCmdList({"set", "width", std::to_string(width_value)});
                command_processor.Set();
                command_processor.SetCmdList({"set", "height", std::to_string(height_value)});
                command_processor.Set();
                command_processor.SetCmdList({"create", "slave"});
                command_processor.Set();
                show_menu = true;
                window.setVisible(true);
            } else if (cmd_list[1] == "off") {
                show_menu = false;
                window.setVisible(false);
            }

            int count = 1;

            while (show_menu && window.isOpen()) {
                sf::Event event;

                bool tempisInCreateMenu = isInCreateMenu;
                bool tempisInStartMenu = isInStartMenu;
                while (window.pollEvent(event)) {
                    if (event.type == sf::Event::Closed) {
                        window.close();
                    }

                    if (event.type == sf::Event::MouseButtonPressed) {
                        auto mousePos = sf::Mouse::getPosition(window);

                        if (!tempisInCreateMenu && !tempisInStartMenu) {
                            if (isButtonPressed(startButton, mousePos)) {
                                command_processor.Start();
                                background.setTexture(backgroundTexture2);
                                isInStartMenu = true;
                            } else if (isButtonPressed(createButton, mousePos)) {
                                isInCreateMenu = true;
                            } else if (isButtonPressed(exitGame, mousePos)) {
                                show_menu = false;
                                std::cout << "interface off\n";
                                window.setVisible(false);
                            }
                        } 

                        if (isButtonPressed(soundOn, mousePos)) {
                            if (is_sound_on) {
                                command_processor.SetCmdList({"sound", "mute"});
                                command_processor.Sound();
                                is_sound_on = false;
                                soundOn.setTexture(sound_off);
                            } else {
                                command_processor.SetCmdList({"sound", "unmute"});
                                command_processor.Sound();
                                is_sound_on = true;
                                soundOn.setTexture(sound_on);
                            }
                        } else if (isButtonPressed(volumeUp, mousePos) && is_sound_on) {
                            command_processor.SetCmdList({"sound", "up"});
                            command_processor.Sound();
                        } else if (isButtonPressed(volumeDown, mousePos) && is_sound_on) {
                            command_processor.SetCmdList({"sound", "down"});
                            command_processor.Sound();
                        } else if (isButtonPressed(Next, mousePos) && is_sound_on) {
                            command_processor.SetCmdList({"sound", "next"});
                            command_processor.Sound();
                        } else if (isButtonPressed(Previous, mousePos) && is_sound_on) {
                            command_processor.SetCmdList({"sound", "previous"});
                            command_processor.Sound();
                        } else if (isButtonPressed(soundPlay, mousePos) && is_sound_on) {
                            if (is_sound_play) {
                                command_processor.SetCmdList({"sound", "pause"});
                                command_processor.Sound();
                                is_sound_play = false;
                                soundPlay.setTexture(sound_play);
                            } else {
                                command_processor.SetCmdList({"sound", "resume"});
                                command_processor.Sound();
                                is_sound_play = true;
                                soundPlay.setTexture(sound_pause);
                            }
                        }

                        if (tempisInCreateMenu) {
                            if (isButtonPressed(createConfirm, mousePos)) {
                                command_processor.SetCmdList({"set", "count", "1", std::to_string(counters[0].value)});
                                command_processor.Set();
                                command_processor.SetCmdList({"set", "count", "2", std::to_string(counters[1].value)});
                                command_processor.Set();
                                command_processor.SetCmdList({"set", "count", "3", std::to_string(counters[2].value)});
                                command_processor.Set();
                                command_processor.SetCmdList({"set", "count", "4", std::to_string(counters[3].value)});
                                command_processor.Set();
                                command_processor.SetCmdList({"set", "width", std::to_string(width_value)});
                                command_processor.Set();
                                command_processor.SetCmdList({"set", "height", std::to_string(height_value)});
                                command_processor.Set();
                                command_processor.SetCmdList({"create", "slave"});
                                command_processor.Set();
                                isInCreateMenu = false;
                            } 
                            

                            for (auto& counter : counters) {
                                if (isButtonPressed(counter.plusButton, mousePos)) {
                                    counter.value++;
                                    counter.labelText.setString("Counter " + std::to_string(&counter - &counters[0] + 1) + ": " + std::to_string(counter.value));
                                } else if (isButtonPressed(counter.minusButton, mousePos)) {
                                    counter.value = std::max(0, counter.value - 1);
                                    counter.labelText.setString("Counter " + std::to_string(&counter - &counters[0] + 1) + ": " + std::to_string(counter.value));
                                }
                            }

                            if (isButtonPressed(widthCounter.plusButton, mousePos)) {
                                width_value += 1;
                                widthCounter.labelText.setString("Field width: " + std::to_string(width_value));
                            } else if (isButtonPressed(widthCounter.minusButton, mousePos)) {
                                width_value = std::max(1, width_value - 1);
                                widthCounter.labelText.setString("Field width: " + std::to_string(width_value));
                            }

                            if (isButtonPressed(heightCounter.plusButton, mousePos)) {
                                height_value += 1;
                                heightCounter.labelText.setString("Field height: " + std::to_string(height_value));
                            } else if (isButtonPressed(heightCounter.minusButton, mousePos)) {
                                height_value = std::max(1, height_value - 1);
                                heightCounter.labelText.setString("Field height: " + std::to_string(height_value));
                            }
                        }

                        if (tempisInStartMenu) {
                            if (isButtonPressed(Pirate1, mousePos)) {
                                ans_int = rand() % 3;
                                pirate1_text.setString(ans[ans_int]);
                                pirate1x = 0;
                            } else if (isButtonPressed(heightCounter.minusButton, mousePos)) {
                                ans_int = rand() % 3;
                                pirate2_text.setString(ans[ans_int]);
                                pirate2x = 0;
                            }
                        }
                    }
                }

                window.clear();
                window.draw(background);

                window.draw(soundOn);
                if (is_sound_on) {
                    window.draw(volumeUp);
                    window.draw(volumeDown);
                    window.draw(soundPlay);
                    window.draw(Next);
                    window.draw(Previous);
                }

                if (!isInCreateMenu && !isInStartMenu) {
                    window.draw(startButton);
                    window.draw(startText);
                    window.draw(createButton);
                    window.draw(createText);
                    window.draw(exitGame);
                } else if (isInCreateMenu) {
                    float baseX = 430;
                    float baseY = 160;

                    for (size_t i = 0; i < counters.size(); ++i) {
                        counters[i].labelText.setPosition(baseX + 33, baseY + i * 60);
                        counters[i].plusButton.setPosition(baseX + 150, baseY + i * 60);
                        counters[i].minusButton.setPosition(baseX - 55, baseY + i * 60);
                    }

                    widthCounter.labelText.setPosition(baseX + 26, baseY + 300);
                    widthCounter.plusButton.setPosition(baseX + 150, baseY + 300);
                    widthCounter.minusButton.setPosition(baseX - 55, baseY + 300);

                    heightCounter.labelText.setPosition(baseX + 26, baseY + 360);
                    heightCounter.plusButton.setPosition(baseX + 150, baseY + 360);
                    heightCounter.minusButton.setPosition(baseX - 55, baseY + 360);

                    window.draw(createBackground);

                    for (const auto& counter : counters) {
                        window.draw(counter.minusButton);
                        window.draw(counter.plusButton);
                        window.draw(counter.labelText);
                    }
                    window.draw(widthCounter.minusButton);
                    window.draw(widthCounter.plusButton);
                    window.draw(widthCounter.labelText);

                    window.draw(heightCounter.minusButton);
                    window.draw(heightCounter.plusButton);
                    window.draw(heightCounter.labelText);

                    window.draw(createConfirm);
                } else if (isInStartMenu) {
                    window.draw(Pirate1);
                    window.draw(Pirate2);
                    if (pirate1x != -1) {
                        window.draw(pirate1_text);
                    }
                    if (pirate2x != -1) {
                        window.draw(pirate2_text);
                    }
                    pirate1x = -1;
                    pirate2x = -1;
                }

                window.display();
            }
            continue;
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