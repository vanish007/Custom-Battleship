add_library(GameStats 
            GameStats/game_stats.cpp 
            GameStats/game_stats.hpp)

add_library(Strategy 
            Strategy/strategy.cpp 
            Strategy/strategy.hpp)
            
add_library(Field 
            Field/field.cpp 
            Field/field.hpp 
            Field/my_field.cpp 
            Field/my_field.hpp 
            Field/enemy_field.cpp 
            Field/enemy_field.hpp)

target_include_directories(GameStats PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})