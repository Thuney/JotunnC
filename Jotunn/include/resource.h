#pragma once

// Every object holding graphics memory (textures, buffers, framebuffers and
//  the like) embeds a resource_t, through which Jotunn keeps a registry of
//  those alive: how many of each kind there are, roughly how much graphics
//  memory they hold, and a list of them to walk for display or for a leak
//  report. Sizes are estimated from each object's dimensions, so drivers that
//  pad or compress will differ. The registry assumes a single thread

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct framebuffer_t;
struct texture_2d_t;

/*******************************************************************************
 *
 *   KINDS
 *
 ******************************************************************************/

enum resource_kind_t
{
  RESOURCE_KIND_TEXTURE = 0x00,
  RESOURCE_KIND_RENDER_BUFFER,
  RESOURCE_KIND_FRAMEBUFFER,
  RESOURCE_KIND_VERTEX_BUFFER,
  RESOURCE_KIND_ELEMENT_BUFFER,
  RESOURCE_KIND_VERTEX_ARRAY,
  RESOURCE_KIND_SHADER_PROGRAM,
  RESOURCE_KIND_COUNT
};

// The resources of one kind
struct resource_kind_stats_t
{
  size_t live_bytes;
  size_t peak_live_bytes;
  uint64_t live_count;
  // Every resource of the kind registered since startup
  uint64_t total_count;
};

/*******************************************************************************
 *
 *   RESOURCES
 *
 ******************************************************************************/

// Embedded in each graphics object, whose init and cleanup functions add it
//  to and remove it from the registry
struct resource_t
{
  enum resource_kind_t kind;
  // Not owned, so it must outlive the resource; NULL when unnamed
  const char* name;
  // Estimated graphics memory held
  size_t size_bytes;
  // Neighbours in the registry, oldest first
  struct resource_t* previous;
  struct resource_t* next;
};

// Names a registered resource for display. Registering a resource clears its
//  name, so name it after its object's init function
//
//  in:     in_name
//  in/out: inout_resource
void resource_set_name(
  struct resource_t* inout_resource,
  const char* in_name);

// Finds the texture a resource is embedded in
//
//  in:     in_resource
//  ret:    The texture when in_resource is a RESOURCE_KIND_TEXTURE; otherwise
//            NULL
const struct texture_2d_t* resource_get_texture(
  const struct resource_t* in_resource);

// Finds the framebuffer a resource is embedded in
//
//  in:     in_resource
//  ret:    The framebuffer when in_resource is a RESOURCE_KIND_FRAMEBUFFER;
//            otherwise NULL
const struct framebuffer_t* resource_get_framebuffer(
  const struct resource_t* in_resource);

/*******************************************************************************
 *
 *   REGISTRY
 *
 ******************************************************************************/

// Starts a walk of the live resources, continued through each one's next
//
//  ret:    The oldest live resource; NULL when none are alive
const struct resource_t* resource_get_first(void);

// Reads the counts for one kind of resource
//
//  in:     in_kind
//  out:    out_stats
//  ret:    true when in_kind is valid; otherwise false, with out_stats zeroed
bool resource_get_kind_stats(
  const enum resource_kind_t in_kind,
  struct resource_kind_stats_t* out_stats);

// Names a kind of resource for display
//
//  in:     in_kind
//  ret:    The kind's name, or "unknown" when in_kind isn't valid
const char* resource_get_kind_name(
  const enum resource_kind_t in_kind);

// Prints every live resource to stdout, as a leak report once everything
//  should have been cleaned up
//
//  ret:    The number of live resources printed
size_t resource_log_live(void);
