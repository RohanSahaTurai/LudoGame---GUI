#include "Player.hpp"
#include "Game.hpp"
#include "SFML/Graphics.hpp"

// look up table for converting position to coord
const COORD PosToCoord [101] =
{
    {230,470}, { 80,350}, {230,410}, {230,380}, {230,350}, {230,320},
    {200,290}, {170,290}, {140,290}, {110,290}, { 80,290}, { 50,290}, { 50,260},
    { 50,230}, { 80, 80}, {110,230}, {140,230}, {170,230}, {200,230},
    {230,200}, {230,170}, {230,140}, {230,110}, {230, 80}, {230, 50}, {260, 50},
    {290, 50}, {350, 80}, {290,110}, {290,140}, {290,170}, {290,200},
    {320,230}, {350,230}, {380,230}, {410,230}, {440,230}, {470,230}, {470,260},
    {470,290}, {350,350}, {410,290}, {380,290}, {350,290}, {320,290},
    {290,320}, {290,350}, {290,380}, {290,410}, {290,440}, {290,470}, {260,470},
    { 00, 00}, { 00, 00}, { 00, 00}, { 00, 00}, { 00, 00}, { 00, 00}, { 00, 00}, { 00, 00},
    {260,440}, {260,410}, {260,380}, {260,350}, {260,320},
    { 00, 00}, { 00, 00}, { 00, 00}, { 00, 00}, { 00, 00},
    { 80,260}, {110,260}, {140,260}, {170,260}, {200,260},
    { 00, 00}, { 00, 00}, { 00, 00}, { 00, 00}, { 00, 00},
    {260, 80}, {260,110}, {260,140}, {260,170}, {260,200},
    { 00, 00}, { 00, 00}, { 00, 00}, { 00, 00}, { 00, 00},
    {440,260}, {410,260}, {380,260}, {350,260}, {320,260},
    { 00, 00}, { 00, 00}, { 00, 00}, { 00, 00}, {-25,-25}, {215, 230}
};

//Global Sprite for background image and board
sf::Sprite SpriteBoard;
sf::Sprite SpriteBackground;
sf::RectangleShape InfoBox(sf::Vector2f(480.f, 60.f));
sf::Text InfoMessage, PlayerMessage;
sf::RenderWindow RndrWindow(sf::VideoMode(540, 600), "Ludo Board Game",
                            sf::Style::Close | sf::Style::Titlebar);


// Number of players currently playing
uint8_t NbPlayerInGame;

/*******************************************************************************
 * Displays one frame
*******************************************************************************/
void UpdateWindowDisplay(const player_t* player)
{
    // Update the window Display
    RndrWindow.clear();
    RndrWindow.draw(SpriteBackground);
    RndrWindow.draw(SpriteBoard);
    RndrWindow.draw(InfoBox);

    for (uint8_t i = 0; i < NbPlayerInGame; i++)
        for (uint8_t j = 0; j < TOKEN_NUM; j++)
        {
            if (player[i].token[j].position != -1)
                RndrWindow.draw(player[i].token[j].tokenPiece);
        }

    RndrWindow.draw(InfoMessage);
    RndrWindow.draw(PlayerMessage);

    RndrWindow.display();
}

/*******************************************************************************
 * Displays a token on the ludo board
*******************************************************************************/
void UpdateTokenDisplay (player_t* players, uint8_t currPLayerNb, uint8_t currTokenNb)
{
    player_t* currPlayer = &players[currPLayerNb];
    token_t* currToken = &currPlayer->token[currTokenNb];
    uint8_t position = currToken->position;
    COORD currPosToCoord = PosToCoord[position];

    // Display procedure inside the home box
    if (position == 1  || position == 14 || position == 27 ||
            position == 40 || position == 100)
    {
        int8_t dx = 0, dy = 0;

        for (uint8_t i = 0; i < NbPlayerInGame; i++)
            for (uint8_t j = 0; j < TOKEN_NUM; j++)
            {
                if (players[i].token[j].tokenPiece.getPosition() == currToken->tokenPiece.getPosition() &&
                    !(i == currPLayerNb && j == currTokenNb))
                {
                    //translate rightwards
                    if (position == 100)
                        dx += 15;
                    else
                        dx += 30;

                    // 30 x 4 = 120
                    if (dx == 150 || dx == 75)
                    {
                        if (position == 100)
                            dx = 15;
                        else
                            dx = 30;

                        //translate downwards
                        if (position == 100)
                            dy += 15;
                        else
                            dy += 30;
                    }
                }
            }

        currToken->tokenPiece.setPosition(currPosToCoord.X + dx,
                                          currPosToCoord.Y + dy);
    }

    else
    {
        uint8_t count = 1;
        sf::Vector2f coordPos;

        // Check if one or more tokens of the same color are placed in that cell
        for (uint8_t i = 0; i < TOKEN_NUM; i++)
        {
            if (currPlayer->token[i].position == position && i != currTokenNb)
            {
                coordPos = currPlayer->token[i].tokenPiece.getPosition();

                if (coordPos.x == currPosToCoord.X &&
                        coordPos.y == currPosToCoord.Y )
                    currPlayer->token[i].tokenPiece.setPosition(currPosToCoord.X - 5,
                            currPosToCoord.Y);

                else if (coordPos.x != currPosToCoord.X &&
                         coordPos.y == currPosToCoord.Y )
                    currPlayer->token[i].tokenPiece.setPosition(coordPos.x,
                            coordPos.y - 5);
                count++;
            }
        }

        switch (count)
        {
        case 1:
            currToken->tokenPiece.setPosition(currPosToCoord.X,
                                              currPosToCoord.Y);
            break;

        case 2:
            currToken->tokenPiece.setPosition(currPosToCoord.X + 10,
                                              currPosToCoord.Y);
            break;

        case 3:
            currToken->tokenPiece.setPosition(currPosToCoord.X - 5,
                                              currPosToCoord.Y + 5);
            break;

        case 4:
            currToken->tokenPiece.setPosition(currPosToCoord.X + 10,
                                              currPosToCoord.Y + 5);
            break;
        }
    }

    // Update the frame
    UpdateWindowDisplay(players);
}

/*******************************************************************************
 * Gets the token number based on COORD
 * Returns -1 if no token is found
*******************************************************************************/
inline int8_t GetTokenNb (const player_t player, const sf::Vector2i coordPos)
{
    uint8_t i;

    for (i = 0; i < TOKEN_NUM; i++)
    {
        if (player.token[i].tokenPiece.getGlobalBounds().contains(coordPos.x, coordPos.y) &&
                player.token[i].has_won == false)
            return i;
    }

    return -1;
}

/*******************************************************************************
 * Gets the token number if only token is in the game
*******************************************************************************/
inline int8_t GetSingleTokenNb (const player_t player)
{
    uint8_t i;

    for (i = 0; i < TOKEN_NUM; i++)
    {
        if (player.token[i].steps_moved != -1 &&
                player.token[i].has_won == false)
            return i;
    }

    return -1;
}

/*******************************************************************************
 * Function to ENTER A NEW TOKEN
*******************************************************************************/
inline MOVESTATUS EnterNewTokenProc(player_t* player, uint8_t playerNb)
{

    uint8_t tokenNb = Game_EnterToken(player[playerNb].token, playerNb,
                                      player[playerNb].colorAttrib);

    UpdateTokenDisplay(player, playerNb, tokenNb);

    return MOVE_END;
}

/*******************************************************************************
 * Function to ROLL THE DICE
*******************************************************************************/
MOVESTATUS RollDiceProc (uint8_t* roll, int8_t& rollNb, const player_t* player)
{
    char buffer[128];
    int8_t startRollNb = rollNb;
    sf::Event event;
    bool getInput;

    // if the dice is six, roll the dice again
    for (; (rollNb) < (startRollNb + 3); (rollNb)++)
    {
        printf("Hit SPACEBAR to roll the dice.\n");

        InfoMessage.setString("Hit SPACEBAR to roll the dice.");
        UpdateWindowDisplay(player);

        getInput = true;

        // Waits until spacebar is pressed
        while (RndrWindow.waitEvent(event) && getInput)
        {
            if (event.type == sf::Event::Closed)
                RndrWindow.close();

            else if (event.type == sf::Event::KeyPressed &&
                     event.key.code == sf::Keyboard::Space)
                getInput = false;

            /*DEBUG
            else if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code >= sf::Keyboard::Num1 &&
                        event.key.code <= sf::Keyboard::Num6)
                {
                    roll[rollNb] = event.key.code - sf::Keyboard::Num1 + 1;
                    getInput = false;
                }
            }*/

        }

        roll[rollNb] = Game_RollDice();

        printf("Dice Score = %d", roll[rollNb]);

        sprintf(buffer, "Dice Score = %d", roll[rollNb]);
        InfoMessage.setString(buffer);
        UpdateWindowDisplay(player);
        Sleep(1000);

        if (roll[rollNb] != 6)
        {
            (rollNb)++;
            printf("\n");
            return MOVE_PROCEED;
        }

        // if the score is 6 three times a row, discard the move
        if (roll[startRollNb + 2] == 6)
        {
            roll[startRollNb + 2] = 0;

            printf("!!! Move Discarded !!!\n");

            InfoMessage.setString("!!! Move Discarded !!!");
            UpdateWindowDisplay(player);
            Sleep(2000);

            rollNb = startRollNb;

            return MOVE_DISCARDED;
        }

        printf("\n");
    }

    printf("\n");
    return MOVE_PROCEED;
}


/*******************************************************************************
 * main
*******************************************************************************/
int main()
{
    std::srand(time(NULL));

    /////////////////////////////////////////////////////////////
    ///// SFML Initializations
    //////////////////////////////////////////////////////////////
    /* Window Init */
    RndrWindow.setVerticalSyncEnabled(true);

    /* Background Image Init */
    sf::Texture textureBackground;
    if (!textureBackground.loadFromFile("images/background2.jpg"))
        return EXIT_FAILURE;
    SpriteBackground.setTexture(textureBackground);

    /* Board Init */
    sf::Texture textureBoard;
    if (!textureBoard.loadFromFile("images/Ludo_Board.png"))
        return EXIT_FAILURE;
    SpriteBoard.setTexture(textureBoard);
    SpriteBoard.setPosition(30,30);

    /* Info Box Init */
    InfoBox.setPosition(30,520);
    InfoBox.setFillColor(sf::Color(255,255,255,200));

    /* Text message Init */
    sf::Font font;
    if(!font.loadFromFile("font/segoe-ui.ttf"))
        return EXIT_FAILURE;

    InfoMessage.setFont(font);
    InfoMessage.setCharacterSize(15);
    InfoMessage.setFillColor(sf::Color::Black);
    InfoMessage.setPosition(45,550);

    PlayerMessage.setFont(font);
    PlayerMessage.setCharacterSize(20);
    PlayerMessage.setStyle(sf::Text::Underlined | sf::Text::Bold);
    PlayerMessage.setOutlineColor(sf::Color::Black);
    PlayerMessage.setOutlineThickness(.5);
    PlayerMessage.setPosition(45,525);

    sf::Text titleMessage;
    titleMessage.setFont(font);
    titleMessage.setCharacterSize(35);
    titleMessage.setFillColor(sf::Color::White);
    titleMessage.setStyle(sf::Text::Bold);
    titleMessage.setOutlineColor(sf::Color::Black);
    titleMessage.setOutlineThickness(1);
    titleMessage.setPosition(5, 100);

    /////////////////////////////////////////////////////////////
    ///// Variable Declarations
    //////////////////////////////////////////////////////////////
    char buffer[128];
    bool isSelectionValid;
    int selectedRoll, input;
    MOVESTATUS move = MOVE_END;
    VALIDMOVETYPE validMove;
    int8_t tokenNb = -1, rollNb, unplayedRollNb, oldRollNb;
    uint8_t nbTokenEliminated, nbTokenWon, nbTokenInGame,
            roll[15] = {0}, playerNb = 0;

    player_t* player = NULL;

    /////////////////////////////////////////////////////////////
    ///// Game Loop
    //////////////////////////////////////////////////////////////
    bool isPlaying = false;
    sf::Event event;
    sf::Vector2i mousePos;

    while (RndrWindow.isOpen())
    {
        // Welcome Screen
        if (!isPlaying && move != GAME_ENDED)
        {
            titleMessage.setString("\t  Welcome to Ludo Game\nHow many players want to play: "
                                   "\n\t\t\t2 or 4 players?"
                                   "\n\n  Press a number key to select...");

            RndrWindow.clear(sf::Color::Green);
            RndrWindow.draw(titleMessage);
            RndrWindow.display();

            isSelectionValid = false;

            while (RndrWindow.waitEvent(event) && !isSelectionValid)
            {
                // "close requested" event: we close the window
                if (event.type == sf::Event::Closed)
                {
                   RndrWindow.close();
                   return 0;
                }

                // Keyboard input
                if (event.type == sf::Event::KeyPressed)
                    if (event.key.code == sf::Keyboard::Num2 ||
                        event.key.code == sf::Keyboard::Num4)
                    {
                        NbPlayerInGame = event.key.code - sf::Keyboard::Num1 + 1;

                        printf("NbPlayerInGame: %d\n", NbPlayerInGame);

                        isSelectionValid = true;
                    }
            }

            player = new player_t[NbPlayerInGame];

            Player_InitPlayers(player);

            isPlaying = true;

            // Update the display the first time
            UpdateWindowDisplay(player);
        }

        // Game
        if (isPlaying)
        {
            nbTokenInGame = Game_GetTokenStat(player[playerNb], &nbTokenEliminated, &nbTokenWon);

            // Skip move for players who have already won
            if (player[playerNb].scorecard == 0)
            {
                printf("\nPlayer %d's Move...\n", playerNb + 1);

                sprintf(buffer, "Player %d's Move...", playerNb + 1);
                PlayerMessage.setString(buffer);
                PlayerMessage.setFillColor(player[playerNb].colorAttrib);
                UpdateWindowDisplay(player);

                // reset the number of rolls to zero at the start
                rollNb = 0;

                // roll the dice
                move = RollDiceProc(roll, rollNb, player);

                // reset the number of unplayedRollNb
                unplayedRollNb = rollNb;

                // Select a score
                for (uint8_t i = 0; i < rollNb && move != MOVE_DISCARDED; i++)
                {
                    /* Change the score to 0 if that roll has been used */

                    //reset the move status
                    move = MOVE_PROCEED;

                    // if there is no token in the game, wait for a six to enter a token
                    // iterate through all the rolls and check if there is a six
                    for (uint8_t j = 0; j < rollNb && nbTokenInGame == 0; j++)
                    {
                        if (roll[j] == 6)
                        {
                            move = EnterNewTokenProc(player, playerNb);

                            roll[j] = 0;
                            unplayedRollNb--;  //one roll score has already been used
                            nbTokenInGame++;
                            nbTokenEliminated--;
                        }
                    }

                    // If the move has ended, skip all the following instructions
                    if (move == MOVE_END)
                        continue;

                    // Select and validate a score
                    printf("Dice Scores-> ");

                    InfoMessage.setString("Dice Scores-> ");

                    // Prints the available dice scores to play
                    for (uint8_t j = 0; j < rollNb; j++)
                    {
                        if (roll[j] != 0)
                        {
                            printf("%d ", roll[j]);

                            sprintf(buffer, "%d ", roll[j]);
                            InfoMessage.setString(InfoMessage.getString() + buffer);

                            // if there is only one score left, find and select that role
                            if (unplayedRollNb == 1)
                            {
                                selectedRoll = roll[j];
                                roll[j] = 0;
                                unplayedRollNb--;
                            }
                        }
                    }

                    UpdateWindowDisplay(player);
                    Sleep(1500);

                    printf("\n");

                    // Ask User to only select the score if more than one is available
                    if (unplayedRollNb > 1)
                    {
                        printf("Input: \n");

                        sprintf(buffer, " Press to select a score.");
                        InfoMessage.setString(InfoMessage.getString() + buffer);
                        UpdateWindowDisplay(player);

                        // reset the input validation flag
                        isSelectionValid = false;

                        // get the roll number
                        while (RndrWindow.waitEvent(event) && !isSelectionValid)
                        {
                            // "close requested" event: we close the window
                            if (event.type == sf::Event::Closed)
                                RndrWindow.close();

                            // Keyboard input
                            else if (event.type == sf::Event::KeyPressed)
                            {
                                if (event.key.code >= sf::Keyboard::Num1 &&
                                        event.key.code <= sf::Keyboard::Num6)
                                {
                                    selectedRoll = event.key.code - sf::Keyboard::Num1 + 1;

                                    // Validate selected score
                                    for (uint8_t j = 0; j < rollNb; j++)
                                        if (roll[j] == selectedRoll)
                                        {
                                            roll[j] = 0;
                                            unplayedRollNb--;
                                            isSelectionValid = true;

                                            printf("Selected: %d\n", selectedRoll);

                                            break;
                                        }
                                }
                            }
                        }
                    }

                    // Check if there is any valid for the selected roll
                    validMove = Game_CheckValidMoveAvailable(player[playerNb].token, selectedRoll);

                    if (validMove == NO_VALID_MOVE)
                    {
                        printf("No Move Available!\n");

                        InfoMessage.setString("No Move Available");
                        UpdateWindowDisplay(player);
                        Sleep(2000);
                        continue;     // skip all the following instruction
                    }

                    // if the selected score is 6, then the user may enter a token
                    // or move a token, given there is a token to be entered
                    if (selectedRoll == 6 && nbTokenEliminated > 0)
                    {
                        // if the only valid move is to enter a token, then don't ask the user
                        // select input = 1 to enter a token
                        if (validMove == ENTER_TOKEN_ONLY)
                            input = 1;

                        else
                        {
                            printf("Choice: 1)Enter Token 2)Move Token\n");
                            printf("Input: \n");

                            InfoMessage.setString("Select a Choice: 1)Enter Token 2)Move Token");
                            UpdateWindowDisplay(player);

                            // reset the input validation flag
                            isSelectionValid = false;

                            while (RndrWindow.waitEvent(event) && !isSelectionValid)
                            {
                                // "close requested" event: we close the window
                                if (event.type == sf::Event::Closed)
                                    RndrWindow.close();

                                // Keyboard input
                                else if (event.type == sf::Event::KeyPressed)
                                {
                                    if (event.key.code >= sf::Keyboard::Num1 &&
                                            event.key.code <= sf::Keyboard::Num2)
                                    {
                                        input = event.key.code - sf::Keyboard::Num1 + 1;
                                        isSelectionValid = true;
                                    }
                                }
                            }
                        }

                        switch (input)
                        {
                        case 1:
                            move = EnterNewTokenProc(player, playerNb);
                            nbTokenInGame++;
                            nbTokenEliminated--;
                            break;

                        case 2:
                            move = MOVE_PROCEED;
                            break;
                        }
                    }

                    /* Select the token and play the move */

                    if (nbTokenInGame && move == MOVE_PROCEED)
                    {
                        // If only one token in game, select that token
                        if (nbTokenInGame == 1)
                        {
                            tokenNb = GetSingleTokenNb(player[playerNb]);

                            move = Game_PlayerMove(player, playerNb, tokenNb, selectedRoll);
                        }

                        // If  more than one token in game, then get the user's input
                        else
                        {
                            while (1)
                            {
                                printf("Click on a Token (Score = %d)\n", selectedRoll);

                                sprintf(buffer, "Click on a Token (Score = %d)", selectedRoll);
                                InfoMessage.setString(buffer);
                                UpdateWindowDisplay(player);

                                tokenNb = -1;

                                // Get token number through mouse input
                                while (RndrWindow.waitEvent(event) && tokenNb == -1)
                                {
                                    // "close requested" event: we close the window
                                    if (event.type == sf::Event::Closed)
                                        RndrWindow.close();

                                    // Mouse Input
                                    else if (event.type == sf::Event::MouseButtonPressed)
                                        if (event.mouseButton.button == sf::Mouse::Left)
                                        {
                                            // get the mouse position
                                            mousePos = sf::Mouse::getPosition(RndrWindow);
                                            tokenNb = GetTokenNb(player[playerNb], mousePos);
                                        }
                                }

                                move = Game_PlayerMove(player, playerNb, tokenNb, selectedRoll);

                                if (move != MOVE_OUTOFSTEPS)
                                    break;

                                printf("Invalid Move!\n");

                                InfoMessage.setString("Invalid Move");
                                UpdateWindowDisplay(player);
                                Sleep(2000);
                            }
                        }

                        UpdateTokenDisplay(player, playerNb, tokenNb);

                        // If the player eliminates another token or wins, roll the dice another time
                        if (move == TOKEN_WON || move == TOKEN_ELIMINATED)
                        {
                            // Increment nbTokenWon and decrement nbTokenInGame
                            if (move == TOKEN_WON)
                            {
                                printf("Token Won! Roll the dice again!");

                                InfoMessage.setString("Token Won! Roll the dice again!");
                                nbTokenWon++;
                                nbTokenInGame--;
                            }

                            else
                            {
                                printf("Token Eliminated! Roll the dice!\n");
                                InfoMessage.setString("Token Eliminated! Roll the dice!");
                            }

                            UpdateWindowDisplay(player);
                            Sleep(2000);

                            oldRollNb = rollNb;
                            move = RollDiceProc(roll, rollNb, player);
                            unplayedRollNb = unplayedRollNb + rollNb  - oldRollNb;
                        }

                        // Check if the player has won
                        else if (move == PLAYER_WON)
                        {
                            printf("Congratulations Player %d!\n", playerNb + 1);

                            sprintf(buffer, "Congratulations Player %d!\n", playerNb + 1);
                            InfoMessage.setString(buffer);
                            UpdateWindowDisplay(player);
                            Sleep(3000);
                        }
                    }
                }
            }

            // Wrap Player Number
            playerNb++;

            if (playerNb == NbPlayerInGame)
                playerNb = 0;

            if (move == GAME_ENDED)
                isPlaying = false;
        }

        // Exit Screen
        if (!isPlaying && move == GAME_ENDED)
        {
            titleMessage.setString("Scorecard:\n"
                                   "Name         Position\n"
                                   "------------------------\n");
            for (uint8_t i = 0; i < NbPlayerInGame; i++)
            {
                sprintf(buffer, "Player %d\t->\t%d\n", i + 1, player[i].scorecard);
                titleMessage.setString(titleMessage.getString() + buffer);
            }

            titleMessage.setString(titleMessage.getString() +
                                   "\nPress any key to exit ... ");

            RndrWindow.clear(sf::Color::Green);
            RndrWindow.draw(titleMessage);
            RndrWindow.display();

            if (RndrWindow.waitEvent(event))
                // "close requested" event: we close the window
                if (event.type == sf::Event::Closed ||
                    event.type == sf::Event::KeyPressed)
                    RndrWindow.close();
        }
    }

    delete [] player;
    player = NULL;

    return 0;
}
