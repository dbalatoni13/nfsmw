#include "Speed/GameCube/Src/Logitech/LGWheels.h"

#include <string.h>

#include "dolphin.h"

LGWheels::LGWheels() {
    LGInit();

    this->OverallGain = 0xFF;

    for (int channel = 0; channel < 4; channel++) {
        this->InitVars(channel);
    }
}

// Limpia el canal: las tres banderas y las ocho ranuras de los cuatro objetos de
// fuerza. El miembro force no se toca aqui.
void LGWheels::InitVars(long channel) {
    this->IsAirborne[channel] = 0;
    this->damperWasPlaying[channel] = 0;
    this->springWasPlaying[channel] = 0;

    for (int effect = 0; effect < 8; effect++) {
        this->condition.EffectID[channel][effect] = -1;
        this->condition.Playing[channel][effect] = 0;
        this->constant.EffectID[channel][effect] = -1;
        this->constant.Playing[channel][effect] = 0;
        this->periodic.EffectID[channel][effect] = -1;
        this->periodic.Playing[channel][effect] = 0;
        this->ramp.EffectID[channel][effect] = -1;
        this->ramp.Playing[channel][effect] = 0;
    }
}

void LGWheels::ReadAll() {
    long channel = this->wheels.ReadAll();
    memcpy(this->Position, this->wheels.Position, 0x28);
    if (channel != -1) {
        this->InitVars(channel);
    }
}

// Cada tipo de fuerza vive en un objeto distinto y en una ranura distinta: el
// muelle y el amortiguador en condition, la constante y la colision lateral en
// constant, y el resto en periodic.
void LGWheels::StopForce(long channel, long forceType) {
    switch (forceType) {
    case LGFORCE_SPRING:
        if (this->IsPlaying(channel, LGFORCE_SPRING)) {
            this->condition.Stop(channel, 0);
        }
        break;

    case LGFORCE_CONSTANT:
        if (this->IsPlaying(channel, LGFORCE_CONSTANT)) {
            this->constant.Stop(channel, 0);
        }
        break;

    case LGFORCE_DAMPER:
        if (this->IsPlaying(channel, LGFORCE_DAMPER)) {
            this->condition.Stop(channel, 1);
        }
        break;

    case LGFORCE_SIDE_COLLISION:
        if (this->IsPlaying(channel, LGFORCE_SIDE_COLLISION)) {
            this->constant.Stop(channel, 1);
        }
        break;

    case LGFORCE_FRONTAL_COLLISION:
        if (this->IsPlaying(channel, LGFORCE_FRONTAL_COLLISION)) {
            this->periodic.Stop(channel, 0);
        }
        break;

    case LGFORCE_DIRT_ROAD:
        if (this->IsPlaying(channel, LGFORCE_DIRT_ROAD)) {
            this->periodic.Stop(channel, 1);
        }
        break;

    case LGFORCE_BUMPY_ROAD:
        if (this->IsPlaying(channel, LGFORCE_BUMPY_ROAD)) {
            this->periodic.Stop(channel, 2);
        }
        break;

    // Parar el efecto de derrape devuelve el amortiguador y el muelle, que se
    // habian apartado para dejarle sitio.
    case LGFORCE_SLIPPERY_ROAD:
        if (this->IsPlaying(channel, LGFORCE_SLIPPERY_ROAD)) {
            this->condition.Stop(channel, 2);
        }

        if (this->damperWasPlaying[channel]) {
            this->PlayDamperForce(channel, this->DamperForceParams[channel].coefficient);
            this->condition.Playing[channel][1] = 1;
            this->damperWasPlaying[channel] = 0;
        }

        if (this->springWasPlaying[channel]) {
            this->PlaySpringForce(channel, this->SpringForceParams[channel].offset, this->SpringForceParams[channel].saturation,
                                  this->SpringForceParams[channel].coefficient);
            this->condition.Playing[channel][0] = 1;
            this->springWasPlaying[channel] = 0;
        }
        break;

    case LGFORCE_SURFACE:
        if (this->IsPlaying(channel, LGFORCE_SURFACE)) {
            this->periodic.Stop(channel, 3);
        }
        break;

    // Aterrizar: restaurar todo lo que sonaba antes de despegar.
    case LGFORCE_CAR_AIRBORNE:
        if (this->IsPlaying(channel, LGFORCE_CAR_AIRBORNE)) {
            this->IsAirborne[channel] = 0;

            if (this->wasPlayingBeforeAirborne[channel][LGFORCE_SPRING] == 1) {
                this->PlaySpringForce(channel, this->SpringForceParams[channel].offset, this->SpringForceParams[channel].saturation,
                                      this->SpringForceParams[channel].coefficient);
            }

            if (this->wasPlayingBeforeAirborne[channel][LGFORCE_CONSTANT] == 1) {
                this->PlayConstantForce(channel, this->ConstantForceParams[channel].magnitude, this->ConstantForceParams[channel].direction);
            }

            if (this->wasPlayingBeforeAirborne[channel][LGFORCE_DAMPER] == 1) {
                this->PlayDamperForce(channel, this->DamperForceParams[channel].coefficient);
            }

            if (this->wasPlayingBeforeAirborne[channel][LGFORCE_DIRT_ROAD] == 1) {
                this->PlayDirtRoadEffect(channel, this->DirtRoadParams[channel].magnitude);
            }

            if (this->wasPlayingBeforeAirborne[channel][LGFORCE_BUMPY_ROAD] == 1) {
                this->PlayBumpyRoadEffect(channel, this->BumpyRoadParams[channel].magnitude);
            }

            if (this->wasPlayingBeforeAirborne[channel][LGFORCE_SLIPPERY_ROAD] == 1) {
                this->PlaySlipperyRoadEffect(channel, this->SlipperyRoadParams[channel].magnitude);
            }

            if (this->wasPlayingBeforeAirborne[channel][LGFORCE_SURFACE] == 1) {
                this->PlaySurfaceEffect(channel, this->SurfaceEffectParams[channel].type, this->SurfaceEffectParams[channel].magnitude,
                                        this->SurfaceEffectParams[channel].period);
            }

            for (int i = 0; i < 10; i++) {
                this->wasPlayingBeforeAirborne[channel][i] = 0;
            }
        }
        break;
    }
}

bool LGWheels::IsConnected(long channel) {
    return this->wheels.IsConnected(channel);
}

// Mismo reparto que StopForce, pero leyendo el Playing de la ranura. El caso de
// aterrizaje no mira ninguna ranura: consulta IsAirborne, y comparando con 1.
bool LGWheels::IsPlaying(long channel, long forceType) {
    switch (forceType) {
    case LGFORCE_SPRING:
        if (this->condition.Playing[channel][0]) {
            return true;
        }
        break;

    case LGFORCE_CONSTANT:
        if (this->constant.Playing[channel][0]) {
            return true;
        }
        break;

    case LGFORCE_DAMPER:
        if (this->condition.Playing[channel][1]) {
            return true;
        }
        break;

    case LGFORCE_SIDE_COLLISION:
        if (this->constant.Playing[channel][1]) {
            return true;
        }
        break;

    case LGFORCE_FRONTAL_COLLISION:
        if (this->periodic.Playing[channel][0]) {
            return true;
        }
        break;

    case LGFORCE_DIRT_ROAD:
        if (this->periodic.Playing[channel][1]) {
            return true;
        }
        break;

    case LGFORCE_BUMPY_ROAD:
        if (this->periodic.Playing[channel][2]) {
            return true;
        }
        break;

    case LGFORCE_SLIPPERY_ROAD:
        if (this->condition.Playing[channel][2]) {
            return true;
        }
        break;

    case LGFORCE_SURFACE:
        if (this->periodic.Playing[channel][3]) {
            return true;
        }
        break;

    case LGFORCE_CAR_AIRBORNE:
        if (this->IsAirborne[channel] == 1) {
            return true;
        }
        break;
    }

    return false;
}

bool LGWheels::ButtonIsPressed(long channel, unsigned long buttonMask) {
    return this->wheels.ButtonIsPressed(channel, buttonMask);
}

bool LGWheels::PedalsConnected(long channel) {
    return this->wheels.PedalsConnected(channel);
}

void LGWheels::PlayAutoCalibAndSpringForce(long channel) {
    if (this->wheels.IsConnected(channel)) {
        if (!this->IsAirborne[channel]) {
            if (this->periodic.EffectID[channel][4] == -1) {
                this->periodic.DownloadForce(channel, 4, this->wheels.WheelHandles[channel], 3, 0x898, 0, 0xB4, 0x5A, 0x898, 0, 0, 0, 0, 0, 0);
                this->periodic.Start(channel, 4);
            }

            if (this->condition.EffectID[channel][0] == -1) {
                this->condition.DownloadForce(channel, 0, this->wheels.WheelHandles[channel], 7, -1, 0x898, 0, 0, 0xB4, 0xB4, 0xB4, 0xB4);
                this->condition.Start(channel, 0);
            }
        }
    }
}

// El muelle tambien cede el paso al derrape.
void LGWheels::PlaySpringForce(long channel, signed char offset, unsigned char saturation, short coefficient) {
    int ret = 0;

    if (this->condition.Playing[channel][2]) {
        return;
    }

    if (this->wheels.IsConnected(channel)) {
        if (!this->IsAirborne[channel]) {
            if (this->condition.Playing[channel][0]) {
                if (!this->SameSpringForceParams(channel, offset, saturation, coefficient)) {
                    ret = this->condition.UpdateForce(channel, 0, 7, -1, 0, offset, 0, saturation, saturation, coefficient, coefficient);
                    if (ret >= 0) {
                        this->SpringForceParams[channel].offset = offset;
                        this->SpringForceParams[channel].saturation = saturation;
                        this->SpringForceParams[channel].coefficient = coefficient;
                    }
                }
            } else {
                if (this->condition.EffectID[channel][0] == -1) {
                    ret = this->condition.DownloadForce(channel, 0, this->wheels.WheelHandles[channel], 7, -1, 0, offset, 0, saturation, saturation,
                                                        coefficient, coefficient);
                    if (ret >= 0) {
                        this->SpringForceParams[channel].offset = offset;
                        this->SpringForceParams[channel].saturation = saturation;
                        this->SpringForceParams[channel].coefficient = coefficient;
                    }
                    this->condition.Start(channel, 0);
                } else if (this->SameSpringForceParams(channel, offset, saturation, coefficient)) {
                    this->condition.Start(channel, 0);
                    return;
                } else {
                    ret = this->condition.UpdateForce(channel, 0, 7, -1, 0, offset, 0, saturation, saturation, coefficient, coefficient);
                    if (ret >= 0) {
                        this->SpringForceParams[channel].offset = offset;
                        this->SpringForceParams[channel].saturation = saturation;
                        this->SpringForceParams[channel].coefficient = coefficient;
                    }
                    this->condition.Start(channel, 0);
                }
            }
        }
    } else {
        OSReport("ERROR: trying to play a force on channel %d but no wheel opened.\n", channel);
    }
}

void LGWheels::StopSpringForce(long channel) {
    this->StopForce(channel, LGFORCE_SPRING);
}

bool LGWheels::SameSpringForceParams(long channel, signed char offset, unsigned char saturation, short coefficient) {
    return this->SpringForceParams[channel].offset == offset && this->SpringForceParams[channel].saturation == saturation &&
           this->SpringForceParams[channel].coefficient == coefficient;
}

void LGWheels::PlayConstantForce(long channel, short magnitude, unsigned short direction) {
    int ret = 0;

    if (this->wheels.IsConnected(channel)) {
        if (!this->IsAirborne[channel]) {
            if (this->constant.Playing[channel][0]) {
                if (!this->SameConstantForceParams(channel, magnitude, direction)) {
                    ret = this->constant.UpdateForce(channel, 0, -1, 0, magnitude, direction, 0, 0, 0, 0);
                    if (ret >= 0) {
                        this->ConstantForceParams[channel].magnitude = magnitude;
                        this->ConstantForceParams[channel].direction = direction;
                    }
                }
            } else {
                if (this->constant.EffectID[channel][0] == -1) {
                    ret = this->constant.DownloadForce(channel, 0, this->wheels.WheelHandles[channel], -1, 0, magnitude, direction, 0, 0, 0, 0);
                    if (ret >= 0) {
                        this->ConstantForceParams[channel].magnitude = magnitude;
                        this->ConstantForceParams[channel].direction = direction;
                    }
                    this->constant.Start(channel, 0);
                } else if (this->SameConstantForceParams(channel, magnitude, direction)) {
                    this->constant.Start(channel, 0);
                    return;
                } else {
                    ret = this->constant.UpdateForce(channel, 0, -1, 0, magnitude, direction, 0, 0, 0, 0);
                    if (ret >= 0) {
                        this->ConstantForceParams[channel].magnitude = magnitude;
                        this->ConstantForceParams[channel].direction = direction;
                    }
                    this->constant.Start(channel, 0);
                }
            }
        }
    } else {
        OSReport("ERROR: trying to play a force on channel %d but no wheel opened.\n", channel);
    }
}

void LGWheels::StopConstantForce(long channel) {
    this->StopForce(channel, LGFORCE_CONSTANT);
}

bool LGWheels::SameConstantForceParams(long channel, short magnitude, unsigned short direction) {
    return this->ConstantForceParams[channel].magnitude == magnitude && this->ConstantForceParams[channel].direction == direction;
}

// El amortiguador cede el paso al derrape: si ese efecto suena, no hace nada.
// Las ramas de descarga y de actualizacion comparten la cola, que guarda los
// parametros y arranca el efecto.
void LGWheels::PlayDamperForce(long channel, short coefficient) {
    int ret = 0;

    if (this->condition.Playing[channel][2]) {
        return;
    }

    if (this->wheels.IsConnected(channel)) {
        if (!this->IsAirborne[channel]) {
            if (this->condition.Playing[channel][1]) {
                if (!this->SameDamperForceParams(channel, coefficient)) {
                    ret = this->condition.UpdateForce(channel, 1, 8, -1, 0, 0, 0, 0xFF, 0xFF, coefficient, coefficient);
                    if (ret >= 0) {
                        this->DamperForceParams[channel].coefficient = coefficient;
                    }
                }
            } else {
                if (this->condition.EffectID[channel][1] == -1) {
                    ret = this->condition.DownloadForce(channel, 1, this->wheels.WheelHandles[channel], 8, -1, 0, 0, 0, 0xFF, 0xFF, coefficient,
                                                        coefficient);
                    if (ret >= 0) {
                        this->DamperForceParams[channel].coefficient = coefficient;
                    }
                    this->condition.Start(channel, 1);
                } else if (this->SameDamperForceParams(channel, coefficient)) {
                    this->condition.Start(channel, 1);
                    return;
                } else {
                    ret = this->condition.UpdateForce(channel, 1, 8, -1, 0, 0, 0, 0xFF, 0xFF, coefficient, coefficient);
                    if (ret >= 0) {
                        this->DamperForceParams[channel].coefficient = coefficient;
                    }
                    this->condition.Start(channel, 1);
                }
            }
        }
    } else {
        OSReport("ERROR: trying to play a force on channel %d but no wheel opened.\n", channel);
    }
}

void LGWheels::StopDamperForce(long channel) {
    this->StopForce(channel, LGFORCE_DAMPER);
}

bool LGWheels::SameDamperForceParams(long channel, short coefficient) {
    return this->DamperForceParams[channel].coefficient == coefficient;
}

// Colision frontal: ranura 0 de periodic, tipo 3 (cuadrada), duracion 0x96 con
// fade 0x14. Todas las ramas terminan arrancando el efecto.
// r76: retirado el pin `register unsigned int magnitude asm("r27")` que copiaba el
// parametro. El DWARF-1 del original da esta misma firma (this r29, channel r31,
// magnitude r27, ret r30) y NINGUNA otra local ni bloque lexico. Sin el pin queda una
// sola permutacion r27<->r28 entre magnitude y el pseudo de &this->periodic (440/440 B,
// 99,45%): prioridad de global-alloc 2673 contra 1250 (6 refs / 96 insns), y no hay forma
// de fuente que la invierta (harian falta 13 refs o la mitad de vida). Medidas de la r76:
// sin `playing` 97,68%; con periodic al principio 97,14%; sin la variable periodic 95,95%
// y 444 B; con guarda temprana en vez de else 0%.
void LGWheels::PlayFrontalCollisionForce(long channel, unsigned char magnitude) {
    int ret = 0;

    if (this->wheels.IsConnected(channel)) {
        int playing = this->periodic.Playing[channel][0];
        Periodic *periodic = &this->periodic;

        if (playing) {
            if (!this->SameFrontalCollisionForceParams(channel, magnitude)) {
                ret = periodic->UpdateForce(channel, 0, 3, 0x96, 0, magnitude, 0x5A, 0x4B, 0, 0, 0, 0x14, 0, 0);
                if (ret >= 0) {
                    this->FrontalCollisionParams[channel].magnitude = magnitude;
                }
            }
        } else {
            if (periodic->EffectID[channel][0] == -1) {
                ret = periodic->DownloadForce(channel, 0, this->wheels.WheelHandles[channel], 3, 0x96, 0, magnitude, 0x5A, 0x4B, 0,
                                                   0, 0, 0x14, 0, 0);
                if (ret >= 0) {
                    this->FrontalCollisionParams[channel].magnitude = magnitude;
                }
            } else if (!this->SameFrontalCollisionForceParams(channel, magnitude)) {
                ret = periodic->UpdateForce(channel, 0, 3, 0x96, 0, magnitude, 0x5A, 0x4B, 0, 0, 0, 0x14, 0, 0);
                if (ret >= 0) {
                    this->FrontalCollisionParams[channel].magnitude = magnitude;
                }
            }
        }

        periodic->Start(channel, 0);
    } else {
        OSReport("ERROR: trying to play a force on channel %d but no wheel opened.
", channel);
    }
}

bool LGWheels::SameFrontalCollisionForceParams(long channel, short magnitude) {
    return this->FrontalCollisionParams[channel].magnitude == magnitude;
}

// Los dos efectos de superficie periodicos son el mismo codigo con otra ranura,
// otro tipo y otro periodo: tierra usa la 1 con tipo 2 y periodo 0x41, y baches
// la 2 con tipo 3 y periodo 0x64. La direccion es 0x5A en ambos.
void LGWheels::PlayDirtRoadEffect(long channel, unsigned char magnitude) {
    int ret = 0;

    if (this->wheels.IsConnected(channel)) {
        if (!this->IsAirborne[channel]) {
            if (this->periodic.Playing[channel][1]) {
                if (!this->SameDirtRoadEffectParams(channel, magnitude)) {
                    ret = this->periodic.UpdateForce(channel, 1, 2, -1, 0, magnitude, 0x5A, 0x41, 0, 0, 0, 0, 0, 0);
                    if (ret >= 0) {
                        this->DirtRoadParams[channel].magnitude = magnitude;
                    }
                }
            } else {
                if (this->periodic.EffectID[channel][1] == -1) {
                    ret = this->periodic.DownloadForce(channel, 1, this->wheels.WheelHandles[channel], 2, -1, 0, magnitude, 0x5A, 0x41, 0, 0, 0,
                                                       0, 0, 0);
                    if (ret >= 0) {
                        this->DirtRoadParams[channel].magnitude = magnitude;
                    }
                    this->periodic.Start(channel, 1);
                } else if (this->SameDirtRoadEffectParams(channel, magnitude)) {
                    this->periodic.Start(channel, 1);
                    return;
                } else {
                    ret = this->periodic.UpdateForce(channel, 1, 2, -1, 0, magnitude, 0x5A, 0x41, 0, 0, 0, 0, 0, 0);
                    if (ret >= 0) {
                        this->DirtRoadParams[channel].magnitude = magnitude;
                    }
                    this->periodic.Start(channel, 1);
                }
            }
        }
    } else {
        OSReport("ERROR: trying to play a force on channel %d but no wheel opened.
", channel);
    }
}

void LGWheels::StopDirtRoadEffect(long channel) {
    this->StopForce(channel, LGFORCE_DIRT_ROAD);
}

bool LGWheels::SameDirtRoadEffectParams(long channel, short magnitude) {
    return this->DirtRoadParams[channel].magnitude == magnitude;
}

void LGWheels::PlayBumpyRoadEffect(long channel, unsigned char magnitude) {
    int ret = 0;

    if (this->wheels.IsConnected(channel)) {
        if (!this->IsAirborne[channel]) {
            if (this->periodic.Playing[channel][2]) {
                if (!this->SameBumpyRoadEffectParams(channel, magnitude)) {
                    ret = this->periodic.UpdateForce(channel, 2, 3, -1, 0, magnitude, 0x5A, 0x64, 0, 0, 0, 0, 0, 0);
                    if (ret >= 0) {
                        this->BumpyRoadParams[channel].magnitude = magnitude;
                    }
                }
            } else {
                if (this->periodic.EffectID[channel][2] == -1) {
                    ret = this->periodic.DownloadForce(channel, 2, this->wheels.WheelHandles[channel], 3, -1, 0, magnitude, 0x5A, 0x64, 0, 0, 0,
                                                       0, 0, 0);
                    if (ret >= 0) {
                        this->BumpyRoadParams[channel].magnitude = magnitude;
                    }
                    this->periodic.Start(channel, 2);
                } else if (this->SameBumpyRoadEffectParams(channel, magnitude)) {
                    this->periodic.Start(channel, 2);
                    return;
                } else {
                    ret = this->periodic.UpdateForce(channel, 2, 3, -1, 0, magnitude, 0x5A, 0x64, 0, 0, 0, 0, 0, 0);
                    if (ret >= 0) {
                        this->BumpyRoadParams[channel].magnitude = magnitude;
                    }
                    this->periodic.Start(channel, 2);
                }
            }
        }
    } else {
        OSReport("ERROR: trying to play a force on channel %d but no wheel opened.
", channel);
    }
}

void LGWheels::StopBumpyRoadEffect(long channel) {
    this->StopForce(channel, LGFORCE_BUMPY_ROAD);
}

bool LGWheels::SameBumpyRoadEffectParams(long channel, short magnitude) {
    return this->BumpyRoadParams[channel].magnitude == magnitude;
}

// El derrape vive en la ranura 2 de condition (tipo 8, coeficientes negados:
// el volante se "afloja"). Antes de sonar aparta al amortiguador y al muelle,
// apuntando en las banderas cuales eran para restaurarlos al parar.
void LGWheels::PlaySlipperyRoadEffect(long channel, short magnitude) {
    int ret = 0;

    if (this->IsPlaying(channel, LGFORCE_DAMPER)) {
        this->StopDamperForce(channel);
        this->condition.Playing[channel][1] = 0;
        this->damperWasPlaying[channel] = 1;
    }

    if (this->IsPlaying(channel, LGFORCE_SPRING)) {
        this->StopSpringForce(channel);
        this->condition.Playing[channel][0] = 0;
        this->springWasPlaying[channel] = 1;
    }

    if (this->wheels.IsConnected(channel)) {
        if (!this->IsAirborne[channel]) {
            if (this->condition.Playing[channel][2]) {
                if (!this->SameSlipperyRoadEffectParams(channel, magnitude)) {
                    ret = this->condition.UpdateForce(channel, 2, 8, -1, 0, 0, 0, 0xFF, 0xFF, -magnitude, -magnitude);
                    if (ret >= 0) {
                        this->SlipperyRoadParams[channel].magnitude = magnitude;
                    }
                }
            } else {
                if (this->condition.EffectID[channel][2] == -1) {
                    ret = this->condition.DownloadForce(channel, 2, this->wheels.WheelHandles[channel], 8, -1, 0, 0, ret, 0xFF, 0xFF,
                                                        -magnitude, -magnitude);
                    if (ret >= 0) {
                        this->SlipperyRoadParams[channel].magnitude = magnitude;
                    }
                    this->condition.Start(channel, 2);
                } else if (this->SameSlipperyRoadEffectParams(channel, magnitude)) {
                    this->condition.Start(channel, 2);
                    return;
                } else {
                    ret = this->condition.UpdateForce(channel, 2, 8, -1, 0, 0, 0, 0xFF, 0xFF, -magnitude, -magnitude);
                    if (ret >= 0) {
                        this->SlipperyRoadParams[channel].magnitude = magnitude;
                    }
                    this->condition.Start(channel, 2);
                }
            }
        }
    } else {
        OSReport("ERROR: trying to play a force on channel %d but no wheel opened.\n", channel);
    }
}

void LGWheels::StopSlipperyRoadEffect(long channel) {
    this->StopForce(channel, LGFORCE_SLIPPERY_ROAD);
}

bool LGWheels::SameSlipperyRoadEffectParams(long channel, short magnitude) {
    return this->SlipperyRoadParams[channel].magnitude == magnitude;
}

// Efecto de superficie generico: ranura 3 de periodic, direccion 0x5A. El tipo
// (forma de onda) no se puede actualizar en caliente: si cambia, se destruye y
// se redescarga la fuerza; si no, basta con UpdateForce.
void LGWheels::PlaySurfaceEffect(long channel, unsigned char type, unsigned char magnitude, unsigned short period) {
    long ret = 0;

    if (this->wheels.IsConnected(channel)) {
        if (!this->IsAirborne[channel]) {
            if (this->periodic.Playing[channel][3]) {
                if (!this->SameSurfaceEffectParams(channel, type, magnitude, period)) {
                    if (type != this->SurfaceEffectParams[channel].type) {
                        this->periodic.Destroy(channel, 3);
                        ret = this->periodic.DownloadForce(channel, 3, this->wheels.WheelHandles[channel], type, -1, 0, magnitude, 0x5A,
                                                           period, 0, 0, 0, 0, 0, 0);
                        this->periodic.Start(channel, 3);
                    } else {
                        ret = this->periodic.UpdateForce(channel, 3, type, -1, 0, magnitude, 0x5A, period, 0, 0, 0, 0, 0, 0);
                    }

                    if (ret >= 0) {
                        this->SurfaceEffectParams[channel].type = type;
                        this->SurfaceEffectParams[channel].magnitude = magnitude;
                        this->SurfaceEffectParams[channel].period = period;
                    }
                }
            } else {
                if (this->periodic.EffectID[channel][3] == -1) {
                    if (this->periodic.DownloadForce(channel, 3, this->wheels.WheelHandles[channel], type, -1, 0, magnitude, 0x5A, period, 0, 0,
                                                     0, 0, 0, 0) >= 0) {
                        this->SurfaceEffectParams[channel].type = type;
                        this->SurfaceEffectParams[channel].magnitude = magnitude;
                        this->SurfaceEffectParams[channel].period = period;
                    }

                    this->periodic.Start(channel, 3);
                } else if (this->SameSurfaceEffectParams(channel, type, magnitude, period)) {
                    this->periodic.Start(channel, 3);
                } else {
                    if (type != this->SurfaceEffectParams[channel].type) {
                        this->periodic.Destroy(channel, 3);
                        ret = this->periodic.DownloadForce(channel, 3, this->wheels.WheelHandles[channel], type, -1, 0, magnitude, 0x5A,
                                                           period, 0, 0, 0, 0, 0, 0);
                    } else {
                        ret = this->periodic.UpdateForce(channel, 3, type, -1, 0, magnitude, 0x5A, period, 0, 0, 0, 0, 0, 0);
                    }

                    if (ret >= 0) {
                        this->SurfaceEffectParams[channel].type = type;
                        this->SurfaceEffectParams[channel].magnitude = magnitude;
                        this->SurfaceEffectParams[channel].period = period;
                    }

                    this->periodic.Start(channel, 3);
                }
            }
        }
    } else {
        OSReport("ERROR: trying to play a force on channel %d but no wheel opened.\n", channel);
    }
}

void LGWheels::StopSurfaceEffect(long channel) {
    this->StopForce(channel, LGFORCE_SURFACE);
}

bool LGWheels::SameSurfaceEffectParams(long channel, unsigned char type, unsigned char magnitude, unsigned short period) {
    return this->SurfaceEffectParams[channel].type == type && this->SurfaceEffectParams[channel].magnitude == magnitude &&
           this->SurfaceEffectParams[channel].period == period;
}

// Al despegar el coche se paran todas las fuerzas que estuvieran sonando y se
// apunta cuales eran, para poder restaurarlas al aterrizar.
void LGWheels::PlayCarAirborne(long channel) {
    if (this->wheels.IsConnected(channel)) {
        this->IsAirborne[channel] = 1;

        if (this->IsPlaying(channel, LGFORCE_SPRING)) {
            this->StopSpringForce(channel);
            this->wasPlayingBeforeAirborne[channel][LGFORCE_SPRING] = 1;
        }

        if (this->IsPlaying(channel, LGFORCE_CONSTANT)) {
            this->StopConstantForce(channel);
            this->wasPlayingBeforeAirborne[channel][LGFORCE_CONSTANT] = 1;
        }

        if (this->IsPlaying(channel, LGFORCE_DAMPER)) {
            this->StopDamperForce(channel);
            this->wasPlayingBeforeAirborne[channel][LGFORCE_DAMPER] = 1;
        }

        if (this->IsPlaying(channel, LGFORCE_DIRT_ROAD)) {
            this->StopDirtRoadEffect(channel);
            this->wasPlayingBeforeAirborne[channel][LGFORCE_DIRT_ROAD] = 1;
        }

        if (this->IsPlaying(channel, LGFORCE_BUMPY_ROAD)) {
            this->StopBumpyRoadEffect(channel);
            this->wasPlayingBeforeAirborne[channel][LGFORCE_BUMPY_ROAD] = 1;
        }

        if (this->IsPlaying(channel, LGFORCE_SLIPPERY_ROAD)) {
            this->StopSlipperyRoadEffect(channel);
            this->wasPlayingBeforeAirborne[channel][LGFORCE_SLIPPERY_ROAD] = 1;
        }

        if (this->IsPlaying(channel, LGFORCE_SURFACE)) {
            this->StopSurfaceEffect(channel);
            this->wasPlayingBeforeAirborne[channel][LGFORCE_SURFACE] = 1;
        }
    } else {
        OSReport("ERROR: trying to play a force on channel %d but no wheel opened.\n", channel);
    }
}

void LGWheels::StopCarAirborne(long channel) {
    this->StopForce(channel, LGFORCE_CAR_AIRBORNE);
}
