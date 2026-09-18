#include "PhysicsUpgrades.hpp"
#include "PhysicsInfo.hpp"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/junkman.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/presetride.h"
#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"

// total size: 0x1C
struct tPartMap {
    Physics::Upgrades::Type type; // offset 0x0, size 0x4
    const char *name;             // offset 0x4, size 0x4
    unsigned int key;             // offset 0x8, size 0x4
    unsigned int countkey;        // offset 0xC, size 0x4
    unsigned int currentkey;      // offset 0x10, size 0x4
    unsigned int junkkey;         // offset 0x14, size 0x4
    unsigned int presetkey;       // offset 0x18, size 0x4
};

tPartMap put_maps[] = {
    {Physics::Upgrades::PUT_TIRES, "tires", 0xBD38D1CA, 0x75B0F76B, 0x34E73F83, 0xC5860F58, 0xBD38D1CA},
    {Physics::Upgrades::PUT_BRAKES, "brakes", 0x36350867, 0x4E168120, 0x552C3D22, 0x56C63B6F, 0x36350867},
    {Physics::Upgrades::PUT_CHASSIS, "chassis", 0xAFA210F0, 0xF62A062D, 0x7845F9BC, 0xB6495C9E, 0xAFA210F0},
    {Physics::Upgrades::PUT_TRANSMISSION, "transmission", 0x07A7A3E5, 0x860E4531, 0x170D5554, 0x25AE629A, 0x07A7A3E5},
    {Physics::Upgrades::PUT_ENGINE, "engine", 0xF1F5FBC7, 0x7708DB25, 0xB12CCB69, 0x9206EFD2, 0xF1F5FBC7},
    {Physics::Upgrades::PUT_INDUCTION, "induction", 0xC92A0142, 0x7202E66E, 0x7F440672, 0x7546359E, 0xC92A0142},
    {Physics::Upgrades::PUT_NOS, "nos", 0xB1669F64, 0x9DC66E81, 0x8F37BEAE, 0x452D2634, 0xB1669F64},
    {Physics::Upgrades::PUT_MAX, nullptr, 0, 0, 0, 0, 0},
};

const tPartMap *FindPartMap(Physics::Upgrades::Type type) {
    const tPartMap *t = put_maps;
    if (t->key != 0) {
        if (t->type == type) {
            return t;
        }
        do {
            t++;
            if (t->key == 0) {
                goto notfound;
            }
        } while (t->type != type);
        return t;
    }
notfound:
    return nullptr;
}

static void DownGradeInternal(Attrib::Gen::pvehicle &vehicle, Physics::Upgrades::Type type) {
    if (vehicle.IsDynamic()) {
        const tPartMap *t = FindPartMap(type);
        if (t != nullptr) {
            vehicle.Remove(t->key);
            vehicle.Remove(t->currentkey);

            int mask = 1 << type;
            int junkman_current = vehicle.junkman_current();

            if (junkman_current & mask) {
                vehicle.Remove(0xCDC136E8u);
                junkman_current = junkman_current & ~mask;
                if (vehicle.AddAndSet(0xCDC136E8u, &junkman_current, 1)) {
                }
            }
        }
    }
}

template <typename T>
void BlendParts(const Attrib::Attribute &start_attribute, const Attrib::Attribute &end_attribute, unsigned int index, float weight, Attrib::Attribute &new_attrib) {
    T start_data = 0;
    T end_data = 0;
    start_attribute.Get(index, start_data);
    end_attribute.Get(index, end_data);

    T new_data = start_data * (1.0f - weight) + end_data * weight;
    new_attrib.Set(index, new_data);
}

template <>
void BlendParts<AxlePair>(const Attrib::Attribute &start_attribute, const Attrib::Attribute &end_attribute, unsigned int index, float weight, Attrib::Attribute &new_attrib) {
    AxlePair start_data;
    AxlePair end_data;
    start_attribute.Get(index, start_data);
    end_attribute.Get(index, end_data);

    AxlePair new_data;
    new_data.Front = start_data.Front * (1.0f - weight) + end_data.Front * weight;
    new_data.Rear = start_data.Rear * (1.0f - weight) + end_data.Rear * weight;
    new_attrib.Set(index, new_data);
}

template <typename T>
void ScalePart(Attrib::Attribute &attribute, unsigned int index, float weight) {
    T start_data = 0;
    attribute.Get(index, start_data);

    T new_data = start_data * weight;
    attribute.Set(index, new_data);
}

template <>
void ScalePart<AxlePair>(Attrib::Attribute &attribute, unsigned int index, float weight) {
    AxlePair data;
    attribute.Get(index, data);

    AxlePair new_data;
    new_data.Front = data.Front * weight;
    new_data.Rear = data.Rear * weight;
    attribute.Set(index, new_data);
}

// total size: 0x18
class PUJunkNode : public Attrib::Instance {
  public:
    PUJunkNode(const Attrib::RefSpec &part, const Attrib::Gen::junkman &junkman, unsigned int junkkey);
};

PUJunkNode::PUJunkNode(const Attrib::RefSpec &part, const Attrib::Gen::junkman &junkman, unsigned int junkkey) : Attrib::Instance(part, 0, nullptr) {
    Attrib::Attribute attrib;
    if (junkman.Lookup(junkkey, attrib)) {
        Modify(GenerateUniqueKey("junk_upgrade", false), 0);

        unsigned int junk_count = attrib.GetLength();

        for (unsigned int i = 0; i < junk_count; i++) {
            JunkmanMod mod;
            if (attrib.Get(i, mod)) {
                Attrib::Attribute junk_attribute = Get(mod.DefinitionKey);
                unsigned int junk_length = junk_attribute.GetLength();
                if (junk_length != 0) {
                    Add(mod.DefinitionKey, junk_length);
                    Attrib::Attribute scaled_attribute = Get(mod.DefinitionKey);
                    unsigned int type = scaled_attribute.GetType();

                    for (unsigned int index = 0; index < junk_length; index++) {
                        switch (type) {
                            case 0x4CB36381:
                                ScalePart<AxlePair>(scaled_attribute, index, mod.Scale);
                                break;
                            case 0x3C16EC5E:
                                ScalePart<float>(scaled_attribute, index, mod.Scale);
                                break;
                            case 0x5763DA41:
                                ScalePart<int>(scaled_attribute, index, mod.Scale);
                                break;
                            default:
                                bBreak();
                                break;
                        }
                    }
                }
            }
        }
    }
}

// total size: 0x18
class PUPartNode : public Attrib::Instance {
  public:
    PUPartNode(const Attrib::RefSpec &basepart, const Attrib::RefSpec &endpart, float weight);
};

PUPartNode::PUPartNode(const Attrib::RefSpec &collection0, const Attrib::RefSpec &collection1, float weight) : Attrib::Instance(collection0, 0, nullptr) {
    if (weight >= 1.0f) {
        ChangeWithDefault(collection1);
    } else if (weight > 0.0f) {
        ChangeWithDefault(collection0);
        Modify(GenerateUniqueKey("part_upgrade", false), 0);

        Attrib::Instance end_instance(collection1, 0, nullptr);
        Attrib::Instance start_instance(collection0, 0, nullptr);

        Attrib::AttributeIterator iter = end_instance.Iterator();

        if (iter.Valid()) {
            do {
                Attrib::Key key = iter.GetKey();
                Attrib::Attribute end_attribute = end_instance.Get(key);
                Attrib::Attribute start_attribute = start_instance.Get(key);

                unsigned int end_count = end_attribute.GetLength();
                unsigned int count = UMath::Max(start_attribute.GetLength(), end_count);

                if (end_attribute.GetType() != start_attribute.GetType()) {
                    continue;
                }

                if (count != 0) {
                    Add(key, count);
                    Attrib::Attribute new_attrib = Get(key);
                    unsigned int type = start_attribute.GetType();

                    for (unsigned int index = 0; index < count; index++) {
                        switch (type) {
                            case 0x4CB36381:
                                BlendParts<AxlePair>(start_attribute, end_attribute, index, weight, new_attrib);
                                break;
                            case 0x3C16EC5E:
                                BlendParts<float>(start_attribute, end_attribute, index, weight, new_attrib);
                                break;
                            case 0x5763DA41:
                                BlendParts<int>(start_attribute, end_attribute, index, weight, new_attrib);
                                break;
                            default:
                                bBreak();
                                break;
                        }
                    }
                }
            } while (iter.Advance());
        }
    }
}
namespace Physics {

namespace Upgrades {

float GetPercent(const Attrib::Gen::pvehicle &vehicle, Type type) {
    int max_level = GetMaxLevel(vehicle, type);
    if (max_level == 0) {
        return 0.0f;
    }
    int level = GetLevel(vehicle, type);
    if (level == max_level) {
        return 1.0f;
    }
    return (float)level / (float)max_level;
}

int GetLevel(const Attrib::Gen::pvehicle &vehicle, Type type) {
    Attrib::Attribute attrib;
    const tPartMap *t = FindPartMap(type);
    if (t == nullptr || !vehicle.Lookup(t->currentkey, attrib)) {
        return 0;
    }
    int level = attrib.Get<int>(0);
    return level;
}

void GetPackage(const Attrib::Gen::pvehicle &vehicle, Package &package) {
    package.Default();
    for (int i = 0; i <= 6; i++) {
        package.Part[i] = GetLevel(vehicle, (Type)i);
        if (GetJunkman(vehicle, (Type)i)) {
            package.Junkman |= 1 << i;
        }
    }
}

bool SetPackage(Attrib::Gen::pvehicle &vehicle, const Package &package) {
    Attrib::Gen::pvehicle newvehicle(vehicle);

    Clear(newvehicle);
    if (!Validate(newvehicle)) {
        return false;
    }

    for (int i = 0; i <= 6; i++) {
        if (!SetLevel(newvehicle, (Type)i, package.Part[i])) {
            return false;
        }
        int mask = 1 << i;
        if (package.Junkman & mask) {
            if (!SetJunkman(newvehicle, (Type)i)) {
                return false;
            }
        } else {
            RemoveJunkman(newvehicle, (Type)i);
        }
    }

    if (!Validate(newvehicle)) {
        return false;
    }

    vehicle = newvehicle;
    return true;
}

bool GetJunkman(const Attrib::Gen::pvehicle &vehicle, Type type) {
    if (vehicle.junkman_current() >> type & 1) {
        return true;
    }
    return false;
}

bool CanInstallJunkman(const Attrib::Gen::pvehicle &vehicle, Type type) {
    const tPartMap *p = FindPartMap(type);
    if (p == nullptr) {
        return false;
    }

    if (p->junkkey == 0) {
        return false;
    }

    switch (type) {
        case PUT_NOS:
            if (!Physics::Info::HasNos(vehicle)) {
                return false;
            }
            break;
        case PUT_INDUCTION:
            if (Physics::Info::InductionType(vehicle) == Physics::Info::INDUCTION_NONE) {
                return false;
            }
            break;
    }

    Attrib::Gen::junkman junkman(vehicle.junkman(), 0, nullptr);
    Attrib::Attribute junk_attribute;
    if (!junkman.Lookup(p->junkkey, junk_attribute) || junk_attribute.GetType() != 0x51EAD18D) {
        return false;
    }
    return junk_attribute.GetLength() != 0;
}

bool SetJunkman(Attrib::Gen::pvehicle &vehicle, Type type) {
    Attrib::Gen::pvehicle newvehicle(vehicle);

    if (GetJunkman(vehicle, type)) {
        return true;
    }
    if (!CanInstallJunkman(vehicle, type)) {
        return false;
    }

    const tPartMap *p = FindPartMap(type);
    if (p == nullptr) {
        return false;
    }
    unsigned int part_key = p->key;
    unsigned int junk_key = p->junkkey;
    if (part_key == 0 || junk_key == 0) {
        return false;
    }

    int junkman_current = vehicle.junkman_current();
    if (newvehicle.IsDynamic()) {
        newvehicle.Remove(0xCDC136E8u);
    }

    Attrib::Attribute part_attribute;
    if (!newvehicle.Lookup(part_key, part_attribute)) {
        return false;
    }
    if (part_attribute.GetType() != 0x2B936EB7) {
        return false;
    }

    Attrib::RefSpec basepart(part_attribute.Get<Attrib::RefSpec>(0));
    Attrib::Gen::junkman junkman(newvehicle.junkman(), 0, nullptr);

    PUJunkNode node(basepart, junkman, junk_key);
    if (!node.IsValid()) {
        return false;
    }

    Attrib::RefSpec newref;
    newref.SetCollection(node.GetConstCollection());
    if (newref != basepart) {
        if (!newvehicle.IsDynamic()) {
            unsigned int dynamicCollectionKey = newvehicle.GenerateUniqueKey(newvehicle.CollectionName(), false);
            newvehicle.Modify(dynamicCollectionKey, 0);
        } else {
            newvehicle.Remove(part_key);
        }
        if (!newvehicle.AddAndSet(part_key, &newref, 1)) {
            return false;
        }
    }

    junkman_current |= 1 << type;
    if (!newvehicle.AddAndSet(0xCDC136E8u, &junkman_current, 1)) {
        return false;
    }

    vehicle = newvehicle;
    return true;
}

bool ApplyPreset(Attrib::Gen::pvehicle &vehicle, const Attrib::Gen::presetride &presetride) {
    if (!presetride.IsValid() || !vehicle.IsValid()) {
        return false;
    }

    Attrib::Gen::pvehicle newvehicle(vehicle);
    Clear(newvehicle);

    for (int i = 0; i <= 6; i++) {
        const tPartMap *part = FindPartMap((Type)i);
        if (part == nullptr) {
            continue;
        }
        int max_level = GetMaxLevel(vehicle, (Type)i);
        if (max_level <= 0) {
            continue;
        }
        Attrib::Attribute attrib;
        if (!presetride.Lookup(part->presetkey, attrib) || !attrib.IsValid()) {
            continue;
        }
        int level = 0;
        if (!attrib.Get(0, level)) {
            continue;
        }
        level = UMath::Min(max_level, level);
        if (!SetLevel(newvehicle, (Type)i, level)) {
            return false;
        }
    }

    vehicle = newvehicle;
    return true;
}

void RemovePart(Attrib::Gen::pvehicle &vehicle, Type type) {
    if (vehicle.IsDynamic()) {
        if (GetLevel(vehicle, type) != 0) {
            const tPartMap *t = FindPartMap(type);
            if (t != nullptr && t->key != 0) {
                bool had_junkman = GetJunkman(vehicle, type);
                DownGradeInternal(vehicle, type);
                if (had_junkman) {
                    SetJunkman(vehicle, type);
                }
            }
        }
    }
}

void RemoveJunkman(Attrib::Gen::pvehicle &vehicle, Type type) {
    if (vehicle.IsDynamic()) {
        const tPartMap *t = FindPartMap(type);
        if (t != nullptr && t->key != 0) {
            if (GetJunkman(vehicle, type)) {
                int old_upgrade_level = GetLevel(vehicle, type);
                DownGradeInternal(vehicle, type);
                if (old_upgrade_level > 0) {
                    SetLevel(vehicle, type, old_upgrade_level);
                }
            }
        }
    }
}

bool Validate(const Attrib::Gen::pvehicle &vehicle, Type type) {
    if (GetJunkman(vehicle, type)) {
        if (!CanInstallJunkman(vehicle, type)) {
            return false;
        }
    }

    int current = GetLevel(vehicle, type);
    int max_level = GetMaxLevel(vehicle, type);

    if (current == 0 && max_level != 0) {
        const tPartMap *t = FindPartMap(type);
        if (t == nullptr) {
            return false;
        }
        Attrib::Gen::pvehicle base(vehicle);
        Clear(base);
        Attrib::Attribute attrib;
        if (!base.Lookup(t->key, attrib) || attrib.GetLength() <= 1) {
            return false;
        }
    }

    return current <= max_level;
}

bool Validate(const Attrib::Gen::pvehicle &vehicle) {
    for (int i = 0; i <= 6; i++) {
        if (!Validate(vehicle, (Type)i)) {
            return false;
        }
    }
    return true;
}

int GetMaxLevel(const Attrib::Gen::pvehicle &vehicle, Type type) {
    Attrib::Attribute attrib;
    const tPartMap *t = FindPartMap(type);
    if (t == nullptr || !vehicle.Lookup(t->countkey, attrib)) {
        return 0;
    }
    int level = attrib.Get<int>(0);
    return level;
}

bool SetMaximum(Attrib::Gen::pvehicle &vehicle) {
    Package package;
    for (int i = 0; i <= 6; i++) {
        package.Part[i] = GetMaxLevel(vehicle, (Type)i);
    }
    return SetPackage(vehicle, package);
}

}; // namespace Upgrades

}; // namespace Physics

static bool UpgradeInternal(Attrib::Gen::pvehicle &vehicle, Physics::Upgrades::Type type, int level, float weight) {
    Attrib::Gen::pvehicle newvehicle(vehicle);

    if (weight <= 0.0f) {
        Physics::Upgrades::RemovePart(vehicle, type);
        return true;
    }
    if (Physics::Upgrades::GetMaxLevel(newvehicle, type) <= 0) {
        return false;
    }
    if (weight > 1.0f) {
        return false;
    }

    const tPartMap *p = FindPartMap(type);
    if (p == nullptr) {
        return false;
    }
    unsigned int part_key = p->key;
    if (part_key == 0) {
        return false;
    }

    bool had_junkman = Physics::Upgrades::GetJunkman(vehicle, type);
    DownGradeInternal(newvehicle, type);

    Attrib::Attribute part_attribute;
    if (!newvehicle.Lookup(part_key, part_attribute)) {
        return false;
    }
    if (part_attribute.GetType() != 0x2B936EB7) {
        return false;
    }
    if (part_attribute.GetLength() <= 1) {
        return false;
    }

    unsigned int base_index = 0;
    unsigned int top_index = 1;
    if (part_attribute.GetLength() == 3) {
        base_index = 1;
        top_index = 2;
    }

    Attrib::RefSpec basepart(part_attribute.Get<Attrib::RefSpec>(base_index));
    Attrib::RefSpec endpart(part_attribute.Get<Attrib::RefSpec>(top_index));

    if (basepart.GetClassKey() == 0 || endpart.GetClassKey() == 0 || basepart.GetCollectionKey() == 0 || endpart.GetCollectionKey() == 0) {
        return false;
    }

    PUPartNode node(basepart, endpart, weight);
    if (!node.IsValid()) {
        return false;
    }

    Attrib::RefSpec newref;
    newref.SetCollection(node.GetConstCollection());
    if (newref == basepart) {
        return true;
    }

    if (!newvehicle.IsDynamic()) {
        unsigned int dynamicCollectionKey = newvehicle.GenerateUniqueKey(newvehicle.CollectionName(), false);
        newvehicle.Modify(dynamicCollectionKey, 0);
    }

    if (!newvehicle.AddAndSet(part_key, &newref, 1)) {
        return false;
    }

    if (!newvehicle.AddAndSet(p->currentkey, &level, 1)) {
        return false;
    }

    if (had_junkman) {
        if (!Physics::Upgrades::SetJunkman(newvehicle, type)) {
            return false;
        }
    }

    vehicle = newvehicle;
    return true;
}

namespace Physics {

namespace Upgrades {

bool SetLevel(Attrib::Gen::pvehicle &vehicle, Type type, int level) {
    Attrib::Gen::pvehicle newvehicle(vehicle);

    if (GetLevel(vehicle, type) == level) {
        return true;
    }
    if (level <= 0) {
        RemovePart(vehicle, type);
        return true;
    }

    int max_level = GetMaxLevel(newvehicle, type);
    if (max_level <= 0 || level > max_level) {
        return false;
    }
    float weight = (float)level / (float)max_level;
    if (!UpgradeInternal(newvehicle, type, level, weight)) {
        return false;
    }
    vehicle = newvehicle;
    return true;
}

void Clear(Attrib::Gen::pvehicle &vehicle) {
    if (vehicle.IsDynamic()) {
        vehicle.Unmodify();
    }
}

static inline float Ramp(const float a, const float amin, const float amax) {
    float arange = amax - amin;
    return arange > UMath::Epsilon ? VU0_floatmax(0.0f, VU0_floatmin((a - amin) / arange, 1.0f)) : 0.0f;
}

bool MatchPerformance(Attrib::Gen::pvehicle &vehicle, const Physics::Info::Performance &matched_performance) {
    Attrib::Gen::pvehicle newvehicle(vehicle);

    Clear(newvehicle);
    Physics::Info::Performance stock_performance;
    Physics::Info::Performance upgraded_performance;
    if (!Physics::Info::GetStockPerformance(newvehicle, stock_performance)) {
        return false;
    }
    if (!Physics::Info::GetMaximumPerformance(newvehicle, upgraded_performance)) {
        return false;
    }

    Physics::Info::Performance match_line;
    match_line.Acceleration = Ramp(matched_performance.Acceleration, stock_performance.Acceleration, upgraded_performance.Acceleration);
    match_line.TopSpeed = Ramp(matched_performance.TopSpeed, stock_performance.TopSpeed, upgraded_performance.TopSpeed);
    match_line.Handling = Ramp(matched_performance.Handling, stock_performance.Handling, upgraded_performance.Handling);

    for (int i = 0; i <= 6; i++) {
        Type type = (Type)i;
        float weight;
        switch (type) {
            case PUT_BRAKES:
                weight = match_line.Handling;
                break;
            case PUT_TIRES:
            case PUT_CHASSIS:
                weight = (match_line.Handling + match_line.TopSpeed) * 0.5f;
                break;
            case PUT_INDUCTION:
            case PUT_NOS:
                weight = match_line.Acceleration;
                break;
            case PUT_TRANSMISSION:
            case PUT_ENGINE:
                weight = (match_line.Acceleration + match_line.TopSpeed) * 0.5f;
                break;
            default:
                weight = (match_line.Acceleration + match_line.TopSpeed + match_line.Handling) * 0.33333334f;
                break;
        }
        weight = UMath::Clamp(weight, 0.0f, 1.0f);

        if (weight > 0.0f) {
            int max_level = GetMaxLevel(vehicle, type);
            if (max_level > 0) {
                float f_index = weight * (float)max_level;
                int level = UMath::Ceil(f_index);
                if (!UpgradeInternal(newvehicle, type, level, weight)) {
                    return false;
                }
            }
        }
    }

    vehicle = newvehicle;
    return true;
}

void Flush() {
    Attrib::Database::Get().CollectGarbage();
}

}; // namespace Upgrades

}; // namespace Physics
