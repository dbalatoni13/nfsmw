#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_PreColWoosh.hpp"
#include "Speed/Indep/Src/EAXSound/SndBase.hpp"
#include "Speed/Indep/Src/EAXSound/SND_GEN/STITCH_WSH.h"

DEFINE_CREATABLE(0x20120, CARSFX_PreColWoosh, SndBase);

// UNSOLVED, stack issues (because the second UCRC32 reuses sp8), but functionally equivalent
CARSFX_PreColWoosh::CARSFX_PreColWoosh()
    : mMsgBarrier(Hermes::Handler::Create<MAudioReflection, CARSFX_PreColWoosh, CARSFX_PreColWoosh>(
          this, &CARSFX_PreColWoosh::MsgBarrier, UCrc32("FRONT_BARRIER"), 0)), //
      mMsgBarrierHit(Hermes::Handler::Create<MAudioReflection, CARSFX_PreColWoosh, CARSFX_PreColWoosh>(
          this, &CARSFX_PreColWoosh::MsgBarrierHit, UCrc32("FRONT_BARRIER_HIT"), 0)) {
    this->m_pWoosh = nullptr;
    this->mResetTime = 0.0f;
    this->mDurationActive = 0.0f;
}

CARSFX_PreColWoosh::~CARSFX_PreColWoosh() {
    if (this->mMsgBarrier != nullptr) {
        Hermes::Handler::Destroy(this->mMsgBarrier);
    }

    if (this->mMsgBarrierHit != nullptr) {
        Hermes::Handler::Destroy(this->mMsgBarrierHit);
    }

    if (this->m_pWoosh != nullptr) {
        delete this->m_pWoosh;
    }

    this->m_pWoosh = nullptr;
}

void CARSFX_PreColWoosh::InitSFX() {
    SndBase::InitSFX();
    this->bGoingToCollide = false;
    this->bBailOnAll = false;
    this->bBarrierDetected = false;
    this->WooshFadeOut.Initialize(1.0f, 1.0f, 1, LINEAR);
}

void CARSFX_PreColWoosh::Destroy() {
    SndBase::Destroy();
}

// UNSOLVED, instruction swap. IRREDUCIBLE, diagnosticado en la r65 sobre el RTL:
// no hay forma de fuente que lo cierre y estos cuatro andamios son el precio.
//
// regmap dice ESTRUCTURA y es CIERTO pero al reves de lo habitual: el original NO
// TIENE NI UNA LOCAL aqui (0 bloques anonimos; las nuestras `fadeOut` e
// `interpolationTime` son «SOLO NUESTRA»). O sea que el objetivo son estas tres
// sentencias a pelo. Escritas a pelo la funcion queda a 94,28571 % y la UNICA
// diferencia es una transposicion:
//     objetivo   addi r3,r10,0x3c | li r4,0x1 | stfs f13,0x34(r10) | fmr f2,f1
//     nuestro    addi r3,r10,0x3c | stfs f13,0x34(r10) | li r4,0x1 | fmr f2,f1
//
// POR QUE NO SALE (volcados `.sched`/`.sched2` con -dS -dR -fsched-verbose-5,
// bloque 4 de esta funcion; las dos pasadas dan EL MISMO horario):
//   insn 101 = `[%10+0x34]=%13` (el stfs)   prio 3, unidad lsu
//   insn 117 = `%4=0x1`         (el li r4)  prio 3, unidad iu2
//   Los dos se emiten en el CICLO 3 y con el MISMO prio 3 (cada uno cuelga solo
//   del `call`, y `insn_cost` devuelve 1 en los dos: la anti-dependencia
//   almacenamiento->llamada pasa por `LINK_COST_FREE` -> cost 1, no 0).
//   rank_for_schedule empata en los cinco niveles -- prio igual; REG_WEIGHT no
//   corre (`!reload_completed` es falso en sched2 y en sched1 el stfs vale 0
//   contra 1 del li, que tambien lo pierde); misma bb; misma clase respecto al
//   ultimo emitido (`addi r3`, independiente de los dos); y `depend_count` 1 y 1.
//   Decide el SEXTO nivel, `INSN_LUID`: gana el LUID MENOR. Y el `li r4` es una
//   carga de parametro en registro duro, que `load_register_parameters` emite
//   SIEMPRE pegada a la llamada, o sea DESPUES del stfs de la sentencia anterior.
//   Para invertirlo haria falta que el `1` se materializase antes de la sentencia
//   `mDurationActive = 0.0f`, y eso no lo hace ninguna forma de estas tres
//   sentencias: poner el almacenamiento detras de la llamada no vale porque
//   `sched_analyze_1` le cuelga una REG_DEP_ANTI de `last_pending_memory_flush`
//   (la llamada) y no puede subir.
//
// MEDIDO en la r65, cada variante con su objeto (.text de zEAXSound2):
//   las cuatro puestas (base)                      100 %       46501ac84a13
//   las cuatro quitadas                            94,28571 %  ad8869e9a825
//   `int interpolationTime = 1;` antes del stfs    94,28571 %  (la constante se
//        propaga y el `li` vuelve a su sitio: una local de mas no es la palanca)
//   solo el pin `asm("r4")`, sin barreras          94,28571 %  (idem)
//   pin + su barrera, sin las otras dos            90,85714 %  bf3e08a7ad06
//   sin el `fadeOut`, con las otras tres           92,57143 %  82e28816f565
// O sea que las cuatro hacen falta y ninguna sobra. NO REINTENTAR sin una palanca
// nueva que adelante una carga de parametro en registro duro.
void CARSFX_PreColWoosh::MsgBarrier(const MAudioReflection &message) {
    if (message.GetPlayerNum() != this->GetStateBase()->m_InstNum) {
        return;
    }

    this->bBarrierDetected = true;
    if (!this->bGoingToCollide && !this->bBailOnAll && this->mResetTime == 0.0f) {
        this->bGoingToCollide = true;
        this->mDurationActive = 0.0f;
        this->WooshFadeOut.Initialize(1.0f, 1.0f, 1, LINEAR);
    }
}

void CARSFX_PreColWoosh::MsgBarrierHit(const MAudioReflection &message) {
    if ((this->GetPhysCar() != nullptr) && message.GetPlayerNum() == static_cast<int>(this->GetPhysCar()->mWorldID)) {
        this->BailOnWoosh();
    }
}

void CARSFX_PreColWoosh::BailOnWoosh() {
    this->bBailOnAll = true;
    this->bGoingToCollide = false;
    this->WooshFadeOut.Initialize(this->WooshFadeOut.GetValue(), 0.0f, 0x82, LINEAR);
    this->mResetTime = 0.4f;
}

void CARSFX_PreColWoosh::Detach() {
    SndBase::Detach();
}

void CARSFX_PreColWoosh::UpdateParams(float t) {
    SndBase::UpdateParams(t);
    this->WooshFadeOut.Update(t);
    this->mResetTime -= t;
    if (this->mResetTime < 0.0f) {
        this->mResetTime = 0.0f;
    }

    this->SetDMIX_Input(2, 0);
    if (this->bGoingToCollide) {
        this->SetDMIX_Input(2, 0x7FFF);
        this->mDurationActive += t;
        if (this->m_pWoosh == nullptr) {
            extern Slope g_WooshVol_vs_Vel;
            float fVelRatio = bClamp(g_WooshVol_vs_Vel.GetValue(this->GetPhysCar()->GetVelocityMagnitude()), 0.0f, 0.99f);
            float fVelInensity = bClamp(fVelRatio * 127.0f, 0.0f, 127.0f);
            int StitchID;

            void GetWooshBlockSizeParams(eDRIVE_BY_TYPE type, enum STICH_WHOOSH_TYPE & base, int &numblocks, int &sizeperblock);
            int numblocks;

            int sizeperblock;
            STICH_WHOOSH_TYPE base;
            GetWooshBlockSizeParams(DRIVE_BY_PRE_COL, base, numblocks, sizeperblock);

            GEN_RND_OFFSET(StitchID, fVelInensity, base, numblocks, sizeperblock);
            SND_Stich *stitchdata = &g_pEAXSound->GetStichPlayer()->GetStich(STICH_TYPE_WOOSH, StitchID);

            SND_Params sndparams;
            sndparams.ID = 0;
            sndparams.Mag = 0;
            sndparams.RVerb = 0;
            sndparams.Az = 0;
            sndparams.Pitch = 0;
            sndparams.Vol = 0;
            this->m_pWoosh = new cStichWrapper(*stitchdata);
            this->m_pWoosh->Play(&sndparams);
        }

        if (!this->bBarrierDetected || this->mDurationActive > 0.4f) {
            this->BailOnWoosh();
        }
    }

    if (this->bBailOnAll && this->WooshFadeOut.GetValue() < 0.01f) {
        delete this->m_pWoosh;
        this->m_pWoosh = nullptr;
        this->bBailOnAll = false;
    }

    this->WooshFadeOut.GetValue();
}

void CARSFX_PreColWoosh::ProcessUpdate() {
    SndBase::ProcessUpdate();
    if (this->m_pWoosh != nullptr) {
        if (!this->m_pWoosh->IsPlaying()) {
            delete this->m_pWoosh;
            this->m_pWoosh = nullptr;
        } else {
            SND_Params params;

            params.ID = 0;
            params.Az = 0;
            params.Mag = 0;
            params.RVerb = 0;
            params.Pitch = 0x1000;
            params.Vol = static_cast<int>(static_cast<float>(this->GetDMixOutput(1, DMX_VOL)) * this->WooshFadeOut.GetValue());
            params.Az = this->GetDMixOutput(0, DMX_AZIM);
            this->m_pWoosh->Update(&params);
        }
    }

    this->bBarrierDetected = false;
}
