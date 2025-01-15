#include "ui_layer_clay.h"

// UI layout library
#define CLAY_IMPLEMENTATION
#include "clay.h"

static void* clay_arena_memory_location = NULL;
static uint64_t clay_arena_memory_size_bytes = 0UL;

static struct typeface_t* clay_ui_layer_typeface = NULL;

//
static inline Clay_Dimensions ui_layer_clay_measure_text(Clay_String *str, Clay_TextElementConfig *config) 
{
	const fvector2 string_dimensions = 
    typeface_calculate_string_dimensions(clay_ui_layer_typeface, str->chars, str->length);

	// Return dimensions
	return (Clay_Dimensions)
  {
		.width =  (float) string_dimensions.comp.x,
		.height = (float) string_dimensions.comp.y
	};
}

//
static void ui_layer_clay_error_handler(Clay_ErrorData error_data) 
{
  fprintf(stderr, "CLAY error: %s\n", error_data.errorText.chars);
}

//
static void ui_layer_clay_render(struct ui_layer_clay_t* ui_layer_clay)
{
  struct renderer_2d_t* renderer = 
    &(ui_layer_clay->ui_renderer_2d); 

  // Get render commands from UI layer
  const Clay_RenderCommandArray* render_commands = 
    (const Clay_RenderCommandArray*)ui_layer_clay->function_clay_ui_create_layout(ui_layer_clay);

  for (uint32_t i = 0; i < render_commands->length; i++)
  {
    Clay_RenderCommand *renderCommand = Clay_RenderCommandArray_Get(render_commands, i);
    Clay_BoundingBox boundingBox = renderCommand->boundingBox;
    
    switch (renderCommand->commandType)
    {
      case CLAY_RENDER_COMMAND_TYPE_RECTANGLE:
      {
        Clay_RectangleElementConfig *config = renderCommand->config.rectangleElementConfig;
        Clay_Color color = config->color;

        fmatrix_4x4 transform_matrix;
        {
            fmatrix_4x4 scale_matrix, translation_matrix;

            const fvector3 scale_factors = (fvector3) 
              {
                {   
                  boundingBox.width,
                  boundingBox.height,
                  1.0 
                } 
              };

            // const fvector3 translation_vector = (fvector3) 
            //   { 
            //     {   
            //       boundingBox.x, 
            //       (renderer->base.parent_window->metadata.height - boundingBox.y),
            //       0.0f
            //     }
            //   };

            const fvector3 translation_vector = (fvector3) 
              { 
                {   
                  boundingBox.x, 
                  boundingBox.y,
                  0.0f
                }
              };

            fmatrix_4x4_init(&scale_matrix);
            fmatrix_4x4_init(&translation_matrix);
            fmatrix_4x4_init(&transform_matrix);

            scale_matrix = fmatrix_4x4_transform_scale(&scale_matrix, scale_factors);
            translation_matrix = fmatrix_4x4_transform_translate(&translation_matrix, translation_vector);
            transform_matrix = fmatrix_4x4_multiply(&scale_matrix, &translation_matrix);
        }

        const fvector4 rect_color = 
        {
          .comp = 
            {
              .w = ((float)color.r / 255.0f),
              .x = ((float)color.g / 255.0f),
              .y = ((float)color.b / 255.0f),
              .z = ((float)color.a / 255.0f)
            }
        };

        renderer_2d_draw_quad(renderer, &transform_matrix, rect_color);
      }
      break;

      case CLAY_RENDER_COMMAND_TYPE_TEXT:
      {
        Clay_TextElementConfig *config = renderCommand->config.textElementConfig;
        Clay_String text = renderCommand->text;

        // char *cloned = (char *)calloc(text.length + 1, 1);
        // memcpy(cloned, text.chars, text.length);
        // TTF_Font* font = SDL2_fonts[config->fontId].font;
        // SDL_Surface *surface = TTF_RenderUTF8_Blended(font, cloned, (SDL_Color) 
        // {
        //   .r = (Uint8)config->textColor.r,
        //   .g = (Uint8)config->textColor.g,
        //   .b = (Uint8)config->textColor.b,
        //   .a = (Uint8)config->textColor.a,
        // });
        // SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

        // SDL_Rect destination = (SDL_Rect)
        // {
        //   .x = boundingBox.x,
        //   .y = boundingBox.y,
        //   .w = boundingBox.width,
        //   .h = boundingBox.height,
        // };
        // SDL_RenderCopy(renderer, texture, NULL, &destination);

        // SDL_DestroyTexture(texture);
        // SDL_FreeSurface(surface);
        // free(cloned);

        renderer_2d_draw_string(renderer, ui_layer_clay->ui_layer_typeface, (fvector3){ boundingBox.x, boundingBox.y, 0.0f }, text.chars);
      }
      break;

      case CLAY_RENDER_COMMAND_TYPE_SCISSOR_START: 
      {
        // currentClippingRectangle = 
        //   (SDL_Rect) 
        //   {
        //     .x = boundingBox.x,
        //     .y = boundingBox.y,
        //     .w = boundingBox.width,
        //     .h = boundingBox.height,
        //   };
        // SDL_RenderSetClipRect(renderer, &currentClippingRectangle);
      }
      break;

      case CLAY_RENDER_COMMAND_TYPE_SCISSOR_END: 
      {
        // SDL_RenderSetClipRect(renderer, NULL);
      }
      break;

      default: 
      {
        fprintf(stderr, "Error: unhandled render command: %d\n", renderCommand->commandType);
        exit(1);
      }
    }
  }
}

//
static void ui_layer_clay_run(struct window_layer_t* window_layer)
{
  struct ui_layer_clay_t* ui_layer_clay = (struct ui_layer_clay_t*)window_layer;

  ui_layer_clay_render(ui_layer_clay);
}

//
void ui_layer_clay_init(struct window_t* parent_window,
                        struct ui_layer_clay_t* ui_layer_clay,
                        struct typeface_t* ui_layer_typeface,
                        uint32_t width,
                        uint32_t height,
                        const void* (*function_clay_ui_create_layout)(struct ui_layer_clay_t* ui_layer_clay))
{
  memset(ui_layer_clay, 0, sizeof(struct ui_layer_clay_t));
  
  framebuffer_init(&ui_layer_clay->ui_framebuffer, width, height);

  // Camera stuff
  const fvector3 camera_position = (fvector3) { {0.0f, 0.0f,  2.0f} };
  const fvector3 camera_up       = (fvector3) { {0.0f, 1.0f,  0.0f} };
  const fvector3 camera_front    = (fvector3) { {0.0f, 0.0f, -1.0f} };

  const float ortho_left   = 0.0f; 
  const float ortho_right  = (float)width;
  const float ortho_top    = 0.0f;
  const float ortho_bottom = (float)height;
  const float ortho_near_plane = -3.0f;
  const float ortho_far_plane  = 100.0f;

  camera_init_orthographic(&(ui_layer_clay->ui_camera_ortho), camera_position, camera_up, camera_front);
  camera_set_projection_orthographic(&(ui_layer_clay->ui_camera_ortho), ortho_left, ortho_right, ortho_top, ortho_bottom, ortho_near_plane, ortho_far_plane);

  renderer_2d_init(&(ui_layer_clay->ui_renderer_2d), &(ui_layer_clay->ui_camera_ortho).base, parent_window, "ui_renderer_2d");

  window_layer_init(parent_window, &ui_layer_clay->ui_window_layer, &ui_layer_clay->ui_framebuffer, &ui_layer_clay->ui_camera_ortho.base, &ui_layer_clay->ui_renderer_2d.base);
  window_layer_set_custom_layer_run(&ui_layer_clay->ui_window_layer, ui_layer_clay_run);
  
  ui_layer_clay->ui_layer_typeface = ui_layer_typeface;
  clay_ui_layer_typeface = ui_layer_typeface;

  ui_layer_clay->function_clay_ui_create_layout = function_clay_ui_create_layout;
  
  //

  clay_arena_memory_size_bytes = Clay_MinMemorySize();
  clay_arena_memory_location = malloc(clay_arena_memory_size_bytes);

  Clay_Arena arena = Clay_CreateArenaWithCapacityAndMemory(clay_arena_memory_size_bytes, clay_arena_memory_location);

  Clay_Initialize(arena, 
                  (Clay_Dimensions) { width, height }, 
                  (Clay_ErrorHandler) { ui_layer_clay_error_handler });

  Clay_SetMeasureTextFunction(ui_layer_clay_measure_text);
}

void ui_layer_clay_cleanup(struct ui_layer_clay_t* ui_layer_clay)
{
  free(clay_arena_memory_location);
  clay_arena_memory_size_bytes = 0UL;

  renderer_2d_cleanup(&(ui_layer_clay->ui_renderer_2d));
  framebuffer_cleanup(&(ui_layer_clay->ui_framebuffer));
}