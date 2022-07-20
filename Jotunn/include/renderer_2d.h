#pragma once

struct renderer_2d_t
{
   char* tag;
};

void renderer_2d_init(struct renderer_2d_t* renderer, char* tag);
void renderer_2d_cleanup(struct renderer_2d_t* renderer);

void renderer_2d_begin(struct renderer_2d_t* renderer);
void renderer_2d_submit(struct renderer_2d_t* renderer);
void renderer_2d_end(struct renderer_2d_t* renderer);