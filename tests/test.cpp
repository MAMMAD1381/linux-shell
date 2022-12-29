#include <conio.h>
int main()
{
    int c;
    while ((c = getch()) != 27) /* exit with Escape key */
        printf( "&#37;d\n", c);
}
