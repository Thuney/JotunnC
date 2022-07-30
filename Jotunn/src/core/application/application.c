#include "application.h"

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int application_init(struct application_t* app, char* app_name)
{
    #ifdef DEBUG
        fprintf(stdout, "Initializing application\n");
    #endif

    memset(app, 0, sizeof(struct application_t));

    int name_length = strlen(app_name);
    app->name = (char*) malloc(name_length*sizeof(char));
    strcpy(app->name, app_name);

    return window_init(&app->window, 800, 600, "JotunnWindow", app);
}

int application_start(struct application_t* app)
{
    #ifdef DEBUG
        fprintf(stdout, "Starting application\n");
    #endif

    app->running = 1;

    return 0;
}

void application_run(struct application_t* app)
{
    int signaled_close = window_run(&app->window);
    if(signaled_close)
    {
        app->running = 0;

        #ifdef DEBUG
            fprintf(stdout, "Window signaled to close\n");
        #endif
    }
}

int application_stop(struct application_t* app)
{
    #ifdef DEBUG
        fprintf(stdout, "Stopping application\n");
    #endif

    app->running = 0;

    return 0;
}

void application_cleanup(struct application_t* app)
{
    #ifdef DEBUG
        fprintf(stdout, "Cleaning up application\n");
    #endif

    if (app->running) application_stop(app);

    free(app->name);
    app->name = 0;

    window_cleanup(&app->window);
}

void application_on_event(struct application_t* app, struct event_base_t* event)
{
    switch (event->event_type)
    {
        case EVENT_WINDOW_RESIZE:
        {
            // Should be safe cast from base to derived form with base as first element
            struct event_window_resize_t* window_resize_event = (struct event_window_resize_t*)event;

            int new_width, new_height;
            new_width  = window_resize_event->width;
            new_height = window_resize_event->height;

            camera_set_projection_orthographic(&app->window.renderer.camera, 0.0f, (float)new_width, (float)new_height, 0.0f, -1.0f, 100.0f);
        }
        break;

        case EVENT_MOUSE_MOVED:
        {
            // Should be safe cast from base to derived form with base as first element
            struct event_mouse_moved_t* mouse_moved_event = (struct event_mouse_moved_t*)event;

            float x, y;
            x = mouse_moved_event->x;
            y = (app->window.metadata.height - mouse_moved_event->y);

            const float triangle_scale_factor = 50.0f;
            const fvector3 triangle_scale_factors      = (fvector3) { {triangle_scale_factor, triangle_scale_factor, triangle_scale_factor} };
            // const fvector3 triangle_translation_coords = (fvector3) { {400.0f, 500.0f, 0.0f} };

            fmatrix_4x4 triangle_transform_matrix;

            fmatrix_4x4_init(&triangle_transform_matrix);
            triangle_transform_matrix = fmatrix_4x4_transform_scale(&triangle_transform_matrix, triangle_scale_factors);
            triangle_transform_matrix = fmatrix_4x4_transform_translate(&triangle_transform_matrix, (fvector3) { {x, y, 0.0f} });

            app->window.triangle.renderable_data.model_matrix = triangle_transform_matrix;
        }
        break;

        default:
        {

        }
        break;
    }
}