
#include "window.h"

#include <GameManager.h>

GameManager* gameManager;

int update()
{
    float delta = 0.1;
    unsigned int elapsed = SDL_GetTicks(); // true delta
    return gameManager->update(delta);
}


int main(int argc, char** argv)
{
    gameManager = new GameManager();
    // etape 1 : creer la fenetre
    Window window = create_window(1024, 640);
    if (window == NULL)
        // erreur lors de la creation de la fenetre ou de l'init de sdl2
        return 1;

    // etape 2 : creer un contexte opengl pour pouvoir dessiner
    Context context = create_context(window);
    if (context == NULL)
        // erreur lors de la creation du contexte opengl
        return 1;

    // etape 3 : creation des objets
    if (gameManager->init() < 0)
    {
        printf("[error] init failed.\n");
        return 1;
    }

    // etape 4 : affichage de l'application
    run(window, update);

    // etape 5 : nettoyage
    gameManager->quit();
    release_context(context);
    release_window(window);
    delete gameManager;
    return 0;
}
