#ifndef _MY_FIELD_
#define _MY_FIELD_

#include <fstream>

#include "field.hpp"

class CMyField : public CField {
public:
    void Dump(std::ofstream& out) const;
    void Load(std::ifstream& in);
    void SetWidth(int64_t x) override;
    void SetHeight(int64_t y) override;
    std::string Hit(int64_t x, int64_t y);
    void CreateMasterGame();
    void CreateDefaultGame() override;
    
private:
    std::vector<std::string> stats_; 
    void PlaceSingleShip_(int64_t x, int64_t y, int64_t length,
                            bool is_vertical);
    void AddShips_(std::vector<std::string> ships);
    void PlaceSingleShip_(std::string s1, std::string s2, 
                    std::string s3, std::string s4);
    bool PlaceShips_();
    bool CanPlaceSingleShip_(int64_t x, int64_t y, int64_t length,
                             bool is_vertical);
    bool PlaceAlongWalls_(int64_t ship_count, int64_t ship_size, bool& left_side);
    bool PlaceAlongFloors_(int64_t ship_count, int64_t ship_size, bool& up_side);
    bool TryPlaceInField_();
    bool TryPlaceOutField_();
    bool PlaceRandomly_(int64_t ship_count);
    bool CheckForKill_(int64_t x, int64_t y);
};

#endif