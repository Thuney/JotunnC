#include <stdio.h>

#include "application.h"

struct application_t jotunn_app;

int main(int argc, char** argv)
{
    struct application_t* app_ptr = &jotunn_app;

    application_init(app_ptr, "JotunnTestApp");
    application_start(app_ptr);

    while (app_ptr->running)
    {
        application_run(app_ptr);
    }

    application_stop(app_ptr);
    application_cleanup(app_ptr);
}