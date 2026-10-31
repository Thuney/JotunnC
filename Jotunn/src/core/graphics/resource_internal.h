#pragma once

// Private to Jotunn: how graphics objects enter and leave the resource
//  registry. Each object's init function registers its resource_t once its
//  graphics memory exists, and its cleanup function unregisters it

#include "resource.h"

#include <stddef.h>

/*******************************************************************************
 *
 *   REGISTRATION
 *
 ******************************************************************************/

// Adds a resource to the registry, clearing its name. A resource already in
//  the registry stays where it is, taking on the new kind and size
//
//  in:     in_kind, in_size_bytes
//  in/out: inout_resource
void resource_internal_register(
  struct resource_t* inout_resource,
  enum resource_kind_t in_kind,
  size_t in_size_bytes);

// Removes a resource from the registry; one not in the registry is ignored
//
//  in/out: inout_resource
void resource_internal_unregister(
  struct resource_t* inout_resource);

// Records a change in the graphics memory a registered resource holds, as
//  when it's resized or given new data; one not in the registry is ignored
//
//  in:     in_size_bytes
//  in/out: inout_resource
void resource_internal_set_size(
  struct resource_t* inout_resource,
  size_t in_size_bytes);
