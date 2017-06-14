#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <time.h>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 780



typedef struct game{
     SDL_Window *g_pWindow;
     SDL_Renderer *g_pRenderer;
     SDL_Texture *g_texture;
     SDL_Surface *g_surface;
}game;

typedef struct gameState{
    int g_bRunning;
    int left;
    int right;
}gameState;


typedef struct coordonnees{
    double x;
    double y;
}coordonnees;

typedef struct balle_jeu{
    SDL_Rect rect;
    int d;
    int colle;
}balle_jeu;


int init(char *title, int xpos,int ypos,int height, int width,int flags,game *myGame);
void handleEvents(gameState *state, balle_jeu *balle);
void cutBitmapTexture(game *myGame,gameState state);
void delay(unsigned int frameLimit);
void destroyTexture(game *myGame);
void destroy(game *myGame);

void initBalle(game *myGame, balle_jeu *balle, SDL_Rect j);
void initRaquette(game *myGame, SDL_Rect *j);
void initLvl(game *myGame, SDL_Rect *brique, gameState *state);

void dplcmtRaquette(gameState state, SDL_Rect *j, balle_jeu *balle);
void dplcmtBalle(game *myGame, balle_jeu *balle, SDL_Rect j);


void affRaquette(game *myGame, SDL_Rect j);
void affBalle(game *myGame, balle_jeu balle);

void checkCollisions();


int main(int argc, char *argv[])
{
     game myGame;
     gameState state;
     balle_jeu balle;
     SDL_Rect j;
     SDL_Rect brique[10];

     srand(time(NULL));

    //Pour les 60 fps
    unsigned int frameLimit = SDL_GetTicks() + 16;

    if(init("Chapter 1 setting up SDL",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,SCREEN_WIDTH,SCREEN_HEIGHT,SDL_WINDOW_SHOWN,&myGame)){
                state.g_bRunning=1;
                initRaquette(&myGame,&j);
                initBalle(&myGame,&balle,j);
                initLvl(&myGame, &brique,&state);
                SDL_RenderPresent(myGame.g_pRenderer);
    }else{
            return 1;//something's wrong
    }

    while(state.g_bRunning){
            SDL_RenderClear(myGame.g_pRenderer);
            handleEvents(&state,&balle);
            dplcmtRaquette(state,&j,&balle);
            if (balle.colle==0){
                dplcmtBalle(&myGame,&balle,j);
            }

            affBalle(&myGame,balle);
            affRaquette(&myGame,j);
            SDL_RenderPresent(myGame.g_pRenderer);
            delay(frameLimit);
            frameLimit = SDL_GetTicks() + 16;
    }

    destroy(&myGame);

    SDL_Quit();

  return 0;
}


int init(char *title, int xpos,int ypos,int height, int width,int flags,game *myGame){

    //initialize SDL

    if(SDL_Init(SDL_INIT_EVERYTHING)>=0)
    {
            //if succeeded create our window
            myGame->g_pWindow=SDL_CreateWindow(title,xpos,ypos,height,width,flags);
            //if succeeded create window, create our render
            if(myGame->g_pWindow!=NULL){
                myGame->g_pRenderer=SDL_CreateRenderer(myGame->g_pWindow,-1,SDL_RENDERER_ACCELERATED);
                SDL_SetRenderDrawColor(myGame->g_pRenderer, 0, 0, 0, 255);
            }
    }else{
        return 0;
    }

    return 1;
}

void handleEvents(gameState *state, balle_jeu *balle){

    SDL_Event event;

    if(SDL_PollEvent(&event)){
        switch(event.type){
        case SDL_QUIT:
              state->g_bRunning=0;break;
        case SDL_KEYDOWN:
                        switch (event.key.keysym.sym)
                            {
                                case SDLK_LEFT:
                                    if (state->right==0){
                                        state->left=1;
                                    }
                                    break;
                                case SDLK_RIGHT:
                                    if (state->left==0){
                                        state->right=1;
                                    }
                                    break;
                               case SDLK_SPACE:
                                    if (balle->colle==1){
                                        balle->colle=0;
                                    }
                                    break;
                                default :
                                    break;
                            }
                            break;

        case SDL_KEYUP:
                        switch (event.key.keysym.sym)
                            {
                                case SDLK_LEFT:state->left=0;state->right=0; break;
                                case SDLK_RIGHT:state->right=0;state->left=0 ; break;
                                default: break;
                            }
                            break;

        default:break;

        }
    }
}

void cutBitmapTexture(game *myGame,gameState state){

        SDL_Rect rectangleDest;
        SDL_Rect rectangleSource;

        myGame->g_surface = IMG_Load("./assets/animate-alpha.png");//Chargement de l'image png

        if(!myGame->g_surface) {
            fprintf(stdout,"IMG_Load: %s\n", IMG_GetError());
            // handle error
        }

        if(myGame->g_surface){

                 myGame->g_texture = SDL_CreateTextureFromSurface(myGame->g_pRenderer,myGame->g_surface); // Préparation du sprite
                 SDL_FreeSurface(myGame->g_surface); // Libération de la ressource occupée par le sprite

                if(myGame->g_texture){

                    rectangleSource.x=0;//128*(int)((SDL_GetTicks()/100)%6);
                    rectangleSource.y=0;
                    rectangleSource.w=128;//1 image = w:128 et h:82
                    rectangleSource.h=82;

                    SDL_QueryTexture(myGame->g_texture,NULL,NULL,NULL,NULL);

                    //Définition du rectangle dest pour dessiner Bitmap
                    rectangleDest.x=0;//debut x
                    rectangleDest.y=0;//debut y
                    rectangleDest.w=rectangleSource.w; //Largeur
                    rectangleDest.h=rectangleSource.h; //Hauteur

                    //SDL_RenderCopy(myGame->g_pRenderer,myGame->g_texture,&rectangleSource,&rectangleDest);

                    if(state.right)
                        SDL_RenderCopy(myGame->g_pRenderer,myGame->g_texture,&rectangleSource,&rectangleDest); // Copie du sprite grâce au SDL_Renderer
                    if(state.left)
                        SDL_RenderCopyEx(myGame->g_pRenderer,myGame->g_texture,&rectangleSource,&rectangleDest,0,0,SDL_FLIP_HORIZONTAL);

                    //SDL_FLIP_NONE
                    //SDL_FLIP_HORIZONTAL
                    //SDL_FLIP_VERTICAL

                    SDL_RenderPresent(myGame->g_pRenderer); // Affichage

                    SDL_RenderClear(myGame->g_pRenderer);

                }

                else{
                        fprintf(stdout,"Échec de création de la texture (%s)\n",SDL_GetError());
                }

        }else{
            fprintf(stdout,"Échec de chargement du sprite (%s)\n",SDL_GetError());
        }


    destroyTexture(myGame);

}

void delay(unsigned int frameLimit){
    // Gestion des 60 fps (images/seconde)
    unsigned int ticks = SDL_GetTicks();

    if (frameLimit < ticks)
    {
        return;
    }

    if (frameLimit > ticks + 16)
    {
        SDL_Delay(16);
    }

    else
    {
        SDL_Delay(frameLimit - ticks);
    }
}

/* ---------------------
BUT : Initialiser les coordonnees de la raquette et l'afficher
ENTREE : L'instance de jeu, la raquette du joueur
SORTIE : Les donnees initialisees, l'affichage de la raquette
---------------------- */

void initRaquette(game *myGame, SDL_Rect *j){

        SDL_Rect rectangleDest;
        SDL_Rect rectangleSource;

        myGame->g_surface = IMG_Load("./assets/paddle.png");

        if(!myGame->g_surface) {
            fprintf(stdout,"IMG_Load: %s\n", IMG_GetError());
        }

        if(myGame->g_surface){
                 myGame->g_texture = SDL_CreateTextureFromSurface(myGame->g_pRenderer,myGame->g_surface);
                 SDL_FreeSurface(myGame->g_surface);

                if(myGame->g_texture){

                    rectangleSource.x=0;
                    rectangleSource.y=0;
                    rectangleSource.w=128;//1 image = w:128 et h:82
                    rectangleSource.h=32;

                    SDL_QueryTexture(myGame->g_texture,NULL,NULL,NULL,NULL);

                    rectangleDest.x=SCREEN_WIDTH/2-rectangleSource.w/2;
                    rectangleDest.y=SCREEN_HEIGHT-rectangleSource.h;
                    rectangleDest.w=rectangleSource.w;
                    rectangleDest.h=rectangleSource.h;

                    j->x=rectangleDest.x;
                    j->y=rectangleDest.y;
                    j->w=rectangleDest.w;
                    j->h=rectangleDest.h;

                    SDL_RenderCopy(myGame->g_pRenderer,myGame->g_texture,&rectangleSource,&rectangleDest);
            }
            else{
                    fprintf(stdout,"Échec de création de la texture (%s)\n",SDL_GetError());
            }

    }else{
        fprintf(stdout,"Échec de chargement du sprite (%s)\n",SDL_GetError());
    }

    destroyTexture(myGame);
}

/* ---------------------
BUT : Initialiser les coordonnees de la balle, selon le sprite de la balle et la hauteur de la raquette, afficher la balle
ENTREE : L'instance de jeu, la balle, la raquette du joueur
SORTIE : Les donnees initialisees, l'affichage de la balle
---------------------- */

void initBalle(game *myGame, balle_jeu *balle, SDL_Rect j){
        SDL_Rect rectangleDest;
        SDL_Rect rectangleSource;

        balle->d=1;
        balle->colle=1;

        myGame->g_surface = IMG_Load("./assets/ball.png");//Chargement de l'image png

        if(!myGame->g_surface) {
            fprintf(stdout,"IMG_Load: %s\n", IMG_GetError());
            // handle error
        }

        if(myGame->g_surface){

                 myGame->g_texture = SDL_CreateTextureFromSurface(myGame->g_pRenderer,myGame->g_surface); // Préparation du sprite
                 SDL_FreeSurface(myGame->g_surface); // Libération de la ressource occupée par le sprite

                if(myGame->g_texture){

                    rectangleSource.x=0;
                    rectangleSource.y=0;
                    rectangleSource.w=24;//1 image = w:128 et h:82
                    rectangleSource.h=24;

                    SDL_QueryTexture(myGame->g_texture,NULL,NULL,NULL,NULL);

                    //Définition du rectangle dest pour dessiner Bitmap
                    rectangleDest.x=SCREEN_WIDTH/2-rectangleSource.w/2;//debut x
                    rectangleDest.y=SCREEN_HEIGHT-rectangleSource.h-j.h;//debut y
                    rectangleDest.w=rectangleSource.w; //Largeur
                    rectangleDest.h=rectangleSource.h; //Hauteur

                    // ==== ATTENTION POINT DE LA BALLE EN HAUT A GAUCHE ==== //
                    balle->rect.x=rectangleDest.x;
                    balle->rect.y=rectangleDest.y;
                    balle->rect.w=rectangleDest.w;
                    balle->rect.h=rectangleDest.h;

                    SDL_RenderCopy(myGame->g_pRenderer,myGame->g_texture,&rectangleSource,&rectangleDest);

                    balle->colle=1; //COLLE LA BALLE A LA RAQUETTE, NE PART PAS TANT QUE ESPACE N EST PAS PRESSEE

            }
            else{
                    fprintf(stdout,"Échec de création de la texture (%s)\n",SDL_GetError());
            }
    }else{
        fprintf(stdout,"Échec de chargement du sprite (%s)\n",SDL_GetError());
    }

    destroyTexture(myGame);

}

/* ---------------------
BUT : Initialiser les coordonnees des briques et les valeurs d'appui de touche, afficher les rebords du jeu et les briques
ENTREE : L'instance de jeu, un tableau de briques, l'etat du jeu
SORTIE : Les donnees initialisees et affichage des briques et rebords
---------------------- */

void initLvl(game *myGame, SDL_Rect *brique, gameState *state){

        int i, nb_lignes;

        SDL_Rect rectangleSource;
        SDL_Rect rectangleDest;
        SDL_Rect rectangleDest2;

        //initialisation valeurs d'input
        state->left=0;
        state->right=0;

        // ------- AFFICHE REBORDS ------ //
        myGame->g_surface = IMG_Load("./assets/side.png");

        if(!myGame->g_surface) {
            fprintf(stdout,"IMG_Load: %s\n", IMG_GetError());
        }

        if(myGame->g_surface){

                 myGame->g_texture = SDL_CreateTextureFromSurface(myGame->g_pRenderer,myGame->g_surface);
                 SDL_FreeSurface(myGame->g_surface);

                if(myGame->g_texture){

                    rectangleSource.x=0;
                    rectangleSource.y=0;
                    rectangleSource.w=16;
                    rectangleSource.h=703;

                    SDL_QueryTexture(myGame->g_texture,NULL,NULL,NULL,NULL);

                    rectangleDest.x=0;
                    rectangleDest.y=0;
                    rectangleDest.w=rectangleSource.w;
                    rectangleDest.h=SCREEN_HEIGHT;

                    SDL_RenderCopy(myGame->g_pRenderer,myGame->g_texture,&rectangleSource,&rectangleDest);

                    /*rectangleDest2.x=SCREEN_WIDTH-rectangleSource.w;
                    rectangleDest2.y=0;
                    rectangleDest2.w=rectangleSource.w;
                    rectangleDest2.h=SCREEN_HEIGHT;

                    SDL_RenderCopy(myGame->g_pRenderer,myGame->g_texture,&rectangleSource,&rectangleDest2);*/

                    printf("ok");

                }else{
                        fprintf(stdout,"Échec de création de la texture (%s)\n",SDL_GetError());
                }

        }else{
            fprintf(stdout,"Échec de chargement du sprite (%s)\n",SDL_GetError());
        }

        // ------ INITIALISE ET AFFICHE BRIQUES ----- //

        /*
        myGame->g_surface = IMG_Load("./assets/brick1.png");//Chargement de l'image png

        if(!myGame->g_surface) {
            fprintf(stdout,"IMG_Load: %s\n", IMG_GetError());
            // handle error
        }

        for (i=0;i<10;i++){

            if(myGame->g_surface){

                myGame->g_texture = SDL_CreateTextureFromSurface(myGame->g_pRenderer,myGame->g_surface);
                SDL_FreeSurface(myGame->g_surface);

                if(myGame->g_texture){
                    rectangleSource.x=0;
                    rectangleSource.y=0;
                    rectangleSource.w=64;
                    rectangleSource.h=25;

                    SDL_QueryTexture(myGame->g_texture,NULL,NULL,NULL,NULL);

                    rectangleDest.x=16+i*rectangleSource.w;
                    rectangleDest.y=0;
                    rectangleDest.w=rectangleSource.w;
                    rectangleDest.h=rectangleSource.h;

                    SDL_RenderCopy(myGame->g_pRenderer,myGame->g_texture,&rectangleSource,&rectangleDest);
                }else{
                        fprintf(stdout,"Échec de création de la texture (%s)\n",SDL_GetError());
                }
            }else{
                fprintf(stdout,"Échec de chargement du sprite (%s)\n",SDL_GetError());
            }

        }
        */

        destroyTexture(myGame);
}

/* ---------------------
BUT : Mettre a jour emplacement raquette si touche pressee, deplacement balle si collee
ENTREE : Etat du jeu, la raquette du joueur, la balle
SORTIE : Les coordonnees de la raquette et eventuellement de la balle mises a jour
---------------------- */

void dplcmtRaquette(gameState state, SDL_Rect *j, balle_jeu *balle){
    if(state.left==1){
        if (j->x>0){
            j->x=j->x-10;
            if(balle->colle==1){
                balle->rect.x=balle->rect.x-10;
            }
        }
    }else if (state.right==1){
        if (j->x<SCREEN_WIDTH-j->w){
            j->x=j->x+10;
            if(balle->colle==1){
                balle->rect.x=balle->rect.x+10;
            }
        }
    }
}

/* ---------------------
BUT : Deplacer la balle, gerer les collisions et la perte de la balle
ENTREE : L'instance de jeu, les coordonnees de la balle
SORTIE : Les coordonnees mises a jour
---------------------- */
void dplcmtBalle(game *myGame, balle_jeu *balle, SDL_Rect j){

    //deplacement balle
    if (balle->d==1){
        balle->rect.x=balle->rect.x-2;
        balle->rect.y=balle->rect.y-2;
    }else if (balle->d==2){
        balle->rect.x=balle->rect.x+2;
        balle->rect.y=balle->rect.y-2;
    }else if (balle->d==3){
        balle->rect.x=balle->rect.x+2;
        balle->rect.y=balle->rect.y+2;
    }else if (balle->d==4){
        balle->rect.x=balle->rect.x-2;
        balle->rect.y=balle->rect.y+2;
    }

    //on corrige la coordonnee de la balle si sortie de la fenetre
    /*
    if (balle->rect.x+balle->rect.w>SCREEN_WIDTH){ //------sortie a droite
        balle->rect.x=SCREEN_WIDTH-balle->rect.w;
    }else if(balle->rect.x+balle->rect.w<0){       //------sortie a gauche
        balle->rect.x=0;
    } else if(balle->rect.y+balle->rect.h<0){   //----------------sortie en haut
        balle->rect.y=0;
    }else if(balle->rect.y+balle->rect.h>SCREEN_HEIGHT){ //sortie en bas
        balle->rect.y=SCREEN_HEIGHT-balle->rect.h;
    }*/

    //collision avec le bord droit
    if ((balle->rect.x+balle->rect.w==SCREEN_WIDTH) && (balle->d==2)){
        balle->d=1;
    }else if((balle->rect.x+balle->rect.w==SCREEN_WIDTH) && (balle->d==3)){
        balle->d=4;
    }

    //collision avec le bord gauche
    if ( (balle->rect.x==0) && (balle->d==1) ){
        balle->d=2;
    }else if( (balle->rect.x==0) && (balle->d==3) ){
        balle->d=4;
    }


    //collision avec le bord du haut (briques detruites)
    if ( (balle->rect.y==0) && (balle->d==1) ){
        balle->d=4;
    }else if ( (balle->rect.y==0) && (balle->d==2) ){
        balle->d=3;
    }

    //collision avec une brique

    //collision avec la raquette
    if ( (balle->rect.y+balle->rect.h>j.x) && (balle->rect.y+balle->rect.h<j.x+j.w) && (balle->rect.x==SCREEN_WIDTH-j.h) && balle->d==3 ){
        balle->d=2;
    }else if( (balle->rect.y+balle->rect.h>j.x) && (balle->rect.y+balle->rect.h<j.x+j.w) && (balle->rect.x==SCREEN_WIDTH-j.h) && balle->d==4 ){
        balle->d=1;
    }

    //perte de la balle
    if (balle->rect.y>=SCREEN_HEIGHT){
        SDL_RenderClear(myGame->g_pRenderer);
        initRaquette(myGame,&j);
        initBalle(myGame,balle,j);

    }

}

/* ---------------------
BUT : Afficher la raquette
ENTREE : L'instance de jeu, les coordonnees de la raquette
SORTIE : La raquette affichee
---------------------- */

void affRaquette(game *myGame, SDL_Rect j){
        SDL_Rect rectangleSource;
        SDL_Rect rectangleDest;

        myGame->g_surface = IMG_Load("./assets/paddle.png");

        if(!myGame->g_surface) {
            fprintf(stdout,"IMG_Load: %s\n", IMG_GetError());
        }

        if(myGame->g_surface){
                 myGame->g_texture = SDL_CreateTextureFromSurface(myGame->g_pRenderer,myGame->g_surface);
                 SDL_FreeSurface(myGame->g_surface);

                if(myGame->g_texture){

                    rectangleSource.x=0;
                    rectangleSource.y=0;
                    rectangleSource.w=128;
                    rectangleSource.h=32;

                    SDL_QueryTexture(myGame->g_texture,NULL,NULL,NULL,NULL);

                    rectangleDest.x=j.x;
                    rectangleDest.y=j.y;
                    rectangleDest.w=j.w;
                    rectangleDest.h=j.h;

                    SDL_RenderCopy(myGame->g_pRenderer,myGame->g_texture,&rectangleSource,&rectangleDest);
            }
            else{
                    fprintf(stdout,"Échec de création de la texture (%s)\n",SDL_GetError());
            }

    }else{
        fprintf(stdout,"Échec de chargement du sprite (%s)\n",SDL_GetError());
    }

    destroyTexture(myGame);
}

/* ---------------------
BUT : Initialiser les coordonnees de la balle, selon le sprite de la balle et la hauteur de la raquette, afficher la balle
ENTREE : L'instance de jeu, la balle, la raquette du joueur
SORTIE : Les donnees initialisees, l'affichage de la balle
---------------------- */

void affBalle(game *myGame, balle_jeu balle){
        SDL_Rect rectangleDest;
        SDL_Rect rectangleSource;

        myGame->g_surface = IMG_Load("./assets/ball.png");

        if(!myGame->g_surface) {
            fprintf(stdout,"IMG_Load: %s\n", IMG_GetError());
        }

        if(myGame->g_surface){

                 myGame->g_texture = SDL_CreateTextureFromSurface(myGame->g_pRenderer,myGame->g_surface);
                 SDL_FreeSurface(myGame->g_surface);

                if(myGame->g_texture){

                    rectangleSource.x=0;
                    rectangleSource.y=0;
                    rectangleSource.w=24;
                    rectangleSource.h=24;

                    SDL_QueryTexture(myGame->g_texture,NULL,NULL,NULL,NULL);

                    rectangleDest.x=balle.rect.x;
                    rectangleDest.y=balle.rect.y;
                    rectangleDest.w=balle.rect.w;
                    rectangleDest.h=balle.rect.h;

                    SDL_RenderCopy(myGame->g_pRenderer,myGame->g_texture,&rectangleSource,&rectangleDest);

            }else{
                    fprintf(stdout,"Échec de création de la texture (%s)\n",SDL_GetError());
            }
    }else{
        fprintf(stdout,"Échec de chargement du sprite (%s)\n",SDL_GetError());
    }

    destroyTexture(myGame);


}


void destroy(game *myGame){

    //Destroy render
    if(myGame->g_pRenderer!=NULL)
        SDL_DestroyRenderer(myGame->g_pRenderer);


    //Destroy window
    if(myGame->g_pWindow!=NULL)
        SDL_DestroyWindow(myGame->g_pWindow);

}

void destroyTexture(game *myGame){

    //Destroy texture
    if(myGame->g_texture!=NULL)
            SDL_DestroyTexture(myGame->g_texture);


}

/* ---------------------
BUT :
ENTREE :
SORTIE :
---------------------- */








