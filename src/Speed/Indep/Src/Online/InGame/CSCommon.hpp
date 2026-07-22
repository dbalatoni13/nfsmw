#ifndef ONLINE_INGAME_CS_COMMON_HPP
#define ONLINE_INGAME_CS_COMMON_HPP

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

enum DiagnosticLevelEnum {
    DIAGNOSTICLEVEL_NONE = 0,
    DIAGNOSTICLEVEL_MEDIUM = 1,
    DIAGNOSTICLEVEL_HIGH = 2,
    DIAGNOSTICLEVEL_EXTREME = 3,
    DIAGNOSTICLEVEL_COUNT = 4
};

struct CSCommon {
    static void SetDiagnosticLevel(DiagnosticLevelEnum level) { m_diagnosticLevel = level; }
    static DiagnosticLevelEnum GetDiagnosticLevel() { return m_diagnosticLevel; }
    static void DumpBytes(const char *raw_data, int num_bytes);
    static void CarOverlayDiagnostics();
    static void ShowDiagnostics();
    static void PossiblyDoDataCorruption(char *raw_data, int num_bytes) {}

  private:
    static DiagnosticLevelEnum m_diagnosticLevel;
};

#endif
