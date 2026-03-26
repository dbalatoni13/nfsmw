#include "PhysicsUpgrades.hpp"
#include "PhysicsInfo.hpp"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/junkman.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/presetride.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

using Attrib::Attribute;
using Attrib::AttributeIterator;
using Attrib::Database;
using Attrib::Key;
using Attrib::RefSpec;

struct PUJunkNode : Attrib::Instance {
    PUJunkNode(const RefSpec &collection, const Attrib::Gen::junkman &junkman, unsigned int junkkey);

    void operator delete(void *ptr, std::size_t bytes) {
        Attrib::Free(ptr, bytes, "Attrib::Instance");
    }
};

struct PUPartNode : Attrib::Instance {
    PUPartNode(const RefSpec &collection0, const RefSpec &collection1, float weight);

    void operator delete(void *ptr, std::size_t bytes) {
        Attrib::Free(ptr, bytes, "Attrib::Instance");
    }
};

namespace Physics {
namespace Upgrades {

struct tPartMap {
    Type type;
    const char *name;
    Key key;
    Key countkey;
    Key currentkey;
    Key junkkey;
    Key presetkey;
};

} // namespace Upgrades
} // namespace Physics


static Physics::Upgrades::tPartMap put_maps[] = {
    {Physics::Upgrades::PUT_TIRES, "tires", 0x570E7E24, 0x3F16D2B1, 0x65E52BDE, 0xC5860F58, 0x5F4A69DB},
    {Physics::Upgrades::PUT_BRAKES, "brakes", 0x2DD1F36A, 0xA2AEA57C, 0xC316DCD1, 0x56C63B6F, 0xE6C23DE6},
    {Physics::Upgrades::PUT_CHASSIS, "chassis", 0xF4E2FAD0, 0x20F6C9D5, 0x58CCF9F1, 0xB6495C9E, 0x21DD95EB},
    {Physics::Upgrades::PUT_TRANSMISSION, "transmission", 0x07A7A3E5, 0x0CB3C7E5, 0x170D5554, 0x25AE629A, 0x3DF99C50},
    {Physics::Upgrades::PUT_ENGINE, "engine", 0x5B862C53, 0x0CA4AD56, 0x5F8AFDDB, 0x9206EFD2, 0x3A93C8C7},
    {Physics::Upgrades::PUT_INDUCTION, "induction", 0xC92A0142, 0x99FCA5D3, 0x4DFC0A63, 0x7546359E, 0xFC61F3A5},
    {Physics::Upgrades::PUT_NOS, "nos", 0xD7B2D8F2, 0x2C48F8EC, 0x1D79C9A4, 0x452D2634, 0xEABB55C5},
    {(Physics::Upgrades::Type)0, nullptr, 0, 0, 0, 0, 0},
};

const Physics::Upgrades::tPartMap *FindPartMap(Physics::Upgrades::Type type) {
    Physics::Upgrades::tPartMap *t;
    const Physics::Upgrades::tPartMap *type_map = put_maps;
    if (type_map->key == 0) {
        goto not_found;
    }
    if (type_map->type == type) {
        return type_map;
    }
    do {
        type_map++;
        if (type_map->key == 0) {
            goto not_found;
        }
    } while (type_map->type != type);
    return type_map;
not_found:
    return nullptr;
}

static void DownGradeInternal(Attrib::Gen::pvehicle &vehicle, Physics::Upgrades::Type type) {
    if (!vehicle.IsDynamic()) {
        return;
    }

    const Physics::Upgrades::tPartMap *t = FindPartMap(type);
    if (t == nullptr) {
        return;
    }

    vehicle.Remove(t->key);
    vehicle.Remove(t->currentkey);

    int mask = 1 << type;
    int junkman_current = vehicle.junkman_current();

    if (junkman_current & mask) {
        vehicle.Remove(0xcdc136e8);
        junkman_current = junkman_current & ~mask;
        vehicle.GetBase().AddAndSet(0xcdc136e8, &junkman_current, 1);
    }
}

template <typename T>
void BlendParts(const Attribute &start_attribute, const Attribute &end_attribute, unsigned int index, float weight, Attribute &new_attrib) {
    T start_data = T();
    T end_data = T();

    start_attribute.Get(index, start_data);
    end_attribute.Get(index, end_data);

    T new_data;

    float *start_ptr = reinterpret_cast<float *>(&start_data);
    float *end_ptr = reinterpret_cast<float *>(&end_data);
    float *new_ptr = reinterpret_cast<float *>(&new_data);

    new_ptr[0] = start_ptr[0] * (1.0f - weight) + end_ptr[0] * weight;
    if (sizeof(T) > sizeof(float)) {
        new_ptr[1] = start_ptr[1] * (1.0f - weight) + end_ptr[1] * weight;
    }

    new_attrib.Set(index, new_data);
}

template <>
void BlendParts<int>(const Attribute &start_attribute, const Attribute &end_attribute, unsigned int index, float weight, Attribute &new_attrib) {
    int start_data = 0;
    int end_data = 0;

    start_attribute.Get(index, start_data);
    end_attribute.Get(index, end_data);

    int new_data = static_cast<int>(static_cast<float>(start_data) * (1.0f - weight) + static_cast<float>(end_data) * weight);

    new_attrib.Set(index, new_data);
}

template <typename T>
void ScalePart(Attribute &attrib, unsigned int index, float scale) {
    T start_data = T();
    T new_data;

    attrib.Get(index, start_data);

    float *start_ptr = reinterpret_cast<float *>(&start_data);
    float *new_ptr = reinterpret_cast<float *>(&new_data);

    new_ptr[0] = start_ptr[0] * scale;
    if (sizeof(T) > sizeof(float)) {
        new_ptr[1] = start_ptr[1] * scale;
    }

    attrib.Set(index, new_data);
}

template <>
void ScalePart<int>(Attribute &attrib, unsigned int index, float scale) {
    int start_data = 0;

    attrib.Get(index, start_data);

    int new_data = static_cast<int>(static_cast<float>(start_data) * scale);

    attrib.Set(index, new_data);
}

float Physics::Upgrades::GetPercent(const Attrib::Gen::pvehicle &vehicle, Physics::Upgrades::Type type) {
    int max_level = GetMaxLevel(vehicle, type);
    if (max_level == 0) {
        return 0.0f;
    }
    int cur_level = GetLevel(vehicle, type);
    if (cur_level == max_level) {
        return 1.0f;
    }
    return static_cast<float>(cur_level) / static_cast<float>(max_level);
}

int Physics::Upgrades::GetLevel(const Attrib::Gen::pvehicle &vehicle, Physics::Upgrades::Type type) {
    Attribute attrib;
    const Physics::Upgrades::tPartMap *t = FindPartMap(type);
    if (t == nullptr || !vehicle.Lookup(t->currentkey, attrib)) {
        return 0;
    }
    return attrib.Get< int >(0u);
}

void Physics::Upgrades::GetPackage(const Attrib::Gen::pvehicle &vehicle, Package &package) {
    package.Default();

    for (int i = 0; i < Physics::Upgrades::PUT_MAX; i++) {
        Physics::Upgrades::Type type = static_cast<Physics::Upgrades::Type>(i);
        package.Part[i] = GetLevel(vehicle, type);
        if (GetJunkman(vehicle, type)) {
            package.Junkman |= (1 << type);
        }
    }
}

bool Physics::Upgrades::SetPackage(Attrib::Gen::pvehicle &vehicle, const Package &package) {
    Attrib::Gen::pvehicle newvehicle(vehicle);
    Clear(newvehicle);

    if (!Validate(newvehicle)) {
        return false;
    }

    for (int i = 0; i < Physics::Upgrades::PUT_MAX; i++) {
        Physics::Upgrades::Type type = static_cast<Physics::Upgrades::Type>(i);
        int mask = 1 << type;

        if (!SetLevel(newvehicle, type, package.Part[i])) {
            return false;
        }

        if ((package.Junkman & mask) == 0) {
            RemoveJunkman(newvehicle, type);
        } else {
            if (!SetJunkman(newvehicle, type)) {
                return false;
            }
        }
    }

    if (!Validate(newvehicle)) {
        return false;
    }

    vehicle = newvehicle;
    return true;
}

bool Physics::Upgrades::GetJunkman(const Attrib::Gen::pvehicle &vehicle, Physics::Upgrades::Type type) {
    int junkman_current = vehicle.junkman_current();
    bool result = true;
    if (((junkman_current >> type) & 1) == 0) {
        result = false;
    }
    return result;
}

bool Physics::Upgrades::CanInstallJunkman(const Attrib::Gen::pvehicle &vehicle, Physics::Upgrades::Type type) {
    const Physics::Upgrades::tPartMap *p = FindPartMap(type);
    if (p == nullptr) {
        return false;
    }
    if (p->junkkey == 0) {
        return false;
    }

    if (type == Physics::Upgrades::PUT_INDUCTION) {
        if (Physics::Info::InductionType(vehicle) == Physics::Info::INDUCTION_NONE) {
            return false;
        }
    } else if (type == Physics::Upgrades::PUT_NOS) {
        if (!Physics::Info::HasNos(vehicle)) {
            return false;
        }
    }

    Attrib::Gen::junkman junkman(vehicle.junkman(), 0, nullptr);
    Attribute junk_attribute;
    bool found = junkman.Lookup(p->junkkey, junk_attribute);
    if (found && junk_attribute.GetType() == 0x51ead18d) {
        unsigned int len = junk_attribute.GetLength();
        return len != 0;
    }

    return false;
}

bool Physics::Upgrades::SetJunkman(Attrib::Gen::pvehicle &vehicle, Physics::Upgrades::Type type) {
    Attrib::Gen::pvehicle newvehicle(vehicle);

    if (GetJunkman(vehicle, type)) {
        return true;
    }

    if (!CanInstallJunkman(vehicle, type)) {
        return false;
    }

    const Physics::Upgrades::tPartMap *p = FindPartMap(type);
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
        newvehicle.Remove(0xcdc136e8);
    }

    Attribute part_attribute;
    if (!newvehicle.Lookup(part_key, part_attribute) || part_attribute.GetType() != 0x2b936eb7) {
        return false;
    }

    RefSpec basepart(part_attribute.Get< RefSpec >(0));

    Attrib::Gen::junkman junkman_inst(newvehicle.junkman(), 0, nullptr);
    PUJunkNode node(basepart, junkman_inst, junk_key);

    if (!node.IsValid()) {
        return false;
    }

    RefSpec newref;
    newref.SetCollection(node.GetConstCollection());

    if (newref != basepart) {
        if (!newvehicle.IsDynamic()) {
            const char *name = newvehicle.CollectionName();
            Key uniqueKey = newvehicle.GenerateUniqueKey(name, false);
            newvehicle.Modify(uniqueKey, newvehicle.LocalAttribCount());
        } else {
            newvehicle.Remove(part_key);
        }

        if (!newvehicle.GetBase().AddAndSet(part_key, &newref, 1)) {
            return false;
        }
    }

    junkman_current |= (1 << type);
    if (!newvehicle.GetBase().AddAndSet(0xcdc136e8, &junkman_current, 1)) {
        return false;
    }

    vehicle = static_cast<const Attrib::Instance &>(newvehicle);
    return true;
}

bool Physics::Upgrades::ApplyPreset(Attrib::Gen::pvehicle &vehicle, const Attrib::Gen::presetride &presetride) {
    if (!presetride.IsValid()) {
        return false;
    }
    if (!vehicle.IsValid()) {
        return false;
    }

    Attrib::Gen::pvehicle newvehicle(vehicle);
    Clear(newvehicle);

    for (int i = 0; i < Physics::Upgrades::PUT_MAX; i++) {
        Physics::Upgrades::Type type = static_cast<Physics::Upgrades::Type>(i);
        const Physics::Upgrades::tPartMap *part = FindPartMap(type);
        if (part == nullptr) {
            continue;
        }

        int max_level = GetMaxLevel(vehicle, type);
        if (max_level <= 0) {
            continue;
        }

        Attribute attrib;
        if (!presetride.Lookup(part->presetkey, attrib) || !attrib.IsValid()) {
            continue;
        }

        int level = UMath::Min(attrib.Get< int >(0u), max_level);

        if (!SetLevel(newvehicle, type, level)) {
            return false;
        }
    }

    vehicle = static_cast<const Attrib::Instance &>(newvehicle);
    return true;
}

void Physics::Upgrades::RemovePart(Attrib::Gen::pvehicle &vehicle, Physics::Upgrades::Type type) {
    if (!vehicle.IsDynamic()) {
        return;
    }

    if (GetLevel(vehicle, type) == 0) {
        return;
    }

    const Physics::Upgrades::tPartMap *t = FindPartMap(type);
    if (t == nullptr || t->key == 0) {
        return;
    }

    bool had_junkman = GetJunkman(vehicle, type);
    DownGradeInternal(vehicle, type);
    if (had_junkman) {
        SetJunkman(vehicle, type);
    }
}

void Physics::Upgrades::RemoveJunkman(Attrib::Gen::pvehicle &vehicle, Physics::Upgrades::Type type) {
    if (!vehicle.IsDynamic()) {
        return;
    }

    const Physics::Upgrades::tPartMap *t = FindPartMap(type);
    if (t == nullptr || t->key == 0) {
        return;
    }

    if (!GetJunkman(vehicle, type)) {
        return;
    }

    int old_upgrade_level = GetLevel(vehicle, type);
    DownGradeInternal(vehicle, type);
    if (old_upgrade_level > 0) {
        SetLevel(vehicle, type, old_upgrade_level);
    }
}

bool Physics::Upgrades::Validate(const Attrib::Gen::pvehicle &vehicle, Physics::Upgrades::Type type) {
    if (GetJunkman(vehicle, type)) {
        if (!CanInstallJunkman(vehicle, type)) {
            return false;
        }
    }

    int current = GetLevel(vehicle, type);
    int max_level = GetMaxLevel(vehicle, type);

    if (current == 0 && max_level != 0) {
        const Physics::Upgrades::tPartMap *t = FindPartMap(type);
        if (t == nullptr) {
            return false;
        }

        Attrib::Gen::pvehicle base(vehicle);
        Clear(base);
        Attribute attrib;
        if (!base.Lookup(t->key, attrib) || attrib.GetLength() < 2) {
            return false;
        }
    }

    return current <= max_level;
}

bool Physics::Upgrades::Validate(const Attrib::Gen::pvehicle &vehicle) {
    for (int i = 0; i < Physics::Upgrades::PUT_MAX; i++) {
        if (!Validate(vehicle, static_cast<Physics::Upgrades::Type>(i))) {
            return false;
        }
    }
    return true;
}

int Physics::Upgrades::GetMaxLevel(const Attrib::Gen::pvehicle &vehicle, Physics::Upgrades::Type type) {
    Attribute attrib;
    const Physics::Upgrades::tPartMap *t = FindPartMap(type);
    if (t == nullptr || !vehicle.Lookup(t->countkey, attrib)) {
        return 0;
    }
    return attrib.Get< int >(0u);
}

bool Physics::Upgrades::SetMaximum(Attrib::Gen::pvehicle &pvehicle) {
    Package package;
    package.Default();

    for (int i = 0; i < Physics::Upgrades::PUT_MAX; i++) {
        package.Part[i] = GetMaxLevel(pvehicle, static_cast<Physics::Upgrades::Type>(i));
    }

    return SetPackage(pvehicle, package);
}

static bool UpgradeInternal(Attrib::Gen::pvehicle &vehicle, Physics::Upgrades::Type type, int level, float weight) {
    Attrib::Gen::pvehicle newvehicle(vehicle);

    if (weight <= 0.0f) {
        RemovePart(vehicle, type);
        return true;
    }

    int max_level = GetMaxLevel(newvehicle, type);
    if (max_level <= 0 || weight > 1.0f) {
        return false;
    }

    const Physics::Upgrades::tPartMap *p = FindPartMap(type);
    if (p == nullptr) {
        return false;
    }

    unsigned int part_key = p->key;
    if (part_key == 0) {
        return false;
    }

    bool had_junkman = GetJunkman(vehicle, type);
    DownGradeInternal(newvehicle, type);

    Attribute part_attribute;
    if (!newvehicle.Lookup(part_key, part_attribute)) {
        return false;
    }

    if (part_attribute.GetType() != 0x2b936eb7 || part_attribute.GetLength() < 2) {
        return false;
    }

    unsigned int base_index = (part_attribute.GetLength() == 3) ? 1 : 0;
    unsigned int top_index = (part_attribute.GetLength() == 3) ? 2 : 1;

    RefSpec basepart;
    part_attribute.Get(base_index, basepart);

    RefSpec endpart;
    part_attribute.Get(top_index, endpart);

    if (basepart.GetClassKey() == 0 || endpart.GetClassKey() == 0 ||
        basepart.GetCollectionKey() == 0 || endpart.GetCollectionKey() == 0) {
        return false;
    }

    PUPartNode node(basepart, endpart, weight);
    if (!node.IsValid()) {
        return false;
    }

    RefSpec newref;
    newref.SetCollection(node.GetConstCollection());

    if (newref == basepart) {
        return true;
    }

    if (!newvehicle.IsDynamic()) {
        const char *name = newvehicle.CollectionName();
        Key uniqueKey = newvehicle.GenerateUniqueKey(name, false);
        newvehicle.Modify(uniqueKey, newvehicle.LocalAttribCount());
    }

    if (!newvehicle.GetBase().AddAndSet(part_key, &newref, 1)) {
        return false;
    }

    if (!newvehicle.GetBase().AddAndSet(p->currentkey, &level, 1)) {
        return false;
    }

    if (had_junkman && !SetJunkman(newvehicle, type)) {
        return false;
    }

    vehicle = newvehicle;
    return true;
}

bool Physics::Upgrades::SetLevel(Attrib::Gen::pvehicle &vehicle, Physics::Upgrades::Type type, int level) {
    Attrib::Gen::pvehicle newvehicle(vehicle);

    int cur_level = GetLevel(vehicle, type);
    if (cur_level == level) {
        return true;
    }

    if (level < 1) {
        RemovePart(vehicle, type);
    } else {
        int max_level = GetMaxLevel(newvehicle, type);
        if (max_level < 1 || level > max_level) {
            return false;
        }

        float weight = static_cast<float>(level) / static_cast<float>(max_level);
        if (!UpgradeInternal(newvehicle, type, level, weight)) {
            return false;
        }

        vehicle = static_cast<const Attrib::Instance &>(newvehicle);
    }

    return true;
}

void Physics::Upgrades::Clear(Attrib::Gen::pvehicle &vehicle) {
    if (vehicle.IsDynamic()) {
        vehicle.Unmodify();
    }
}

bool Physics::Upgrades::MatchPerformance(Attrib::Gen::pvehicle &vehicle, const Physics::Info::Performance &matched_performance) {
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

    float acc_range = upgraded_performance.Acceleration - stock_performance.Acceleration;
    if (acc_range > 1e-06f) {
        match_line.Acceleration = UMath::Ramp(
            (matched_performance.Acceleration - stock_performance.Acceleration) / acc_range,
            0.0f, 1.0f);
    } else {
        match_line.Acceleration = 0.0f;
    }

    float top_range = upgraded_performance.TopSpeed - stock_performance.TopSpeed;
    if (top_range > 1e-06f) {
        match_line.TopSpeed = UMath::Ramp(
            (matched_performance.TopSpeed - stock_performance.TopSpeed) / top_range,
            0.0f, 1.0f);
    } else {
        match_line.TopSpeed = 0.0f;
    }

    float hand_range = upgraded_performance.Handling - stock_performance.Handling;
    if (hand_range > 1e-06f) {
        match_line.Handling = UMath::Ramp(
            (matched_performance.Handling - stock_performance.Handling) / hand_range,
            0.0f, 1.0f);
    } else {
        match_line.Handling = 0.0f;
    }

    for (int i = 0; i < Physics::Upgrades::PUT_MAX; i++) {
        Physics::Upgrades::Type type = static_cast<Physics::Upgrades::Type>(i);
        float weight;

        switch (type) {
        case Physics::Upgrades::PUT_TIRES:
            weight = match_line.Handling;
            break;
        case Physics::Upgrades::PUT_BRAKES:
            weight = (match_line.Handling + match_line.TopSpeed) * 0.5f;
            break;
        case Physics::Upgrades::PUT_CHASSIS:
            weight = match_line.Handling;
            break;
        case Physics::Upgrades::PUT_TRANSMISSION:
            weight = (match_line.Acceleration + match_line.TopSpeed) * 0.5f;
            break;
        case Physics::Upgrades::PUT_ENGINE:
            weight = (match_line.Acceleration + match_line.TopSpeed) * 0.5f;
            break;
        case Physics::Upgrades::PUT_INDUCTION:
            weight = match_line.Acceleration;
            break;
        case Physics::Upgrades::PUT_NOS:
            weight = match_line.Acceleration;
            break;
        default:
            weight = (match_line.Acceleration + match_line.TopSpeed + match_line.Handling) / 3.0f;
            break;
        }

        weight = UMath::Clamp(weight, 0.0f, 1.0f);

        if (weight > 0.0f) {
            int max_level = GetMaxLevel(vehicle, type);
            if (max_level > 0) {
                float f_index = UMath::Ceil(weight * static_cast<float>(max_level));
                int level = static_cast<int>(f_index);
                if (!UpgradeInternal(newvehicle, type, level, weight)) {
                    return false;
                }
            }
        }
    }

    vehicle = newvehicle;
    return true;
}

void Physics::Upgrades::Flush() {
    Database::Get().CollectGarbage();
}

PUJunkNode::PUJunkNode(const RefSpec &collection, const Attrib::Gen::junkman &junkman, unsigned int junkkey)
    : Instance(collection, 0, nullptr) {
    Attribute junk_attribute;
    if (junkman.Lookup(junkkey, junk_attribute)) {
        Key uniqueKey = GenerateUniqueKey("junk_upgrade", false);
        Modify(uniqueKey, 0);
        unsigned int len = junk_attribute.GetLength();
        for (unsigned int index = 0; index < len; index++) {
            JunkmanMod modifire;
            junk_attribute.Get(index, modifire);
            if (&modifire != nullptr) {
                Attribute start_attribute = Get(modifire.DefinitionKey);
                unsigned int count = start_attribute.GetLength();
                if (count != 0) {
                    Add(modifire.DefinitionKey, count);
                    Attribute attribute = Get(modifire.DefinitionKey);
                    unsigned int type = attribute.GetType();
                    for (unsigned int i = 0; i < count; i++) {
                        if (type == 0x4cb36381) {
                            ScalePart<AxlePair>(attribute, i, modifire.Scale);
                        } else if (type < 0x4cb36382) {
                            if (type == 0x3c16ec5e) {
                                ScalePart<float>(attribute, i, modifire.Scale);
                            } else {
                                bBreak();
                            }
                        } else {
                            if (type != 0x5763da41) {
                                bBreak();
                                continue;
                            }
                            ScalePart<int>(attribute, i, modifire.Scale);
                        }
                    }
                }
            }
        }
    }
}

PUPartNode::PUPartNode(const RefSpec &collection0, const RefSpec &collection1, float weight)
    : Instance(collection0, 0, nullptr) {
    if (weight >= 1.0f) {
        ChangeWithDefault(collection1);
    } else if (weight > 0.0f) {
        ChangeWithDefault(collection0);
        Key uniqueKey = GenerateUniqueKey("part_upgrade", false);
        Modify(uniqueKey, 0);
        Instance end_instance(collection1, 0, nullptr);
        Instance start_instance(collection0, 0, nullptr);
        AttributeIterator iter = end_instance.Iterator();
        while (iter.Valid()) {
            Key key = iter.GetKey();
            Attribute end_attribute = end_instance.Get(key);
            Attribute start_attribute = start_instance.Get(key);
            unsigned int end_count = end_attribute.GetLength();
            unsigned int start_count = start_attribute.GetLength();
            unsigned int count = UMath::Max(end_count, start_count);
            unsigned int end_type = end_attribute.GetType();
            unsigned int start_type = start_attribute.GetType();
            if (end_type == start_type) {
                if (count != 0) {
                    Add(key, count);
                    Attribute new_attrib = Get(key);
                    unsigned int type = start_attribute.GetType();
                    for (unsigned int i = 0; i < count; i++) {
                        if (type == 0x4cb36381) {
                            BlendParts<AxlePair>(start_attribute, end_attribute, i, weight, new_attrib);
                        } else if (type < 0x4cb36382) {
                            if (type == 0x3c16ec5e) {
                                BlendParts<float>(start_attribute, end_attribute, i, weight, new_attrib);
                            } else {
                                bBreak();
                            }
                        } else {
                            if (type != 0x5763da41) {
                                bBreak();
                                continue;
                            }
                            BlendParts<int>(start_attribute, end_attribute, i, weight, new_attrib);
                        }
                    }
                }
            }
            iter.Advance();
        }
    }
}

// Explicit template instantiations
template void BlendParts<AxlePair>(const Attribute &, const Attribute &, unsigned int, float, Attribute &);
template void BlendParts<float>(const Attribute &, const Attribute &, unsigned int, float, Attribute &);
template void ScalePart<AxlePair>(Attribute &, unsigned int, float);
template void ScalePart<float>(Attribute &, unsigned int, float);

namespace Attrib {
namespace Gen {
const Attrib::Gen::pvehicle &pvehicle::operator=(const Instance &rhs) {
    Instance::operator=(rhs);
    return *this;
}
} // namespace Gen
} // namespace Attrib
