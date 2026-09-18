#include "Chassis.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/brakes.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/tires.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/transmission.h"
#include "Speed/Indep/Src/Generated/Hash.hpp"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Physics/Common/VehicleSystem.h"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"
#include "Speed/Indep/Src/Physics/Wheel.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"

#define EPSILON 0.000001f

#define MAKEATTRIB const

// total size: 0x110
class SuspensionTraffic : public Chassis {
  public:
    // total size: 0x10C
    class Tire : public Wheel {
      public:
        enum LastRotationSign {
            WAS_POSITIVE = 0,
            WAS_ZERO = 1,
            WAS_NEGATIVE = 2,
        };

        Tire(float radius, int index, const Attrib::Gen::tires *specs, const Attrib::Gen::brakes *brakes);

        void SetBrake(float brake) {
            this->mBrake = brake;
        }
        void SetEBrake(float ebrake) {
            this->mEBrake = ebrake;
        }
        float GetEBrake() const {
            return this->mEBrake;
        }

        float GetRadius() const {
            return this->mRadius;
        }
        float GetAngularVelocity() const {
            return this->mAV;
        }
        void SetAngularVelocity(float av) {
            this->mAV = av;
        }

        void Stop() {
            this->mAV = 0.0f;
            this->mSlip = 0.0f;
            this->mRoadSpeed = 0.0f;
            this->mSlipAngle = 0.0f;
        }

        float GetCurrentSlip() const {
            return this->mSlip;
        }
        float GetTraction() const {
            return mEBrake > 0.0f ? 0.0f : 1.0f;
        }

        void BeginFrame();
        void EndFrame(float dT);

        float GetLoad() const {
            return mLoad;
        }
        void ApplyTorque(float torque) {
            this->mAppliedTorque += torque;
        }
        float GetLateralForce() const {
            return this->mLateralForce;
        }
        float GetLongitudeForce() const {
            return this->mLongitudeForce;
        }
        float GetLateralSpeed() const {
            return this->mLateralSpeed;
        }

        void UpdateLoaded(float lat_vel, float fwd_vel, float load, float dT);
        void UpdateFree(float dT);

        float GetSlipAngle() const {
            return mSlipAngle;
        }

      private:
        const float mRadius;                // offset 0xC4, size 0x4
        float mBrake;                       // offset 0xC8, size 0x4
        float mEBrake;                      // offset 0xCC, size 0x4
        float mAV;                          // offset 0xD0, size 0x4
        float mLoad;                        // offset 0xD4, size 0x4
        float mLateralForce;                // offset 0xD8, size 0x4
        float mLongitudeForce;              // offset 0xDC, size 0x4
        float mAppliedTorque;               // offset 0xE0, size 0x4
        float mSlip;                        // offset 0xE4, size 0x4
        float mLastTorque;                  // offset 0xE8, size 0x4
        const int mWheelIndex;              // offset 0xEC, size 0x4
        float mRoadSpeed;                   // offset 0xF0, size 0x4
        const Attrib::Gen::tires *mSpecs;   // offset 0xF4, size 0x4
        const Attrib::Gen::brakes *mBrakes; // offset 0xF8, size 0x4
        float mSlipAngle;                   // offset 0xFC, size 0x4
        const int mAxleIndex;               // offset 0x100, size 0x4
        bool mSlipping;                     // offset 0x104, size 0x1
        float mLateralSpeed;                // offset 0x108, size 0x4
    };

    SuspensionTraffic(const BehaviorParams &bp, const SuspensionParams &sp);
    ~SuspensionTraffic() override;
    static Behavior *Construct(const BehaviorParams &params);

    // ISuspension
    void MatchSpeed(float speed) override;
    float GetWheelAngularVelocity(int index) const override {
        return mTires[index]->GetAngularVelocity();
    }
    float GetWheelRadius(unsigned int index) const override {
        return mTires[index]->GetRadius();
    }
    float GetWheelSlip(unsigned int idx) const override {
        return mTires[idx]->GetCurrentSlip();
    }
    Mps GetToleratedSlip(unsigned int idx) const override {
        return 1.0f;
    }
    float GetWheelSkid(unsigned int idx) const override {
        return mTires[idx]->GetLateralSpeed();
    }
    Newtons GetWheelLoad(unsigned int i) const override {
        return mTires[i]->GetLoad();
    }
    float GetWheelTraction(unsigned int index) const override {
        return mTires[index]->GetTraction();
    }
    void SetWheelAngularVelocity(int wheel, float w) override {}
    unsigned int GetNumWheels() const override {
        return 4;
    }
    const UMath::Vector3 &GetWheelPos(unsigned int i) const override {
        return mTires[i]->GetPosition();
    }
    const UMath::Vector3 &GetWheelLocalPos(unsigned int i) const override {
        return mTires[i]->GetLocalArm();
    }
    UMath::Vector3 GetWheelCenterPos(unsigned int i) const override;
    void ApplyVehicleEntryForces(bool enteringVehicle, const UMath::Vector3 &pos, bool calledfromEvent) override {}
    const float GetWheelRoadHeight(unsigned int i) const override {
        return mTires[i]->GetNormal().w;
    }
    float GetCompression(unsigned int i) const override {
        return mTires[i]->GetCompression();
    }
    const UMath::Vector4 &GetWheelRoadNormal(unsigned int i) const override {
        return mTires[i]->GetNormal();
    }
    bool IsWheelOnGround(unsigned int i) const override {
        return mTires[i]->IsOnGround();
    }
    const SimSurface &GetWheelRoadSurface(unsigned int i) const override {
        return mTires[i]->GetSurface();
    }
    const UMath::Vector3 &GetWheelVelocity(unsigned int i) const override {
        return mTires[i]->GetVelocity();
    }
    int GetNumWheelsOnGround() const override {
        return mNumWheelsOnGround;
    }
    float GetWheelSteer(unsigned int wheel) const override {
        return DEG2ANGLE(mLastSteer);
    }
    float GetMaxSteering() const override {
        return DEG2ANGLE(mMaxSteering);
    }
    Angle GetWheelSlipAngle(unsigned int idx) const override {
        return mTires[idx]->GetSlipAngle();
    }

    // Behavior
    void OnTaskSimulate(float dT) override;
    void Reset() override;

  protected:
    void DoSimpleAero(State &state);
    void DoWheelForces(State &state);
    void DoDriveForces(State &state);
    void DoSteering(State &state, UMath::Vector3 &right, UMath::Vector3 &left);
    float DoHP2Steering(State &state);

    // Behavior
    void OnBehaviorChange(const UCrc32 &mechanic) override;

    Tire &GetWheel(unsigned int i) {
        return *this->mTires[i];
    }

    const Tire &GetWheel(unsigned int i) const {
        return *this->mTires[i];
    }

  private:
#ifdef EA_BUILD_A124
    void CreateTires();
#endif

    BehaviorSpecsPtr<Attrib::Gen::tires> mTireInfo;              // offset 0x94, size 0x14
    BehaviorSpecsPtr<Attrib::Gen::brakes> mBrakeInfo;            // offset 0xA8, size 0x14
    BehaviorSpecsPtr<Attrib::Gen::chassis> mSuspensionInfo;      // offset 0xBC, size 0x14
    BehaviorSpecsPtr<Attrib::Gen::transmission> mDrivetrainInfo; // offset 0xD0, size 0x14
    IRigidBody *mRB;                                             // offset 0xE4, size 0x4
    ICollisionBody *mRBComplex;                                  // offset 0xE8, size 0x4
    IInput *mInput;                                              // offset 0xEC, size 0x4
    ITransmission *mTransmission;                                // offset 0xF0, size 0x4
    float mLastSteer;                                            // offset 0xF4, size 0x4
    unsigned int mNumWheelsOnGround;                             // offset 0xF8, size 0x4
    float mMaxSteering;                                          // offset 0xFC, size 0x4
    Tire *mTires[4];                                             // offset 0x100, size 0x10
};

BIND_BEHAVIOR_FACTORY(SuspensionTraffic);

SuspensionTraffic::Tire::Tire(float radius, int index, const Attrib::Gen::tires *specs, const Attrib::Gen::brakes *brakes)
    : Wheel(0),                          //
      mRadius(UMath::Max(radius, 0.1f)), //
      mWheelIndex(index),                //
      mAxleIndex(index >> 1),            //
      mSpecs(specs),                     //
      mBrakes(brakes),                   //
      mSlipping(false),                  //
      mLateralSpeed(0.0f),               //
      mBrake(0.0f),                      //
      mEBrake(0.0f),                     //
      mAV(0.0f),                         //
      mLoad(0.0f),                       //
      mLateralForce(0.0f),               //
      mLongitudeForce(0.0f),             //
      mAppliedTorque(0.0f),              //
      mSlip(0.0f),                       //
      mLastTorque(0.0f),                 //
      mRoadSpeed(0.0f),                  //
      mSlipAngle(0.0f) {}

void SuspensionTraffic::Tire::BeginFrame() {
    this->mAppliedTorque = 0.0f;
    this->SetForce(UMath::Vector3::kZero);
    this->mLateralForce = 0.0f;
    this->mLongitudeForce = 0.0f;
}

void SuspensionTraffic::Tire::EndFrame(float dT) {}

void SuspensionTraffic::Tire::UpdateFree(float dT) {
    this->mSlipping = false;
    this->mLoad = 0.0f;
    this->mSlip = 0.0f;
    this->mSlipAngle = 0.0f;
    if ((this->mEBrake > 0.0f) || (this->mBrake > 0.0f)) {
        this->mAV = 0.0f;
    }
    this->mLateralForce = 0.0f;
    this->mLongitudeForce = 0.0f;
}

extern float BrakingTorque;
extern float EBrakingTorque;

// UNSOLVED, 856 B al 97,21028 %: las 21 filas son UNA causa y NO es la fuente.
// El objetivo eleva `lis r30, <1.0f>@ha` al bloque de `bl VU0_Atan2` y lo usa en
// las DOS primeras comparaciones contra 1.0f; nosotros lo recalculamos en cada
// una. De ahi salen el segundo GPR salvado (`stmw r30` contra `stw r31`), el
// marco 0x30 contra 0x28 y los 4 B (860 contra 856).
//
// r36c, leido de los volcados RTL de cc1plus sobre una mini-TU de este .cpp
// (11 s por prueba; `-dG -ds -dt -dl -dg`). El mecanismo, entero:
//   1. PRE (gcse.c, `one_pre_gcse_pass`) SI hace el trabajo: elimina las CINCO
//      apariciones redundantes de `(high (*$LC))` (bb 23/26/31/36/39) contra un
//      solo pseudo 362 e inserta dos copias, una de ellas al FINAL del bloque
//      basico que contiene `bl VU0_Atan2` -- exactamente donde el objetivo la
//      tiene, porque sched1 luego la sube por delante de la llamada.
//   2. cse2 (`-frerun-cse-after-loop`) lo DESHACE: las cinco copias
//      `(set (reg N) (reg 362))` vuelven a ser `(set (reg N) (high (*$LC)))`.
//      Con `-fno-rerun-cse-after-loop` el objeto cae a 848 B / 84,67 % (se
//      conserva de mas), o sea que la bandera es la correcta.
//   3. al pseudo 362 le queda UN solo uso, y entonces `update_equiv_regs` de
//      local-alloc.c ("move the register initialization just before the use")
//      lo BAJA hasta su uso; ahi ya no cruza ninguna llamada y global_alloc le
//      da r9 en vez de un preservado.
// O sea: la divergencia esta dentro de cse2 y no hay construccion de fuente en
// esta funcion que la toque. Comprobado ademas que la fuente ES la del original:
// el volcado DWARF trae las mismas tres locales (slip_speed f11, catchupfriction
// sin registro, skid_speed f1), los mismos dos bloques anonimos con
// brake_spec/bt y ebrake_spec/ebt, y el mismo arbol de inlines (Max, BRAKES/At/
// FTLB2NM/ApplyTorque x2, cuatro Abs, Atan2a, Sqrt, GRIP_SCALE/At x2, Sina, Min).
// Banderas barridas y NEGATIVAS: -fno-cse-skip-blocks (85,16 %, 892 B),
// -fno-force-mem (96,98 %, 26 filas), -fno-schedule-insns (75,81 %),
// -fno-omit-frame-pointer (96,05 %, 868 B); IDENTICAS: -fno-cse-follow-jumps,
// -fno-expensive-optimizations, -fno-move-all-movables, -fno-rerun-loop-opt,
// -fcaller-saves.
//
// r36d: la cuarta palanca (barrera contra un plegado de CSE) tampoco llega
// aqui, y la razon es de fondo: lo que hay que mantener vivo NO es ninguna
// variable de la fuente sino el pseudo del compilador que guarda
// `high(*$LC917)` (el @ha del 1.0f), y a un pseudo del compilador no se le
// puede poner un `asm`. Todo lo que se puede nombrar desde C es el FLOAT
// (`const float one = 1.0f` con `"+f"`), y eso lo dejaria en un FPR
// preservado con UN solo `lfs`, mientras que el objetivo hace `lfs` DOS
// veces desde r30: es otra forma, no la del objetivo.
// La condicion exacta que hay que romper esta en local-alloc.c
// (`update_equiv_regs`): solo baja el pseudo si le queda UN uso. Si a cse2
// se le escapasen DOS de los cinco plegados, el pseudo no se hundiria y
// global_alloc le daria un preservado -- que es el `lis r30` de la fila 86.
//
// r54: CORRECCION -- cse2 NO es el frente, y no hay forma de fuente que le
// haga escaparse un plegado. cse2 rehace SIEMPRE la copia de PRE porque en
// `cse_insn` el candidato `src_eqv` (la nota REG_EQUAL con el `high`) vale
// COST 0 -- rs6000.h, CONST_COSTS, `case HIGH: return 0` -- contra COST 1
// del pseudo (cse.c:519), y 0 < 1 en todos los bloques y para toda fuente.
// La cadena que SI produce el `lis r30` esta medida en dos funciones que
// casan al 100 % (AddRoadNoise de zWorld y UpdateForces de zMain), y es:
//   (1) cse1 le da al `high` de un sitio un SEGUNDO uso `lo_sum` en OTRO
//       bloque basico  ->  (2) PRE lo iza y deja una copia  ->  (3) el
//       `one_cprop_pass` siguiente mete el reaching_reg en ese uso lejano
//       ("COPY-PROP: Replacing reg N in insn M with reg R") -- y eso cse2
//       ya no lo puede deshacer  ->  (4) REG_N_REFS = 3, no se hunde,
//       global_alloc le da un preservado.
// Aqui el paso (1) es imposible: los dos usos que el objetivo comparte
// (filas 101 y 120) los separa la etiqueta de union del if/else de mSlip,
// que tiene LABEL_NUSES == 2 --los dos `bso` del `&&`, que el objetivo
// tambien tiene-- y `cse_end_of_basic_block` corta en toda CODE_LABEL que
// no pueda seguir (exige NUSES == 1). Y cprop de gcse.c 2.95 es SOLO
// global (`cprop` resetea por bloque; `oprs_not_set_p` rechaza el mismo
// bloque), asi que la copia de PRE en el bloque del uso nunca se propaga.
// Ver docs/analisis/r54-loaded.md: 30 formas de fuente medidas, ninguna
// mueve `lis30`.
//
// r61 (phys): NEGATIVO DURO, y ahora con la desigualdad EXACTA y su linea.
// Reproducido en un mini-TU (prefijo de zPhysicsBehaviors.cpp + este .cpp,
// 2,5 s por prueba): 860 B, 97,21028 %, 21 filas -- identico a la SourceList.
// Volcados FRESCOS de cc1plus (-dG -ds -dt -dl -dg) sobre esa base:
//   * PRE hace el trabajo BIEN: `PRE: redundant insn 528/694/751/860/920
//     (expression 40) ... reaching reg is 362`, e inserta el `(set (reg 362)
//     (high *$LC251))` DOS veces, la primera como insn 988 al final del bloque
//     del `bl VU0_Atan2` -- que es EXACTAMENTE la ranura del `lis r30` del
//     objetivo (fila 86). CERO lineas COPY-PROP, igual que en la r54.
//   * DATO NUEVO que la r54 no vio: cse2 NO deshace las cinco. Deja viva la del
//     if de la linea 372 (insn 468 usa `(lo_sum (reg 362) $LC251)` y mata el
//     `(set (reg 218) (high))` de insn 465) porque cae en el MISMO bloque
//     extendido `955..480`. O sea que nos falta UNA sola referencia, no cinco:
//     `.lreg` dice `Register 362 used 2 times ... in block 17`, y por eso
//     `update_equiv_regs` lo hunde (insn 1049) y global_alloc le da r9.
//   * La que falta es la de la linea 379, insn 528, en el bloque extendido
//     `965..609`. Ahi cse2 la rematerializa, y la razon es una desigualdad de
//     tabla, no una forma de fuente: cse.c:7191 elige `src` (el pseudo) solo si
//     `src_cost <= src_eqv_cost`; COST de un pseudo es 1 (cse.c:519-524, la
//     rama `REGNO >= FIRST_PSEUDO_REGISTER`) y `notreg_cost` del `(high ...)`
//     de la nota REG_EQUAL es 0 (rs6000.h:2513, CONST_COSTS `case HIGH:
//     return 0`). 1 <= 0 es FALSO para TODO pseudo y TODA fuente.
//   * La UNICA puerta que deja el codigo es cse.c:6837
//     (`if (elt && src_eqv_here && src_eqv_elt) src_eqv_here = 0;`): exige que
//     el reg 362 Y el `(high $LC251)` YA esten en la tabla de cse2 en ese insn,
//     o sea que el uso caiga en el MISMO bloque extendido que la insercion de
//     PRE. `cse_end_of_basic_block` (cse.c:8508) corta el bloque extendido en
//     la primera CODE_LABEL y solo sigue un salto con `LABEL_NUSES == 1`. El
//     bloque de la linea 379 es una UNION de dos caminos (el `&&` de la 372),
//     luego siempre abre bloque extendido nuevo. **Y el objetivo tiene esa
//     misma union** (sus dos `bso` a 0x19a50). No hay fuente que lo cambie.
// Formas NUEVAS medidas esta ronda (ninguna esta en la tabla de la r54):
//   ifs anidados con el `else` DUPLICADO (parte la etiqueta de union en dos,
//     NUSES==1 cada una, contando con que el cross-jump final las funda):
//     864 B, 52 filas, SIN `lis r30`. Mata la hipotesis del cross-jump.
//   `if (!(0<mEBrake)) ... else if (!(1<Abs)) ... else ...`: 856 B EXACTOS
//     pero 42 filas -- el `!` cambia la comparacion a `bgt` y pierde el
//     `cror un,eq,lt; bso` del objetivo. El tamano correcto por el motivo
//     equivocado; es la trampa de "una diferencia de tamano no es cercania".
// CONTROL que TENIA que cambiar y cambio: `-fno-rerun-cse-after-loop` da
//   848 B / 84,67 % y ADEMAS emite `lis r29, $LC249@ha` en un preservado con
//   cuatro usos. O sea: la forma del objetivo la fabrica PRE y la borra cse2,
//   confirmado en las dos direcciones.
// CONSECUENCIA: esta funcion NO se cierra desde la fuente con estas banderas.
//   Deja de ser DUDOSA y pasa a VEDA DURA con cita. zPhysicsBehaviors sigue a
//   `.text +4` (linkdelta) y es su UNICO bloqueo; el dia que se abra sera por
//   una palanca de banderas por unidad, no por una sentencia.
// Arnes: scratchpad/phys61/{mini2.cpp,cc.py,dif.py,rtl.py} (borrado al cerrar,
//   se rehace en dos minutos con el prefijo de zPhysicsBehaviors.cpp).
// r63 (orden-phys): NO REABIERTA, y la razon es de AUDITORIA, no de pereza. El
// encargo de la ronda la daba como "familia A, salio VIVA" citando una linea
// `COPY-PROP: Replacing reg 625 in insn 1210 with reg 753` del decisor de la r61;
// esa linea es de ActualReadJoystickData (zPlatform), no de esta funcion. El
// volcado FRESCO de la r61 sobre ESTA funcion dice literalmente "CERO lineas
// COPY-PROP", que es el paso (3) de la cadena y sin el no hay `lis r30`. La ficha
// de vedas (r60b) es ANTERIOR al volcado de la r61: manda la r61. Sigue VEDA DURA.
// Lo que SI cambia esta ronda: la unidad ya no tiene mas bloqueos de ORDEN antes
// del bloque diferido (textorder: 50 -> 38 saltos), asi que estos 4 B son ahora el
// UNICO obstaculo entre zPhysicsBehaviors y poder medir su DOL con dolwhere
// (`LAS SECCIONES NO COINCIDEN: obj 803A41B8 / nue 803A41C0`, +8 por el `.text +4`).
//
// r64 (diferido-phys): el encargo de la r64 VUELVE a darla como "familia A que
// salio VIVA" con la misma cita equivocada. NO la he reabierto: la correccion de
// la r63 de aqui arriba sigue en pie y no ha aparecido dato nuevo. Lo unico que
// he medido esta ronda sobre zPhysicsBehaviors son las tres guardas del bloque
// diferido de zPhysics (UTL_NO_COPY_CTOR, UTL_IMPLICIT_LIST_DTOR,
// UCOLLECTIONS_H_IMPLICIT_STORAGE_DTOR): CERO cambio aqui -- 811/1121
// descolocadas y 38 saltos antes y despues, `linkdelta .text +4 / resto IGUAL`.
// Esta unidad no instancia ni un Listable, asi que el frente nuevo no la toca.
// Si alguien reabre esto, que empiece por el volcado RTL, no por la fuente.
void SuspensionTraffic::Tire::UpdateLoaded(float lat_vel, float fwd_vel, float load, float dT) {
    float slip_speed;
    float catchupfriction;
    float skid_speed;

    if (this->mLoad <= 0.0f) {
        this->mAV = fwd_vel / this->mRadius;
    }

    this->mRoadSpeed = fwd_vel;
    this->mLateralSpeed = lat_vel;
    this->mLoad = UMath::Max(load, 0.0f);

    if (0.0f < this->mBrake) {
        const float brake_spec = this->mBrakes->BRAKES().At(this->mAxleIndex);
        float bt = this->mBrake * (FTLB2NM(brake_spec) * BrakingTorque);
        if (0.0f < this->mAV) {
            bt = -bt;
        }
        this->ApplyTorque(bt);
    }

    if (0.0f < this->mEBrake) {
        const float ebrake_spec = this->mBrakes->EBRAKE();
        float ebt = this->mEBrake * (FTLB2NM(ebrake_spec) * EBrakingTorque);
        if (0.0f < this->mAV) {
            ebt = -ebt;
        }
        this->ApplyTorque(ebt);
    }

    this->mSlipAngle = UMath::Atan2a(lat_vel, UMath::Abs(fwd_vel));
    slip_speed = this->mAV * this->mRadius - fwd_vel;

    if (0.0f < this->mEBrake && 1.0f < UMath::Abs(fwd_vel)) {
        this->mSlip = slip_speed;
    } else {
        this->mSlip = 0.0f;
    }

    skid_speed = UMath::Sqrt(slip_speed * slip_speed + lat_vel * lat_vel);
    if (this->mEBrake > 0.5f && skid_speed > 1.0f) {
        this->mSlipping = true;
        this->mLongitudeForce = ((-fwd_vel * 2) * this->mLoad * this->mSpecs->GRIP_SCALE().At(this->mAxleIndex)) / skid_speed;
    } else {
        this->mLongitudeForce = this->mAppliedTorque / this->mRadius;
    }

    this->mLateralForce = (-lat_vel * 2) * this->mLoad * this->mSpecs->GRIP_SCALE().At(this->mAxleIndex);
    if (1.0f < skid_speed) {
        this->mLateralForce /= skid_speed;
    }

    if (1.0f < UMath::Abs(fwd_vel)) {
        this->mLongitudeForce -= UMath::Sina(this->mSlipAngle) * this->mLateralForce * 0.5f;
    } else {
        catchupfriction = UMath::Min(UMath::Abs(lat_vel), 1.0f);
        this->mLateralForce *= catchupfriction;
    }

    this->mAV = ((1.0f - this->mEBrake) * fwd_vel) / this->mRadius;
}

Behavior *SuspensionTraffic::Construct(const BehaviorParams &params) {
    SuspensionParams sp(params.fparams.Fetch<SuspensionParams>(UCrc32(UCRC32_BASE)));
    return new SuspensionTraffic(params, sp);
}

SuspensionTraffic::SuspensionTraffic(const BehaviorParams &bp, const SuspensionParams &sp)
    : Chassis(bp),              //
      mTireInfo(this, 0),       //
      mBrakeInfo(this, 0),      //
      mSuspensionInfo(this, 0), //
      mDrivetrainInfo(this, 0), //
      mLastSteer(0.0f),         //
      mMaxSteering(45.0f) {
    this->mRB = nullptr;
    this->mRBComplex = nullptr;
    this->mInput = nullptr;
    this->mNumWheelsOnGround = 0;

    this->EnableProfile("SuspensionTraffic");

    this->GetOwner()->QueryInterface(&this->mRB);
    this->GetOwner()->QueryInterface(&this->mRBComplex);
    this->GetOwner()->QueryInterface(&this->mInput);
    this->GetOwner()->QueryInterface(&this->mTransmission);

    for (int i = 0; i < 4; ++i) {
        bool is_front = i < 2;
        float diameter = Physics::Info::WheelDiameter(this->mTireInfo, is_front);
        this->mTires[i] = new Tire(diameter * 0.5f, i, this->mTireInfo, this->mBrakeInfo);
    }

    UMath::Vector3 dimension;
    this->GetOwner()->GetRigidBody()->GetDimension(dimension);

    float wheelbase = this->mSuspensionInfo->WHEEL_BASE();
    float axle_width_f = this->mSuspensionInfo->TRACK_WIDTH().Front - this->mTireInfo->SECTION_WIDTH().Front * 0.001f;
    float axle_width_r = this->mSuspensionInfo->TRACK_WIDTH().Rear - this->mTireInfo->SECTION_WIDTH().Rear * 0.001f;
    float front_axle = this->mSuspensionInfo->FRONT_AXLE();

    UVector3 fl(-axle_width_f * 0.5f, -dimension.y, front_axle);
    UVector3 fr(axle_width_f * 0.5f, -dimension.y, front_axle);
    UVector3 rl(-axle_width_r * 0.5f, -dimension.y, front_axle - wheelbase);
    UVector3 rr(axle_width_r * 0.5f, -dimension.y, front_axle - wheelbase);

    this->GetWheel(0).SetLocalArm(fl);
    this->GetWheel(1).SetLocalArm(fr);
    this->GetWheel(2).SetLocalArm(rl);
    this->GetWheel(3).SetLocalArm(rr);
}

SuspensionTraffic::~SuspensionTraffic() {
    for (int i = 0; i < 4; ++i) {
        delete mTires[i];
    }
}

void SuspensionTraffic::OnBehaviorChange(const UCrc32 &mechanic) {
    Chassis::OnBehaviorChange(mechanic);

    if (mechanic == BEHAVIOR_MECHANIC_INPUT) {
        this->GetOwner()->QueryInterface(&this->mInput);
    } else if (mechanic == BEHAVIOR_MECHANIC_RIGIDBODY) {
        this->GetOwner()->QueryInterface(&this->mRBComplex);
        this->GetOwner()->QueryInterface(&this->mRB);
    } else if (mechanic == BEHAVIOR_MECHANIC_ENGINE) {
        this->GetOwner()->QueryInterface(&this->mTransmission);
    }
}

void SuspensionTraffic::OnTaskSimulate(float dT) {
    if ((this->mInput == nullptr) || (this->mRBComplex == nullptr) || (this->mRB == nullptr)) {
        return;
    }
    this->SetCOG(0.0f, 0.0f);

    ISimable *owner = this->GetOwner();
    State state;
    this->ComputeState(dT, state);
    for (unsigned int i = 0; i < 4; ++i) {
        this->mTires[i]->BeginFrame();
    }

    this->DoSimpleAero(state);
    this->DoDriveForces(state);
    this->DoWheelForces(state);

    for (unsigned int i = 0; i < 4; ++i) {
        this->mTires[i]->UpdateTime(dT);
    }
    for (unsigned int i = 0; i < 4; ++i) {
        this->mTires[i]->EndFrame(dT);
    }
    if (this->DoSleep(state) == SS_ALL) {
        for (unsigned int i = 0; i < 4; ++i) {
            this->mTires[i]->Stop();
        }
    }
    Chassis::OnTaskSimulate(dT);
}

UMath::Vector3 SuspensionTraffic::GetWheelCenterPos(unsigned int i) const {
    UMath::Vector3 pos = this->mTires[i]->GetPosition();
    if (this->mRBComplex == nullptr) {
        return pos;
    }
    UMath::ScaleAdd(this->mRBComplex->GetUpVector(), this->GetWheelRadius(i), pos, pos);
    return pos;
}

void SuspensionTraffic::MatchSpeed(float speed) {
    for (int i = 0; i < 4; ++i) {
        float w = this->mTires[i]->GetRadius();
        this->mTires[i]->SetAngularVelocity(speed / w);
    }
}

void SuspensionTraffic::Reset() {
    ISimable *owner = this->GetOwner();
    IRigidBody *rigidBody = owner->GetRigidBody();
    unsigned int numonground = 0;

    for (int i = 0; i < this->GetNumWheels(); ++i) {
        Tire &wheel = this->GetWheel(i);
        if (wheel.InitPosition(*rigidBody, wheel.GetRadius())) {
            float newCompression = wheel.GetNormal().w + this->GetRideHeight(i);
            if (newCompression < 0.0f) {
                newCompression = 0.0f;
            }
            wheel.SetCompression(newCompression);
            if (newCompression > 0.0f) {
                numonground++;
            }
        }
    }
    this->mNumWheelsOnGround = numonground;
}

void SuspensionTraffic::DoSimpleAero(State &state) {
    const float dragcoef_spec = this->mSuspensionInfo->DRAG_COEFFICIENT();
    float speed = state.speed;
    float drag = speed * dragcoef_spec;
    UVector3 drag_vector(state.linear_vel);

    drag_vector *= -drag;
    this->mRB->ResolveForce(drag_vector);
}

float SuspensionTraffic::DoHP2Steering(State &state) {
    float steer_input = state.steer_input;
    float newsteer = steer_input * this->mMaxSteering;

    this->mLastSteer = newsteer;
    return newsteer * DEG2ANGLE(1.0f);
}

void SuspensionTraffic::DoSteering(State &state, UMath::Vector3 &right, UMath::Vector3 &left) {
    float truesteer = this->DoHP2Steering(state);
    float steer1 = ANGLE2RAD(1.0f);
    float ca = cosf(truesteer * steer1);
    float sa = sinf(truesteer * steer1);

    right.x = sa;
    right.y = 0.0f;
    right.z = ca;
    UMath::Rotate(right, state.matrix, right);
    left = right;
}

void SuspensionTraffic::DoDriveForces(State &state) {
    if (this->mTransmission == nullptr) {
        return;
    }
    float drive_torque = this->mTransmission->GetDriveTorque();
    if (drive_torque == 0.0f) {
        return;
    }
    float torquesplit = this->mDrivetrainInfo->TORQUE_SPLIT();
    for (unsigned int tire = 0; tire < 4; ++tire) {
        if (this->mTires[tire]->IsOnGround()) {
            float torque = drive_torque;
            torque = tire < 2 ? torque * (1.0f - torquesplit) : (torque * torquesplit);

            if (UMath::Abs(torque) >= 0.0f) {
                this->mTires[tire]->ApplyTorque(torque * 0.5f);
            }
        }
    }
}

static const float TrafficRollAdjust = 0.0f; // TODO value and use

// UNSOLVED, float math
void SuspensionTraffic::DoWheelForces(State &state) {
    const float dT = state.time;
    UVector3 steerR;
    UVector3 steerL;

    this->DoSteering(state, steerR, steerL);

    for (int i = 0; i < 4; ++i) {
        this->mTires[i]->SetBrake(state.brake_input);
        if (i > 1) {
            this->mTires[i]->SetEBrake(state.ebrake_input);
        }
    }

    unsigned int wheelsOnGround = 0;
    float maxDelta = 0.0f;
    const UMath::Vector3 &vFwd = state.GetForwardVector();
    const UMath::Vector3 &vUp = state.GetUpVector();
    const float mass = state.mass;

    UMath::Vector3 cog;
    UMath::Rotate(state.cog, state.matrix, cog);

    float shock_specs[2];
    float spring_specs[2];
    float sway_specs[2];
    float travel_specs[2];
    float rideheight_specs[2];
    float progression[2];

    for (unsigned int i = 0; i < 2; ++i) {
        shock_specs[i] = LBIN2NM(this->mSuspensionInfo->SHOCK_STIFFNESS().At(i));
        spring_specs[i] = LBIN2NM(this->mSuspensionInfo->SPRING_STIFFNESS().At(i));
        sway_specs[i] = LBIN2NM(this->mSuspensionInfo->SWAYBAR_STIFFNESS().At(i));
        travel_specs[i] = INCH2METERS(this->mSuspensionInfo->TRAVEL().At(i));
        rideheight_specs[i] = INCH2METERS(this->mSuspensionInfo->RIDE_HEIGHT().At(i));
        progression[i] = this->mSuspensionInfo->SPRING_PROGRESSION().At(i);
    }

    float sway_stiffness[4];
    sway_stiffness[0] = (this->mTires[0]->GetCompression() - this->mTires[1]->GetCompression()) * sway_specs[0];
    sway_stiffness[1] = -sway_stiffness[0];
    sway_stiffness[2] = (this->mTires[2]->GetCompression() - this->mTires[3]->GetCompression()) * sway_specs[1];
    sway_stiffness[3] = -sway_stiffness[2];

    UMath::Vector4 steering_normals[4];
    steering_normals[0] = UMath::Vector4Make(steerL, 1.0f);
    steering_normals[1] = UMath::Vector4Make(steerR, 1.0f);
    steering_normals[2] = UMath::Vector4Make(vFwd, 1.0f);
    steering_normals[3] = UMath::Vector4Make(vFwd, 1.0f);

    bool resolve = false;

    for (unsigned int i = 0; i < 4; ++i) {
        int axle = i / 2;
        Tire &wheel = this->GetWheel(i);
        UMath::Vector3 wp = wheel.GetWorldArm();

        wheel.UpdatePosition(state.angular_vel, state.linear_vel, state.matrix, cog, state.time, wheel.GetRadius(), true, state.collider,
                             state.dimension.y * 2.0f);

        const UVector3 groundNormal(wheel.GetNormal());
        const UVector3 forwardNormal(steering_normals[i]);
        UVector3 lateralNormal;
        UMath::UnitCross(groundNormal, forwardNormal, lateralNormal);

        float penetration = wheel.GetNormal().w;
        float upness = UMath::Clamp(UMath::Dot(groundNormal, vUp), 0.0f, 1.0f);
        const float oldCompression = wheel.GetCompression();
        float newCompression = rideheight_specs[axle] * upness + penetration;
        float max_compression = travel_specs[axle];

        if (wheel.GetCompression() == 0.0f) {
            maxDelta = UMath::Max(maxDelta, newCompression - max_compression);
        }

        newCompression = UMath::Max(newCompression, 0.0f);
        if (newCompression > max_compression) {
            float delta = newCompression - max_compression;
            maxDelta = UMath::Max(maxDelta, delta);
            newCompression = max_compression;
        }

        if (newCompression > 0.0f && upness > VehicleSystem::ENABLE_ROLL_STOPS_THRESHOLD) {
            ++wheelsOnGround;

            float springForce;
            const float diff = newCompression - wheel.GetCompression();
            const float rise = diff / dT;

            float spring = (newCompression * spring_specs[axle]) * (newCompression * progression[axle] + 1.0f);
            float damp = rise * shock_specs[axle];

            if (damp > this->mSuspensionInfo.SHOCK_BLOWOUT() * 9.81f * mass) {
                damp = 0.0f;
            }

            springForce = damp + spring + sway_stiffness[i];
            springForce = UMath::Max(springForce, 0.0f);

            UVector3 verticalForce = vUp * springForce;
            UVector3 driveForce;
            UVector3 lateralForce;
            UVector3 c;

            UMath::Cross(forwardNormal, groundNormal, c);
            UMath::Cross(c, forwardNormal, c);

            float d2 = UMath::Dot(c, groundNormal);
            float load = UMath::Max(d2 * 4.0f - 3.0f, 0.3f) * springForce;

            const UMath::Vector3 &pointVelocity = wheel.GetVelocity();
            UVector3 vNorm(pointVelocity);
            float speed = vNorm.Normalize();
            float vdot = UMath::Dot(vNorm, lateralNormal);
            float xspeed = UMath::Dot(pointVelocity, lateralNormal);
            float zspeed = UMath::Dot(pointVelocity, forwardNormal);

            wheel.UpdateLoaded(xspeed, zspeed, load, state.time);

            float traction_force = wheel.GetLateralForce();
            float max_traction = UMath::Abs((xspeed / dT) * (0.25f * mass));
            traction_force = UMath::Clamp(traction_force, -max_traction, max_traction);

            lateralForce = lateralNormal * traction_force;
            UMath::UnitCross(lateralNormal, groundNormal, driveForce);
            UMath::Scale(driveForce, wheel.GetLongitudeForce());

            UMath::Vector3 force;
            UMath::Add(lateralForce, driveForce, force);
            UMath::Add(force, verticalForce, force);

            wheel.SetForce(force);
            resolve = true;
        } else {
            wheel.SetForce(UMath::Vector3::kZero);
            wheel.UpdateFree(dT);
        }

        if (newCompression == 0.0f) {
            wheel.IncAirTime(dT);
        } else {
            wheel.SetAirTime(0.0f);
        }

        wheel.SetCompression(newCompression);
    }

    if (resolve) {
        UMath::Vector3 cg;
        UMath::RotateTranslate(state.cog, state.matrix, cg);

        for (unsigned int i = 0; i < this->GetNumWheels(); ++i) {
            Tire &wheel = this->GetWheel(i);
            UVector3 p(wheel.GetLocalArm());

            p.y += wheel.GetCompression();
            const UMath::Vector3 &force = wheel.GetForce();
            p.y = p.y - rideheight_specs[i / 2];

            UMath::RotateTranslate(p, state.matrix, p);
            wheel.SetPosition(p);

            UMath::Vector3 torque;
            UMath::Vector3 r;
            UMath::Sub(p, cg, r);
            r.y *= 0.5f;

            UMath::Cross(r, force, torque);
            this->mRB->Resolve(force, torque);
        }
    }

    if (0.0f < maxDelta) {
        for (unsigned int i = 0; i < this->GetNumWheels(); ++i) {
            Wheel &wheel = this->GetWheel(i);
            wheel.SetY(wheel.GetPosition().y + maxDelta);
        }

        this->mRB->ModifyYPos(maxDelta);
    }

    this->mNumWheelsOnGround = wheelsOnGround;
}
