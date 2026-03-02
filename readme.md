# vi-log

Headers-only logging library written in C language for C/C++ projects.

## Description

This library was born from my desire to create an easy-to-use fast-to-setup logging library for C language.\
It is also inspired by [stb single-file public domain libraries for C/C++](https://github.com/nothings/stb).

## Features

Depends only on **Standard C library**.

Support for compile-time configuration using **feature-test-macros**.\
Support for search and auto-inclusion of a configuration header containing these macros.\
Example of such header is [`vi-log-config-template.h`](/vi-log-config-template.h) which contains all supported macros with their documentation.

Out of the box configurable logging agents (aka sinks):\
**stderr agent** - outputs logging messages to ~~you guessed it~~ [**stderr** I/O stream](https://www.man7.org/linux/man-pages/man3/stderr.3.html) with configurable output format\
**syslog agent** - outputs logging messages using [**system logger**](https://www.man7.org/linux/man-pages/man3/syslog.3.html)

Support for **user-defined (aka custom)** logging agents.

Support for **stripping of logging calls** that exceed configured log level limit.\
Which helps with the task of stripping logging calls based on release/debug version of an application.

Support for **auto initialization/finalization** using **init_array/fini_array sections of ELF binaries**.

Documentation is WIP.\
Most features and functions are self-explanatory, but I have tried to explain other in as much detail as possible.

## Usage

1. Copy [`vi-log.h`](/vi-log.h) and [`vi-log-common.h`](/vi-log-common.h) in a desirable place where your compiler can find them.

2. Provide feature-test-macros for library configuration.\
   Either via compilation command line or via configuration header.\
   *This step is of optional if user is satisfied with default library configuration and desires to add a custom logging agent.*

3. As for most **stb-like libraries** you should select *exactly one* C/C++ source file that actually instantiates the code,\
   preferably a file you're not editing frequently. This file should define one of the implementation macros:\
   `VI_LOG_IMPLEMENTATION` or `VI_LIB_IMPLEMENTATION` to actually enable the function definitions.

4. Create and add custom logging agents. using `vi_log_agent_add(...)` function.\
   *This step is optional if user is satisfied with out-of-the-box logging agents and enabled at least one during step 2.*

5. Initialize library.

6. Use [`vi-log.h`](/vi-log.h) header throughout your project, log messages using `vi_log_<lvl>(...)` macro-functions.

7. ***???***

8. ***PROFIT***

## Examples

### minimal setup for stderr-agent with custom logging levels

```C
// vi-log-config.h
//   file with custom configuration that will be found by <vi-log.h>
#ifndef __VI_LOG_CONFIG_H
#define __VI_LOG_CONFIG_H

#define VI_LOG_LEVEL        VI_LOG_LEVEL_WRN // set initial logging level
#define VI_LOG_LEVEL_LIMIT  VI_LOG_LEVEL_INF // set maximal logging level
#define VI_LOG_AGENT_STDERR 1                // enable stderr logging agent

#endif//__VI_LOG_CONFIG_H

// main.c
//   because we have only one source file it will also contain an implementation
#define VI_LOG_IMPLEMENTATION
#include <vi-log.h>

int main(int argc, char **argv) {
  // 1. initialize library with defaults
  vi_log_init(argv[0], NULL);

  // 2. use macros to log messages
  if (argc < 2) {
    // this message will be printed
    vi_log_err("Expected at least one argument");
    return 1;
  }

  if (argc > 2) {
    // this message will be printed
    vi_log_wrn("Got too many arguments, only the first will be used");
  }

  // this message will NOT be printed because inital logging level is VI_LOG_LEVEL_WRN
  vi_log_inf("Hello %s?", argv[1]);

  { // this logging calls will be STRIPPED because maximal logging level is VI_LOG_LEVEL_INF
    vi_log_dbg("%d", 42);
    vi_log_trc("%zu", sizeof(int));
  }

  // change log level at runtime
  vi_log_level_set(VI_LOG_LEVEL_INF);

  vi_log_inf("Hello %s!", argv[1]);

  { // this is a no-op because VI_LOG_LEVEL_TRC is greater than maximal logging level
    vi_log_level_set(VI_LOG_LEVEL_TRC);
  }

  // skip any messages after this call
  vi_log_quiet_set(true);

  // 3. finalize library
  vi_log_fini();
  return 0;
}
```
