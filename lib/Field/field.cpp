#include "field.hpp"

void CField::CreateEmptyField(int64_t x, int64_t y) { 
    coord_x_ = x;
    coord_y_ = y;
    field_ = new std::pair<bool, bool>* [y]{};
    for (size_t i = 0; i < y; ++i) {
        field_[i] = new std::pair<bool, bool>[x]{};
    }
}

void CField::DecreaseTotalShipCounter_() { 
    ship_counter_[0] = std::max(0LL, ship_counter_[0]-1);
}

void CField::SetShipAmount(short size, int64_t amount) { 
    ship_counter_[size] = amount;
    ship_counter_[0] += amount;
}

int64_t CField::GetTotalShipCnt() const { 
    return ship_counter_[0];
}

int64_t CField::GetShipAmount(short size) const { 
    return ship_counter_[size];
}

int64_t CField::GetWidth() const { 
    return coord_x_;
}

int64_t CField::GetHeight() const {  
    return coord_y_;
}

void CField::PrintField() { 
    auto GetSeparatorWidth = [](int64_t max_value) {
        return std::to_string(max_value).length() + 1;
    };

    // Determine the width needed for formatting.
    int x_separator_width = GetSeparatorWidth(coord_x_);
    int y_separator_width = GetSeparatorWidth(coord_y_);

    // Print x-axis header
    std::cout << std::string(y_separator_width, ' '); // Initial space for y-coordinates
    for (size_t j = 0; j < coord_x_; ++j) {
        std::cout << std::setw(x_separator_width) << j;
    }
    std::cout << '\n';

    // Print each row
    for (size_t i = 0; i < coord_y_; ++i) {
        // Print y-coordinate
        std::cout << std::setw(y_separator_width) << i;

        // Print row contents
        for (size_t j = 0; j < coord_x_; ++j) {
            char cell_content;
            if (field_[i][j].first && field_[i][j].second) {
                cell_content = '*';
            } else if (field_[i][j].second) {
                cell_content = 'O';
            } else if (field_[i][j].first) {
                cell_content = 'X';
            } else {
                cell_content = '.';
            }

            std::cout << std::setw(x_separator_width) << cell_content;
        }
        std::cout << '\n';
    }
}

void CField::SetWidth(int64_t x) {
    coord_x_ = x;
}

void CField::SetHeight(int64_t y) {
    coord_y_ = y;
}

CField::~CField() { 
    for (size_t i = 0; i < coord_y_; ++i) {
        delete [] field_[i];
    }
    delete [] field_;
}