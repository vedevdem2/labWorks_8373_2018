#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
using namespace std;

const unsigned short    frameW = 341,
                        frameH = 76,
                        frameX = 227,
                        frameY = 167,
                        dY = 5,
                        tubeW = 137,
                        tubeH = 921,
                        tubeHalfY = tubeH * 3 / 8, 
                        groundOffset = 23,
                        groundW = 1366 + groundOffset,
                        groundH = 77,
                        groundSpeed = 3,
                        deerW = 80,
                        deerH = 80,
                        freeSpaceY = 15,
                        tubeSpeed = 10,
                        tubeBonusSpeed = 20,
                        deerOffsetX = 5,
                        deerStartX = 50,
                        textCoords = 25,
                        fontSize = 100,
                        ScreenW = 1366,
                        ScreenH = 768;

const char  *defGr_p = "textures/ground.png",
            *bonusGr_p = "textures/groundBonus.png";

enum ground {defGr, bonusGr};

enum tubeType {def, health, bonus};

enum menuType {start, inGame, death};

enum frame {FrameBlue, FrameWhite};

enum menuExitCode {quitApp, gameStart};

struct text
{
    SDL_Rect rect;
    SDL_Color color;
    SDL_Texture *texture;
    short fontSize;
};

struct obj
{
    SDL_Texture *texture;
    SDL_Rect rect;
};

struct animated_obj
{
    SDL_Texture *texture;
    SDL_Rect srcrect;
    SDL_Rect dstrect;
};


SDL_Texture* renderText(const string &text, SDL_Color color, SDL_Renderer *renderer, short fontSize)
{
    TTF_Font *font = TTF_OpenFont("ttf/sus.ttf", fontSize);
    SDL_Surface *surface = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    TTF_CloseFont(font);
    return texture;
}

tubeType generator()
{
    int random_number = rand() % 100;
    if (random_number < 10)
    {
        return health;
    } 
    else
    {    
        if (random_number < 20)
        {
            return bonus;
        }
        else
        {
            return def;
        }
    }
}

void bonusLvl(unsigned int &sessionHighscore, unsigned int move, unsigned int deerX, unsigned int deerAnimationSize, unsigned int deerTextureW, bool& gameOver, bool& gamehappening, bool& mainMenu, const Uint8 *keyboardState, animated_obj deer, unsigned int& score, SDL_Renderer* ren, obj *ground, text scoreText)
{
    SDL_Texture* bg = IMG_LoadTexture(ren, "textures/2newbg.png");
    
    obj tube1;
    tube1.texture = IMG_LoadTexture(ren, "textures/newBonusPipe.png");
    tube1.rect.x = ScreenW;
    tube1.rect.y = -rand() % groundH * 3;
    tube1.rect.w = tubeW;
    tube1.rect.h = tubeH;

    obj tube2;
    tube2.texture = IMG_LoadTexture(ren, "textures/newBonusPipe.png");
    tube2.rect.x = ScreenW;
    tube2.rect.y = tube1.rect.y;
    tube2.rect.w = tubeW;
    tube2.rect.h = tubeH;

    unsigned int    bonusLives = 1,
                    animationTimer = SDL_GetTicks();

    bool    bonus = true,
            pipe2generated = false,
            pipe1_crossed_centre = false,
            pipe2_crossed_centre = false,
            pipe3_crossed_centre = false,
            pipe1_crossed = false,
            pipe2_crossed = false,
            pipe3_crossed = false,
            pipe1_failed = false,
            pipe2_failed = false,
            pipe3_failed = false,
                _pipe2generated = false;
    SDL_Event bonusE;
    while(bonus)
    {
        SDL_RenderClear(ren);
        //event handler
        while (SDL_PollEvent(&bonusE))
        {
            if (bonusE.type == SDL_KEYDOWN)
            {
                if(keyboardState[SDL_SCANCODE_ESCAPE])
                {
                    gameOver = true;
                    gamehappening = false;
                    mainMenu = true;
                    bonus = false;
                }
                if(keyboardState[SDL_SCANCODE_SPACE])
                {
                    move = 15;
                }
            }
        }
        ground[defGr].rect.x -= groundSpeed;
        if (ground[defGr].rect.x <= -groundOffset)
        {
            ground[defGr].rect.x = 0;
        }
        //ground physics
        ground[bonusGr].rect.x -= groundSpeed;
        if (ground[bonusGr].rect.x <= -groundOffset)
        {
            ground[bonusGr].rect.x = 0;
        }
        //scoreText.rect.w = 25*numberOfDigits(score);
        scoreText.rect.x = (ScreenW - scoreText.rect.w) / 2;
        //deer animation
        if (SDL_GetTicks() - animationTimer > 200)
        {
            animationTimer = SDL_GetTicks();
            deer.srcrect.x += deerAnimationSize;
            if (deer.srcrect.x >= deerTextureW)
            {
                deer.srcrect.x = 0;
            }
        }
        //deer acceleration and physics
        move--;
        deer.dstrect.y -= move;
        //collision with the sky or ground
        if (deer.dstrect.y >= ScreenH - deer.dstrect.h - groundH)// - deerAnimationSize)
        {
            deer.dstrect.y = ScreenH - deer.dstrect.h - groundH;// - deerAnimationSize;
            move = 0;
        }
        if (deer.dstrect.y <= 0 - freeSpaceY)
        {
            deer.dstrect.y = 0 - freeSpaceY;
            move = 0;
        }
        //tube1 centre cross check
        if (tube1.rect.x <= (ScreenW - tube1.rect.w) / 2 && !pipe1_crossed_centre && !_pipe2generated)
        {
            if (!pipe2generated)
                pipe2generated = true;
            pipe1_crossed_centre = true;
        }
        //
        if (tube1.rect.x <= (deerX - (tube1.rect.w -deer.dstrect.w)) && !pipe1_crossed)
        {
            score += 2;
            if (score > sessionHighscore)
            {
                sessionHighscore = score;
            }
            pipe1_crossed = true;
        } 
        //tubes physics
        tube1.rect.x -= tubeBonusSpeed;
        if (!pipe1_failed && tube1.rect.x < deerX + deer.dstrect.w  && tube1.rect.x > deerX - tube1.rect.w && (deer.dstrect.y <= tube1.rect.y + tubeHalfY - freeSpaceY || deer.dstrect.y >= tube1.rect.y + tube1.rect.h - tubeHalfY - deer.dstrect.h))
        {
            bonusLives--;
            if (bonusLives == 0)
            {
                bonus = false;
            }
            pipe1_failed = true;
        }

        if (tube1.rect.x <= -tube1.rect.w)
        {
            tube1.rect.x = ScreenW;
            tube1.rect.y = -rand() % groundH * 3;

            pipe1_crossed_centre = false;
            pipe1_crossed = false;
            pipe1_failed = false;
        }
        if (tube2.rect.x <= (ScreenW - tube2.rect.w) / 2 && !pipe2_crossed_centre)
        {
            pipe2_crossed_centre = true;
        }
        if (tube2.rect.x <= (deerX - (tube2.rect.w - deer.dstrect.w)) && !pipe2_crossed)
        {
            score += 2;
            if (score > sessionHighscore)
            {
                sessionHighscore = score;
            }
            pipe2_crossed = true;
        } 
        if (pipe2generated)
        {
            tube2.rect.x = ScreenW;
            tube2.rect.y = -rand() % groundH * 3;
            pipe2generated = false;
            _pipe2generated = true;
        }
        tube2.rect.x -= tubeBonusSpeed;
        if (!pipe2_failed && tube2.rect.x < deerX + deer.dstrect.w  && tube2.rect.x > deerX - tube2.rect.w && (deer.dstrect.y <= tube2.rect.y + tubeHalfY - freeSpaceY || deer.dstrect.y >= tube2.rect.y + tube2.rect.h - tubeHalfY - deer.dstrect.h))
        {
            bonusLives--;
            if (bonusLives == 0)
            {
                bonus = false;
            }
            pipe2_failed = true;
        }
        if (tube2.rect.x <= -tube2.rect.w)
        {
            tube2.rect.x = ScreenW;
            tube2.rect.y = -rand() % groundH * 3;

            pipe2_crossed_centre = false;
            pipe2_crossed = false;
            pipe2_failed = false;
        }
        scoreText.texture = renderText(to_string(score), scoreText.color, ren, scoreText.fontSize);
        SDL_RenderCopy(ren, bg, NULL, NULL);
        SDL_RenderCopy(ren, tube1.texture, NULL, &tube1.rect);
        SDL_RenderCopy(ren, tube2.texture, NULL, &tube2.rect);
        SDL_RenderCopy(ren, deer.texture, &deer.srcrect, &deer.dstrect);
        SDL_RenderCopy(ren, ground[bonusGr].texture, NULL, &ground[bonusGr].rect);
        SDL_RenderCopy(ren, scoreText.texture, NULL, &scoreText.rect);
        SDL_RenderPresent(ren);
    }
    SDL_DestroyTexture(bg);
    SDL_DestroyTexture(tube1.texture);
    SDL_DestroyTexture(tube2.texture);
}

void gameLoop(unsigned int &sessionHighscore, SDL_Renderer *ren, const Uint8 *keyboardState, bool& mainMenu)
{
    SDL_Texture *gameBg = IMG_LoadTexture(ren, "textures/1newbg.jpg");

    const unsigned short    tubeNumber = 3;

    SDL_Texture *tubeTexture[tubeNumber];
    tubeTexture[def] = IMG_LoadTexture(ren, "textures/1newPipe.png");
    tubeTexture[health] = IMG_LoadTexture(ren, "textures/2newPipe.png");
    tubeTexture[bonus] = IMG_LoadTexture(ren, "textures/3newPipe.png");

    obj ground[2];
    ground[defGr].texture = IMG_LoadTexture(ren, defGr_p);
    ground[defGr].rect.x = 0;
    ground[defGr].rect.y = ScreenH - groundH;
    ground[defGr].rect.w = groundW;
    ground[defGr].rect.h = groundH;
    ground[bonusGr].texture = IMG_LoadTexture(ren, bonusGr_p);
    ground[bonusGr].rect.x = 0;
    ground[bonusGr].rect.y = ScreenH - groundH;
    ground[bonusGr].rect.w = groundW;
    ground[bonusGr].rect.h = groundH;

    obj tube1;
    short tube1type = generator();
    tube1.texture = tubeTexture[tube1type];
    tube1.rect.x = ScreenW;
    tube1.rect.y = -rand() % groundH * 3;
    tube1.rect.w = tubeW;
    tube1.rect.h = tubeH;

    obj tube2;
    short tube2type = generator();
    tube2.texture = tubeTexture[tube1type];
    tube2.rect.x = ScreenW;
    tube2.rect.y = tube1.rect.y;
    tube2.rect.w = tubeW;
    tube2.rect.h = tubeH;

    obj tube3;
    short tube3type = generator();
    tube3.texture = tubeTexture[tube3type];
    tube3.rect.x = ScreenW;
    tube3.rect.y = -rand() % groundH * 3;
    tube3.rect.w = tubeW;
    tube3.rect.h = tubeH;

    int deerTextureW, deerTextureH, treeW, treeH;

    animated_obj tree;
    tree.texture = IMG_LoadTexture(ren, "textures/christmas_tree_animation.png");

    SDL_QueryTexture(tree.texture, NULL, NULL, &treeW, &treeH);
    short   treeAnimationW = treeW / 2,
            treeAnimationH = treeH / 2;
    tree.dstrect.x = ScreenW / 4;
    tree.dstrect.y = ScreenH / 2.4;
    tree.dstrect.w = treeAnimationW / 2;
    tree.dstrect.h = treeAnimationH / 2;
    tree.srcrect.x = 0;
    tree.srcrect.y = 0;
    tree.srcrect.w = treeAnimationW;
    tree.srcrect.h = treeAnimationH;

    animated_obj deer;
    deer.texture = IMG_LoadTexture(ren, "textures/deerMale.png");
    SDL_QueryTexture(deer.texture, NULL, NULL, &deerTextureW, &deerTextureH);
    short deerAnimationSize = deerTextureW / 5;
    deer.dstrect.x = deerStartX;
    deer.dstrect.y = ScreenH - deerH - groundH;
    deer.dstrect.w = deerW;
    deer.dstrect.h = deerH;
    deer.srcrect.x = 0;
    deer.srcrect.y = 0;
    deer.srcrect.w = deerAnimationSize;
    deer.srcrect.h = deerAnimationSize;

    animated_obj santa;
    

    bool    gameOver = true,
            gamehappening = true,
            start = false,
            jumpOnStartHappend = false,
            pipe2generated = false,
            pipe1_crossed_centre = false,
            pipe2_crossed_centre = false,
            pipe3_crossed_centre = false,
            pipe1_crossed = false,
            pipe2_crossed = false,
            pipe3_crossed = false,
            pipe1_failed = false,
            pipe2_failed = false,
            pipe3_failed = false,
                _pipe2generated = false,
            bonushappend = false;

    short   move = 0,
            deerX,
            lives = 1;

    unsigned int    timer = SDL_GetTicks(),
                    moveTimer = SDL_GetTicks(),
                    animationTimer = SDL_GetTicks(),
                    treeTimer = SDL_GetTicks(),
                    score = 0;

    text scoreText;
    scoreText.rect.y = textCoords;
    scoreText.color = {0, 255, 255, 25};
    scoreText.fontSize = fontSize;

    text livesText;
    livesText.rect.x = textCoords;
    livesText.rect.y = textCoords;
    livesText.color = {255, 0, 0, 255};
    livesText.fontSize = fontSize;

    text SH;
    SH.rect.y = textCoords;
    SH.color = {0, 255, 0, 255};
    SH.fontSize = fontSize;

    SDL_Event game;
    while (gamehappening)
    {
        SDL_RenderClear(ren);
        //event handler
        while (SDL_PollEvent(&game))
        {
            if (game.type == SDL_KEYDOWN)
            {
                if(keyboardState[SDL_SCANCODE_ESCAPE])
                {
                    return;
                    //gamehappening = false;
                    //mainMenu = true;
                }
                if(keyboardState[SDL_SCANCODE_SPACE])
                {
                    start = true;
                    deer.srcrect.x = 0;
                    timer = SDL_GetTicks();
                    deer.srcrect.y += deerTextureW / 5;
                    if(deer.srcrect.y == deerTextureW)
                    {
                        deer.srcrect.y = 0;
                    }
                }
                if (game.type == SDL_QUIT)
                {
                    gamehappening = false;
                    mainMenu = false;
                }
            }
        }
        //tree animation
        if (SDL_GetTicks() - treeTimer > 100)
        {
            treeTimer = SDL_GetTicks();
            tree.srcrect.x += treeAnimationW;
            if (tree.srcrect.x >= treeW)
            {
                tree.srcrect.y += treeAnimationH;
                tree.srcrect.x = 0;
            }
            if (tree.srcrect.y >= treeH)
            {
                tree.srcrect.y = 0;
            }
        }
        //deer start animation
        if (SDL_GetTicks() - timer > 500 + rand() % 1500)
        {
            timer = SDL_GetTicks();
            deer.srcrect.x = (rand() % 5)*deerAnimationSize;
            if (deer.srcrect.x >= deerTextureW)
            {
                deer.srcrect.x = 0;
            }
        }
        while (start)
        {
            SDL_RenderClear(ren);
            //event handler
            while (SDL_PollEvent(&game))
            {
                if (game.type == SDL_KEYDOWN)
                {
                    if(keyboardState[SDL_SCANCODE_ESCAPE])
                    {
                        return;/*
                        gamehappening = false;
                        start = false;
                        gamehappening = false;
                        start = false;*/
                    }
                    if(keyboardState[SDL_SCANCODE_SPACE])
                    {
                        deer.srcrect.y = 2 * deerAnimationSize;
                        move = 20;
                        jumpOnStartHappend = true;
                        mainMenu = true;
                    }
                }
                if (game.type == SDL_QUIT)
                {
                    start = false;
                    gamehappening = false;
                }
            }
            //ground physics
            ground[defGr].rect.x -= groundSpeed;
            if (ground[defGr].rect.x <= -groundOffset)
            {
                ground[defGr].rect.x = 0;
            }
            //stop jump animation and start fly-run animation
            
            if (SDL_GetTicks() - animationTimer > 200)
            {
                animationTimer = SDL_GetTicks();
                deer.srcrect.x += deerAnimationSize;
                if (deer.srcrect.x >= deerTextureW)
                {
                    deer.srcrect.x = 0;
                }
            }
            
            //deer acceleration and physics
                move--;
        
            deer.dstrect.y -= move;
            deer.dstrect.x += deerOffsetX;
            deer.srcrect.y = 2 * deerAnimationSize;//change deer animation line (in sprite sheet) to the 2nd
            //collision with sky or ground
            if (deer.dstrect.y >= ScreenH - deer.dstrect.h - groundH)// - deerAnimationSize)
            {
                deer.dstrect.y = ScreenH - deer.dstrect.h - groundH;// - deerAnimationSize;
                move = 0;
            }
            if (deer.dstrect.y <= 0 - freeSpaceY)// - deerAnimationSize)
            {
                deer.dstrect.y = 0 - freeSpaceY;// - deerAnimationSize;
                move = 0;
            }
            //start timer
            if(SDL_GetTicks() - timer > 500 + rand() % 500)
            {
                timer = SDL_GetTicks();
                start = false;
                gameOver = false;
                deerX = deer.dstrect.x;
            }
            SDL_RenderCopy(ren, gameBg, NULL, NULL);
            SDL_RenderCopy(ren, deer.texture, &deer.srcrect, &deer.dstrect);
            SDL_RenderCopy(ren, ground[defGr].texture, NULL, &ground[defGr].rect);
            SDL_RenderPresent(ren);
        }
        
        while (!gameOver)
        {
            SDL_RenderClear(ren);
            if(bonushappend)
            {
                SDL_Delay(200);
                bonushappend = false;
            }
            //event handler
            while (SDL_PollEvent(&game))
            {
                if (game.type == SDL_KEYDOWN)
                {
                    if(keyboardState[SDL_SCANCODE_ESCAPE])
                    {
                        gameOver = true;
                        gamehappening = false;
                        mainMenu = true;
                    }
                    if(keyboardState[SDL_SCANCODE_SPACE])
                    {
                        move = 15;
                    }
                }
            }
            ground[defGr].rect.x -= groundSpeed;
            if (ground[defGr].rect.x <= -groundOffset)
            {
                ground[defGr].rect.x = 0;
            }

            //scoreText.rect.w = 25*numberOfDigits(score);
            //deer animation
            if (SDL_GetTicks() - animationTimer > 200)
            {
                animationTimer = SDL_GetTicks();
                deer.srcrect.x += deerAnimationSize;
                if (deer.srcrect.x >= deerTextureW)
                {
                    deer.srcrect.x = 0;
                }
            }
            //deer acceleration and physics
            move--;
            deer.dstrect.y -= move;
            //collision with the sky or ground
            if (deer.dstrect.y >= ScreenH - deer.dstrect.h - groundH)// - deerAnimationSize)
            {
                deer.dstrect.y = ScreenH - deer.dstrect.h - groundH;// - deerAnimationSize;
                move = 0;
            }
            if (deer.dstrect.y <= 0 - freeSpaceY)
            {
                deer.dstrect.y = 0 - freeSpaceY;
                move = 0;
            }
            //tube1 centre cross check
            if (tube1.rect.x <= (ScreenW - tube1.rect.w) / 2 && !pipe1_crossed_centre && !_pipe2generated)
            {
                if (!pipe2generated)
                    pipe2generated = true;
                pipe1_crossed_centre = true;
            }
            //
            if (tube1.rect.x <= (deerX - (tube1.rect.w -deer.dstrect.w)) && !pipe1_crossed)
            {
                score++;
                if (score > sessionHighscore)
                {
                    sessionHighscore = score;
                }
                if (tube1type == bonus)
                {
                    bonusLvl(sessionHighscore, move, deerX, deerAnimationSize, deerTextureW, gameOver, gamehappening, mainMenu, keyboardState, deer, score, ren, ground, scoreText);
                    bonushappend = true;
                }
                if (tube1type == health)
                {
                    lives++;
                }
                pipe1_crossed = true;
            }
            //tubes physics
            tube1.rect.x -= tubeSpeed;
            if (!pipe1_failed && tube1.rect.x < deerX + deer.dstrect.w && tube1.rect.x > deerX - tube1.rect.w && (deer.dstrect.y <= tube1.rect.y + tubeHalfY - freeSpaceY || deer.dstrect.y >= tube1.rect.y + tube1.rect.h - tubeHalfY - deer.dstrect.h))
            {
                lives--;
                if (lives == 0)
                {
                    gameOver = true;
                    gamehappening = false;
                }
                pipe1_failed = true;
            }

            if (tube1.rect.x <= -tube1.rect.w)
            {
                tube1type = generator();
                tube1.texture = tubeTexture[tube1type];
                tube1.rect.x = ScreenW;
                tube1.rect.y = -rand() % groundH * 3;

                pipe1_crossed_centre = false;
                pipe1_crossed = false;
                pipe1_failed = false;
            }
            if (tube2.rect.x <= (ScreenW - tube2.rect.w) / 2 && !pipe2_crossed_centre)
            {
                pipe2_crossed_centre = true;
            }
            if (tube2.rect.x <= (deerX - (tube2.rect.w - deer.dstrect.w)) && !pipe2_crossed)
            {
                score++;
                if (score > sessionHighscore)
                {
                    sessionHighscore = score;
                }
                if (tube2type == bonus)
                {
                    bonusLvl(sessionHighscore ,move, deerX, deerAnimationSize, deerTextureW, gameOver, gamehappening, mainMenu, keyboardState, deer, score, ren, ground, scoreText);
                    bonushappend = true;
                }
                if (tube2type == health)
                {
                    lives++;
                }
                pipe2_crossed = true;
            } 
            if (pipe2generated)
            {
                tube2type = generator();
                tube2.texture = tubeTexture[tube2type];
                tube2.rect.x = ScreenW;
                tube2.rect.y = -rand() % groundH * 3;
                pipe2generated = false;
                _pipe2generated = true;
            }
            tube2.rect.x -= tubeSpeed;
            if (!pipe2_failed && tube2.rect.x < deerX + deer.dstrect.w  && tube2.rect.x > deerX - tube2.rect.w && (deer.dstrect.y <= tube2.rect.y + tubeHalfY - freeSpaceY || deer.dstrect.y >= tube2.rect.y + tube2.rect.h - tubeHalfY - deer.dstrect.h))
            {
                lives--;
                if (lives == 0)
                {
                    gameOver = true;
                    gamehappening = false;
                }
                pipe2_failed = true;
            }
            if (tube2.rect.x <= -tube2.rect.w)
            {
                tube2type = generator();
                tube2.texture = tubeTexture[tube2type];
                tube2.rect.x = ScreenW;
                tube2.rect.y = -rand() % groundH * 3;

                pipe2_crossed_centre = false;
                pipe2_crossed = false;
                pipe2_failed = false;
            }
            scoreText.texture = renderText(to_string(score), scoreText.color, ren, scoreText.fontSize);
            SDL_QueryTexture(scoreText.texture, nullptr, nullptr, &scoreText.rect.w, &scoreText.rect.h);
            scoreText.rect.x = (ScreenW - scoreText.rect.w) / 2;
            
            livesText.texture = renderText(to_string(lives), livesText.color, ren, livesText.fontSize);
            SDL_QueryTexture(livesText.texture, nullptr, nullptr, &livesText.rect.w, &livesText.rect.h);

            SH.texture = renderText(to_string(sessionHighscore), SH.color, ren, SH.fontSize);
            SDL_QueryTexture(SH.texture, nullptr, nullptr, &SH.rect.w, &SH.rect.h);
            SH.rect.x = ScreenW - SH.rect.w - textCoords;

            SDL_RenderCopy(ren, gameBg, NULL, NULL);
            SDL_RenderCopy(ren, tube1.texture, NULL, &tube1.rect);
            SDL_RenderCopy(ren, tube2.texture, NULL, &tube2.rect);
            SDL_RenderCopy(ren, deer.texture, &deer.srcrect, &deer.dstrect);
            SDL_RenderCopy(ren, ground[defGr].texture, NULL, &ground[defGr].rect);
            SDL_RenderCopy(ren, scoreText.texture, NULL, &scoreText.rect);
            SDL_RenderCopy(ren, livesText.texture, NULL, &livesText.rect);
            SDL_RenderCopy(ren, SH.texture, NULL, &SH.rect);
            SDL_RenderPresent(ren);
        }
        SDL_RenderCopy(ren, gameBg, NULL, NULL);
        SDL_RenderCopy(ren, tree.texture, &tree.srcrect, &tree.dstrect);
        SDL_RenderCopy(ren, tube1.texture, NULL, &tube1.rect);
        SDL_RenderCopy(ren, tube2.texture, NULL, &tube2.rect);
        SDL_RenderCopy(ren, deer.texture, &deer.srcrect, &deer.dstrect);
        SDL_RenderCopy(ren, ground[defGr].texture, NULL, &ground[defGr].rect);
        SDL_RenderPresent(ren);
    }
    SDL_DestroyTexture(gameBg);
    SDL_DestroyTexture(tube1.texture);
    SDL_DestroyTexture(tube2.texture);
    SDL_DestroyTexture(deer.texture);
    SDL_DestroyTexture(tree.texture);
    SDL_DestroyTexture(scoreText.texture);
    SDL_DestroyTexture(ground[bonusGr].texture);
    SDL_DestroyTexture(ground[defGr].texture);
}

void settingsMenu(bool& mainMenu, SDL_Renderer *ren, SDL_Texture *menuBg, const Uint8 *keyboardState, short frameX, short frameY, short frameW, short frameH, short dY, obj *frame, Mix_Music *avaria, Mix_Music *blestyashie, Mix_Music *digi, Mix_Music *elka, Mix_Music *verka)
{
    enum blue {CTBS, AvariaBlue, BlestyashieBlue, DigiBlue, ElkaBlue, VerkaBlue};
    enum white {AvariaWhite, BlestyashieWhite, DigiWhite, ElkaWhite, VerkaWhite};
    enum enterCase {Avaria, Blestyashie, Digi, Elka, Verka};


    const short blues = 6,
                whites = 5;

    obj blue[blues];
    obj white[whites];

    frame[FrameBlue].rect = {frameX, 0/*inicialization in da cicle*/, frameW, frameH};
    frame[FrameWhite].rect = {frameX, frameY, frameW, frameH};

    blue[CTBS].texture = IMG_LoadTexture(ren, "textures/menu/textCTBS.png");    //Choose the background song
    blue[CTBS].rect.w = frameW * 12 / 5;
    blue[CTBS].rect.h = frameH * 4 / 3;
    blue[CTBS].rect.x = frameX - 3*dY;
    blue[CTBS].rect.y = blue[CTBS].rect.h / 2;

    //avaria text
    blue[AvariaBlue].texture = IMG_LoadTexture(ren, "textures/menu/textMusicAvariaBlue.png");
    white[AvariaWhite].texture = IMG_LoadTexture(ren, "textures/menu/textMusicAvariaWhite.png");
    blue[AvariaBlue].rect = white[AvariaWhite].rect = {frameX, frameY, frameW, frameH};

    //blestyashie text
    blue[BlestyashieBlue].texture = IMG_LoadTexture(ren, "textures/menu/textMusicBlestyashieBlue.png");
    white[BlestyashieWhite].texture = IMG_LoadTexture(ren, "textures/menu/textMusicBlestyashieWhite.png");
    blue[BlestyashieBlue].rect = white[BlestyashieWhite].rect = {frameX, frameY + frameH - dY, frameW, frameH};
    
    //digi text
    blue[DigiBlue].texture = IMG_LoadTexture(ren, "textures/menu/textMusicDigiBlue.png");
    white[DigiWhite].texture = IMG_LoadTexture(ren, "textures/menu/textMusicDigiWhite.png");
    blue[DigiBlue].rect = white[DigiWhite].rect = {frameX, frameY + 2*(frameH-dY), frameW, frameH};

    //elka text
    blue[ElkaBlue].texture = IMG_LoadTexture(ren, "textures/menu/textMusicElkaBlue.png");
    white[ElkaWhite].texture = IMG_LoadTexture(ren, "textures/menu/textMusicElkaWhite.png");
    blue[ElkaBlue].rect = white[ElkaWhite].rect = {frameX, frameY + 3*(frameH-dY), frameW, frameH};

    //verka text
    blue[VerkaBlue].texture = IMG_LoadTexture(ren, "textures/menu/textMusicVerkaBlue.png");
    white[VerkaWhite].texture = IMG_LoadTexture(ren, "textures/menu/textMusicVerkaWhite.png");
    blue[VerkaBlue].rect = white[VerkaWhite].rect = {frameX, frameY + 4*(frameH-dY), frameW, frameH};
    
    bool settingsMenu = true;
    short   buttons = 5,
            whiteTextPos = 0;
    SDL_Event SME;  //settings menu event

    while (settingsMenu)
    {
        SDL_RenderClear(ren);
        SDL_RenderCopy(ren, menuBg, NULL, NULL);
        SDL_RenderCopy(ren, blue[CTBS].texture, NULL, &blue[CTBS].rect);
        frame[FrameBlue].rect.y = frameY;
        while(SDL_PollEvent(&SME))
        {
            if(SME.type == SDL_KEYDOWN)
            {
                if (keyboardState[SDL_SCANCODE_ESCAPE])
                {
                    settingsMenu = false;
                }
                if (keyboardState[SDL_SCANCODE_DOWN]||keyboardState[SDL_SCANCODE_S])
                {
                    if (frame[FrameWhite].rect.y <= frameY + 3*(frameH - dY))
                    {
                        frame[FrameWhite].rect.y += frame[FrameWhite].rect.h - dY;
                        whiteTextPos++;
                    }
                    else
                    {
                        whiteTextPos = 0;
                        frame[FrameWhite].rect.y = frameY;
                    }
                }
                if (keyboardState[SDL_SCANCODE_W]||keyboardState[SDL_SCANCODE_UP])
                {   
                    if (frame[FrameWhite].rect.y >= frameY + frameH - dY)
                    {
                        frame[FrameWhite].rect.y -= frame[FrameWhite].rect.h -dY;
                        whiteTextPos--;
                    }
                    else
                    {
                        frame[FrameWhite].rect.y = frameY + 4*(frameH - dY);
                        whiteTextPos = buttons - 1;
                    }
                }
                if (keyboardState[SDL_SCANCODE_RETURN]||keyboardState[SDL_SCANCODE_KP_ENTER])
                {
                    Mix_HaltMusic();
                    switch(whiteTextPos)
                    {
                    case(Avaria):
                        Mix_PlayMusic(avaria , -1);
                        break;
                    case(Blestyashie):
                        Mix_PlayMusic(blestyashie , -1);
                        break;
                    case(Digi):
                        Mix_PlayMusic(digi , -1);
                        break;
                    case(Elka):
                        Mix_PlayMusic(elka , -1);
                        break;
                    case(Verka):
                        Mix_PlayMusic(verka , -1);
                    }
                }
            }/*
            if (SME.type == SDL_QUIT)
            {
                settingsMenu = false;
                mainMenu = false;
            }*/
        }
        /*for (int i = 0; i < buttons; i++)
        {
            SDL_RenderCopy(ren, frame[FrameBlue].texture, NULL, &frame[FrameBlue].rect);
            frame[FrameBlue].rect.y += frame[FrameBlue].rect.h - dY;
        }*/
        SDL_RenderCopy(ren, blue[AvariaBlue].texture, NULL, &blue[AvariaBlue].rect);
        SDL_RenderCopy(ren, blue[BlestyashieBlue].texture, NULL, &blue[BlestyashieBlue].rect);
        SDL_RenderCopy(ren, blue[DigiBlue].texture, NULL, &blue[DigiBlue].rect);
        SDL_RenderCopy(ren, blue[ElkaBlue].texture, NULL, &blue[ElkaBlue].rect);
        SDL_RenderCopy(ren, blue[VerkaBlue].texture, NULL, &blue[VerkaBlue].rect);
        SDL_RenderCopy(ren, white[whiteTextPos].texture, NULL, &white[whiteTextPos].rect);
        //SDL_RenderCopy(ren, frame[FrameWhite].texture, NULL, &frame[FrameWhite].rect);
        SDL_RenderPresent(ren);
    }

    for (int i = 0; i < blues; i++)
        SDL_DestroyTexture(blue[i].texture);
    for (int i = 0; i < whites; i++)
        SDL_DestroyTexture(white[i].texture);
}

menuExitCode startMenu(const Uint8 *keyboardState, bool& mainMenu, SDL_Renderer *ren, Mix_Music *avaria, Mix_Music *blestyashie, Mix_Music *digi, Mix_Music *elka, Mix_Music *verka)
{
    ////
    SDL_Texture *menuBg = IMG_LoadTexture(ren, "textures/anime.jpg");

    enum blue {WTTCDG, PlayBlue, SettingsBlue, QuitBlue}; //WTTCDG = Welcome to the christmas deer game
    enum white {PlayWhite, SettingsWhite, QuitWhite};
    enum enterCase {game, settings, quit};

    const short blues = 4,
                whites = 3,
                frames = 2;

    obj blue[blues];
    obj white[whites];
    obj frame[frames];

    
    //blue frame
    frame[FrameBlue].texture = IMG_LoadTexture(ren, "textures/menu/buttonFrameBlue.png");
    frame[FrameBlue].rect = {frameX, 0/*inicialization in da cicle*/, frameW, frameH};

    //white frame
    frame[FrameWhite].texture = IMG_LoadTexture(ren, "textures/menu/buttonFrameWhite.png");
    frame[FrameWhite].rect = {frameX, frameY, frameW, frameH};

    //head text
    blue[WTTCDG].texture = IMG_LoadTexture(ren, "textures/menu/textWTTCDG.png");
    blue[WTTCDG].rect.w = ScreenW * 4 / 5;
    blue[WTTCDG].rect.h = frameH * 2 / 3;
    blue[WTTCDG].rect.x = (ScreenW - blue[WTTCDG].rect.w)/2;
    blue[WTTCDG].rect.y = blue[WTTCDG].rect.h;

    //play text
    blue[PlayBlue].texture = IMG_LoadTexture(ren, "textures/menu/textPlayBlue.png");
    white[PlayWhite].texture = IMG_LoadTexture(ren, "textures/menu/textPlayWhite.png");
    blue[PlayBlue].rect = white[PlayWhite].rect = {frameX, frameY, frameW, frameH};

    //settings text
    blue[SettingsBlue].texture = IMG_LoadTexture(ren, "textures/menu/textSettingsBlue.png");
    white[SettingsWhite].texture = IMG_LoadTexture(ren, "textures/menu/textSettingsWhite.png");
    blue[SettingsBlue].rect = white[SettingsWhite].rect = {frameX, frameY + frameH - dY, frameW, frameH};
    
    //quit text
    blue[QuitBlue].texture = IMG_LoadTexture(ren, "textures/menu/textQuitBlue.png");
    white[QuitWhite].texture = IMG_LoadTexture(ren, "textures/menu/textQuitWhite.png");
    blue[QuitBlue].rect = white[QuitWhite].rect = {frameX, frameY + 2*(frameH-dY), frameW, frameH};
    //

    //
    short   buttons = 3,
            whiteTextPos = 0;

    SDL_Event menuEvent;

    while (mainMenu)
    {
        SDL_ShowCursor(SDL_DISABLE);
        SDL_RenderClear(ren);
        frame[FrameBlue].rect.y = frameY;
        while(SDL_PollEvent(&menuEvent))
        {
            if(menuEvent.type == SDL_KEYDOWN)
            {
                if (keyboardState[SDL_SCANCODE_DOWN]||keyboardState[SDL_SCANCODE_S])
                {
                    if (frame[FrameWhite].rect.y <= frameY + frameH - dY)
                    {
                        frame[FrameWhite].rect.y += frame[FrameWhite].rect.h - dY;
                        whiteTextPos++;
                    }
                    else
                    {
                        whiteTextPos = 0;
                        frame[FrameWhite].rect.y = frameY;
                    }
                }
                if (keyboardState[SDL_SCANCODE_W]||keyboardState[SDL_SCANCODE_UP])
                {   
                    if (frame[FrameWhite].rect.y >= frameY + frameH - dY)
                    {
                        frame[FrameWhite].rect.y -= frame[FrameWhite].rect.h -dY;
                        whiteTextPos--;
                    }
                    else
                    {
                        frame[FrameWhite].rect.y = frameY + 2*(frameH - dY);
                        whiteTextPos = buttons - 1;
                    }
                }
                if (keyboardState[SDL_SCANCODE_RETURN]||keyboardState[SDL_SCANCODE_KP_ENTER])
                {
                    switch(whiteTextPos)
                    {
                    case(game):
                        return gameStart;
                        break;
                    case(settings):
                        settingsMenu(mainMenu, ren, menuBg, keyboardState, frameX, frameY, frameW, frameH, dY, frame, avaria, blestyashie, digi, elka, verka);
                        whiteTextPos = 0;
                        frame[FrameWhite].rect.y = frameY;
                        break;
                    case(quit):
                        return quitApp;
                    }
                }
            }
        }
        SDL_RenderCopy(ren, menuBg, NULL, NULL);
        SDL_RenderCopy(ren, blue[WTTCDG].texture, NULL, &blue[WTTCDG].rect);
        //button shown
        /*for (int i = 0; i < buttons; i++)
        {
            SDL_RenderCopy(ren, frame[FrameBlue].texture, NULL, &frame[FrameBlue].rect);
            frame[FrameBlue].rect.y += frame[FrameBlue].rect.h - dY;
        }*/
        //text render
        SDL_RenderCopy(ren, blue[PlayBlue].texture, NULL, &blue[PlayBlue].rect);
        SDL_RenderCopy(ren, blue[SettingsBlue].texture, NULL, &blue[SettingsBlue].rect);
        SDL_RenderCopy(ren, blue[QuitBlue].texture, NULL, &blue[QuitBlue].rect);
        SDL_RenderCopy(ren, white[whiteTextPos].texture, NULL, &white[whiteTextPos].rect);
        //SDL_RenderCopy(ren, frame[FrameWhite].texture, NULL, &frame[FrameWhite].rect);
        SDL_RenderPresent(ren);
    }
    
    SDL_DestroyTexture(menuBg);
    for (int i = 0; i < blues; i++)
        SDL_DestroyTexture(blue[i].texture);
    for (int i = 0; i < whites; i++)
        SDL_DestroyTexture(white[i].texture);

    return quitApp;
}


int main()
{
    srand(time(nullptr));
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        cout << SDL_GetError() << endl;
        return 1;
    }
    if (TTF_Init() != 0)
    {
        cout << TTF_GetError() << endl;
        SDL_Quit();
        return 1;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) != 0)
    {
        cout << Mix_GetError() << endl;
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Window *win = SDL_CreateWindow("Christmas Deer", 0, 0, ScreenW, ScreenH, SDL_WINDOW_FULLSCREEN);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    Mix_Music *avaria = Mix_LoadMUS("audio/avaria.mp3");
    Mix_Music *blestyashie = Mix_LoadMUS("audio/blestyashie.mp3");
    Mix_Music *digi = Mix_LoadMUS("audio/digi.mp3");
    Mix_Music *elka = Mix_LoadMUS("audio/elka.mp3");
    Mix_Music *verka = Mix_LoadMUS("audio/verka.mp3");


    SDL_Event event;
    bool quit = false;
    const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);
    bool mainMenu = true;
    unsigned int sessionHighscore = 0;
    while (!quit)
    {
        if(startMenu(keyboardState, mainMenu, ren, avaria, blestyashie, digi, elka, verka))
            gameLoop(sessionHighscore, ren, keyboardState, mainMenu);
        else
            quit = true;
    }

    Mix_FreeMusic(avaria);
    Mix_FreeMusic(blestyashie);
    Mix_FreeMusic(digi);
    Mix_FreeMusic(elka);
    Mix_FreeMusic(verka);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    TTF_Quit();
    Mix_Quit();

    return 0;
}