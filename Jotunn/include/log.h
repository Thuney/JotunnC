#pragma once

// Jotunn's log. Messages are written to stdout, and in builds with
//  diagnostics are also kept for the Log tab of the diagnostics overlay

#include <stdarg.h>

/*******************************************************************************
 *
 *   LEVELS
 *
 ******************************************************************************/

enum log_level_t
{
  LOG_LEVEL_DEBUG = 0x00,
  LOG_LEVEL_INFO,
  LOG_LEVEL_WARNING,
  LOG_LEVEL_ERROR,
  LOG_LEVEL_COUNT
};

/*******************************************************************************
 *
 *   MESSAGES
 *
 ******************************************************************************/

// Writes a message, formatted as printf() would, on a line of its own; any
//  trailing newlines in the message are dropped. Messages longer than
//  LOG_MESSAGE_MAX_LENGTH are cut short
//
//  in:     in_level, in_format, ...
void log_write(
  const enum log_level_t in_level,
  const char* in_format,
  ...) __attribute__((format(printf, 2, 3)));

// As log_write(), taking the format's arguments as a va_list
//
//  in:     in_level, in_format, in_arguments
void log_write_va(
  const enum log_level_t in_level,
  const char* in_format,
  va_list in_arguments) __attribute__((format(printf, 2, 0)));

// Names a level for display
//
//  in:     in_level
//  ret:    The level's name, or "unknown" when in_level isn't valid
const char* log_get_level_name(
  const enum log_level_t in_level);

#define LOG_MESSAGE_MAX_LENGTH 1024
