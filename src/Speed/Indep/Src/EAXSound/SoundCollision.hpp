#ifndef SOUND_COLLISION_HPP_
#define SOUND_COLLISION_HPP_

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/World/WorldTypes.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

namespace Sound {

struct AudioEventParams {
    bVector3 position;          // offset 0x0, size 0x10
    bVector3 normal;            // offset 0x10, size 0x10
    bVector3 velocity;          // offset 0x20, size 0x10
    float magnitude;            // offset 0x30, size 0x4
    Attrib::RefSpec attributes; // offset 0x34, size 0xC
    WUID object;                // offset 0x40, size 0x4
    WUID other_object;          // offset 0x44, size 0x4
};

class AudioEvent : public UTL::COM::Factory<const AudioEventParams &, AudioEvent, unsigned int> {
  public:
    static AudioEvent *CreateInstance(const AudioEventParams &params) {
        return UTL::COM::Factory<const AudioEventParams &, AudioEvent, unsigned int>::CreateInstance(params.attributes.GetClassKey(), params);
    }

    virtual ~AudioEvent() {}
    virtual void Release() = 0;
    virtual void Pause(bool pause) = 0;
    virtual void Update(const bVector3 &p, const bVector3 &n, const bVector3 &v, float mag) {}

    const AudioEventParams &GetParameters() const {
        return mParams;
    }

    const Attrib::Instance &GetAttributes() const {
        return mAttributes;
    }

  private:
    AudioEventParams mParams;     // offset 0x4, size 0x48
    Attrib::Instance mAttributes; // offset 0x4C, size 0x14
};

float DistanceToView(const bVector3 *position);

} // namespace Sound

#endif
