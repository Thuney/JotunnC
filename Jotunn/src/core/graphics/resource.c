#include "resource_internal.h"

#include "framebuffer.h"
#include "texture.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

/*******************************************************************************
 *
 *   STATE
 *
 ******************************************************************************/

static const char* const resource_kind_names[] =
{
  [RESOURCE_KIND_TEXTURE]        = "texture",
  [RESOURCE_KIND_RENDER_BUFFER]  = "render buffer",
  [RESOURCE_KIND_FRAMEBUFFER]    = "framebuffer",
  [RESOURCE_KIND_VERTEX_BUFFER]  = "vertex buffer",
  [RESOURCE_KIND_ELEMENT_BUFFER] = "element buffer",
  [RESOURCE_KIND_VERTEX_ARRAY]   = "vertex array",
  [RESOURCE_KIND_SHADER_PROGRAM] = "shader program",
};

static_assert(
  (sizeof(resource_kind_names) / sizeof(resource_kind_names[0])) == RESOURCE_KIND_COUNT,
  "Every resource kind needs a name");

// The registry, oldest first
static struct resource_t* resource_first = NULL;
static struct resource_t* resource_last = NULL;

static struct resource_kind_stats_t resource_kind_stats[RESOURCE_KIND_COUNT];

/*******************************************************************************
 *
 *   STATIC PROTOTYPES
 *
 ******************************************************************************/

// Checks that a kind is one of resource_kind_t's
//
//  in:     in_kind
//  ret:    true when in_kind is valid; otherwise false
static bool _resource_is_kind_valid(
  const enum resource_kind_t in_kind);

// Looks for a resource in the registry. Only addresses are compared, so a
//  resource whose links are uninitialized is safely found absent
//
//  in:     in_resource
//  ret:    true when in_resource is in the registry; otherwise false
static bool _resource_is_registered(
  const struct resource_t* in_resource);

// Adds a live resource to its kind's counts
//
//  in:     in_kind, in_size_bytes
static void _resource_count_live(
  const enum resource_kind_t in_kind,
  const size_t in_size_bytes);

// Removes a live resource from its kind's counts
//
//  in:     in_kind, in_size_bytes
static void _resource_count_release(
  const enum resource_kind_t in_kind,
  const size_t in_size_bytes);

/*******************************************************************************
 *
 *   Function: resource_set_name()
 *
 ******************************************************************************/
void resource_set_name(
  struct resource_t* inout_resource,
  const char* in_name)
{
  inout_resource->name = in_name;
}

/*******************************************************************************
 *
 *   Function: resource_get_texture()
 *
 ******************************************************************************/
const struct texture_2d_t* resource_get_texture(
  const struct resource_t* in_resource)
{
  if ((in_resource == NULL) ||
      (in_resource->kind != RESOURCE_KIND_TEXTURE))
  {
    return NULL;
  }

  return (const struct texture_2d_t*)(
    (const char*)in_resource - offsetof(
      struct texture_2d_t,
      resource));
}

/*******************************************************************************
 *
 *   Function: resource_get_framebuffer()
 *
 ******************************************************************************/
const struct framebuffer_t* resource_get_framebuffer(
  const struct resource_t* in_resource)
{
  if ((in_resource == NULL) ||
      (in_resource->kind != RESOURCE_KIND_FRAMEBUFFER))
  {
    return NULL;
  }

  return (const struct framebuffer_t*)(
    (const char*)in_resource - offsetof(
      struct framebuffer_t,
      resource));
}

/*******************************************************************************
 *
 *   Function: resource_get_first()
 *
 ******************************************************************************/
const struct resource_t* resource_get_first(void)
{
  return resource_first;
}

/*******************************************************************************
 *
 *   Function: resource_get_kind_stats()
 *
 ******************************************************************************/
bool resource_get_kind_stats(
  const enum resource_kind_t in_kind,
  struct resource_kind_stats_t* out_stats)
{
  memset(
    out_stats,
    0,
    sizeof(struct resource_kind_stats_t));

  if (!_resource_is_kind_valid(in_kind)) {
    return false;
  }

  *out_stats = resource_kind_stats[in_kind];

  return true;
}

/*******************************************************************************
 *
 *   Function: resource_get_kind_name()
 *
 ******************************************************************************/
const char* resource_get_kind_name(
  const enum resource_kind_t in_kind)
{
  return _resource_is_kind_valid(in_kind) ?
    resource_kind_names[in_kind] :
    "unknown";
}

/*******************************************************************************
 *
 *   Function: resource_log_live()
 *
 ******************************************************************************/
size_t resource_log_live(void)
{
  size_t live_count = 0;

  for (const struct resource_t* resource = resource_first;
       resource != NULL;
       resource = resource->next)
  {
    fprintf(
      stdout,
      "Live graphics resource: %s '%s', %zu bytes\n",
      resource_get_kind_name(resource->kind),
      (resource->name != NULL) ? resource->name : "unnamed",
      resource->size_bytes);

    live_count++;
  }

  return live_count;
}

/*******************************************************************************
 *
 *   Function: resource_internal_register()
 *
 ******************************************************************************/
void resource_internal_register(
  struct resource_t* inout_resource,
  const enum resource_kind_t in_kind,
  const size_t in_size_bytes)
{
  if (!_resource_is_kind_valid(in_kind)) {
    return;
  }

  if (_resource_is_registered(inout_resource)) {
    _resource_count_release(
      inout_resource->kind,
      inout_resource->size_bytes);
  }
  else {
    inout_resource->previous = resource_last;
    inout_resource->next = NULL;

    if (resource_last != NULL) {
      resource_last->next = inout_resource;
    }
    else {
      resource_first = inout_resource;
    }

    resource_last = inout_resource;
    resource_kind_stats[in_kind].total_count++;
  }

  inout_resource->kind = in_kind;
  inout_resource->name = NULL;
  inout_resource->size_bytes = in_size_bytes;

  _resource_count_live(
    in_kind,
    in_size_bytes);
}

/*******************************************************************************
 *
 *   Function: resource_internal_unregister()
 *
 ******************************************************************************/
void resource_internal_unregister(
  struct resource_t* inout_resource)
{
  if (!_resource_is_registered(inout_resource)) {
    return;
  }

  if (inout_resource->previous != NULL) {
    inout_resource->previous->next = inout_resource->next;
  }
  else {
    resource_first = inout_resource->next;
  }

  if (inout_resource->next != NULL) {
    inout_resource->next->previous = inout_resource->previous;
  }
  else {
    resource_last = inout_resource->previous;
  }

  _resource_count_release(
    inout_resource->kind,
    inout_resource->size_bytes);

  inout_resource->previous = NULL;
  inout_resource->next = NULL;
  inout_resource->size_bytes = 0;
}

/*******************************************************************************
 *
 *   Function: resource_internal_set_size()
 *
 ******************************************************************************/
void resource_internal_set_size(
  struct resource_t* inout_resource,
  const size_t in_size_bytes)
{
  if (!_resource_is_registered(inout_resource)) {
    return;
  }

  _resource_count_release(
    inout_resource->kind,
    inout_resource->size_bytes);
  _resource_count_live(
    inout_resource->kind,
    in_size_bytes);

  inout_resource->size_bytes = in_size_bytes;
}

/*******************************************************************************
 *
 *   Function: _resource_is_kind_valid()
 *
 ******************************************************************************/
static bool _resource_is_kind_valid(
  const enum resource_kind_t in_kind)
{
  // An enum may be signed, so a negative kind wraps past the end here too
  return ((unsigned int)in_kind < (unsigned int)RESOURCE_KIND_COUNT);
}

/*******************************************************************************
 *
 *   Function: _resource_is_registered()
 *
 ******************************************************************************/
static bool _resource_is_registered(
  const struct resource_t* in_resource)
{
  for (const struct resource_t* resource = resource_first;
       resource != NULL;
       resource = resource->next)
  {
    if (resource == in_resource) {
      return true;
    }
  }

  return false;
}

/*******************************************************************************
 *
 *   Function: _resource_count_live()
 *
 ******************************************************************************/
static void _resource_count_live(
  const enum resource_kind_t in_kind,
  const size_t in_size_bytes)
{
  struct resource_kind_stats_t* stats = &(resource_kind_stats[in_kind]);

  stats->live_bytes += in_size_bytes;
  stats->live_count++;

  if (stats->live_bytes > stats->peak_live_bytes) {
    stats->peak_live_bytes = stats->live_bytes;
  }
}

/*******************************************************************************
 *
 *   Function: _resource_count_release()
 *
 ******************************************************************************/
static void _resource_count_release(
  const enum resource_kind_t in_kind,
  const size_t in_size_bytes)
{
  struct resource_kind_stats_t* stats = &(resource_kind_stats[in_kind]);

  stats->live_bytes -= in_size_bytes;
  stats->live_count--;
}
