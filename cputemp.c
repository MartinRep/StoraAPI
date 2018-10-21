#include <stdio.h>
#include <time.h>
#include "board_api.h"

int main(void)
{
    float curr_temp = 0;
    time_t start_time = 0;
    start_time = time(NULL);
    while(1)
    {
        sleep(1);
        sysGetTemp(&curr_temp);
        printf("Current temp = %3.2f\n", curr_temp);
    }
}


