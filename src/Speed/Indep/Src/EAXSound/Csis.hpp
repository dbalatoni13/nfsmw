#ifndef EAXSOUND_CSIS_HPP
#define EAXSOUND_CSIS_HPP

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

// Todo lo que este fichero declaraba por su cuenta (InterfaceId, ClassHandle,
// FunctionHandle, FunctionClient, Class, Parameter, System, Function) esta ya
// en la libreria csis, y los enums Type_location* en SND_GEN/copspeech.hpp.
// Duplicarlo hacia imposible incluir esta cabecera despues de cualquiera de
// las dos.
#include "csis/csis.h"

#endif
