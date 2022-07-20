#pragma once

struct window_data_t
{
    char* tag;

    int width;
    int height;

    int signaled_close;
};

struct window_context_data_t
{
    void* window_handle;
};

struct window_t
{
    struct window_data_t metadata;
    struct window_context_data_t context_data;
};

int window_init(struct window_t* window, int width, int height, char* tag);

int window_run(struct window_t* window);

void window_cleanup(struct window_t* window);