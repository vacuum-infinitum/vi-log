// ----------------------------------------------------------------------------
// vi-log-config-template.h - v1.0 - public domain
//
// Part of a headers-only library for logging.
//
// This header is a template for a custom configuration header that can be used
// by vi-log.h in a compile-time configuration step. It contains all possible
// feature-test-macros with their documentation.
//
// After compile-time configuration step `USE_VI_LOG_*` macros (with similar to
// feature-test-macros names) will be defined for library's internal use.
// To print them user needs to compile without NDEBUG macro and call
// `vi_log_config_dump()` function.
//
// WARNING: Values used in this file for feature-test-macros ARE NOT defaults.
//          They are a valid values chosen randomly to display possible usage.
// ----------------------------------------------------------------------------

#ifndef __VI_LOG_CONFIG_TEMPLATE_H
#define __VI_LOG_CONFIG_TEMPLATE_H

#include "vi-log-common.h"

// Defines initial logging level at compile-time. This value will be assigned
// to logger's current logging level at initialization step. After that
// it can be changed by runtime calls to `vi_log_level_set()`.
//
// Possible values are definied in vi-log-common.h as `VI_LOG_LEVEL_*` macros.
//
// If this macro is not defined configuration will use a default value that
// determined by standard-ish NDEBUG macro:
//  `VI_LOG_LEVEL_ERR` for "Release"
//  `VI_LOG_LEVEL_DBG` for "Debug"
#define VI_LOG_LEVEL VI_LOG_LEVEL_ERR

// Defines maximal logging level at compile-time.
//
// Possible values are definied in vi-log-common.h as `VI_LOG_LEVEL_*` macros.
//
//  1. Controls upper limit (inclusive) for values that can be accepted by
//     runtime function `vi_log_level_set()`.
//
//  2. Optimizes-out (soft-strips) `vi_log_<lvl>()` macros calls.
//
//     Implemented as a replacement: `vi_log_exec()` -> `vi_discard()`.
//     `vi_discard()` is as an empty function marked `always_inline`,
//     essentialy discarding all arguments passed to original logging call.
//
//     Defining this value to VI_LOG_LEVEL_OFF will basically strip all calls,
//     which can be helpful if you think that logger devours performance of
//     your application, and you don't want to remove all of them by hand :)
//
//     Reason for this way of calls stripping is to trick compilers into
//     not throwing warnings of 'unused-*' nature when compiled with
//     flags like: -Wall -Wextra -Werror.
//
// If this macro is not defined configuration will use a default value:
//   `VI_LOG_LEVEL_TRC` (aka maximum possible level)
#define VI_LOG_LEVEL_LIMIT VI_LOG_LEVEL_TRC

// Defines presence of an out-of-the-box agent that outputs to stderr.
// To add an agent this macro must be defined with a value of `1`.
// If value is `0` or macro is not defined agent is ignored.
//
// WARNING: This agent uses functions under `_GNU_SOURCE` and `_DEFAULT_SOURCE`
//          feature list from standard library. So your application must either 
//          define them before vi-log.h or pass to a compilation command line.
#define VI_LOG_AGENT_STDERR 1

// Feature macros in this section are used only if `VI_LOG_AGENT_STDERR == 1`
#if defined(VI_LOG_AGENT_STDERR) && (VI_LOG_AGENT_STDERR)

  // Defines output format for stderr-agent.
  // vi-log-common.h describes how to do it properly.
  //
  // If this macro is not defined default format is used:
  //   "{time} {lvl-str} {proc-name}[{proc-id}]: [{file-name}:{file-line}]: {msg}\n"
  #define VI_LOG_AGENT_STDERR_FORMAT \
    VI_LOG_FMTARR_LINE(VI_LOG_FMTSYM_MESSAGE_CONTENT)

#endif

// Defines presence of an out-of-the-box agent that uses <syslog.h>.
// To add an agent this macro must be defined with a value of `1`.
// If value is `0` or macro is not defined agent is ignored.
//
// NOTE: priority of a syslog message is derived from `VI_LOG_LEVEL_*`:
//   VI_LOG_LEVEL_ERR -> LOG_ERR
//   VI_LOG_LEVEL_WRN -> LOG_WARNING
//   VI_LOG_LEVEL_INF -> LOG_INFO
//   VI_LOG_LEVEL_DBG -> LOG_DEBUG
//   VI_LOG_LEVEL_TRC -> LOG_DEBUG
//
// WARNING: This agent uses `vsyslog()` function under `_DEFAULT_SOURCE`
//          feature list from standard library. So your application must either
//          define it before vi-log.h or pass to a compilation command line.
#define VI_LOG_AGENT_SYSLOG 0

// Feature macros in this section are used only if `VI_LOG_AGENT_SYSLOG == 1`
#if defined(VI_LOG_AGENT_SYSLOG) && (VI_LOG_AGENT_SYSLOG)
  #include <syslog.h>

  // Defines options passed to `openlog()` from <syslog.h>.
  // Look at syslog documentation for more details.
  //
  // If this macro is not defined default options are:
  //   LOG_PID
  //   LOG_NDELAY
  //   LOG_PERROR if `VI_LOG_AGENT_STDERR != 1`
  #define VI_LOG_AGENT_SYSLOG_OPTIONS  LOG_PID | LOG_PERROR

  // Defines facility passed to `openlog()` from <syslog.h>.
  // Look at syslog documentation for more details.
  //
  // If this macro is not defined default facility is:
  //   LOG_USER
  #define VI_LOG_AGENT_SYSLOG_FACILITY LOG_DAEMON

#endif

// Defines count of custom agents user wishes to add manually.
// If value is `0` or macro is not defined final agents count defaults
// either to a total count of used out-of-the-box agents, 
// or a value of `4` if none were selected.
//
// To add custom agent `vi_log_agent_add()` should be used.
#define VI_LOG_AGENTS_EXT_COUNT 0

// Defines presence of an out-of-the-box thread syncronization mechanism
// implemented using `pthread_mutex_t` from <pthread.h>.
// To add this mutex implementation macro must be defined with a value of `1`
// If value is `0` or macro is not defined this mutex type is ignored.
//
// To set custom mutex `vi_log_mutex_set()` should be used.
#define VI_LOG_MUTEX_PTHREAD 0

// Defines if auto initialization/finalization is enabled.
//
// By default logger needs to be initialized/finalized manually using
// `vi_log_init()` & `vi_log_fini()` functions respectevly.
// Usually it happens somewhere around the start/end of a `main()` function.
//
// This macro exploits init_array/fini_array sections in ELF binaries
// to initialize/finalize logger gracefuly before/after call to `main()`
// function, relieving the user from thinking about it.
//
// This kind of initialization uses argv[0] to determine program name
// and `vi_log_config_default()` function to setup initial configuration.
#define VI_LOG_INITFINI_ARRAY 1

// Defines custom format for `VI_LOG_FMTSYM_MESSAGE_TIME`.
// "Under the hood" `strftime()` function is used to output time at
// which logging message was created.
//
// If this macro is not defined default format is:
//   "%h %e %T"
#define VI_LOG_FMTSYM_MESSAGE_TIME_FORMAT "%F"

#endif//__VI_LOG_CONFIG_TEMPLATE_H
