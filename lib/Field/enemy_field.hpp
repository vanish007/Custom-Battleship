#ifndef _ENEMY_FIELD_
#define _ENEMY_FIELD_

#include "my_field.hpp"

class CEnemyField : public CField {
public:
    void MarkAsHit(int64_t x, int64_t y);
    void MarkAsShip(int64_t x, int64_t y);
    void CopyValues(CMyField& field);
    void CreateDefaultGame() override;
    bool IsAlreadyHit(int64_t x, int64_t y) const;
    bool IsThisCellShip(int64_t x, int64_t y) const;
};

#endif