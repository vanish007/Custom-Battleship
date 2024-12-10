#include <cstdint>
#include <string>
#include <deque>
#include <vector>

#include "../Field/enemy_field.hpp"

class CStrategy {
public:
    void SetOrderedStrategy();
    void SetCustomStrategy();
    std::pair<int64_t, int64_t> GetNextShot(CEnemyField& field);
    void ShotResult(const std::string& result, CEnemyField& field);

private:
    int64_t next_x_shot_ = 0;
    int64_t next_y_shot_ = 0;
    bool is_custom_ = true;
    int64_t diagonal_index_ = 0;
    bool ship_is_vertical_ = false;
    int64_t ship_direction_ = 0;

    std::deque<std::pair<int64_t, int64_t>> current_target_cells_;
    std::vector<std::pair<int64_t, int64_t>> hits_;
    std::pair<int64_t, int64_t> last_shot_ = {-1, -1};

    void DetermineShipOrientation_();

    void DetermineTargetCells_(CEnemyField& field);

    void MaskAroundShip_(CEnemyField& field);


    void AddNeighborCellsToTarget_(int64_t x, int64_t y, CEnemyField& field);

    std::pair<int64_t, int64_t> GetNextOrderedShot_(CEnemyField& field);

    std::pair<int64_t, int64_t> GetNextCustomShot_(CEnemyField& field);

    bool TryShootEveryDiagonal_(short start_index, 
                                short diag_index, 
                                CEnemyField& field);
};