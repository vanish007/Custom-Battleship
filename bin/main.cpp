#include <string>
#include <iostream>
#include <sstream>
#include <iterator>
#include <vector>
#include <deque>
#include <cstdint>
#include <fstream>

#define _DEV_

#include <SFML/Audio.hpp>

class CGameStats{
public:
    bool IsGameInProcess() const {
        return is_game_in_process_;
    }

    void StartGame() {
        is_game_in_process_ = true;
    }

    void EndGame() {
        is_game_in_process_ = false;
    }

    bool IsGameWon() const {
        if (is_game_in_process_ == true) {
            return false;
        }
        return is_game_won_;
    }

    void SetGameWon(bool value = true) {
        is_game_lost_ = false;
        is_game_won_ = true;
    }
    
    bool IsGameLost() const {
        if (is_game_in_process_ == true) {
            return false;
        }
        return is_game_lost_;
    }

    void SetGameLost(bool value = true) {
        is_game_won_ = false;
        is_game_lost_ = true;
    }

private:
    bool is_game_in_process_ = true;
    bool is_game_won_ = false;
    bool is_game_lost_ = false;
};

CGameStats game_stats;

class CField {
public:
    void Dump(std::ofstream& out) {
        out << stats_[0] << ' ' << stats_[1] << std::endl;
        for (size_t i = 2; i < stats_.size(); i += 4) {
            out << stats_[i] << ' ' << stats_[i+1] << ' ' 
                << stats_[i+2] << ' ' << stats_[i+3] << std::endl;
        }
    }

    void Load(std::ifstream& in) {
        std::string cmd;
        std::vector<std::string> vec;
        while (in >> cmd) {
            vec.push_back(cmd);
        }
        AddShips_(vec);
    }

    int64_t GetWidth() {
        return coord_x_;
    }

    int64_t GetHeight() {
        return coord_y_;
    }

    void SetWidth(int64_t x) {
        coord_x_ = x;
        stats_.push_back(std::to_string(x));
    }

    void SetHeight(int64_t y) {
        coord_y_ = y;
        stats_.push_back(std::to_string(y));
    }

    void CreateEmptyField(int64_t x, int64_t y) {
        coord_x_ = x;
        coord_y_ = y;
        field_ = new std::pair<bool, bool>* [y]{};
        for (size_t i = 0; i < y; ++i) {
            field_[i] = new std::pair<bool, bool>[x]{};
        }
    }

    bool IsAlreadyHit(int64_t x, int64_t y) {
        return field_[y][x].first;
    }

    void MarkAsHit(int64_t x, int64_t y) {
        field_[y][x].first = true;
    }

    bool IsShip(int64_t x, int64_t y) {
        return field_[y][x].second;
    }

    void MarkAsShip(int64_t x, int64_t y) {
        field_[y][x].second = true;
        field_[y][x].first = true;
    }

    void KillShip() {
        total_ship_cnt_ = std::max(0LL, total_ship_cnt_-1);
    }

    std::string Hit(int64_t x, int64_t y) {
        field_[y][x].first = true;
        if (field_[y][x].second) {
            if (CheckForKill_(y, x)) {
                total_ship_cnt_ = std::max(0LL, total_ship_cnt_-1);
                if (total_ship_cnt_ == 0) {
                    game_stats.EndGame();
                    game_stats.SetGameLost();
                }
                return "kill";
            } else {
                return "hit";
            }
        } else {
            return "miss";
        }
    }

    void SetTotalShipCnt(int64_t value) {
        total_ship_cnt_ = value;
    }

    int64_t GetTotalShipCnt() {
        return total_ship_cnt_;
    }

    void SetOneShipCnt(int64_t value) {
        one_ship_cnt_ = value;
        total_ship_cnt_ += value;
    }

    int64_t GetOneShipCnt() {
        return one_ship_cnt_;
    }

    void SetTwoShipCnt(int64_t value) {
        two_ship_cnt_ = value;
        total_ship_cnt_ += value;
    }

    int64_t GetTwoShipCnt() {
        return two_ship_cnt_;
    }

    void SetThreeShipCnt(int64_t value) {
        three_ship_cnt_ = value;
        total_ship_cnt_ += value;
    }

    int64_t GetThreeShipCnt() {
        return three_ship_cnt_;
    }

    void SetFourShipCnt(int64_t value) {
        four_ship_cnt_ = value;
        total_ship_cnt_ += value;
    }

    int64_t GetFourShipCnt() {
        return four_ship_cnt_;
    }

    bool CanPlaceShip(int64_t x, int64_t y, int64_t length) {
        for (int64_t i = 0; i < length; ++i) {
            int64_t ny = y + i;
            int64_t nx = x;

            if (nx < 0 || ny < 0 || nx >= coord_x_ || ny >= coord_y_) {
                return false;
            }

            for (int64_t dx = -1; dx <= 1; ++dx) {
                for (int64_t dy = -1; dy <= 1; ++dy) {
                    int64_t checkX = nx + dx;
                    int64_t checkY = ny + dy;

                    if (checkX >= 0 && checkX < coord_x_ 
                        && checkY >= 0 && checkY < coord_y_ &&
                        field_[checkY][checkX].second) {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    void PlaceShip(int64_t x, int64_t y, int64_t length) {
        for (int64_t i = 0; i < length; ++i) {
            int64_t ny = y + i;
            int64_t nx = x;
            field_[ny][nx].second = true;
        }
    }

    void PrintField() {
        for (size_t i = 0; i < coord_y_; ++i) {
            for (size_t j = 0; j < coord_x_; ++j) {
                if (field_[i][j].second && field_[i][j].first) {
                    std::cout << "*" << ' ';
                } else if (field_[i][j].second) {
                    std::cout << "O" << ' ';
                } else if (field_[i][j].first) {
                    std::cout << 'X' << ' ';
                } else {
                    std::cout << '.' << ' ';
                }
            }
            std::cout << std::endl;
        }
    }

    std::vector<std::string> GetShips() {
        return stats_;
    }

    bool PlaceAlongWalls(int64_t ship_count, int64_t ship_size, bool& left_side) {
        int64_t offset = 0;

        for (int64_t i = 0; i < ship_count; ++i) {
            bool placed = false;

            while (offset < coord_x_ / 2) {
                for (int64_t attempt = 0; attempt < 2; ++attempt) { 
                    for (int64_t y = 0; y < coord_y_; ++y) {
                        int64_t x = left_side ? offset : coord_x_ - 1 - offset;

                        if (CanPlaceShip(x, y, ship_size)) {
                            PlaceShip(x, y, ship_size);
                            stats_.push_back(std::to_string(ship_size));
                            stats_.push_back("v");
                            stats_.push_back(std::to_string(x));
                            stats_.push_back(std::to_string(y));
                            placed = true;
                            break;
                        }
                    }

                    if (placed) {
                        break; 
                    }

                    left_side = !left_side;
                }

                if (placed) {
                    break;
                }

                offset += 2;
            }

            if (!placed) {
                return false;
            }
        }

        return true;
    }


    bool PlaceRandomly(int64_t ship_count) {
        std::srand(time(NULL));

        for (int64_t i = 0; i < ship_count; ++i) {
            bool placed = false;

            for (int attempts = 0; attempts < 10000; ++attempts) { 
                int64_t x = std::rand() % (coord_x_ - 2) + 1; 
                int64_t y = std::rand() % (coord_y_ - 2) + 1;

                if (CanPlaceShip(x, y, 1)) {
                    PlaceShip(x, y, 1);
                    placed = true;
                    break;
                }
            }

            if (!placed) {
                return false;
            }
        }
        return true;
    }

    bool PlaceShips() {
        int64_t required_cells = four_ship_cnt_ * 4 + three_ship_cnt_ * 3 +
                                two_ship_cnt_ * 2 + one_ship_cnt_;
        if (required_cells > coord_x_ * coord_y_) return false;

        bool left_side = true;
        if (!PlaceAlongWalls(four_ship_cnt_, 4, left_side)) {
            return false;
        }
        if (!PlaceAlongWalls(two_ship_cnt_, 2, left_side)) {
            return false;
        }   
        if (!PlaceAlongWalls(three_ship_cnt_, 3, left_side)) {
            return false;
        }
        if (!PlaceRandomly(one_ship_cnt_)) {
            return false;
        }

        return true;
    }

    void SetEnemy() {
        is_enemy = true;
    }

    bool IsEnemy() {
        return is_enemy;
    }

    ~CField() {
        for (size_t i = 0; i < coord_y_; ++i) {
            delete [] field_[i];
        }
        delete [] field_;
    }
private:
    int64_t coord_x_;
    int64_t coord_y_;
    std::pair<bool, bool>** field_;

    int64_t total_ship_cnt_ = 0;
    int64_t one_ship_cnt_ = 0;
    int64_t two_ship_cnt_ = 0;
    int64_t three_ship_cnt_ = 0;
    int64_t four_ship_cnt_ = 0;

    std::vector<std::string> stats_;

    bool is_enemy = false;

    void AddShips_(std::vector<std::string> ships) {
        coord_x_ = std::stoull(ships[0]);
        stats_.push_back(ships[0]);
        coord_y_ = std::stoull(ships[1]);
        stats_.push_back(ships[1]);
        CreateEmptyField(coord_x_, coord_y_);
        for (size_t i = 2; i < ships.size(); i += 4) {
            stats_.push_back(ships[i]);
            stats_.push_back(ships[i+1]);
            stats_.push_back(ships[i+2]);
            stats_.push_back(ships[i+3]);
            PlaceShip_(ships[i], ships[i+1], ships[i+2], ships[i+3]);
        }
    }

    void PlaceShip_(std::string s1, std::string s2, 
                    std::string s3, std::string s4) {
        short size = std::stoull(s1);
        total_ship_cnt_ += 1;
        if (size == 1) {
            ++one_ship_cnt_;
        } else if (size == 2) {
            ++two_ship_cnt_;
        } else if (size == 3) {
            ++three_ship_cnt_;
        } else if (size == 4) {
            ++four_ship_cnt_;
        }
        short x = std::stoull(s3);
        short y = std::stoull(s4);
        if (s2 == "v") {
            for (int i = y; i < y+size; ++i) {
                field_[i][x].second = true;
            }
        } else {
            for (int i = x; i < x+size; ++i) {
                field_[y][i].second = true;
            }
        }
    }

    bool  CheckForKill_(int64_t x, int64_t y) {
        for (int64_t i = x; i >= 0; --i) {
            if (field_[i][y].second) {
                if (!field_[i][y].first) {
                    return false;
                }
            } else {
                break;
            }
        }

        for (int64_t i = x; i < coord_y_; ++i) {
            if (field_[i][y].second) {
                if (!field_[i][y].first) {
                    return false;
                }
            } else {
                break;
            }
        }

        for (int64_t j = y; j >= 0; --j) {
            if (field_[x][j].second) {
                if (!field_[x][j].first) {
                    return false;
                }
            } else {
                break;
            }
        }

        for (int64_t j = y; j < coord_x_; ++j) {
            if (field_[x][j].second) {
                if (!field_[x][j].first) {
                    return false;
                }
            } else {
                break;
            }
        }

        return true;
    }

    friend void CreateMasterGame(CField& field);
    friend void CreateSlaveGame(CField& field);
};

class CStrategy {
public:
    void SetOrderedStrategy() {
        is_custom_ = false;
        next_x_shot_ = 0;
        next_y_shot_ = 0;
        diagonal_index_ = 0;
        target_cells_.clear();
        ship_is_vertical_ = false;
        ship_direction_ = 0;
    }

    void SetCustomStrategy() {
        is_custom_ = true;
        diagonal_index_ = 0;
        target_cells_.clear();
        ship_is_vertical_ = false;
        ship_direction_ = 0;
    }

    std::pair<int64_t, int64_t> GetNextShot(CField& field) {
        if (!target_cells_.empty()) {
            last_shot_ = target_cells_.front();
            target_cells_.pop_front();
            return last_shot_;
        }

        if (is_custom_) {
            last_shot_ = GetNextCustomShot(field);
        } else {
            last_shot_ = GetNextOrderedShot(field);
        }

        return last_shot_;
    }

    void ShotResult(const std::string& result, CField& field) {
        if (result == "miss") {
            field.MarkAsHit(last_shot_.first, last_shot_.second);
        } else if (result == "hit") {
            field.MarkAsShip(last_shot_.first, last_shot_.second);

            hits_.push_back(last_shot_);

            if (hits_.size() > 1) {
                DetermineShipOrientation();
                if (ship_is_vertical_) {
                    while (!target_cells_.empty() && target_cells_.front().first != last_shot_.first) {
                        target_cells_.pop_front();
                    }
                     while (!target_cells_.empty() && target_cells_.back().first != last_shot_.first) {
                        target_cells_.pop_back();
                    }
                    AddNeighborCellsToTarget(last_shot_.first, last_shot_.second + 1, field);
                    AddNeighborCellsToTarget(last_shot_.first, last_shot_.second - 1, field);
                } else {
                    while (!target_cells_.empty() && target_cells_.front().second != last_shot_.second) {
                        target_cells_.pop_front();
                    }
                     while (!target_cells_.empty() && target_cells_.back().second != last_shot_.second) {
                        target_cells_.pop_back();
                    }
                    AddNeighborCellsToTarget(last_shot_.first + 1, last_shot_.second, field);
                    AddNeighborCellsToTarget(last_shot_.first - 1, last_shot_.second, field);
                }
            } else {
                    AddNeighborCellsToTarget(last_shot_.first, last_shot_.second + 1, field);
                    AddNeighborCellsToTarget(last_shot_.first, last_shot_.second - 1, field);
                    AddNeighborCellsToTarget(last_shot_.first + 1, last_shot_.second, field);
                    AddNeighborCellsToTarget(last_shot_.first - 1, last_shot_.second, field);
            }
        } else if (result == "kill") {
            field.MarkAsShip(last_shot_.first, last_shot_.second);  
            field.KillShip();
            if (field.GetTotalShipCnt() == 0) {
                game_stats.EndGame();
                game_stats.SetGameWon();
            }
            MaskAroundShip_(field);
            ship_is_vertical_ = false;
            ship_direction_ = 0;
            target_cells_.clear();
            hits_.clear();
        }
    }


private:
    int64_t next_x_shot_ = 0;
    int64_t next_y_shot_ = 0;
    bool is_custom_ = false;
    int64_t diagonal_index_ = 0;
    bool ship_is_vertical_ = false;
    int64_t ship_direction_ = 0;
    std::deque<std::pair<int64_t, int64_t>> target_cells_;
    std::vector<std::pair<int64_t, int64_t>> hits_;
    std::pair<int64_t, int64_t> last_shot_ = {-1, -1};

    void DetermineShipOrientation() {
        if (hits_.size() >= 2) {
            if (hits_[0].first == hits_[1].first) {
                ship_is_vertical_ = true;
            } else if (hits_[0].second == hits_[1].second) {
                ship_is_vertical_ = false;
            }
        }
    }

    void MaskAroundShip_(CField& field) {
        const int64_t dx[] = {-1, 0, 1, -1, 1, -1, 0, 1};
        const int64_t dy[] = {-1, -1, -1, 0, 0, 1, 1, 1};

        for (int64_t x = 0; x < field.GetWidth(); ++x) {
            for (int64_t y = 0; y < field.GetHeight(); ++y) {
                if (field.IsShip(x, y)) {
                    for (int i = 0; i < 8; ++i) {
                        int64_t nx = x + dx[i];
                        int64_t ny = y + dy[i];
                        if (nx >= 0 && nx < field.GetWidth() && ny >= 0 && ny < field.GetHeight()) {
                            field.MarkAsHit(nx, ny);
                        }
                    }
                }
            }
        }
    }


    void AddNeighborCellsToTarget(int64_t x, int64_t y, CField& field) {
        if (x >= 0 && x < field.GetWidth() && y >= 0 && y < field.GetHeight() 
            && !field.IsAlreadyHit(x, y) && !field.IsShip(x, y)) {
            target_cells_.push_back({x, y});
        }
    }

    std::pair<int64_t, int64_t> GetNextOrderedShot(CField& field) {
        auto temp = std::make_pair(next_x_shot_, next_y_shot_);
        next_x_shot_ += 1;

        if (next_x_shot_ == field.GetWidth()) {
            next_x_shot_ = 0;
            next_y_shot_ += 1;
        }

        return temp;
    }

    std::pair<int64_t, int64_t> GetNextCustomShot(CField& field) {
        while (diagonal_index_ < 3) {
            if (diagonal_index_ == 0 && TryShootEveryThirdDiagonalStartingFrom(3, field)) {
                return last_shot_;
            }
            if (diagonal_index_ == 1 && TryShootEveryThirdDiagonalStartingFrom(2, field)) {
                return last_shot_;
            }
            if (diagonal_index_ == 2 && TryShootEverySecondDiagonalStartingFrom(1, field)) {
                return last_shot_;
            }
            diagonal_index_++;
        }
        return {-1, -1};
    }

    bool TryShootEveryThirdDiagonalStartingFrom(int start_index, CField& field) {
        for (int64_t diag = start_index; diag < field.GetWidth() + field.GetHeight(); diag += 3) {
            for (int64_t x = 0; x < field.GetWidth(); ++x) {
                int64_t y = diag - x;
                if (y >= 0 && y < field.GetHeight() && !field.IsAlreadyHit(x, y)) {
                    last_shot_ = {x, y};
                    field.MarkAsHit(x, y);
                    return true;
                }
            }
        }
        return false;
    }

    bool TryShootEverySecondDiagonalStartingFrom(int start_index, CField& field) {
        for (int64_t diag = start_index; diag < field.GetWidth() + field.GetHeight(); diag += 2) {
            for (int64_t x = 0; x < field.GetWidth(); ++x) {
                int64_t y = diag - x;
                if (y >= 0 && y < field.GetHeight() && !field.IsAlreadyHit(x, y)) {
                    last_shot_ = {x, y};
                    field.MarkAsHit(x, y);
                    return true;
                }
            }
        }
        return false;
    }
};


void CreateMasterGame(CField& field) {
    std::vector<std::string> ships = {
        "15", "15",
        "4", "v", "0", "0",
        "3", "v", "0", "5",
        "3", "v", "0", "12",
        "3", "v", "14", "12",
        "2", "v", "0", "9",
        "2", "v", "14", "0",
        "2", "v", "14", "3",
        "2", "v", "14", "6",
        "2", "v", "14", "9",
        "1", "v", "3", "9",
        "1", "v", "7", "5",
        "1", "v", "9", "12",
        "1", "v", "10", "7",
    };
    field.AddShips_(ships);
}

void CreateSlaveGame(CField& field) {
    field.CreateEmptyField(field.GetWidth(), field.GetHeight());
    field.PlaceShips();
}

void CreateEnemyGame(CField& enemy_field, CField& my_field) {
    enemy_field.CreateEmptyField(my_field.GetWidth(), my_field.GetHeight());
    enemy_field.SetOneShipCnt(my_field.GetOneShipCnt());
    enemy_field.SetTwoShipCnt(my_field.GetTwoShipCnt());
    enemy_field.SetThreeShipCnt(my_field.GetThreeShipCnt());
    enemy_field.SetFourShipCnt(my_field.GetFourShipCnt());
}

std::vector<std::string> SplitString(const std::string& str) {
    std::stringstream ss(str);

    return {std::istream_iterator<std::string>(ss),
            std::istream_iterator<std::string>()};
}

int main() {
    std::cout << "Program has started!" << std::endl;

    int64_t one_ship_cnt_;
    int64_t two_ship_cnt_;
    int64_t three_ship_cnt_;
    int64_t four_ship_cnt_;

    sf::Music music;
    music.openFromFile("resources/heartbeat.wav");
    music.setLoop(true);
    double previous_volume = 30.0f;
    music.setVolume(previous_volume);
    music.play();

    CField my_field;
    my_field.SetTotalShipCnt(0);
    CField enemy_field;
    enemy_field.SetTotalShipCnt(0);
    enemy_field.SetEnemy();
    CStrategy strategy;
    
    bool is_master = false;
    while(true) {
        std::string str;
        std::getline(std::cin, str);
        std::vector<std::string> cmd_list = SplitString(str);

        if (cmd_list.empty()) {
            std::cout << "Please enter a command!" << std::endl;
        } else if(cmd_list[0] == "ping" && cmd_list.size() == 1) {
            std::cout << "pong" << std::endl;
        } else if(cmd_list[0] == "start" && cmd_list.size() == 1) {
            if (!is_master) {
                CreateSlaveGame(my_field);
            }
            CreateEnemyGame(enemy_field, my_field);
            #ifdef _DEV_
                std::cout << "My field:" << std::endl;
                my_field.PrintField();
                std::cout << "Enemy field:" << std::endl;
                enemy_field.PrintField();
            #endif
            std::cout << "ok" << std::endl;
        } else if(cmd_list[0] == "create" && cmd_list.size() == 1) {
            std::cout << "Choose your role: create [master/slave]" << std::endl;
        } else if(cmd_list[0] == "dump" && cmd_list.size() == 1) {
            std::cout << "Please input a path" << std::endl;
        } else if(cmd_list[0] == "dump" && cmd_list.size() == 2) {
            std::string file_name = cmd_list[1];

            if (!std::fstream(file_name)) {
                std::cout << "failed" << std::endl;
            } else {
                std::ofstream fout(file_name);
                my_field.Dump(fout);
                std::cout << "ok" << std::endl;
            }
        } else if(cmd_list[0] == "load" && cmd_list.size() == 1) {
            std::cout << "Please input a path" << std::endl;
        } else if (cmd_list[0] == "load" && cmd_list.size() == 2) {
            std::string file_name = cmd_list[1];

            std::ifstream fin(file_name);
            if (!fin.is_open()) {
                std::cout << "failed" << std::endl;
            } else {
                my_field.Load(fin);
                is_master = true;
                std::cout << "ok"  << std::endl;
            }
        } else if(cmd_list[0] == "create") {
            if (cmd_list[1] == "master" && cmd_list.size() == 2) {
                is_master = true;
                CreateMasterGame(my_field);
                std::cout << "ok" << std::endl;
            } else if (cmd_list[1] == "slave" && cmd_list.size() == 2) {
                is_master = false;
                std::cout << "ok" << std::endl;
            } else {
                std::cout << "Oops! No such role" << std::endl;
            }
        } else if(cmd_list[0] == "get" && cmd_list.size() >= 1) {
            if(cmd_list[1] == "width" && cmd_list.size() == 2) {
                std::cout << my_field.GetWidth()
                          << std::endl;
            } else if(cmd_list[1] == "height" && cmd_list.size() == 2) {
                std::cout << my_field.GetHeight() << std::endl;
            } else if (cmd_list[1] == "count" && cmd_list.size() == 3) {
                if (cmd_list[2] == "1") {
                    std::cout << my_field.GetOneShipCnt() << std::endl;
                } else if (cmd_list[2] == "2") {
                    std::cout << my_field.GetTwoShipCnt() << std::endl;
                } else if (cmd_list[2] == "3") {
                    std::cout << my_field.GetThreeShipCnt() << std::endl;
                } else if (cmd_list[2] == "4") {
                    std::cout << my_field.GetFourShipCnt() << std::endl;
                } else {
                    std::cout << "Wrong ship size" << std::endl;
                }
            }
        } else if(cmd_list[0] == "set" && cmd_list.size() >= 1) {
            if(cmd_list[1] == "strategy" && cmd_list.size() == 3) {
                if (cmd_list[2] == "ordered") {
                    strategy.SetOrderedStrategy();
                    std::cout << "ok" << std::endl;
                } else if (cmd_list[2] == "custom") {
                    std::cout << "ok" << std::endl;
                    strategy.SetCustomStrategy();
                } else {
                    std::cout << "Unknown strategy!" << std::endl;
                }
            } else if(cmd_list[1] == "result" && cmd_list.size() == 3) {
                strategy.ShotResult(cmd_list[2], enemy_field);
                #ifdef _DEV_
                    std::cout << "My field:" << std::endl;
                    my_field.PrintField();
                    std::cout << "Enemy field:" << std::endl;
                    enemy_field.PrintField();
                #endif
            } else if(cmd_list[1] == "width" && cmd_list.size() == 3) {
                my_field.SetWidth(std::stoull(cmd_list[2]));
                std::cout << "ok" << std::endl;
            } else if(cmd_list[1] == "height" && cmd_list.size() == 3) {
                my_field.SetHeight(std::stoull(cmd_list[2]));
                std::cout << "ok" << std::endl;
             } else if (cmd_list[1] == "count" && cmd_list.size() == 4) {
                if (cmd_list[2] == "1") {
                    my_field.SetOneShipCnt(std::stoull(cmd_list[3]));
                    std::cout << "ok" << std::endl;
                } else if (cmd_list[2] == "2") {
                    my_field.SetTwoShipCnt(std::stoull(cmd_list[3]));
                    std::cout << "ok" << std::endl;
                } else if (cmd_list[2] == "3") {
                    my_field.SetThreeShipCnt(std::stoull(cmd_list[3]));
                    std::cout << "ok" << std::endl;
                } else if (cmd_list[2] == "4") {
                    my_field.SetFourShipCnt(std::stoull(cmd_list[3]));
                    std::cout << "ok" << std::endl;
                } else {
                    std::cout << "Wrong ship size" << std::endl;
                }
            }
        } else if(cmd_list[0] == "shot" && cmd_list.size() == 1) {
            std::pair<int64_t, int64_t> coords = strategy.GetNextShot(enemy_field);
            std::cout << coords.first << ' ' << coords.second << std::endl;
        } else if(cmd_list[0] == "shot" && cmd_list.size() == 2) {
            std::cout << "Please enter second coordinate" << std::endl;
        } else if(cmd_list[0] == "shot" && cmd_list.size() == 3) {
            std::cout << my_field.Hit(std::stoull(cmd_list[1]), 
                                      std::stoull(cmd_list[2])) 
                      << std::endl;
            #ifdef _DEV_
                std::cout << "My field:" << std::endl;
                my_field.PrintField();
                std::cout << "Enemy field:" << std::endl;
                enemy_field.PrintField();
            #endif
        } else if(cmd_list[0] == "stop" && cmd_list.size() == 1) {
            std::cout << "ok" << std::endl;
        } else if(cmd_list[0] == "finished" && cmd_list.size() == 1) {
            if (game_stats.IsGameInProcess()) {
                std::cout << "no" << std::endl;
            } else {
                std::cout << "yes" << std::endl;
            }
        } else if(cmd_list[0] == "win" && cmd_list.size() == 1) {
            if (game_stats.IsGameInProcess() || game_stats.IsGameLost()) {
                std::cout << "no" << std::endl;
            } else {
                std::cout << game_stats.IsGameLost() << std::endl;
                std::cout << "yes" << std::endl;
            }
        } else if(cmd_list[0] == "lose" && cmd_list.size() == 1) {
            if (game_stats.IsGameInProcess() || game_stats.IsGameWon()) {
                std::cout << "no" << std::endl;
            } else {
                std::cout << game_stats.IsGameWon() << std::endl;
                std::cout << "yes" << std::endl;
            }
        } else if(cmd_list[0] == "exit" && cmd_list.size() == 1) {
            std::cout << "ok" << std::endl;
            break;
        } 
        else if (cmd_list[0] == "sound" && cmd_list.size() == 1) {
            std::cout << "sound control commands:" << std::endl;
            std::cout << "mute   - turn off the sound" << std::endl;
            std::cout << "unmute - restore the previous sound volume" 
                      << std::endl;
            std::cout << "pause  - pause the sound playback" << std::endl;
            std::cout << "resume - resume sound playback" << std::endl;
            std::cout << "down   - decrease volume" << std::endl;
            std::cout << "up     - increase volume" << std::endl;
        } else if (cmd_list.size() == 2 && cmd_list[0] == "sound") {
            if (cmd_list[1] == "mute") {
                previous_volume = music.getVolume();
                music.setVolume(0);
                std::cout << "muted" << std::endl;
            } else if (cmd_list[1] == "unmute") {
                music.setVolume(previous_volume);
                std::cout << "unmuted. volume restored to " << previous_volume 
                          << "%" << std::endl;
            } else if (cmd_list[1] == "pause") {
                music.pause();
                std::cout << "sound paused" << std::endl;
            } else if (cmd_list[1] == "resume") {
                music.play();
                std::cout << "sound resumed" << std::endl;
            } else if (cmd_list[1] == "down") {
                float current_volume = music.getVolume();
                current_volume = std::max(0.0f, current_volume - 10.0f);
                music.setVolume(current_volume);
                std::cout << "sound volume decreased to " << current_volume 
                          << "%" << std::endl;
            } else if (cmd_list[1] == "up") {
                float current_volume = music.getVolume();
                current_volume = std::min(100.0f, current_volume + 10.0f);
                music.setVolume(current_volume);
                std::cout << "sound volume increased to " << current_volume 
                          << "%" << std::endl;
            } else {
                std::cout << "unknown sound command: " << cmd_list[1] 
                          << std::endl;
            }
        } 
        else {
            std::cerr << "'";
            for (std::string& s : cmd_list) {
                std::cerr << s << ' ';
            }
            std::cerr << "\b' is an unknown command!" 
                      << " Please enter a correct command." << std::endl;
        }
    }

    return EXIT_SUCCESS;
}