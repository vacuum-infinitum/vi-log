// ----------------------------------------------------------------------------
// vi-log-common.h - v1.0 - public domain
//
// Part of a headers-only library for logging.
//
// This part contains common definitions that are used by vi-log.h
// and can be used by custom configuration header.
//
// For more details look inside main header: vi-log.h
// ----------------------------------------------------------------------------

#ifndef __VI_LOG_COMMON_H
#define __VI_LOG_COMMON_H

// ----------------------------------------------------------------------------
// `VI_LOG_LEVEL_*` macros are used to set current and maximum level
// for logging messages.
// ----------------------------------------------------------------------------

#define VI_LOG_LEVEL_OFF 0
#define VI_LOG_LEVEL_ERR 1
#define VI_LOG_LEVEL_WRN 2
#define VI_LOG_LEVEL_INF 3
#define VI_LOG_LEVEL_DBG 4
#define VI_LOG_LEVEL_TRC 5

// ----------------------------------------------------------------------------
// `VI_LOG_FMT*` marcos are used to define output format for messages.
//
// To define an output format use `VI_LOG_FMTARR*()` macro passing as arguments
// either ASCII symbols in range [0..255] or `VI_LOG_FMTSYM_*` macros
// in range [VI_LOG_FORMAT_SYM_OFF, VI_LOG_FORMAT_SYM_END].
//
// EXAMPLE:
//  #define VI_LOG_AGENT_STDERR
//  #define VI_LOG_AGENT_STDERR_FORMAT   \
//    VI_LOG_FMTARR(                     \
//      '[',                             \
//      VI_LOG_FMTSYM_MESSAGE_LEVEL_STR, \
//      ']',                             \
//      ' ',                             \
//      VI_LOG_FMTSYM_MESSAGE_CONTENT,   \
//      '\n'                             \
//    )
//
//  When a message is processed by the logger, for example:
//    vi_log_inf("some info message %d", 42);
//  stderr-agent, enabled by the first macro, will compose an output message
//  according to format, defined by the second macro, resulting in this output:
//    [inf] some info message 42
//
// NOTE_1: there is a default format for stderr-agent,
//         so it's not required to define one yourself
//
// NOTE_2: in current implementation any unrecognized
//         characters or values out of range are ignored
//
// NOTE_3: For now this format is used only by stderr-agent.
// ----------------------------------------------------------------------------

#define VI_LOG_FMTSYM_OFF                  256
#define VI_LOG_FMTIDX2SYM(x)               (x + VI_LOG_FMTSYM_OFF)
#define VI_LOG_FMTSYM2IDX(x)               (x - VI_LOG_FMTSYM_OFF)

#define VI_LOG_FMTSYM_PROCESS_ID           VI_LOG_FMTIDX2SYM(0)
#define VI_LOG_FMTSYM_PROCESS_NAME         VI_LOG_FMTIDX2SYM(1)
#define VI_LOG_FMTSYM_SOURCE_FILE_NAME     VI_LOG_FMTIDX2SYM(2)
#define VI_LOG_FMTSYM_SOURCE_FILE_LINE     VI_LOG_FMTIDX2SYM(3)
#define VI_LOG_FMTSYM_SOURCE_FUNCTION_NAME VI_LOG_FMTIDX2SYM(4)
#define VI_LOG_FMTSYM_MESSAGE_TIME         VI_LOG_FMTIDX2SYM(5)
#define VI_LOG_FMTSYM_MESSAGE_LEVEL_NUM    VI_LOG_FMTIDX2SYM(6)
#define VI_LOG_FMTSYM_MESSAGE_LEVEL_STR    VI_LOG_FMTIDX2SYM(7)
#define VI_LOG_FMTSYM_MESSAGE_CONTENT      VI_LOG_FMTIDX2SYM(8)
#define VI_LOG_FMTSYM_END                  VI_LOG_FMTIDX2SYM(9)

#define VI_LOG_FMTARR(...)                 { __VA_ARGS__ }
#define VI_LOG_FMTARR_LINE(...)            VI_LOG_FMTARR(__VA_ARGS__, '\n')

#endif//__VI_LOG_COMMON_H
