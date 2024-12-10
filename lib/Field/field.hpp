#ifndef _FIELD_
#define _FIELD_

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

class CField {
public:
    void CreateEmptyField(int64_t x, int64_t y);
    void DecreaseTotalShipCounter_();
    void SetShipAmount(short size, int64_t amount);
    int64_t GetTotalShipCnt() const;
    int64_t GetShipAmount(short size) const;
    int64_t GetWidth() const;
    int64_t GetHeight() const;
    /**
     * @brief Prints the field with coordinates and content.
     * 
     * The field is displayed as a grid where:
     *   '*' indicates a cell with destroyed ship.
     *   'O' indicates a cell where the ship is.
     *   'X' indicates a cell where the strike already was.
     *   '.' indicates an empty cell.
     */
    void PrintField();
    virtual void SetWidth(int64_t x);
    virtual void SetHeight(int64_t y);
    virtual void CreateDefaultGame() = 0;
    ~CField();
protected:
    int64_t coord_x_;
    int64_t coord_y_;
    std::pair<bool, bool>** field_;

    std::vector<int64_t> ship_counter_ = {0, 0, 0, 0, 0}; 
};

#endif