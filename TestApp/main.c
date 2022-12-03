#include <stdio.h>

#include "application.h"

struct application_t jotunn_app;

// static void jotunn_app_init(struct application_t* app)
// {

// }

int main(int argc, char** argv)
{
    struct application_t* app_ptr = &jotunn_app; 

    uint8_t init_error = application_init(app_ptr, "JotunnTestApp", 1);
    if (!init_error)
    {
        application_start(app_ptr);

        #ifdef DEBUG
            fprintf(stdout, "Running application\n");
        #endif

        while (app_ptr->running)
        {
            application_run(app_ptr);
        }

        application_stop(app_ptr);
        application_cleanup(app_ptr);
    }
#ifdef DEBUG
    else
    {
        fprintf(stdout, "Error initializing application\n");
    }
    fprintf(stdout, "Jobs done from main.c\n");
    fflush(stdout);
#endif  
}