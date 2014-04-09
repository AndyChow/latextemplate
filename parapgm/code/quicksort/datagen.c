#include <time.h>
#include <stdlib.h>


int main(int argc, char *argv[])
{
    srand(time(NULL));
    int count = atoi(argv[1]);

    int i;
    for (i = 0; i< count; i++)
    {
        printf("%d ", rand()%5000);
    }

    return 0;
}
