// ----------------------------------------------------------------------------
// vi-log.h - v1.0 - public domain
//
// Part of a headers-only library for logging.
//
// GENERAL INFO:
//  This library uses two mechanics to work:
//
//  1. STB-like implementation (https://github.com/nothings/stb):
//     Header file contains both declarations and definitions.
//
//     Declarations are accessable at all times just by including
//     this header.
//
//     Definitions are guarded by the preprocessor condition:
//       #if defined(VI_LIB_IMPLEMENTATION) || defined(VI_LOG_IMPLEMENTATION)
//
//     Which means that using this library requires "generating" library
//     implementation in one of the source files (*.c) like so:
//       #define VI_LOG_IMPLEMENTATION
//       #include <vi-log.h>
//
//  2. Auto-configuration:
//     Header-only nature of this library opens possibility
//     for compile-time configuration via feature-test-macros.
//
//     To exploit this mechanic user needs to introduce
//     feature-test-macros before including this file.
//
//     There are many ways to do so, I'll describe some of them:
//     1) During compilation:
//       gcc main.c -o main -DVI_LOG_LEVEL="VI_LOG_LEVEL_INF"
//
//     2) Before inclusion of this file:
//       #define VI_LOG_LEVEL VI_LOG_LEVEL_INF
//       #include <vi-log.h>
//
//     3) Auto-inclusion of a 'configuration' header while including this file:
//        This method has two ways of work:
//          a) Library checks exsitence of a header named "vi-log-config.h"
//             via `__has_include()` macro. And, if present, includes it
//             before configuration step in this header.
//
//          b) Library checks if `VI_LOG_CONFIG_LOCAL` macro is defined
//             and if it is includes it like so:
//               #include VI_LOG_CONFIG_LOCAL
//
//        Method 'b' takes priority over method 'a'.
//
// STRUCTURE:
//  1. vi-log.h
//     'Main' header with configuration, declarations and definitions.
//
//  2. vi-log-common.h
//     Contains common definitions that can be used
//     in a custom 'configuration' header and in the 'main' header.
//
//  3. vi-log-config-template.h
//     Contains all feature-test-macros definitions and their explanations
//     that can be used to configure library.
//
// USAGE:
//  1. Copy vi-log.h and vi-log-common.h in a desirable place
//     where your compiler can find them.
//
//  2. Provide feature-test-macros for auto-configuration using
//     one of the methods mentioned above.
//
//  3. Include vi-log.h and "generate" it's implementation
//     in on of the *.c files.
//
//  4. Setup logging agents:
//     1) Manually add custom agents using runtime function:
//        `vi_log_agent_add()`
//
//     2) Automagically add out-of-box agents using feature macros:
//        `VI_LOG_AGENT_STDERR` and/or `VI_LOG_AGENT_SYSLOG`
//
//  5. Initialize library:
//     1) Manually using runtime function:
//        `vi_log_init()`
//
//     2) Automagically using feature macro:
//        `VI_LOG_INITFINI_ARRAY`
//
//  5. Use vi-log.h throughout your project:
//     1) With direct function calls:
//        `vi_log_exec()`
//        `vi_log_vexec()`
//
//     2) With helper macros:
//        `vi_log_<lvl>()`
//
// NOTE_1: step 2 is optional.
//  But logger has no agents to work with by default. And if you desided
//  that no out-of-box agents can solve your problem, you need to provide
//  at least one custom-made via `vi_log_agent_add()`.
//  There's a maximum count of 4 agents for this case, that can be overwritten
//  by VI_LOG_AGENTS_EXT_COUNT feature-macro.
//
// ----------------------------------------------------------------------------

#ifndef __VI_LOG_H
#define __VI_LOG_H

#if 1  /* >> configuration */

#include "vi-log-common.h"

#define VI_LOG_CONFIG_LOCAL_DEFAULT "vi-log-config.h"

#define HAVE_VI_LOG_CONFIG_LOCAL_DEFAULT 0
#ifdef __has_include
  #if __has_include(VI_LOG_CONFIG_LOCAL_DEFAULT)
    #undef  HAVE_VI_LOG_CONFIG_LOCAL_DEFAULT
    #define HAVE_VI_LOG_CONFIG_LOCAL_DEFAULT 1
  #endif
#endif

#define USE_VI_LOG_CONFIG_TYPE 0
#ifdef VI_LOG_CONFIG_LOCAL
  #include VI_LOG_CONFIG_LOCAL
  #undef  USE_VI_LOG_CONFIG_TYPE
  #define USE_VI_LOG_CONFIG_TYPE 1
#elif HAVE_VI_LOG_CONFIG_LOCAL_DEFAULT
  #include VI_LOG_CONFIG_LOCAL_DEFAULT
  #undef  USE_VI_LOG_CONFIG_TYPE
  #define USE_VI_LOG_CONFIG_TYPE 2
#endif

#ifdef VI_LOG_LEVEL
  #if (VI_LOG_LEVEL_OFF > VI_LOG_LEVEL) || (VI_LOG_LEVEL > VI_LOG_LEVEL_TRC)
    #error "Provided VI_LOG_LEVEL is OOB [VI_LOG_LEVEL_OFF..VI_LOG_LEVEL_TRC]"
  #else
    #define USE_VI_LOG_LEVEL VI_LOG_LEVEL
  #endif
#else
  #ifdef NDEBUG
    #define USE_VI_LOG_LEVEL VI_LOG_LEVEL_ERR
  #else
    #define USE_VI_LOG_LEVEL VI_LOG_LEVEL_DBG
  #endif
#endif

#ifdef VI_LOG_LEVEL_LIMIT
  #if (VI_LOG_LEVEL_OFF > VI_LOG_LEVEL_LIMIT) || (VI_LOG_LEVEL_LIMIT > VI_LOG_LEVEL_TRC)
    #error "Provided VI_LOG_LEVEL_LIMIT is OOB [VI_LOG_LEVEL_OFF..VI_LOG_LEVEL_TRC]"
  #else
    #define USE_VI_LOG_LEVEL_LIMIT VI_LOG_LEVEL_LIMIT
  #endif
#else
  #define USE_VI_LOG_LEVEL_LIMIT VI_LOG_LEVEL_TRC
#endif

#if (USE_VI_LOG_LEVEL > USE_VI_LOG_LEVEL_LIMIT)
  #error "USE_VI_LOG_LEVEL can't be greater than USE_VI_LOG_LEVEL_LIMIT"
#endif

#if defined(VI_LOG_AGENT_STDERR) && (VI_LOG_AGENT_STDERR)
  #define USE_VI_LOG_AGENT_STDERR 1
#else
  #define USE_VI_LOG_AGENT_STDERR 0
#endif

#ifdef VI_LOG_AGENT_SYSLOG
  #define USE_VI_LOG_AGENT_SYSLOG 1
#else
  #define USE_VI_LOG_AGENT_SYSLOG 0
#endif

#define USE_VI_LOG_AGENTS_LIB_COUNT USE_VI_LOG_AGENT_STDERR + USE_VI_LOG_AGENT_SYSLOG

#if defined(VI_LOG_AGENTS_EXT_COUNT) && (VI_LOG_AGENTS_EXT_COUNT > 0)
  #define USE_VI_LOG_AGENTS_COUNT (USE_VI_LOG_AGENTS_LIB_COUNT + VI_LOG_AGENTS_EXT_COUNT)
#else
  #if USE_VI_LOG_AGENTS_LIB_COUNT
    #define USE_VI_LOG_AGENTS_COUNT (USE_VI_LOG_AGENTS_LIB_COUNT)
  #else
    #define USE_VI_LOG_AGENTS_COUNT (4)
  #endif
#endif

#if defined(VI_LOG_MUTEX_PTHREAD) && (VI_LOG_MUTEX_PTHREAD)
  #define USE_VI_LOG_MUTEX_PTHREAD 1
#else
  #define USE_VI_LOG_MUTEX_PTHREAD 0
#endif

#if defined(VI_LOG_INITFINI_ARRAY) && (VI_LOG_INITFINI_ARRAY)
  #define USE_VI_LOG_INITFINI_ARRAY 1
#else
  #define USE_VI_LOG_INITFINI_ARRAY 0
#endif

#if USE_VI_LOG_AGENT_STDERR
  #ifdef VI_LOG_AGENT_STDERR_FORMAT
    #define USE_VI_LOG_AGENT_STDERR_FORMAT VI_LOG_AGENT_STDERR_FORMAT
  #else
    #define USE_VI_LOG_AGENT_STDERR_FORMAT \
      VI_LOG_FMTARR_LINE(                  \
        VI_LOG_FMTSYM_MESSAGE_TIME,        \
        ' ',                               \
        VI_LOG_FMTSYM_MESSAGE_LEVEL_STR,   \
        ' ',                               \
        VI_LOG_FMTSYM_PROCESS_NAME,        \
        '[',                               \
        VI_LOG_FMTSYM_PROCESS_ID,          \
        ']',                               \
        ':',                               \
        ' ',                               \
        '[',                               \
        VI_LOG_FMTSYM_SOURCE_FILE_NAME,    \
        ':',                               \
        VI_LOG_FMTSYM_SOURCE_FILE_LINE,    \
        ']',                               \
        ':',                               \
        ' ',                               \
        VI_LOG_FMTSYM_MESSAGE_CONTENT      \
      )
  #endif
#endif

#if USE_VI_LOG_AGENT_SYSLOG
  #ifdef VI_LOG_AGENT_SYSLOG_OPTIONS
    #define USE_VI_LOG_AGENT_SYSLOG_OPTIONS (VI_LOG_AGENT_SYSLOG_OPTIONS)
  #else
    #define USE_VI_LOG_AGENT_SYSLOG_OPTIONS (LOG_PID | LOG_NDELAY | (USE_VI_LOG_AGENT_STDERR ? 0 : LOG_PERROR))
  #endif
  #ifdef VI_LOG_AGENT_SYSLOG_FACILITY
    #define USE_VI_LOG_AGENT_SYSLOG_FACILITY VI_LOG_AGENT_SYSLOG_FACILITY
  #else
    #define USE_VI_LOG_AGENT_SYSLOG_FACILITY LOG_USER
  #endif
#endif

#ifdef VI_LOG_FMTSYM_MESSAGE_TIME_FORMAT
  #define USE_VI_LOG_FMTSYM_MESSAGE_TIME_FORMAT VI_LOG_FMTSYM_MESSAGE_TIME_FORMAT
#else
  #define USE_VI_LOG_FMTSYM_MESSAGE_TIME_FORMAT "%h %e %T"
#endif

#endif /* << configuration */
#if 1  /* >> declarations */

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>

typedef uint32_t vi_log_level_t;

typedef struct {
  vi_log_level_t level;
  bool           quiet;
  bool           no_stderr_agent;
  bool           no_syslog_agent;
  bool           no_pthread_mutex;
} vi_log_config_t;

static inline vi_log_config_t __attribute__((always_inline))
vi_log_config_default()
{
  return (vi_log_config_t) {
    .level            = USE_VI_LOG_LEVEL,
    .quiet            = false,
    .no_stderr_agent  = false,
    .no_syslog_agent  = false,
    .no_pthread_mutex = false
  };
}

typedef struct {
  void  *user;
  int  (*lock)   (void *user);
  int  (*unlock) (void *user);
} vi_log_mutex_t;

typedef struct {
  int         id;
  char const *name;
} vi_log_process_info_t;

typedef struct {
  char const *file_name;
  int         file_line;
  char const *function_name;
} vi_log_source_location_t;

static inline vi_log_source_location_t __attribute__((always_inline))
__vi_log_source_location(char const *file_name, int file_line, char const *function_name)
{
  return (vi_log_source_location_t) {
    .file_name     = file_name,
    .file_line     = file_line,
    .function_name = function_name
  };
}

#define vi_log_source_location() \
  __vi_log_source_location(__FILE__, __LINE__, __FUNCTION__)

typedef struct {
  time_t                time;
  vi_log_level_t        level;
  char           const *format;
  va_list               arguments;
} vi_log_message_t;

typedef struct {
  vi_log_process_info_t    const *process_info;
  vi_log_source_location_t const *source_location;
  vi_log_message_t                message;
} vi_log_query_t;

typedef struct {
  void  *user;
  void (*init) (void *user, vi_log_process_info_t const *process_info);
  void (*exec) (void *user, vi_log_query_t        const *query);
  void (*fini) (void *user);
} vi_log_agent_t;

int
vi_log_init(char const *pexec, vi_log_config_t const *config);

void
vi_log_fini();

int
vi_log_vexec(vi_log_source_location_t source_location, vi_log_level_t level, char const *format, va_list arguments)
  __attribute__((format(printf, 3, 0)));

int
vi_log_exec(vi_log_source_location_t source_location, vi_log_level_t level, char const *format, ...)
  __attribute__((format(printf, 3, 4)));

int
vi_log_agent_add(vi_log_agent_t const *agent);

int
vi_log_mutex_set(vi_log_mutex_t const *mutex);

vi_log_level_t
vi_log_level_limit();

vi_log_level_t
vi_log_level();

int
vi_log_level_set(vi_log_level_t level);

bool
vi_log_quiet();

void
vi_log_quiet_set(bool quiet);

bool
vi_log_ready();

#ifndef NDEBUG
void
vi_log_config_dump();
#else
static inline void
vi_log_config_dump() {}
#endif

#endif /* << declarations */
#if 1  /* >> utils */

#ifndef vi_discard
static inline __attribute__((__always_inline__)) int
__vi_discard(int x, ...)
{
  return x;
}
#define vi_discard(...) __vi_discard(0, __VA_ARGS__)
#endif

#ifndef vi_cstr_bool
#define vi_cstr_bool(x) ((x) ? "true" : "false")
#endif

#endif /* << utils */
#if 1  /* >> marco-interface */

#define vi_log(level, format, ...) \
  vi_log_exec(vi_log_source_location(), level, format, ##__VA_ARGS__)

#if VI_LOG_LEVEL_ERR <= USE_VI_LOG_LEVEL_LIMIT
  #define vi_log_err(format, ...) vi_log(VI_LOG_LEVEL_ERR, format, ##__VA_ARGS__)
#else
  #define vi_log_err(...)         vi_discard(__VA_ARGS__)
#endif
#if VI_LOG_LEVEL_WRN <= USE_VI_LOG_LEVEL_LIMIT
  #define vi_log_wrn(format, ...) vi_log(VI_LOG_LEVEL_WRN, format, ##__VA_ARGS__)
#else
  #define vi_log_wrn(...)         vi_discard(__VA_ARGS__)
#endif
#if VI_LOG_LEVEL_INF <= USE_VI_LOG_LEVEL_LIMIT
  #define vi_log_inf(format, ...) vi_log(VI_LOG_LEVEL_INF, format, ##__VA_ARGS__)
#else
  #define vi_log_inf(...)         vi_discard(__VA_ARGS__)
#endif
#if VI_LOG_LEVEL_DBG <= USE_VI_LOG_LEVEL_LIMIT
  #define vi_log_dbg(format, ...) vi_log(VI_LOG_LEVEL_DBG, format, ##__VA_ARGS__)
#else
  #define vi_log_dbg(...)         vi_discard(__VA_ARGS__)
#endif
#if VI_LOG_LEVEL_TRC <= USE_VI_LOG_LEVEL_LIMIT
  #define vi_log_trc(format, ...) vi_log(VI_LOG_LEVEL_TRC, format, ##__VA_ARGS__)
#else
  #define vi_log_trc(...)         vi_discard(__VA_ARGS__)
#endif

#endif /* << marco-interface */
#if 1  /* >> implementation */
#if defined(VI_LIB_IMPLEMENTATION) || defined(VI_LOG_IMPLEMENTATION)
#include <assert.h>
#include <string.h>
#include <unistd.h> /* getpid() */

static inline char const * __attribute__((always_inline))
vi_log_level2s(vi_log_level_t level)
{
  static char const *lookup[] = {
    [VI_LOG_LEVEL_ERR] = "err",
    [VI_LOG_LEVEL_WRN] = "wrn",
    [VI_LOG_LEVEL_INF] = "inf",
    [VI_LOG_LEVEL_DBG] = "dbg",
    [VI_LOG_LEVEL_TRC] = "trc",
  };

  assert(level < sizeof(lookup) / sizeof(0[lookup]));
  return lookup[level];
}

#if USE_VI_LOG_AGENT_STDERR
#include <stdlib.h>
#include <stdio.h>
#include <stdio_ext.h>
#include <time.h>
#include <locale.h>

typedef void (* vi_log_format_exec_t)(FILE *, vi_log_query_t const *);

static inline void
vi_log_format_process_info_id(FILE *stream, vi_log_query_t const *query)
{
  fprintf(stream, "%d", query->process_info->id);
}

static inline void
vi_log_format_process_info_name(FILE *stream, vi_log_query_t const *query)
{
  fputs(query->process_info->name, stream);
}

static inline void
vi_log_format_source_location_file_name(FILE *stream, vi_log_query_t const *query)
{
  fputs(query->source_location->file_name, stream);
}

static inline void
vi_log_format_source_location_file_line(FILE *stream, vi_log_query_t const *query)
{
  fprintf(stream, "%d", query->source_location->file_line);
}

static inline void
vi_log_format_source_location_function_name(FILE *stream, vi_log_query_t const *query)
{
  fputs(query->source_location->function_name, stream);
}

static inline void
vi_log_format_message_time(FILE *stream, vi_log_query_t const *query)
{
  struct tm message_time_tm;
  stream->_IO_write_ptr +=
    strftime(stream->_IO_write_ptr,
             stream->_IO_write_end - stream->_IO_write_ptr,
             USE_VI_LOG_FMTSYM_MESSAGE_TIME_FORMAT,
             localtime_r(&query->message.time, &message_time_tm));
}

static inline void
vi_log_format_message_level_num(FILE *stream, vi_log_query_t const *query)
{
  fprintf(stream, "%u", query->message.level);
}

static inline void
vi_log_format_message_level_str(FILE *stream, vi_log_query_t const *query)
{
  fputs(vi_log_level2s(query->message.level), stream);
}

static inline void
vi_log_format_message_content(FILE *stream, vi_log_query_t const *query)
{
  vfprintf(stream, query->message.format, *(va_list *)&(query->message.arguments));
}

static inline vi_log_format_exec_t __attribute__((always_inline))
vi_log_format_by_sym(int sym)
{
  static vi_log_format_exec_t lookup[] = {
    [VI_LOG_FMTSYM2IDX(VI_LOG_FMTSYM_PROCESS_ID)] =
      &vi_log_format_process_info_id,
    [VI_LOG_FMTSYM2IDX(VI_LOG_FMTSYM_PROCESS_NAME)] =
      &vi_log_format_process_info_name,
    [VI_LOG_FMTSYM2IDX(VI_LOG_FMTSYM_SOURCE_FILE_NAME)] =
      &vi_log_format_source_location_file_name,
    [VI_LOG_FMTSYM2IDX(VI_LOG_FMTSYM_SOURCE_FILE_LINE)] =
      &vi_log_format_source_location_file_line,
    [VI_LOG_FMTSYM2IDX(VI_LOG_FMTSYM_SOURCE_FUNCTION_NAME)] =
      &vi_log_format_source_location_function_name,
    [VI_LOG_FMTSYM2IDX(VI_LOG_FMTSYM_MESSAGE_TIME)] =
      &vi_log_format_message_time,
    [VI_LOG_FMTSYM2IDX(VI_LOG_FMTSYM_MESSAGE_LEVEL_NUM)] =
      &vi_log_format_message_level_num,
    [VI_LOG_FMTSYM2IDX(VI_LOG_FMTSYM_MESSAGE_LEVEL_STR)] =
      &vi_log_format_message_level_str,
    [VI_LOG_FMTSYM2IDX(VI_LOG_FMTSYM_MESSAGE_CONTENT)] =
      &vi_log_format_message_content,
  };

  assert(sym >= VI_LOG_FMTSYM_OFF
      && sym <  VI_LOG_FMTSYM_END);
  return lookup[VI_LOG_FMTSYM2IDX(sym)];
}

static int vi_log_agent_stderr_format[] = USE_VI_LOG_AGENT_STDERR_FORMAT;

static inline size_t __attribute__((always_inline))
vi_log_agent_stderr_format_length()
{
  return sizeof(vi_log_agent_stderr_format) / sizeof(0[vi_log_agent_stderr_format]);
}

static inline int __attribute__((always_inline))
vi_log_agent_stderr_format_symbol(size_t index)
{
  assert(index < vi_log_agent_stderr_format_length());
  return vi_log_agent_stderr_format[index];
}

static void
vi_log_agent_stderr_exec(void *user, vi_log_query_t const *query)
{
  char   *query_str;
  size_t  query_str_sz;
  FILE   *query_stream;
  size_t  index;

  vi_discard(user);

  query_str    = NULL;
  query_str_sz = 0;
  query_stream = open_memstream(&query_str, &query_str_sz);
  __fsetlocking(query_stream, FSETLOCKING_BYCALLER);
  for (index = 0; index < vi_log_agent_stderr_format_length(); ++index) {
    int sym = vi_log_agent_stderr_format_symbol(index);

    if (sym >= 0
     && sym <  VI_LOG_FMTSYM_OFF) {
      fputc(sym, query_stream);
      continue;
    }

    if (sym >= VI_LOG_FMTSYM_OFF
     && sym <  VI_LOG_FMTSYM_END) {
      vi_log_format_by_sym(sym)(query_stream, query);
      continue;
    }
  }
  fclose(query_stream);
  fwrite(query_str, sizeof(*query_str), query_str_sz, stderr);
  free(query_str);
}

static vi_log_agent_t vi_log_agent_stderr = {
  .exec = &vi_log_agent_stderr_exec,
};

#endif

#if USE_VI_LOG_AGENT_SYSLOG
#include <syslog.h>

static void
vi_log_agent_syslog_init(void *user, vi_log_process_info_t const *process_info)
{
  vi_discard(user);

  openlog(process_info->name, USE_VI_LOG_AGENT_SYSLOG_OPTIONS, USE_VI_LOG_AGENT_SYSLOG_FACILITY);
}

static void
vi_log_agent_syslog_exec(void *user, vi_log_query_t const *query)
{
  vi_discard(user);

  int priority;
  switch (query->message.level) {
    case VI_LOG_LEVEL_ERR: priority = LOG_ERR;     break;
    case VI_LOG_LEVEL_WRN: priority = LOG_WARNING; break;
    case VI_LOG_LEVEL_INF: priority = LOG_INFO;    break;
    case VI_LOG_LEVEL_DBG: priority = LOG_DEBUG;   break;
    case VI_LOG_LEVEL_TRC: priority = LOG_DEBUG;   break;
    default:               priority = -1;          break;
  }
  if (priority < 0)
    return;

  vsyslog(priority, query->message.format, *(va_list *)&(query->message.arguments));
}

static void
vi_log_agent_syslog_fini(void *user)
{
  vi_discard(user);

  closelog();
}

static vi_log_agent_t vi_log_agent_syslog = {
  .init = &vi_log_agent_syslog_init,
  .exec = &vi_log_agent_syslog_exec,
  .fini = &vi_log_agent_syslog_fini,
};

#endif

#if USE_VI_LOG_MUTEX_PTHREAD
#include <pthread.h>

typedef struct {
  pthread_mutex_t mutex;
} vi_log_mutex_pthread_data_t;

static inline int
vi_log_mutex_pthread_lock(void *user)
{
  return pthread_mutex_lock(&((vi_log_mutex_pthread_data_t *)(user))->mutex);
}

static inline int
vi_log_mutex_pthread_unlock(void *user)
{
  return pthread_mutex_unlock(&((vi_log_mutex_pthread_data_t *)(user))->mutex);
}

static vi_log_mutex_pthread_data_t vi_log_mutex_pthread_data = {
  .mutex = PTHREAD_MUTEX_INITIALIZER,
};
static vi_log_mutex_t vi_log_mutex_pthread = {
  .user   = &vi_log_mutex_pthread_data,
  .lock   = &vi_log_mutex_pthread_lock,
  .unlock = &vi_log_mutex_pthread_unlock
};

#endif

typedef struct {
  vi_log_process_info_t        process_info;
  vi_log_mutex_t        const *mutex;
  vi_log_agent_t        const *agents[USE_VI_LOG_AGENTS_COUNT];
  vi_log_level_t               level;
  bool                         quiet;
  bool                         ready;
  bool                         locked;
} vi_log_state_t;

static vi_log_state_t vi_log_state;

static int
vi_log_state_mutex_lock()
{
  vi_log_mutex_t const *mutex = vi_log_state.mutex;
  int                   rc;

  if (!(mutex)
   || !(mutex->lock)
   || !(mutex->unlock))
    return -1;

  rc = mutex->lock(mutex->user);
  if (rc < 0)
    return rc;

  vi_log_state.locked = true;
  return rc;
}

static int
vi_log_state_mutex_unlock()
{
  vi_log_mutex_t const *mutex = vi_log_state.mutex;
  int                   rc;

  if (!(mutex)
   || !(mutex->lock)
   || !(mutex->unlock))
    return -1;

  rc = mutex->unlock(mutex->user);
  if (rc < 0)
    return rc;

  vi_log_state.locked = false;
  return rc;
}

static inline size_t
vi_log_state_agents_count()
{
  return sizeof(vi_log_state.agents) / sizeof(0[vi_log_state.agents]);
}

static inline vi_log_agent_t const *
vi_log_state_agent(size_t index)
{
  assert(index < vi_log_state_agents_count());
  return vi_log_state.agents[index];
}

static int
vi_log_state_init(char const *pexec, vi_log_config_t const *config)
{
  vi_log_state_t              *state = &vi_log_state;
  vi_log_process_info_t       *pinfo = &state->process_info;
  size_t                       index;
  vi_log_agent_t        const *agent;

  if (state->ready)
    return 1;

  pinfo->id   = getpid();
  pinfo->name = strrchr(pexec, '/');
  pinfo->name = pinfo->name ? pinfo->name + 1 : pexec ? pexec : "";

  state->quiet = config->quiet;
  state->level = config->level;

#if USE_VI_LOG_AGENT_STDERR
  if (!config->no_stderr_agent)
    vi_log_agent_add(&vi_log_agent_stderr);
#endif
#if USE_VI_LOG_AGENT_SYSLOG
  if (!config->no_syslog_agent)
    vi_log_agent_add(&vi_log_agent_syslog);
#endif
#if USE_VI_LOG_MUTEX_PTHREAD
  if (!config->no_pthread_mutex)
    vi_log_mutex_set(&vi_log_mutex_pthread);
#endif

  for (index = 0; index < vi_log_state_agents_count(); ++index) {
    agent = vi_log_state_agent(index);
    if (!agent || !agent->init)
      continue;

    agent->init(agent->user, pinfo);
  }

  state->ready = true;
  return 0;
}

static void
vi_log_state_fini()
{
  vi_log_state_t       *state = &vi_log_state;
  size_t                index;
  vi_log_agent_t const *agent;

  if (!state->ready)
    return;

  state->ready = false;

  for (index = 0; index < vi_log_state_agents_count(); ++index) {
    agent = vi_log_state_agent(index);
    if (!agent || !agent->fini)
      continue;

    agent->fini(agent->user);
  }
}

int
vi_log_init(char const *pexec, vi_log_config_t const *config)
{
  vi_log_config_t config_default = vi_log_config_default();
  return vi_log_state_init(pexec, config ? config : &config_default);
}

void
vi_log_fini()
{
  vi_log_state_fini();
}

#if USE_VI_LOG_INITFINI_ARRAY

static void
vi_log_init_array(int argc, char **argv, char **envp)
{
  vi_log_config_t  config = vi_log_config_default();
  char            *pexec  = NULL;

  vi_discard(envp);

  if (argc && argv[0])
    pexec = argv[0];

  vi_log_state_init(pexec, &config);
}

static void
vi_log_fini_array()
{
  vi_log_state_fini();
}

void (* const __vi_log_init_array[])(int, char **, char **)
  __attribute__((section(".init_array"), aligned(sizeof(void *)))) =
{
  &vi_log_init_array
};

void (* const __vi_log_fini_array[])()
  __attribute__((section(".fini_array"), aligned(sizeof(void *)))) =
{
  &vi_log_fini_array
};

#endif

static inline time_t
vi_log_time_now()
{
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME_COARSE, &ts);
  return ts.tv_sec;
}

static void
vi_vlog_exec_core(vi_log_source_location_t const *source_location, vi_log_level_t level, char const *format, va_list arguments)
{
  vi_log_query_t        query;
  size_t                index;
  vi_log_agent_t const *agent;

  query.process_info    = &vi_log_state.process_info;
  query.source_location = source_location;
  query.message.time    = vi_log_time_now();
  query.message.level   = level;
  query.message.format  = format;

  vi_log_state_mutex_lock();
  for (index = 0; index < vi_log_state_agents_count(); ++index) {
    agent = vi_log_state_agent(index);
    if (!agent || !agent->exec)
      continue;

    va_copy(query.message.arguments, arguments);
    agent->exec(agent->user, &query);
    va_end(query.message.arguments);
  }
  vi_log_state_mutex_unlock();
}

int
vi_log_vexec(vi_log_source_location_t source_location, vi_log_level_t level, char const *format, va_list arguments)
{
  if (!vi_log_state.ready)
    return -1;

  if (vi_log_state.level < level
   || vi_log_state.quiet)
    return 1;

  vi_vlog_exec_core(&source_location, level, format, arguments);
  return 0;
}

int
vi_log_exec(vi_log_source_location_t source_location, vi_log_level_t level, char const *format, ...)
{
  va_list arguments;

  if (!vi_log_state.ready)
    return -1;

  if (vi_log_state.level < level
   || vi_log_state.quiet)
    return 1;

  va_start(arguments, format);
  vi_vlog_exec_core(&source_location, level, format, arguments);
  va_end(arguments);
  return 0;
}

int
vi_log_agent_add(vi_log_agent_t const *agent)
{
  size_t                 index;
  vi_log_agent_t const **agent_slot;

  for (index = 0; index < vi_log_state_agents_count(); ++index) {
    agent_slot = &vi_log_state.agents[index];
    if (*agent_slot)
      continue;

    *agent_slot = agent;
    return (int)(index);
  }

  return -1;
}

int
vi_log_mutex_set(vi_log_mutex_t const *mutex)
{
  if (vi_log_state.locked)
    return -1;

  if (!mutex->lock
   || !mutex->unlock)
    return -2;

  vi_log_state.mutex = mutex;
  return 0;
}

vi_log_level_t
vi_log_level_limit()
{
  return USE_VI_LOG_LEVEL_LIMIT;
}

vi_log_level_t
vi_log_level()
{
  return vi_log_state.level;
}

int
vi_log_level_set(vi_log_level_t level)
{
  if (vi_log_level_limit() < level)
    return -1;

  vi_log_state.level = level;
  return 0;
}

bool
vi_log_quiet()
{
  return vi_log_state.quiet;
}

void
vi_log_quiet_set(bool quiet)
{
  vi_log_state.quiet = quiet;
}

bool
vi_log_ready()
{
  return vi_log_state.ready;
}

#ifndef NDEBUG
#include <stdio.h>

#if USE_VI_LOG_AGENT_STDERR

static inline char const * __attribute__((always_inline))
vi_log_format_dsc_by_sym(int sym)
{
  static char const *lookup[] = {
    [VI_LOG_FMTSYM2IDX(VI_LOG_FMTSYM_PROCESS_ID)] =
      "{proc-id}",
    [VI_LOG_FMTSYM2IDX(VI_LOG_FMTSYM_PROCESS_NAME)] =
      "{proc-name}",
    [VI_LOG_FMTSYM2IDX(VI_LOG_FMTSYM_SOURCE_FILE_NAME)] =
      "{file-name}",
    [VI_LOG_FMTSYM2IDX(VI_LOG_FMTSYM_SOURCE_FILE_LINE)] =
      "{file-line}",
    [VI_LOG_FMTSYM2IDX(VI_LOG_FMTSYM_SOURCE_FUNCTION_NAME)] =
      "{func-name}",
    [VI_LOG_FMTSYM2IDX(VI_LOG_FMTSYM_MESSAGE_TIME)] =
      "{time}",
    [VI_LOG_FMTSYM2IDX(VI_LOG_FMTSYM_MESSAGE_LEVEL_NUM)] =
      "{lvl-num}",
    [VI_LOG_FMTSYM2IDX(VI_LOG_FMTSYM_MESSAGE_LEVEL_STR)] =
      "{lvl-str}",
    [VI_LOG_FMTSYM2IDX(VI_LOG_FMTSYM_MESSAGE_CONTENT)] =
      "{msg}",
  };

  assert(sym >= VI_LOG_FMTSYM_OFF
      && sym <  VI_LOG_FMTSYM_END);
  return lookup[VI_LOG_FMTSYM2IDX(sym)];
}

static void
vi_log_agent_stderr_format_dump()
{
  size_t index;

  for (index = 0; index < vi_log_agent_stderr_format_length(); ++index) {
    int sym = vi_log_agent_stderr_format_symbol(index);

    if (sym == '\n') {
      printf("\\n");
      continue;
    }

    if (sym >= 0
     && sym <  VI_LOG_FMTSYM_OFF) {
      fputc(sym, stdout);
      continue;
    }

    if (sym >= VI_LOG_FMTSYM_OFF
     && sym <  VI_LOG_FMTSYM_END) {
      fputs(vi_log_format_dsc_by_sym(sym), stdout);
      continue;
    }
  }
}
#endif

void
vi_log_config_dump()
{
  printf("Using: "
#if   (USE_VI_LOG_CONFIG_TYPE == 0)
    "default configuration values"
#elif (USE_VI_LOG_CONFIG_TYPE == 1)
    "values from configuration at: \"" VI_LOG_CONFIG_LOCAL "\""
#elif (USE_VI_LOG_CONFIG_TYPE == 2)
    "values from configuration at: \"" VI_LOG_CONFIG_LOCAL_DEFAULT "\""
#endif
  "\n");

  printf("  USE_VI_LOG_LEVEL: %d (%s)\n", USE_VI_LOG_LEVEL, vi_log_level2s(USE_VI_LOG_LEVEL));
  printf("  USE_VI_LOG_LEVEL_LIMIT: %d (%s)\n", USE_VI_LOG_LEVEL_LIMIT, vi_log_level2s(USE_VI_LOG_LEVEL_LIMIT));
  printf("  USE_VI_LOG_AGENT_STDERR: %s\n", vi_cstr_bool(USE_VI_LOG_AGENT_STDERR));
#if USE_VI_LOG_AGENT_STDERR
  printf("    USE_VI_LOG_AGENT_STDERR_FORMAT:\n");
  printf("      \"");
  vi_log_agent_stderr_format_dump();
  printf("\"\n");
#endif
  printf("  USE_VI_LOG_AGENT_SYSLOG: %s\n", vi_cstr_bool(USE_VI_LOG_AGENT_SYSLOG));
#if USE_VI_LOG_AGENT_SYSLOG
  printf("    USE_VI_LOG_AGENT_SYSLOG_OPTIONS: 0x%x\n", USE_VI_LOG_AGENT_SYSLOG_OPTIONS);
  printf("    USE_VI_LOG_AGENT_SYSLOG_FACILITY: %d\n", LOG_FAC(USE_VI_LOG_AGENT_SYSLOG_FACILITY));
#endif
  printf("  USE_VI_LOG_AGENTS_COUNT: %d\n", USE_VI_LOG_AGENTS_COUNT);
  printf("  USE_VI_LOG_MUTEX_PTHREAD: %s\n", vi_cstr_bool(USE_VI_LOG_MUTEX_PTHREAD));
  printf("  USE_VI_LOG_INITFINI_ARRAY: %s\n", vi_cstr_bool(USE_VI_LOG_INITFINI_ARRAY));

}

#endif

#endif /* defined(VI_LIB_IMPLEMENTATION) || defined(VI_LOG_IMPLEMENTATION) */
#endif /* << implementation */

#endif//__VI_LOG_H
