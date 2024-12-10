#include "game_stats.hpp"


bool CGameStats::IsGameInProcess() const {
    return is_game_in_process_;
}

void CGameStats::StartGame() {
    is_game_in_process_ = true;
    is_game_won_ = false;
    is_game_lost_ = false;
}

void CGameStats::EndGame() {
    is_game_in_process_ = false;
}

bool CGameStats::IsGameWon() const {
    if (is_game_in_process_ == true) {
        return false;
    }
    return is_game_won_;
}

void CGameStats::SetGameWon() {
    is_game_lost_ = false;
    is_game_won_ = true;
}

bool CGameStats::IsGameLost() const {
    if (is_game_in_process_ == true) {
        return false;
    }
    return is_game_lost_;
}

void CGameStats::SetGameLost() {
    is_game_won_ = false;
    is_game_lost_ = true;
}