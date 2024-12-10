#ifndef _GAME_STATS_
#define _GAME_STATS_


/**
 * @class CGameStats
 * @brief A class to manage and track the state of a game.
 *
 * This class provides methods to start, end, and check the status of a game.
 * It also tracks whether the game has been won or lost.
 */
class CGameStats{
public:

    /**
     * @return True if the game is in progress, false otherwise.
     */
    bool IsGameInProcess() const;

    /**
     * Sets the game state to "in process" and resets win/loss flags.
     */
    void StartGame();

    /**
     * Sets the game state to "not in process" while keeping the current win/loss status.
     */
    void EndGame();

    /**
     * @brief Checks if the game is marked as won.
     * @return True if the game has been won, false otherwise.
     */
    bool IsGameWon() const;

    /**
     * Sets the win flag to true and the loss flag to false.
     */
    void SetGameWon();

    /**
     * @brief Checks if the game is marked as lost.
     * @return True if the game has been lost, false otherwise.
     */
    bool IsGameLost() const;

    /**
     * Sets the loss flag to true and the win flag to false.
     */
    void SetGameLost();

private:

    bool is_game_in_process_ = true; // Tracks whether the game is currently in progress.
    bool is_game_won_ = false; // Tracks whether the game has been marked as won.
    bool is_game_lost_ = false; // Tracks whether the game has been marked as lost.
    
};

#endif 