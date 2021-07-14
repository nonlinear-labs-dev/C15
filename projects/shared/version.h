#pragma once

// A) Global "Version Name" String :
// 1. Version String for official releases, build from an official release branch :
// The syntax is "YY-WW", a 4-digit code, first two digits is year and second two
// digits is calender week (1..52), eg 20-22 means 2020, week 22
//
// 2. Version String for inofficial beta builds, build from any branch :
// The syntax is "Beta: <arbitrary string>"
// The version string may also indicate year and week as before but may contain different/other info.
// Master is just a special beta which is always beta.
// At any rate, it MUST mark the version beta status in a clear way:
// Whatever string is choosen, it must always start with "Beta:", like "Beta: Preview-MS1.8".
//
// Pieces of software that can be executed on the command line in some way shall put out
// this version string (among other info, at will) when called with option "--version", via
// the GetC15Version() function, and the function shall be used to get and display the version
// in other ways, too (HWUI, WebUI)
// The complete string shall also reside in the build artifact, notably for software components
// of the microcontrollers in the system, which can be searched for with "grep", or viewed and
// and searched in an editor (text or hex), etc. That's why the "\n\n" are part of the strings so
// that the text is easily spotted.
//
// The Version String shall contain all required info to easily back-track what we are dealing with,
// and note this is the only string that is available when using the WebUI (no more detailed info there)
// That's why some form date conding is required.
//
//  Final Versions         "YY-WW"   YY-WW : release date (tbd).
//  Everything else        whatever seems appropriate
#define C15_VERSION_STRING "21-25"
#pragma message("make sure version string '" C15_VERSION_STRING "' is up-to date when building a release or beta")

// do not change these two strings:
#define C15_VERSION_STRING_IN_MEMORY_PREFIX "\n\nC15 Version: "
#define C15_VERSION_STRING_IN_MEMORY_POSTFIX "\0\n\n"

static const char C15_VERSION_STRING_IN_MEMORY[]
    = C15_VERSION_STRING_IN_MEMORY_PREFIX C15_VERSION_STRING C15_VERSION_STRING_IN_MEMORY_POSTFIX;

static inline const char* GetC15Version(void)
{
  return &(C15_VERSION_STRING_IN_MEMORY[15]);  // extract date only
}

// B) Specific Build Info
// TODO : replace this with auto-extracted data from git at compile time (branch name, commit date, commit ID,
// also the seperate functions for each of these items)

static const char C15_BUILD_STRING_IN_MEMORY[] = "\n\nthis C15 program was built on " __DATE__ " " __TIME__ "\0\n\n";

static inline const char* GetC15Build(void)
{  // TODO: fill with auto-generated content, concatenated string: branch name, commit date, commit ID
  return &(C15_BUILD_STRING_IN_MEMORY[32]);  // extract date only
}

static inline const char* GetC15BuildBranch(void)
{  // TODO: fill with auto-generated content
  return "<branch>";
}

static inline const char* GetC15BuildCommitDate(void)
{  // TODO: fill with auto-generated content
  return "<commit-date>";
}

static inline const char* GetC15BuildCommitID(void)
{  // TODO: fill with auto-generated content
  return "<commit-ID>";
}
