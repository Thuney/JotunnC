#include "log.h"

#include "../diagnostics/diagnostics_internal.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

/*******************************************************************************
 *
 *   STATE
 *
 ******************************************************************************/

static const char* const log_level_names[] =
{
  [LOG_LEVEL_DEBUG]   = "debug",
  [LOG_LEVEL_INFO]    = "info",
  [LOG_LEVEL_WARNING] = "warning",
  [LOG_LEVEL_ERROR]   = "error",
};

static_assert(
  (sizeof(log_level_names) / sizeof(log_level_names[0])) == LOG_LEVEL_COUNT,
  "Every log level needs a name");

/*******************************************************************************
 *
 *   Function: log_write()
 *
 ******************************************************************************/
void log_write(
  const enum log_level_t in_level,
  const char* in_format,
  ...)
{
  va_list arguments;
  va_start(
    arguments,
    in_format);

  log_write_va(
    in_level,
    in_format,
    arguments);

  va_end(arguments);
}

/*******************************************************************************
 *
 *   Function: log_write_va()
 *
 ******************************************************************************/
void log_write_va(
  const enum log_level_t in_level,
  const char* in_format,
  va_list in_arguments)
{
  char message[LOG_MESSAGE_MAX_LENGTH];
  vsnprintf(
    message,
    sizeof(message),
    in_format,
    in_arguments);

  // Every message gets exactly one line ending, wherever it came from
  size_t length = strlen(message);
  while ((length > 0) &&
         (message[length - 1] == '\n'))
  {
    length--;
    message[length] = '\0';
  }

#ifdef JOTUNN_DIAGNOSTICS
  diagnostics_internal_log_record(
    in_level,
    message);
#else
  // Without the diagnostics' Log tab, stdout is the only place to show it
  (void)in_level;

  fprintf(
    stdout,
    "%s\n",
    message);
#endif
}

/*******************************************************************************
 *
 *   Function: log_get_level_name()
 *
 ******************************************************************************/
const char* log_get_level_name(
  const enum log_level_t in_level)
{
  // An enum may be signed, so a negative level wraps past the end here too
  return ((unsigned int)in_level < (unsigned int)LOG_LEVEL_COUNT) ?
    log_level_names[in_level] :
    "unknown";
}
