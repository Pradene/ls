#include "ls.h"

int main(int ac, char **av) {
    int result = 0;
    
    if (ac == 1) {
        result = ls(".", ALL | TIME);
    } else {
        result = ls(av[1], NONE);    
    }

    return (result);
}