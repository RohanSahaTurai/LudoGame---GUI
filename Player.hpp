#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <windows.h>
#include <SFML/Graphics.hpp>

/*******************************************************************************
 * Preprocessing directives
*******************************************************************************/
#define TOKEN_NUM 4

/*******************************************************************************
 * GLOBAL VARIABLE
*******************************************************************************/
// Number of players in the current game
// The number of player is set by the user
extern uint8_t NbPlayerInGame;

extern const COORD PosToCoord[101];
extern sf::Color YellowCustom;

/*******************************************************************************
 * Custom Defined Structure for Tokens
*******************************************************************************/
typedef struct token
{
    int8_t position;
    int8_t steps_moved;

    bool has_won;
    bool is_safe;

    sf::CircleShape tokenPiece;

}token_t;

typedef struct player
{
    token_t token [TOKEN_NUM];
    sf::Color colorAttrib;
    uint8_t scorecard;

}player_t;

/*******************************************************************************
 * Function Prototype
*******************************************************************************/
void Player_ResetToken (token_t* token);

void Player_InitPlayers(player_t* player);

void Player_PrintPlayers (const player_t* player);

#endif // TOKEN_H

