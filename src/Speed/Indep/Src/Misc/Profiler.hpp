#ifndef MISC_PROFILER_H
#define MISC_PROFILER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

class ProfileNode {
  public:
    ProfileNode() {}

    ProfileNode(const char *section_name, int categories) {}

    void Begin(const char *, int) {}

    void End() {}

    float GetTime() {}
};

#endif
