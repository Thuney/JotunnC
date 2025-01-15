#include "ball_ui_clay.h"

#include "font.h"

#include "clay.h"

static struct typeface_t ball_ui_clay_typeface;

static Clay_Color COLOR_LIGHT  = (Clay_Color) { 224, 215, 210, 200 };
static Clay_Color COLOR_RED    = (Clay_Color) { 168,  66,  28, 200 };
static Clay_Color COLOR_ORANGE = (Clay_Color) { 225, 138,  50, 200 };

Clay_LayoutConfig sidebarItemLayout = (Clay_LayoutConfig) 
{
  .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(50) },
};

// Re-usable components are just normal functions
void SidebarItemComponent() 
{
  CLAY(CLAY_LAYOUT(sidebarItemLayout), CLAY_RECTANGLE({ .color = COLOR_ORANGE })) {}
}

static Clay_LayoutConfig layoutElement = (Clay_LayoutConfig) { .padding = {5} };

static const void* function_clay_ui_create_layout(struct ui_layer_clay_t* ui_layer_clay)
{
  static Clay_RenderCommandArray render_commands = {0};

  Clay_BeginLayout();

  // Do layout things here
  {
    // An example of laying out a UI with a fixed width sidebar and flexible width main content
    CLAY(
      CLAY_ID("OuterContainer"),
      CLAY_LAYOUT(
        { 
          .sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0)}, 
          .padding = {16, 16}, 
          .childGap = 16 
        }), 
        CLAY_RECTANGLE(
        { 
          .color = { 0, 255, 0, 200 },
        })
    )
    {
      CLAY(CLAY_ID("SideBar"),
           CLAY_LAYOUT({ .layoutDirection = CLAY_TOP_TO_BOTTOM, .sizing = { .width = CLAY_SIZING_FIXED(300), .height = CLAY_SIZING_GROW(0) }, .padding = {16, 16}, .childGap = 16 }),
           CLAY_RECTANGLE({ .color = COLOR_LIGHT }))
      {
        CLAY(
          CLAY_ID("ProfilePictureOuter"), 
          CLAY_LAYOUT(
            { 
              .sizing = { .width = CLAY_SIZING_GROW(0) }, 
              .padding = {16, 16}, 
              .childGap = 16, 
              .childAlignment = 
              { 
                .y = CLAY_ALIGN_Y_CENTER 
              }
            }
          ),
          CLAY_RECTANGLE({ .color = COLOR_RED })
        )
        {
          CLAY(CLAY_ID("ProfilePicture"), CLAY_LAYOUT({ .sizing = { .width = CLAY_SIZING_FIXED(60), .height = CLAY_SIZING_FIXED(60) }}), CLAY_RECTANGLE({ .color = COLOR_RED })) {}
          CLAY_TEXT(CLAY_STRING("Clay - UI Library"), CLAY_TEXT_CONFIG({ .fontSize = 18, .textColor = {255, 255, 255, 255} }));
        }

        // Standard C code like loops etc work inside components
        for (int i = 0; i < 5; i++) 
        {
          SidebarItemComponent();
        }
      }

      CLAY(CLAY_ID("MainContent"), CLAY_LAYOUT({ .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) }}), CLAY_RECTANGLE({ .color = COLOR_LIGHT })) {}
    }
  }

  // CLAY(CLAY_ID("MainContent"), CLAY_LAYOUT({ .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) }, .padding = {16, 16} }), CLAY_RECTANGLE({ .color = { 0, 255, 0, 255 } })) {}

  render_commands = Clay_EndLayout();
  return (const void*)&render_commands;
}

//
void ball_ui_layer_clay_init(struct ball_ui_layer_clay_t* ball_layer_clay, uint16_t width, uint16_t height, struct window_t* parent_window)
{
  typeface_init(&ball_ui_clay_typeface, "/usr/share/fonts/noto/NotoSansMono-Regular.ttf", 18);

  ui_layer_clay_init(parent_window, &(ball_layer_clay->ui_layer), &ball_ui_clay_typeface, width, height, function_clay_ui_create_layout);
}

void ball_ui_layer_clay_cleanup(struct ball_ui_layer_clay_t* ball_layer_clay)
{
  ui_layer_clay_cleanup(&(ball_layer_clay->ui_layer));

  typeface_cleanup(&ball_ui_clay_typeface);
}