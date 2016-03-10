/* Nom : GUI.c

Dernier Auteur  : Armand

Creation :
26/02/2014

Dernière modification :
26/02/2014

Description :
	Fichier qui contient des fonctions relatives au fonctionnement de l'environnement graphique utilisateur
*///

#include "../headers/linker.h"

// Fonction qui permet d'afficher un message dans une simple text_box avec un accepter et un refiser
int GUI_message_box(SDL_Window *window,char *message)
{
    struct GUI_object obj;
    int return_value = 0;
    SDL_Surface *ecran = SDL_GetWindowSurface(window);

    obj.largeur = 250;
    obj.hauteur = 100;
    obj.pos_x = ecran->w/2 - obj.largeur/2;
    obj.pos_y = ecran->h/2 - obj.hauteur/2;
    obj.id = 0;

    return_value = GUI_spam(window,obj,1,message,NULL,NULL);
    SDL_FreeSurface(ecran);

    return return_value;
}

// Fonction ANIM qui permet de faire un fondus entre deux écrans
void ANIM_fade_in(SDL_Window *window,SDL_Surface *appear)
{
    // variable compteur défini la vitesse du fade
    SDL_Surface *ecran   = SDL_GetWindowSurface(window);
    SDL_Surface *fader   = IMG_Load("artwork/fade_in.png");

    SDL_Rect center;
    center.w = ecran->w;
    center.h = ecran->h;
    center.x = appear->w/2 - ecran->w/2;
    center.y = 0;

    Uint8 i,j;
    Uint32 start;

    int max = 4;
    int count = 3;

    for ( i = 0; i < max; i++)
    {
        start = SDL_GetTicks(); // on récupère le temps

        SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 255, 255, 255));
        SDL_BlitSurface(appear,&center,ecran,NULL);

        for (j = 0; j < count; ++j)
        {
            SDL_BlitSurface(fader,NULL,ecran,NULL);
        }

        count--;

        SDL_UpdateWindowSurface(window);

        if(1000/IMAGE_PAR_SECONDES > SDL_GetTicks()-start)
        {
            SDL_Delay(1000/IMAGE_PAR_SECONDES-(SDL_GetTicks()-start));
        }
    }

    SDL_FreeSurface(ecran);
    SDL_FreeSurface(fader);
    SDL_FreeSurface(appear);
}

// Fonction ANIM qui permet de faire un fondus vers le noir
void ANIM_fade_out(SDL_Window *window)
{
    // variable compteur défini la vitesse du fade
    SDL_Surface *ecran   = SDL_GetWindowSurface(window);
    SDL_Surface *fader   = IMG_Load("artwork/fade_out.png");

    Uint8 i;
    int max = 13;
    Uint32 start;

    for ( i = 0; i < max; i++)
    {
        start = SDL_GetTicks(); // on récupère le temps

        SDL_BlitSurface(fader,NULL,ecran,NULL);

        SDL_UpdateWindowSurface(window);


        if(1000/60 > SDL_GetTicks()-start)
        {
            SDL_Delay(1000/60-(SDL_GetTicks()-start));
        }
    }

    SDL_FillRect(ecran, NULL, 0x000000);
    SDL_FreeSurface(ecran);
    SDL_FreeSurface(fader);
}

// Fonction GUI qui gère des layout types de fenetres
int GUI_spam(SDL_Window *window, struct GUI_object get_box,int type,char *texte,char *get_String,struct GUI_object *Tbox)
{
    /*--------------------------------------------------------------------*/

        char T[201];
        char   temps[200]   = "";

        struct GUI_object box[10];
        struct state etat[50];

        int    quit         = 0;
        int    edit         = 0;
        int    x,y;
        int    curseur      = 0;
        int    return_value = 0;
        int    start;
        int    i = 0;
        int    count = 0;
        int    tab_curseur = 0;

        const int FPS = IMAGE_PAR_SECONDES;

        SDL_Event event;

        SDL_Rect step1;
        SDL_Rect step2;
        SDL_Rect step3;
        SDL_Rect step4;
        SDL_Rect position;

        SDL_Surface *render;
        SDL_Surface *ecran = SDL_GetWindowSurface(window);

        TTF_Font *light = TTF_OpenFont("fonts/Roboto-Medium.ttf", 12);
        SDL_Color blanc = {255, 255, 255};
        SDL_Color noir = {0, 0, 0};

    /*--------------------------------------------------------------------*/


    if (type == 1) // box classique avec un simple texte ecrit
    {
        box[0].largeur = 25;
        box[0].hauteur = 25;
        box[0].pos_x = get_box.largeur + get_box.pos_x -25;
        box[0].pos_y = get_box.pos_y;
        box[0].id = 10;

        step1.x = get_box.pos_x;
        step1.y = get_box.pos_y;
        step1.w = get_box.largeur;
        step1.h = get_box.hauteur;

        step3.x = get_box.pos_x + 1;
        step3.y = get_box.pos_y + 1;
        step3.w = get_box.largeur - 2;
        step3.h = get_box.hauteur - 2;

        while(quit==0)
        {
            start = SDL_GetTicks(); // on récupère le temps

            SDL_FillRect(ecran,&step1,SDL_MapRGB(ecran->format, 65, 65, 65));
            SDL_FillRect(ecran,&step3,SDL_MapRGB(ecran->format, 255, 255, 255));

            render = TTF_RenderText_Blended(light, texte, noir);

            position.x = get_box.pos_x + get_box.largeur/2 - render->w/2;
            position.y = get_box.pos_y + get_box.hauteur/2 - render->h/2;

            etat[0] = GUI_DISPLAY_button(box[0],ecran,"X");

            SDL_BlitSurface(render, NULL, ecran, &position);
            SDL_FreeSurface(render);

            SDL_UpdateWindowSurface(window);

            SDL_WaitEvent(&event);
            while(SDL_PollEvent(&event));
            {
                switch(event.type)
                {
                    case SDL_KEYDOWN: // Relâchement d'une touche
                    {
                        switch(event.key.keysym.sym)
                        {
                            case SDLK_ESCAPE :
                                quit = 1;
                                break;
                        }
                    }

                    case SDL_MOUSEMOTION: // Mouvement de la souris
                    {
                        etat[0] = GUI_DISPLAY_button(box[0],ecran,"X");
                    }

                    case SDL_MOUSEBUTTONDOWN: // clic de la souris
                    {
                        if (event.button.button == SDL_BUTTON_LEFT)
                        {
                            if(etat[0].etat[0] == 1) // Quitter
                            {
                                quit=1;
                                return_value = -1;
                            }
                        }
                    }
                }
            }

            if(1000/FPS > SDL_GetTicks()-start)
            {
                SDL_Delay(1000/FPS-(SDL_GetTicks()-start));
            }
        }
    }
    else if (type == 2) // box avec un texte simple et un bouton de validation
    {
        box[0].largeur = 25;
        box[0].hauteur = 25;
        box[0].pos_x = get_box.largeur + get_box.pos_x -25;
        box[0].pos_y = get_box.pos_y;
        box[0].id = 10;

        box[1].largeur = 75;
        box[1].hauteur = 25;
        box[1].pos_x = get_box.pos_x + get_box.largeur/2 - box[1].largeur/2;
        box[1].pos_y = get_box.pos_y + get_box.hauteur - 3 - box[1].hauteur;
        box[1].id = 11;

        step1.x = get_box.pos_x;
        step1.y = get_box.pos_y;
        step1.w = get_box.largeur;
        step1.h = get_box.hauteur;

        step3.x = get_box.pos_x + 1;
        step3.y = get_box.pos_y + 1;
        step3.w = get_box.largeur - 2;
        step3.h = get_box.hauteur - 2;

        GUI_DISPLAY_button(box[0],ecran,"X");
        GUI_DISPLAY_button(box[1],ecran,"Ok");

        SDL_UpdateWindowSurface(window);

        while(quit==0)
        {
            start = SDL_GetTicks(); // on récupère le temps

            SDL_WaitEvent(&event);
            while(SDL_PollEvent(&event)); // Récupération des actions de l'utilisateur(while pour une bonne utilisation)
            {
                switch(event.type)
                {

                    case SDL_KEYDOWN: // Relâchement d'une touche
                    {
                        switch(event.key.keysym.sym)
                        {
                            case SDLK_ESCAPE :
                                quit = 1;
                                break;
                        }
                    }

                    case SDL_MOUSEMOTION: // Mouvement de la souris
                    {
                        etat[0] = GUI_DISPLAY_button(box[0],ecran,"X");
                        etat[1] = GUI_DISPLAY_button(box[1],ecran,"Ok");
                    }

                    case SDL_MOUSEBUTTONDOWN: // clic de la souris
                    {
                        if (event.button.button == SDL_BUTTON_LEFT)
                        {
                            if(etat[0].etat[0] == 1) // Quitter par la croix
                            {
                                quit=1;
                                return_value = -1;
                            }
                            else if(etat[1].etat[0] == 1) // Quitter en validant
                            {
                                quit=1;
                                return_value = 1;
                            }
                        }
                    }
                }
            }

            SDL_FillRect(ecran,&step1,SDL_MapRGB(ecran->format, 65, 65, 65));
            SDL_FillRect(ecran,&step3,SDL_MapRGB(ecran->format, 255, 255, 255));

            render = TTF_RenderText_Blended(light, texte, noir);

            position.x = get_box.pos_x + get_box.largeur/2 - render->w/2;
            position.y = get_box.pos_y + get_box.hauteur/2 - render->h/2 - box[1].hauteur/2;

            etat[0] = GUI_DISPLAY_button(box[0],ecran,"X");
            etat[1] = GUI_DISPLAY_button(box[1],ecran,"Ok");

            SDL_BlitSurface(render, NULL, ecran, &position);
            SDL_FreeSurface(render);

            SDL_UpdateWindowSurface(window);

            if(1000/FPS > SDL_GetTicks()-start)
            {
                SDL_Delay(1000/FPS-(SDL_GetTicks()-start));
            }
        }
    }
    else if (type == 3) // Box avec du texte et un Get_String
    {
        box[0].largeur = 25;
        box[0].hauteur = 25;
        box[0].pos_x = get_box.largeur + get_box.pos_x -25;
        box[0].pos_y = get_box.pos_y;
        box[0].id = 10;

        box[1].largeur = 75;
        box[1].hauteur = 25;
        box[1].pos_x = get_box.pos_x + get_box.largeur/2 - box[1].largeur/2;
        box[1].pos_y = get_box.pos_y + get_box.hauteur - 3 -box[1].hauteur;
        box[1].id = 11;

        box[2].largeur = get_box.largeur - 10;
        box[2].hauteur = 24;
        box[2].pos_x = get_box.pos_x + get_box.largeur/2 - box[2].largeur/2;
        box[2].pos_y = get_box.pos_y + get_box.hauteur - 10 -box[2].hauteur - box[1].hauteur;
        box[2].id = 12;

        step1.x = get_box.pos_x;
        step1.y = get_box.pos_y;
        step1.w = get_box.largeur;
        step1.h = get_box.hauteur;

        step3.x = get_box.pos_x + 1;
        step3.y = get_box.pos_y + 1;
        step3.w = get_box.largeur - 2;
        step3.h = get_box.hauteur - 2;

        step2.x = box[2].pos_x;
        step2.y = box[2].pos_y;
        step2.w = box[2].largeur;
        step2.h = box[2].hauteur;

        step4.x = box[2].pos_x + 1;
        step4.y = box[2].pos_y + 1;
        step4.w = box[2].largeur - 2;
        step4.h = box[2].hauteur - 2;

        for (i = 0; i < 200; ++i)
        {
            T[i] = '\0';
        }

        SDL_FillRect(ecran,&step1,SDL_MapRGB(ecran->format, 65, 65, 65));
        SDL_FillRect(ecran,&step3,SDL_MapRGB(ecran->format, 255, 255, 255));

        render = TTF_RenderText_Blended(light, texte, noir);

        position.x = get_box.pos_x + get_box.largeur/2 - render->w/2;
        position.y = get_box.pos_y + get_box.hauteur/2 - render->h/2 - box[1].hauteur/2 -box[2].hauteur/2 - 2;

        etat[0] = GUI_DISPLAY_button(box[0],ecran,"X");
        etat[1] = GUI_DISPLAY_button(box[1],ecran,"Ok");

        SDL_FillRect(ecran,&step2,SDL_MapRGB(ecran->format, 110, 110, 110));
        SDL_FillRect(ecran,&step4,SDL_MapRGB(ecran->format, 38, 38, 38));

        SDL_BlitSurface(render, NULL, ecran, &position);
        SDL_FreeSurface(render);

        SDL_UpdateWindowSurface(window);

        while(quit==0)
        {
            start = SDL_GetTicks(); // on récupère le temps

            SDL_WaitEvent(&event);
            while(SDL_PollEvent(&event)); // Récupération des actions de l'utilisateur(while pour une bonne utilisation)
            {
                switch(event.type)
                {
                    case SDL_KEYUP: // Relâchement d'une touche
                    {
                        if((edit == 1)
                        || (event.key.keysym.sym == SDLK_ESCAPE)
                        || (event.key.keysym.sym == SDLK_TAB))
                        {
                            switch(event.key.keysym.sym)
                            {
                                case SDLK_RETURN :
                                        quit = 2;
                                        return_value = 1;
                                    break;
                                case SDLK_TAB :
                                    tab_curseur++;
                                    if (tab_curseur > 1)
                                    {
                                        tab_curseur = 0;
                                    }

                                    if (tab_curseur == 1)
                                    {
                                        edit = 1;
                                    }
                                    else
                                    {
                                        edit = 0;
                                    }

                                    break;
                                case SDLK_BACKSPACE :
                                    T[curseur - 1] = '\0';
                                    curseur--;
                                    if (curseur < 0)
                                    {
                                        curseur = 0;
                                    }
                                    break;
                                case SDLK_ESCAPE :
                                    quit = -1;
                                    break;
                                case SDLK_KP_0 :
                                    T[curseur] = '0';
                                    curseur++;
                                    break;
                                case SDLK_KP_1 :
                                    T[curseur] = '1';
                                    curseur++;
                                    break;
                                case SDLK_KP_2 :
                                    T[curseur] = '2';
                                    curseur++;
                                    break;
                                case SDLK_KP_3 :
                                    T[curseur] = '3';
                                    curseur++;
                                    break;
                                case SDLK_KP_4 :
                                    T[curseur] = '4';
                                    curseur++;
                                    break;
                                case SDLK_KP_5 :
                                    T[curseur] = '5';
                                    curseur++;
                                    break;
                                case SDLK_KP_6 :
                                    T[curseur] = '6';
                                    curseur++;
                                    break;
                                case SDLK_KP_7 :
                                    T[curseur] = '7';
                                    curseur++;
                                    break;
                                case SDLK_KP_8 :
                                    T[curseur] = '8';
                                    curseur++;
                                    break;
                                case SDLK_KP_9 :
                                    T[curseur] = '9';
                                    curseur++;
                                    break;
                                case SDLK_KP_PERIOD :
                                    T[curseur] = '.';
                                    curseur++;
                                    break;
                                case SDLK_a :
                                    T[curseur] = 'a';
                                    curseur++;
                                    break;
                                case SDLK_b :
                                    T[curseur] = 'b';
                                    curseur++;
                                    break;
                                case SDLK_c :
                                    T[curseur] = 'c';
                                    curseur++;
                                    break;
                                case SDLK_d :
                                    T[curseur] = 'd';
                                    curseur++;
                                    break;
                                case SDLK_e :
                                    T[curseur] = 'e';
                                    curseur++;
                                    break;
                                case SDLK_f :
                                    T[curseur] = 'f';
                                    curseur++;
                                    break;
                                case SDLK_g :
                                    T[curseur] = 'g';
                                    curseur++;
                                    break;
                                case SDLK_h :
                                    T[curseur] = 'h';
                                    curseur++;
                                    break;
                                case SDLK_i :
                                    T[curseur] = 'i';
                                    curseur++;
                                    break;
                                case SDLK_j :
                                    T[curseur] = 'j';
                                    curseur++;
                                    break;
                                case SDLK_k :
                                    T[curseur] = 'k';
                                    curseur++;
                                    break;
                                case SDLK_l :
                                    T[curseur] = 'l';
                                    curseur++;
                                    break;
                                case SDLK_m :
                                    T[curseur] = 'm';
                                    curseur++;
                                    break;
                                case SDLK_n :
                                    T[curseur] = 'n';
                                    curseur++;
                                    break;
                                case SDLK_o :
                                    T[curseur] = 'o';
                                    curseur++;
                                    break;
                                case SDLK_p :
                                    T[curseur] = 'p';
                                    curseur++;
                                    break;
                                case SDLK_q :
                                    T[curseur] = 'q';
                                    curseur++;
                                    break;
                                case SDLK_r :
                                    T[curseur] = 'r';
                                    curseur++;
                                    break;
                                case SDLK_s :
                                    T[curseur] = 's';
                                    curseur++;
                                    break;
                                case SDLK_t :
                                    T[curseur] = 't';
                                    curseur++;
                                    break;
                                case SDLK_u :
                                    T[curseur] = 'u';
                                    curseur++;
                                    break;
                                case SDLK_v :
                                    T[curseur] = 'v';
                                    curseur++;
                                    break;
                                case SDLK_w :
                                    T[curseur] = 'w';
                                    curseur++;
                                    break;
                                case SDLK_x :
                                    T[curseur] = 'x';
                                    curseur++;
                                    break;
                                case SDLK_y :
                                    T[curseur] = 'y';
                                    curseur++;
                                    break;
                                case SDLK_z :
                                    T[curseur] = 'z';
                                    curseur++;
                                    break;
                            }
                        }
                    }

                    case SDL_MOUSEMOTION: // Mouvement de la souris
                    {
                        if(edit != 1)
                        {
                            etat[0] = GUI_DISPLAY_button(box[0],ecran,"X");
                            etat[1] = GUI_DISPLAY_button(box[1],ecran,"Ok");

                            SDL_GetMouseState(&x, &y);

                            if (x >= box[2].pos_x
                            && x <= box[2].pos_x + box[2].largeur
                            && y >= box[2].pos_y
                            && y <= box[2].pos_y + box[2].hauteur)
                            {

                                SDL_FillRect(ecran,&step2,SDL_MapRGB(ecran->format, 65, 65, 65));
                                SDL_FillRect(ecran,&step4,SDL_MapRGB(ecran->format, 200, 200, 200));

                                sprintf(temps, "%s",T);

                                render = TTF_RenderText_Blended(light, temps, noir);

                                if (render != NULL)
                                {
                                    position.x = box[2].pos_x + 5;
                                    position.y = box[2].pos_y + box[2].hauteur/2 - render->h/2;

                                    SDL_BlitSurface(render, NULL, ecran, &position);
                                    SDL_FreeSurface(render);
                                }
                            }
                            else
                            {
                                SDL_FillRect(ecran,&step2,SDL_MapRGB(ecran->format, 110, 110, 110));
                                SDL_FillRect(ecran,&step4,SDL_MapRGB(ecran->format, 38, 38, 38));

                                sprintf(temps, "%s",T);

                                render = TTF_RenderText_Blended(light, temps, blanc);

                                if (render != NULL)
                                {
                                    position.x = box[2].pos_x + 5;
                                    position.y = box[2].pos_y + box[2].hauteur/2 - render->h/2;

                                    SDL_BlitSurface(render, NULL, ecran, &position);
                                    SDL_FreeSurface(render);
                                }
                            }
                        }
                        else
                        {
                            etat[0] = GUI_DISPLAY_button(box[0],ecran,"X");
                            etat[1] = GUI_DISPLAY_button(box[1],ecran,"Ok");
                        }
                    }

                    case SDL_MOUSEBUTTONDOWN: // clic de la souris
                    {
                        if (event.button.button == SDL_BUTTON_LEFT)
                        {
                            SDL_GetMouseState(&x, &y);
                            if (x >= box[2].pos_x
                            && x <= box[2].pos_x + box[2].largeur
                            && y >= box[2].pos_y
                            && y <= box[2].pos_y + box[2].hauteur) // On Edite le texte dans la TextBox
                            {
                                edit = 1;
                            }

                            if(etat[0].etat[0] == 1) // Quitter par la croix
                            {
                                quit=1;
                                return_value = -1;
                            }
                            else if(etat[1].etat[0] == 1) // Quitter en validant
                            {
                                quit=2;
                                return_value = 1;
                            }
                        }
                    }
                }
            }

            if (edit == 1)
            {
                SDL_FillRect(ecran,&step2,SDL_MapRGB(ecran->format, 65, 65, 65));
                SDL_FillRect(ecran,&step4,SDL_MapRGB(ecran->format, 255, 255, 255));

                sprintf(temps, "%s",T);

                render = TTF_RenderText_Blended(light, temps, noir);

                if (render != NULL)
                {
                    position.x = box[2].pos_x + 5;
                    position.y = box[2].pos_y + box[2].hauteur/2 - render->h/2;

                    SDL_BlitSurface(render, NULL, ecran, &position);
                    SDL_FreeSurface(render);
                }
            }

            SDL_UpdateWindowSurface(window);

            if(1000/FPS > SDL_GetTicks()-start)
            {
                SDL_Delay(1000/FPS-(SDL_GetTicks()-start));
            }
        }

        if (quit == 2)
        {
            strcpy(get_String,T);
        }
    }
    else if (type == 4) // Spam avec une liste de GUI_objects alignement horizontale
    {
        while(Tbox[i].id < 999)
        {
            i++;
        }

        count = i;

        step1.x = get_box.pos_x;
        step1.y = get_box.pos_y;
        step1.w = get_box.largeur;
        step1.h = i*24+55;

        step3.x = get_box.pos_x + 1;
        step3.y = get_box.pos_y + 1;
        step3.w = get_box.largeur - 2;
        step3.h = i*24-2+55;

        box[0].largeur = 25;
        box[0].hauteur = 25;
        box[0].pos_x = get_box.largeur + get_box.pos_x -25;
        box[0].pos_y = get_box.pos_y;
        box[0].id = 10;

        SDL_FillRect(ecran,&step1,SDL_MapRGB(ecran->format, 65, 65, 65));
        SDL_FillRect(ecran,&step3,SDL_MapRGB(ecran->format, 255, 255, 255));

        render = TTF_RenderText_Blended(light, texte, noir);

        position.x = get_box.pos_x + get_box.largeur/2 - render->w/2;
        position.y = get_box.pos_y + 10;

        etat[0] = GUI_DISPLAY_button(box[0],ecran,"X");

        i = 0;
        while(Tbox[i].id < 999)
        {
            Tbox[i].pos_x = get_box.pos_x + get_box.largeur/2 - Tbox[i].largeur/2;
            Tbox[i].pos_y = get_box.pos_y + render->h + get_box.hauteur/2 + Tbox[i].hauteur*i;
            etat[i+1] = GUI_DISPLAY_button(Tbox[i],ecran,Tbox[i].text);
            i++;
        }

        SDL_BlitSurface(render, NULL, ecran, &position);
        SDL_FreeSurface(render);

        SDL_UpdateWindowSurface(window);

        while(quit==0)
        {
            start = SDL_GetTicks(); // on récupère le temps

            SDL_WaitEvent(&event);
            while(SDL_PollEvent(&event)); // Récupération des actions de l'utilisateur(while pour une bonne utilisation)
            {
                switch(event.type)
                {

                    case SDL_KEYDOWN: // Relâchement d'une touche
                    {
                        switch(event.key.keysym.sym)
                        {
                            case SDLK_ESCAPE :
                                quit = 1;
                                return_value = -1;
                                break;
                        }
                    }

                    case SDL_MOUSEMOTION: // Mouvement de la souris
                    {
                        SDL_FillRect(ecran,&step1,SDL_MapRGB(ecran->format, 65, 65, 65));
                        SDL_FillRect(ecran,&step3,SDL_MapRGB(ecran->format, 255, 255, 255));

                        render = TTF_RenderText_Blended(light, texte, noir);

                        position.x = get_box.pos_x + get_box.largeur/2 - render->w/2;
                        position.y = get_box.pos_y + 10;

                        SDL_BlitSurface(render, NULL, ecran, &position);
                        SDL_FreeSurface(render);

                        i = 0;
                        while(Tbox[i].id < 999)
                        {
                            etat[i+1] = GUI_DISPLAY_button(Tbox[i],ecran,Tbox[i].text);
                            i++;
                        }

                        box[0].largeur = 25;
                        box[0].hauteur = 25;
                        box[0].pos_x = get_box.largeur + get_box.pos_x -25;
                        box[0].pos_y = get_box.pos_y;
                        box[0].id = 10;
                        etat[0] = GUI_DISPLAY_button(box[0],ecran,"X");
                    }

                    case SDL_MOUSEBUTTONDOWN: // clic de la souris
                    {
                        if (event.button.button == SDL_BUTTON_LEFT)
                        {
                            if(etat[0].etat[0] == 1) // Quitter par la croix
                            {
                                quit=1;
                                return_value = -1;
                            }

                            for ( i = 1; i <= count; ++i)
                            {
                                if(etat[i].etat[0] == 1) // Quitter en cliquant sur un bouton
                                {
                                    quit=1;
                                    return_value = i - 1;
                                    break;
                                }
                            }
                        }
                    }
                }
            }

            SDL_UpdateWindowSurface(window);
            if(1000/FPS > SDL_GetTicks()-start)
            {
                SDL_Delay(1000/FPS-(SDL_GetTicks()-start));
            }
        }
    }

    SDL_FreeSurface(ecran);
    TTF_CloseFont(light);

    return return_value;
}

// Fonction qui enregistre les frappes du clavier pour les convertir en chaine de caractères
void GUI_GET_string(struct GUI_object box,char *T,int taille,SDL_Window *window)
{
    /*--------------------------------------------------------------------*/

        int quitter = 0;
        int curseur = 0;
        int x,y;
        int i ;
        int edit = 0;
        int start;

        const int FPS = IMAGE_PAR_SECONDES; // le nombres d'images par secondes que doit gérer le moteur graphique

        char temps[500] = "";

        TTF_Font *light = TTF_OpenFont("fonts/Roboto-Medium.ttf", 12);

        SDL_Rect position;
        SDL_Rect step1;
        SDL_Rect step3;

        SDL_Surface *ecran = SDL_GetWindowSurface(window);
        SDL_Surface *render;

        SDL_Color blanc = {255, 255, 255};
        SDL_Color noir = {0, 0, 0};

        SDL_Event event;

    /*--------------------------------------------------------------------*/

    SDL_GetMouseState(&x, &y); // récupération des coordonnées de la souris

    step1.x = box.pos_x;
    step1.y = box.pos_y;
    step1.w = box.largeur;
    step1.h = box.hauteur;

    step3.x = box.pos_x + 1;
    step3.y = box.pos_y + 1;
    step3.w = box.largeur - 2;
    step3.h = box.hauteur - 2;

    SDL_FillRect(ecran,&step1,SDL_MapRGB(ecran->format, 110, 110, 110));
    SDL_FillRect(ecran,&step3,SDL_MapRGB(ecran->format, 38, 38, 38));

    SDL_UpdateWindowSurface(window);

    for (i = 0; i < taille; ++i)
    {
        T[i] = '\0';
    }

    curseur=0;
    while((quitter == 0) && (curseur < (taille-1)))
    {
        start = SDL_GetTicks();

        while (SDL_PollEvent(&event)) // Récupération des actions de l'utilisateur
        {
            switch(event.type)
            {
                case SDL_QUIT: // Clic sur la croix
                    quitter=2;
                    break;

                case SDL_KEYDOWN: // appuit d'une touche
                    if(edit == 1)
                    {
                        switch(event.key.keysym.sym)
                        {
                            case SDLK_RETURN :
                                quitter = 1;
                                break;
                            case SDLK_BACKSPACE:
                                T[curseur - 1] = '\0';
                                curseur--;
                                if (curseur < 0)
                                {
                                    curseur = 0;
                                }
                                break;
                            case SDLK_ESCAPE :
                                quitter = 1;
                                break;
                            case SDLK_KP_PERIOD :
                                T[curseur] = '.';
                                curseur++;
                                break;
                            case SDLK_a :
                                T[curseur] = 'a';
                                curseur++;
                                break;
                            case SDLK_b :
                                T[curseur] = 'b';
                                curseur++;
                                break;
                            case SDLK_c :
                                T[curseur] = 'c';
                                curseur++;
                                break;
                            case SDLK_d :
                                T[curseur] = 'd';
                                curseur++;
                                break;
                            case SDLK_e :
                                T[curseur] = 'e';
                                curseur++;
                                break;
                            case SDLK_f :
                                T[curseur] = 'f';
                                curseur++;
                                break;
                            case SDLK_g :
                                T[curseur] = 'g';
                                curseur++;
                                break;
                            case SDLK_h :
                                T[curseur] = 'h';
                                curseur++;
                                break;
                            case SDLK_i :
                                T[curseur] = 'i';
                                curseur++;
                                break;
                            case SDLK_j :
                                T[curseur] = 'j';
                                curseur++;
                                break;
                            case SDLK_k :
                                T[curseur] = 'k';
                                curseur++;
                                break;
                            case SDLK_l :
                                T[curseur] = 'l';
                                curseur++;
                                break;
                            case SDLK_m :
                                T[curseur] = 'm';
                                curseur++;
                                break;
                            case SDLK_n :
                                T[curseur] = 'n';
                                curseur++;
                                break;
                            case SDLK_o :
                                T[curseur] = 'o';
                                curseur++;
                                break;
                            case SDLK_p :
                                T[curseur] = 'p';
                                curseur++;
                                break;
                            case SDLK_q :
                                T[curseur] = 'q';
                                curseur++;
                                break;
                            case SDLK_r :
                                T[curseur] = 'r';
                                curseur++;
                                break;
                            case SDLK_s :
                                T[curseur] = 's';
                                curseur++;
                                break;
                            case SDLK_t :
                                T[curseur] = 't';
                                curseur++;
                                break;
                            case SDLK_u :
                                T[curseur] = 'u';
                                curseur++;
                                break;
                            case SDLK_v :
                                T[curseur] = 'v';
                                curseur++;
                                break;
                            case SDLK_w :
                                T[curseur] = 'w';
                                curseur++;
                                break;
                            case SDLK_x :
                                T[curseur] = 'x';
                                curseur++;
                                break;
                            case SDLK_y :
                                T[curseur] = 'y';
                                curseur++;
                                break;
                            case SDLK_z :
                                T[curseur] = 'z';
                                curseur++;
                                break;
                        }
                    }
                    break;

                case SDL_MOUSEMOTION: // Mouvement de la souris
                {
                    if(edit != 1)
                    {
                        SDL_GetMouseState(&x, &y);

                        if (x >= box.pos_x
                        && x <= box.pos_x + box.largeur
                        && y >= box.pos_y
                        && y <= box.pos_y + box.hauteur)
                        {

                            SDL_FillRect(ecran,&step1,SDL_MapRGB(ecran->format, 65, 65, 65));
                            SDL_FillRect(ecran,&step3,SDL_MapRGB(ecran->format, 200, 200, 200));

                            sprintf(temps, "%s",T);

                            render = TTF_RenderText_Blended(light, temps, noir);

                            if (render != NULL)
                            {
                                position.x = box.pos_x + 5;
                                position.y = box.pos_y + box.hauteur/2 - render->h/2;

                                SDL_BlitSurface(render, NULL, ecran, &position);
                                SDL_FreeSurface(render);
                            }
                        }
                        else
                        {
                            SDL_FillRect(ecran,&step1,SDL_MapRGB(ecran->format, 110, 110, 110));
                            SDL_FillRect(ecran,&step3,SDL_MapRGB(ecran->format, 38, 38, 38));

                            sprintf(temps, "%s",T);

                            render = TTF_RenderText_Blended(light, temps, blanc);

                            if (render != NULL)
                            {
                                position.x = box.pos_x + 5;
                                position.y = box.pos_y + box.hauteur/2 - render->h/2;

                                SDL_BlitSurface(render, NULL, ecran, &position);
                                SDL_FreeSurface(render);
                            }
                        }
                    }
                }

                case SDL_MOUSEBUTTONDOWN: // clic de la souris
                {
                    if (event.button.button == SDL_BUTTON_LEFT)
                    {
                        SDL_GetMouseState(&x, &y);

                        if (x >= box.pos_x
                        && x <= box.pos_x + box.largeur
                        && y >= box.pos_y
                        && y <= box.pos_y + box.hauteur)
                        {
                            SDL_ShowCursor(0);

                            edit = 1;
                        }
                    }
                }
            }
        }

        if (edit == 1)
        {
            SDL_FillRect(ecran,&step1,SDL_MapRGB(ecran->format, 65, 65, 65));
            SDL_FillRect(ecran,&step3,SDL_MapRGB(ecran->format, 255, 255, 255));

            sprintf(temps, "%s",T);

            render = TTF_RenderText_Blended(light, temps, noir);
            if (render == NULL)
            {

            }
            else
            {
                position.x = box.pos_x + 5;
                position.y = box.pos_y + box.hauteur/2 - render->h/2;

                SDL_BlitSurface(render, NULL, ecran, &position);
                SDL_FreeSurface(render);
            }
        }

        SDL_UpdateWindowSurface(window);

        if(1000/FPS > SDL_GetTicks()-start)
        {
            SDL_Delay(1000/FPS-(SDL_GetTicks()-start));
        }
    }

    curseur++;
    T[curseur] = '\0';
    TTF_CloseFont(light);
    light=NULL;
    SDL_ShowCursor(1);
}

// Fonction qui affiche un bouton
struct state GUI_DISPLAY_button(struct GUI_object box,SDL_Surface *ecran,char *nom_bouton)
{
    /*--------------------------------------------------------------------*/

        struct state etat;
        int x,y;
        char temps[200] = "";

        SDL_Rect step1;
        SDL_Rect step3;
        SDL_Rect position;

        SDL_Surface *render;

        TTF_Font *light = TTF_OpenFont("fonts/Roboto-Medium.ttf", 12);

        SDL_Color blanc = {255, 255, 255};
        SDL_Color noir = {0, 0, 0};

    /*--------------------------------------------------------------------*/

    SDL_GetMouseState(&x, &y); // récupération des coordonnées de la souris

    step1.x = box.pos_x;
    step1.y = box.pos_y;
    step1.w = box.largeur;
    step1.h = box.hauteur;

    step3.x = box.pos_x + 1;
    step3.y = box.pos_y + 1;
    step3.w = box.largeur - 2;
    step3.h = box.hauteur - 2;

    sprintf(temps, "%s",nom_bouton);

    if (x >= box.pos_x
    && x <= box.pos_x + box.largeur
    && y >= box.pos_y
    && y <= box.pos_y + box.hauteur)
    {
       etat.etat[0] = 1;

       SDL_FillRect(ecran,&step1,SDL_MapRGB(ecran->format, 65, 65, 65));
       SDL_FillRect(ecran,&step3,SDL_MapRGB(ecran->format, 200, 200, 200));

       render = TTF_RenderText_Blended(light, temps, noir);
    }
    else
    {
       etat.etat[0] = 0;

       SDL_FillRect(ecran,&step1,SDL_MapRGB(ecran->format, 110, 110, 110));
       SDL_FillRect(ecran,&step3,SDL_MapRGB(ecran->format, 38, 38, 38));

       render = TTF_RenderText_Blended(light, temps, blanc);
    }

    position.x = box.pos_x + box.largeur/2 - render->w/2;
    position.y = box.pos_y + box.hauteur/2 - render->h/2;

    SDL_BlitSurface(render, NULL, ecran, &position);

    SDL_FreeSurface(render);

    TTF_CloseFont(light);
    light=NULL;

    return etat;
}

// Fonction qui contrôle la position de la souris dans la fenetre
void GUI_LOG_souris()
{
    SDL_Point mouse_position;

    SDL_GetMouseState(
        &mouse_position.x,
        &mouse_position.y
    );

    printf("\nMouse position: x=%i y=%i",
         mouse_position.x, mouse_position.y
    );
}

// Fonction qui contrôle l'état des touches du clavier
void GUI_LOG_clavier(SDL_Event event_keyboard)
{
    switch (event_keyboard.type)
    {
        case SDL_KEYUP:
        {
            printf("\nClavier %s Up ",SDL_GetKeyName(event_keyboard.key.keysym.sym));
            break;
        }

        case SDL_KEYDOWN:
        {
            printf("\nClavier %s Down ",SDL_GetKeyName(event_keyboard.key.keysym.sym));
            break;
        }
    }
}

// Fonction qui charge les boutons
void GUI_chargement_boutons(const char **image,SDL_Surface **Tableau_BOUTON, int count)
{
    /*--------------*/

        int i = 0;

    /*--------------*/

    for (i = 0; i < count; ++i)
    {
        Tableau_BOUTON[i] = IMG_Load(image[i]);
        if(Tableau_BOUTON[i] == NULL)
        {
            printf("\tIMG_Load: %s\n", IMG_GetError());
        }
    }
}

// Fonction qui supprime toutes les surfaces des boutons
void GUI_free_tab_boutons(SDL_Surface **Tableau_BOUTON,int count)
{
    /*--------------*/

        int i = 0;

    /*--------------*/

    for (i = 0; i < count; ++i)
    {
        SDL_FreeSurface(Tableau_BOUTON[i]);
    }
}

// Fonction qui affiche un menu en tenant compte uniquement de l'état des boutons et non pas de la position de la souris
struct state GUI_check_key_state(SDL_Surface *ecran,SDL_Surface **Tableau_BOUTON,struct state etat_bouton)
{
    /*--------------*/

        struct state etat;
        int i=0;
        int temp_hauteur = TOP_MENU_Y;

    /*--------------*/

    for (i = 0; i < NBR_BOUTON; i++)
    {

        SDL_Rect position_bouton = {ecran->w/2-Tableau_BOUTON[2*i]->w/2,
                                 temp_hauteur,
                                 Tableau_BOUTON[2*i]->w,
                                 Tableau_BOUTON[2*i]->h};

        if(etat_bouton.etat[i] == 1)
        {
            etat.etat[i] = 1;
            SDL_BlitSurface(Tableau_BOUTON[2*i+1], NULL, ecran, &position_bouton);
        }
        else
        {
            etat.etat[i] = 0;
           SDL_BlitSurface(Tableau_BOUTON[2*i], NULL, ecran, &position_bouton);
        }

        temp_hauteur += Tableau_BOUTON[i]->h + SEPARATEUR;
    }

    return etat;
}

// Fonction qui renvoit l'état des boutons et qui les blites en tenant compte de la position de la souris
struct state GUI_check_state(SDL_Surface *ecran,SDL_Surface **Tableau_BOUTON, int mode)
{
    /*-----------------------*/

        struct state etat;
        int i;
        int x,y;
        int top_menu = TOP_MENU_Y;

    /*-----------------------*/

    SDL_GetMouseState(&x, &y); // récupération des coordonnées de la souris

    for (i = 0; i < NBR_BOUTON; i++)
    {

        /*-------------------------------------------------------------------*/

            SDL_Rect position_bouton = {ecran->w/2-Tableau_BOUTON[2*i]->w/2,
                                        top_menu,
                                        Tableau_BOUTON[2*i]->w,
                                        Tableau_BOUTON[2*i]->h};

        /*-------------------------------------------------------------------*/

        if (x >= ecran->w/2-Tableau_BOUTON[2*i]->w/2
        &&  x <= (ecran->w/2-Tableau_BOUTON[2*i]->w/2+Tableau_BOUTON[i*2]->w)
        &&  y >= top_menu
        &&  y <= top_menu + Tableau_BOUTON[i*2]->h)
        {
            etat.etat[i] = 1;

            if (mode == 1)
            {
                SDL_BlitSurface(Tableau_BOUTON[2*i+1], NULL, ecran, &position_bouton);
            }
        }
        else
        {
            etat.etat[i] = 0;

            if (mode == 1)
            {
                SDL_BlitSurface(Tableau_BOUTON[2*i], NULL, ecran, &position_bouton);
            }
        }

        top_menu += Tableau_BOUTON[i]->h + SEPARATEUR;
    }

    return etat;
}

// Fonction qui met l'etat nombre à un bouton en retournant le nouvel etat du bouton
struct state GUI_set(int nombre)
{
    /*-----------------------*/

        struct state state;
        Uint32 i = 0;

    /*-----------------------*/

    for (i = 0; i < NBR_BOUTON; ++i)
    {
        if (i == nombre)
        {
            state.etat[i] = 1;
        }
        else
        {
            state.etat[i] = 0;
        }
    }

    return state;
}
