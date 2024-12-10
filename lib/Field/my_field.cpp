#include "my_field.hpp"

void CMyField::Dump(std::ofstream& out) const {
    out << stats_[0] << ' ' << stats_[1] << std::endl;
    for (size_t i = 2; i < stats_.size(); i += 4) {
        out << stats_[i] << ' ' << stats_[i+1] << ' ' 
            << stats_[i+2] << ' ' << stats_[i+3] << std::endl;
    }
}

void CMyField::Load(std::ifstream& in) {
    std::string cmd;
    std::vector<std::string> vec;
    while (in >> cmd) {
        vec.push_back(cmd);
    }
    AddShips_(vec);
}

void CMyField::SetWidth(int64_t x) {
    coord_x_ = x;
    stats_.push_back(std::to_string(x));
}

void CMyField::SetHeight(int64_t y) {
    coord_y_ = y;
    stats_.push_back(std::to_string(y));
}

std::string CMyField::Hit(int64_t x, int64_t y) { 
    field_[y][x].first = true;
    if (field_[y][x].second) {
        if (CheckForKill_(y, x)) {
            DecreaseTotalShipCounter_();
            return "kill";
        } else {
            return "hit";
        }
    } else {
        return "miss";
    }
}

void CMyField::CreateMasterGame() {
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
    AddShips_(ships);
}

void CMyField::CreateDefaultGame() {
    CreateEmptyField(GetWidth(), GetHeight());
    PlaceShips_();
}

void CMyField::PlaceSingleShip_(int64_t x, int64_t y, int64_t length,
                        bool is_vertical) { 
    for (int64_t i = 0; i < length; ++i) {
        int64_t ny;
        int64_t nx;
        if (is_vertical) {
            ny = y + i;
            nx = x;
        } else {
            ny = y;
            nx = x + i;
        }
        field_[ny][nx].second = true;
    }
}

void CMyField::AddShips_(std::vector<std::string> ships) { 
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
        PlaceSingleShip_(ships[i], ships[i+1], ships[i+2], ships[i+3]);
    }
}

void CMyField::PlaceSingleShip_(std::string s1, std::string s2, 
                std::string s3, std::string s4) { 
    short size = std::stoull(s1);
    ++ship_counter_[size];
    ++ship_counter_[0];

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

bool CMyField::PlaceShips_() { 
    int64_t required_cells = ship_counter_[1] * 1 +
                                ship_counter_[2] * 2 +
                                ship_counter_[3] * 3 +
                                ship_counter_[4] * 4;
    if (required_cells > coord_x_ * coord_y_) return false;

    bool left_side = true;
    bool up_side = true;
    if (!PlaceAlongWalls_(ship_counter_[4], 4, left_side)
        && !PlaceAlongFloors_(ship_counter_[4], 4, up_side)) {
        return false;
    }
    if (!PlaceAlongWalls_(ship_counter_[2], 2, left_side)
        && !PlaceAlongFloors_(ship_counter_[2], 2, up_side)) {
        return false;
    }   
    if (!PlaceAlongWalls_(ship_counter_[3], 3, left_side)
        && !PlaceAlongFloors_(ship_counter_[3], 2, up_side)) {
        return false;
    }
    if (!PlaceRandomly_(ship_counter_[1])) {
        return false;
    }

    return true;
}

bool CMyField::CanPlaceSingleShip_(int64_t x, int64_t y, int64_t length,
                            bool is_vertical) { 
    for (int64_t i = 0; i < length; ++i) {
        int64_t ny;
        int64_t nx;
        if (is_vertical) {
            ny = y + i;
            nx = x;
        } else {
            ny = y;
            nx = x + i;
        }

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

bool CMyField::PlaceAlongWalls_(int64_t ship_count, int64_t ship_size, bool& left_side) { 
    int64_t offset = 0;

    for (int64_t i = 0; i < ship_count; ++i) {
        bool placed = false;

        while (offset < coord_x_ / 2) {
            for (int64_t attempt = 0; attempt < 2; ++attempt) { 
                for (int64_t y = 0; y < coord_y_; ++y) {
                    int64_t x = left_side ? offset : coord_x_ - 1 - offset;

                    if (CanPlaceSingleShip_(x, y, ship_size, true)) {
                        PlaceSingleShip_(x, y, ship_size, true);
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

bool CMyField::PlaceAlongFloors_(int64_t ship_count, int64_t ship_size, bool& up_side) { 
    int64_t offset = 0;

    for (int64_t i = 0; i < ship_count; ++i) {
        bool placed = false;

        while (offset < coord_y_ / 2) {
            for (int64_t attempt = 0; attempt < 2; ++attempt) { 
                for (int64_t x = 0; x < coord_x_; ++x) {
                    int64_t y = up_side ? offset : coord_y_ - 1 - offset;

                    if (CanPlaceSingleShip_(x, y, ship_size, false)) {
                        PlaceSingleShip_(x, y, ship_size, false);
                        stats_.push_back(std::to_string(ship_size));
                        stats_.push_back("h");
                        stats_.push_back(std::to_string(x));
                        stats_.push_back(std::to_string(y));
                        placed = true;
                        break;
                    }
                }

                if (placed) {
                    break; 
                }

                up_side = !up_side;
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

bool CMyField::TryPlaceInField_() { 
    std::srand(time(NULL));

    for (int attempts = 0; attempts < 10000; ++attempts) { 
        int64_t x;
        if (coord_x_ == 2) {
            x = std::rand() % 2;
        } else {
            x = std::rand() % (coord_x_ - 2) + 1; 
        }
        int64_t y;
        if (coord_y_ == 2) {
            y = std::rand() % 2;
        } else {
            y = std::rand() % (coord_y_ - 2) + 1; 
        }

        if (CanPlaceSingleShip_(x, y, 1, true)) {
            PlaceSingleShip_(x, y, 1, true);
            return true;
        }
    }
    return false;
}

bool CMyField::TryPlaceOutField_() {
    std::srand(time(NULL));

    for (int attempts = 0; attempts < 10000; ++attempts) { 
        int64_t x, y;
        if (std::rand() % 2 == 0) { 
            x = std::rand() % coord_x_;
            y = (std::rand() % 2 == 0) ? 0 : coord_y_ - 1; 
        } else { 
            y = std::rand() % coord_y_;
            x = (std::rand() % 2 == 0) ? 0 : coord_x_ - 1;
        }

        if (CanPlaceSingleShip_(x, y, 1, true)) {
            PlaceSingleShip_(x, y, 1, true);
            return true;
        }
    }
    return false;
}

bool CMyField::PlaceRandomly_(int64_t ship_count) { 
    for (int64_t i = 0; i < ship_count; ++i) {
        if (!TryPlaceInField_() && !TryPlaceOutField_()) {
            return false;
        }
    }
    return true;
}

bool CMyField::CheckForKill_(int64_t x, int64_t y) { 
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
