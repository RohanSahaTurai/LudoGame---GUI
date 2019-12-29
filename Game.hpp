#ifndef GAME_HPP
#define GAME_HPP

#include <time.h>
#include <stdlib.h>
#include "Player.hpp"

/*******************************************************************************
 * Probabilities of the dice
 * The probabilities are scaled to 6 i.e p/6
*******************************************************************************/
#define ONE_PROBAB    1
#define TWO_PROBAB    1
#define THREE_PROBAB  1
#define FOUR_PROBAB   1
#define FIVE_PROBAB   1
#define SIX_PROBAB    1

#define TRACK_RESET_POSITION  51
#define WIN_POSITION          100
#define WIN_STEPS_MOVED       56
#define FINISH_STEPS_MOVED    51

extern const COORD PosToCoord[101];

typedef enum
{
  MOVE_PROCEED,
  MOVE_END,
  MOVE_DISCARDED,
  MOVE_OUTOFSTEPS,
  PLAYER_WON,
  TOKEN_WON,
  TOKEN_ELIMINATED,
  GAME_ENDED

}MOVESTATUS;

typedef enum
{
  ENTER_TOKEN_ONLY,
  MOVE_TOKEN,
  NO_VALID_MOVE

}VALIDMOVETYPE;

/*******************************************************************************
 * Function Prototype
*******************************************************************************/
uint8_t Game_RollDice();

int8_t Game_EnterToken (token_t* token, uint8_t playerNb, sf::Color colorAttrib);

uint8_t Game_GetTokenStat (const player_t player, uint8_t* NbEliminated, uint8_t* NbWon);

VALIDMOVETYPE Game_CheckValidMoveAvailable (token_t* token, uint8_t selectedRoll);

MOVESTATUS Game_PlayerMove (player_t* player, uint8_t playerNb, uint8_t tokenNb, uint8_t diceNb);

#endif // GAME_H

