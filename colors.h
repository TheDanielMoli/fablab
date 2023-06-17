#ifndef FABLAB_COLORS_H
#define FABLAB_COLORS_H

#include <stdio.h>

typedef enum {
    BLACK, RED, GREEN, YELLOW, BLUE, PURPLE, CYAN, WHITE, DEFAULT
} Colors;

void color(Colors color) {
    switch (color) {
        case BLACK:
            printf("\033[0;30m");
            break;
        case RED:
            printf("\033[0;31m");
            break;
        case GREEN:
            printf("\033[0;32m");
            break;
        case YELLOW:
            printf("\033[0;33m");
            break;
        case BLUE:
            printf("\033[0;34m");
            break;
        case PURPLE:
            printf("\033[0;35m");
            break;
        case CYAN:
            printf("\033[0;36m");
            break;
        case WHITE:
            printf("\033[0;37m");
            break;
        case DEFAULT:
            printf("\033[0m");
            break;
    }
}

#endif //FABLAB_COLORS_H
