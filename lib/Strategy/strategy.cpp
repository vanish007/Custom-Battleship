#include "strategy.hpp"

void CStrategy::SetOrderedStrategy() {
    is_custom_ = false;
    next_x_shot_ = 0;
    next_y_shot_ = 0;
    diagonal_index_ = 0;
    current_target_cells_.clear();
    ship_is_vertical_ = false;
    ship_direction_ = 0;
}

void CStrategy::SetCustomStrategy() {
    is_custom_ = true;
    diagonal_index_ = 0;
    current_target_cells_.clear();
    ship_is_vertical_ = false;
    ship_direction_ = 0;
}

std::pair<int64_t, int64_t> CStrategy::GetNextShot(CEnemyField& field) {
    if (!current_target_cells_.empty()) {
        last_shot_ = current_target_cells_.front();
        current_target_cells_.pop_front();
        return last_shot_;
    }

    if (is_custom_) {
        last_shot_ = GetNextCustomShot_(field);
    } else {
        last_shot_ = GetNextOrderedShot_(field);
    }

    return last_shot_;
}

void CStrategy::ShotResult(const std::string& result, CEnemyField& field) {
    if (result == "miss") {
        field.MarkAsHit(last_shot_.first, last_shot_.second);
    } else if (result == "hit") {
        field.MarkAsShip(last_shot_.first, last_shot_.second);
        if (is_custom_) {
            hits_.push_back(last_shot_);

            DetermineTargetCells_(field);
        }
    } else if (result == "kill") {
        field.MarkAsShip(last_shot_.first, last_shot_.second);  
        field.DecreaseTotalShipCounter_();
        MaskAroundShip_(field);
        ship_is_vertical_ = false;
        ship_direction_ = 0;
        current_target_cells_.clear();
        hits_.clear();
    }
}



void CStrategy::DetermineShipOrientation_() {
    if (hits_.size() >= 2) {
        if (hits_[0].first == hits_[1].first) {
            ship_is_vertical_ = true;
        } else if (hits_[0].second == hits_[1].second) {
            ship_is_vertical_ = false;
        }
    }
}

void CStrategy::DetermineTargetCells_(CEnemyField& field) {
    if (hits_.size() > 1) {
        DetermineShipOrientation_();
        if (ship_is_vertical_) {
            while (!current_target_cells_.empty() 
                    && current_target_cells_.front().first
                    != last_shot_.first) {
                current_target_cells_.pop_front();
            }
            while (!current_target_cells_.empty() 
                && current_target_cells_.back().first 
                != last_shot_.first) {
            current_target_cells_.pop_back();
            }
            AddNeighborCellsToTarget_(last_shot_.first, 
                                        last_shot_.second + 1, field);
            AddNeighborCellsToTarget_(last_shot_.first, 
                                        last_shot_.second - 1, field);
        } else {
            while (!current_target_cells_.empty() 
                    && current_target_cells_.front().second 
                    != last_shot_.second) {
                current_target_cells_.pop_front();
            }
            while (!current_target_cells_.empty() 
                    && current_target_cells_.back().second 
                    != last_shot_.second) {
            current_target_cells_.pop_back();
            }
            AddNeighborCellsToTarget_(last_shot_.first + 1, 
                                        last_shot_.second, field);
            AddNeighborCellsToTarget_(last_shot_.first - 1, 
                                        last_shot_.second, field);
        }
    } else {
            AddNeighborCellsToTarget_(last_shot_.first, 
                                        last_shot_.second + 1, field);
            AddNeighborCellsToTarget_(last_shot_.first, 
                                        last_shot_.second - 1, field);
            AddNeighborCellsToTarget_(last_shot_.first + 1, 
                                        last_shot_.second, field);
            AddNeighborCellsToTarget_(last_shot_.first - 1, 
                                        last_shot_.second, field);
    }
}

void CStrategy::MaskAroundShip_(CEnemyField& field) {
    const int64_t dx[] = {-1, 0, 1, -1, 1, -1, 0, 1};
    const int64_t dy[] = {-1, -1, -1, 0, 0, 1, 1, 1};

    for (int64_t x = 0; x < field.GetWidth(); ++x) {
        for (int64_t y = 0; y < field.GetHeight(); ++y) {
            if (field.IsThisCellShip(x, y)) {
                for (int i = 0; i < 8; ++i) {
                    int64_t nx = x + dx[i];
                    int64_t ny = y + dy[i];
                    if (nx >= 0 && nx < field.GetWidth() 
                        && ny >= 0 && ny < field.GetHeight()) {
                        field.MarkAsHit(nx, ny);
                    }
                }
            }
        }
    }
}


void CStrategy::AddNeighborCellsToTarget_(int64_t x, int64_t y, CEnemyField& field) {
    if (x >= 0 && x < field.GetWidth() && y >= 0 && y < field.GetHeight()
        && !field.IsAlreadyHit(x, y) && !field.IsThisCellShip(x, y)) {
        current_target_cells_.push_back({x, y});
    }
}

std::pair<int64_t, int64_t> CStrategy::GetNextOrderedShot_(CEnemyField& field) {
    auto temp = std::make_pair(next_x_shot_, next_y_shot_);
    next_x_shot_ += 1;

    if (next_x_shot_ == field.GetWidth()) {
        next_x_shot_ = 0;
        next_y_shot_ += 1;
    }

    return temp;
}

std::pair<int64_t, int64_t> CStrategy::GetNextCustomShot_(CEnemyField& field) {
    while (diagonal_index_ < 4) {
        if (diagonal_index_ == 0 && TryShootEveryDiagonal_(0, 3, field)) {
            return last_shot_;
        }
        if (diagonal_index_ == 1 && TryShootEveryDiagonal_(2, 3, field)) {
            return last_shot_;
        }
        if (diagonal_index_ == 2 && TryShootEveryDiagonal_(1, 2, field)) {
            return last_shot_;
        } 
        if (diagonal_index_ == 3 && TryShootEveryDiagonal_(0, 2, field)) {
            return last_shot_;
        }
        diagonal_index_++;
    }
    return {-1, -1};
}

bool CStrategy::TryShootEveryDiagonal_(short start_index, 
                            short diag_index, 
                            CEnemyField& field) {
    for (int64_t diag = start_index; 
            diag < field.GetWidth() + field.GetHeight(); 
            diag += diag_index) {
        for (int64_t x = 0; x < field.GetWidth(); ++x) {
            int64_t y = diag - x;
            if (y >= 0 && y < field.GetHeight() 
                && !field.IsAlreadyHit(x, y)) {
                last_shot_ = {x, y};
                field.MarkAsHit(x, y);
                return true;
            }
        }
    }
    return false;
}