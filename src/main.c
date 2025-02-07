#include "ls.h"

int main(int ac, char **av) {
    int result = 0;
    
    if (ac == 1) {
        result = ls(".", ALL);
    } else {
        result = ls(av[1], ALL);
    }

    return (result);
}