#include "enemy_field.hpp"


void CEnemyField::MarkAsHit(int64_t x, int64_t y) { 
    field_[y][x].first = true;
}

void CEnemyField::MarkAsShip(int64_t x, int64_t y) {
    field_[y][x].second = true;
    field_[y][x].first = true;
}

void CEnemyField::CopyValues(CMyField& field) {
    coord_x_ = field.GetWidth();
    coord_y_ = field.GetHeight();
    SetShipAmount(1, field.GetShipAmount(1));
    SetShipAmount(2, field.GetShipAmount(1));
    SetShipAmount(3, field.GetShipAmount(1));
    SetShipAmount(4, field.GetShipAmount(1));
}

void CEnemyField::CreateDefaultGame() {
    CreateEmptyField(GetWidth(), GetHeight());
}

bool CEnemyField::IsAlreadyHit(int64_t x, int64_t y) const { 
    return field_[y][x].first;
}

bool CEnemyField::IsThisCellShip(int64_t x, int64_t y) const { 
    return field_[y][x].second;
}