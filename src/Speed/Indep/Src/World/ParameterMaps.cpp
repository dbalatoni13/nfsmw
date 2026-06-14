#include "ParameterMaps.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Src/Misc/SpeedChunks.hpp"
#include <cstring>

ParameterMapLayer::ParameterMapLayer()
    : Header(nullptr),        //
      FieldTypes(nullptr),    //
      FieldOffsets(nullptr),  //
      ParameterData(nullptr), //
      QuadData8(nullptr),     //
      QuadData16(nullptr) {}

ParameterMapLayer::~ParameterMapLayer() {
    Unload();
}

void ParameterMapLayer::Load(bChunk **chunk) {
    bChunk *last_chunk = (*chunk)->GetLastChunk();

    *chunk = (*chunk)->GetFirstChunk();

    while (*chunk < last_chunk) {
        switch ((*chunk)->GetID()) {
            case BCHUNK_PARAMETER_MAPS_LAYER_HEADER:
                this->Header = reinterpret_cast<ParameterMapLayerHeader *>((*chunk)->GetData());
                bPlatEndianSwap(&this->Header->NameHash);
                bPlatEndianSwap(&this->Header->QuadLeft);
                bPlatEndianSwap(&this->Header->QuadTop);
                bPlatEndianSwap(&this->Header->QuadRight);
                bPlatEndianSwap(&this->Header->QuadBottom);
                bPlatEndianSwap(&this->Header->NumberOfQuadNodes);
                bPlatEndianSwap(&this->Header->NumberOfParameterSets);
                bPlatEndianSwap(&this->Header->SizeOfParameterSet);
                bPlatEndianSwap(&this->Header->NumberOfFields);
                this->FieldTypes = nullptr;
                this->FieldOffsets = nullptr;
                this->ParameterData = nullptr;
                this->QuadData8 = nullptr;
                this->QuadData16 = nullptr;
                break;

            case BCHUNK_PARAMETER_MAPS_LAYER_FIELD_TYPES:
                if (this->Header != nullptr) {
                    int expected_size = this->Header->NumberOfFields * sizeof(*this->FieldTypes);

                    if ((*chunk)->GetSize() == expected_size) {
                        this->FieldTypes = reinterpret_cast<int *>((*chunk)->GetData());

                        for (int i = 0; i < this->Header->NumberOfFields; i++) {
                            bPlatEndianSwap(&this->FieldTypes[i]);
                        }
                    }
                }
                break;

            case BCHUNK_PARAMETER_MAPS_LAYER_FIELD_OFFSETS:
                if (this->Header != nullptr) {
                    int expected_size = this->Header->NumberOfFields * sizeof(*this->FieldTypes);

                    if ((*chunk)->GetSize() == expected_size) {
                        this->FieldOffsets = reinterpret_cast<int *>((*chunk)->GetData());

                        for (int i = 0; i < this->Header->NumberOfFields; i++) {
                            bPlatEndianSwap(&this->FieldOffsets[i]);
                        }
                    }
                }
                break;

            case BCHUNK_PARAMETER_MAPS_LAYER_PARAMETER_DATA:
                if ((this->Header != nullptr) && (this->FieldTypes != nullptr) && (this->FieldOffsets != nullptr)) {
                    int expected_size = this->Header->NumberOfParameterSets * this->Header->SizeOfParameterSet;

                    if ((*chunk)->GetSize() == expected_size) {
                        this->ParameterData = (*chunk)->GetData();

                        for (int current_set = 0; current_set < this->Header->NumberOfParameterSets; current_set++) {
                            for (int current_field = 0; current_field < this->Header->NumberOfFields; current_field++) {
                                void *field_pointer = this->GetFieldPointer(current_set, current_field);

                                switch (this->FieldTypes[current_field]) {
                                    case 0:
                                        bPlatEndianSwap(static_cast<float *>(field_pointer));
                                        break;
                                    case 1:
                                        bPlatEndianSwap(static_cast<int *>(field_pointer));
                                        break;
                                }
                            }
                        }
                    }
                }
                break;

            case BCHUNK_PARAMETER_MAPS_LAYER_QUAD_DATA_8:
                if (this->Header != nullptr) {
                    int expected_size = this->Header->NumberOfQuadNodes * sizeof(ParameterMapQuad8);

                    if ((*chunk)->GetSize() == expected_size) {
                        this->QuadData8 = reinterpret_cast<ParameterMapQuad8 *>((*chunk)->GetData());
                    }
                }
                break;

            case BCHUNK_PARAMETER_MAPS_LAYER_QUAD_DATA_16:
                if (this->Header != nullptr) {
                    int expected_size = this->Header->NumberOfQuadNodes * sizeof(ParameterMapQuad16);

                    if ((*chunk)->GetSize() == expected_size) {
                        this->QuadData16 = reinterpret_cast<ParameterMapQuad16 *>((*chunk)->GetData());

                        for (int i = 0; i < this->Header->NumberOfQuadNodes; i++) {
                            this->QuadData16[i].DoEndianSwap();
                        }
                    }
                }
                break;
        }

        *chunk = (*chunk)->GetNext();
    }
}

void ParameterMapLayer::Unload() {
    this->Header = nullptr;
    this->FieldTypes = nullptr;
    this->FieldOffsets = nullptr;
    this->ParameterData = nullptr;

    while (!this->ParameterAccessors.IsEmpty()) {
        ParameterAccessor *accessor = this->ParameterAccessors.GetHead();
        accessor->ClearLayer();
    }
}

void ParameterMapLayer::AddParameterAccessor(ParameterAccessor *accessor) {
    this->ParameterAccessors.AddTail(accessor);
}

void ParameterMapLayer::RemoveParameterAccessor(ParameterAccessor *accessor) {
    this->ParameterAccessors.Remove(accessor);
}

void *ParameterMapLayer::GetParameterData(float x, float y) {
    int parameter_set_index = 0;

    if (this->QuadData8 != nullptr) {
        parameter_set_index = this->GetParameterSetIndexFromQuadData8(x, y);
    } else if (this->QuadData16 != nullptr) {
        parameter_set_index = this->GetParameterSetIndexFromQuadData16(x, y);
    }

    int data_offset = this->Header->SizeOfParameterSet * parameter_set_index;
    return reinterpret_cast<char *>(this->ParameterData) + data_offset;
}

float ParameterMapLayer::GetDataFloat(int field_index, void *parameter_data) {
    float *data = reinterpret_cast<float *>(static_cast<char *>(parameter_data) + FieldOffsets[field_index]);
    if (data == nullptr) {
        return 0.0f;
    }
    return *data;
}

int ParameterMapLayer::GetDataInt(int field_index, void *parameter_data) {
    int *data = reinterpret_cast<int *>(static_cast<char *>(parameter_data) + FieldOffsets[field_index]);
    if (data == nullptr) {
        return 0;
    }
    return *data;
}

// STRIPPED
int ParameterMapLayer::GetParameterSetIndexFromMapData(float x, float y) {
    int parameter_set_index;
    if (this->QuadData8 != nullptr) {
        return this->GetParameterSetIndexFromQuadData8(x, y);
    }
    if (this->QuadData16 != nullptr) {
        return this->GetParameterSetIndexFromQuadData16(x, y);
    }
    return 0;
}

// UNSOLVED regswap
int ParameterMapLayer::GetParameterSetIndexFromQuadData8(float x, float y) {
    float left = this->Header->QuadLeft;
    float top = this->Header->QuadTop;
    float right = this->Header->QuadRight;
    float bottom = this->Header->QuadBottom;
    int current_index = 0;

    if (x < left && x > right && y < top && y > bottom) {
        return 0;
    }

    while (true) {
        if (this->QuadData8[current_index].IsLeave()) {
            return this->QuadData8[current_index].GetData();
        }

        float centre_x = (left + right) * 0.5f;
        float centre_y = (top + bottom) * 0.5f;

        if (x < centre_x) {

            if (y < centre_y) {
                right = centre_x;
                bottom = centre_y;
                current_index = this->QuadData8[current_index].GetChild0();
            } else {
                right = centre_x;
                current_index = this->QuadData8[current_index].GetChild1();
                top = centre_y;
            }
        } else {
            if (y < centre_y) {
                left = centre_x;
                current_index = this->QuadData8[current_index].GetChild2();
                bottom = centre_y;
            } else {
                left = centre_x; // TODO regswap if this is not commented out
                current_index = this->QuadData8[current_index].GetChild3();
                top = centre_y;
            }
        }
    }
}

// UNSOLVED regswap
int ParameterMapLayer::GetParameterSetIndexFromQuadData16(float x, float y) {
    float left = this->Header->QuadLeft;
    float top = this->Header->QuadTop;
    float right = this->Header->QuadRight;
    float bottom = this->Header->QuadBottom;
    int current_index = 0;

    if (x < left && x > right && y < top && y > bottom) {
        return 0;
    }

    while (true) {
        if (this->QuadData16[current_index].IsLeave()) {
            return this->QuadData16[current_index].GetData();
        }

        float centre_x = (left + right) * 0.5f;
        float centre_y = (top + bottom) * 0.5f;

        if (x < centre_x) {
            if (y < centre_y) {
                right = centre_x;
                bottom = centre_y;
                current_index = this->QuadData16[current_index].GetChild0();
            } else {
                right = centre_x;
                current_index = this->QuadData16[current_index].GetChild1();
                top = centre_y;
            }
        } else {
            if (y < centre_y) {
                left = centre_x;
                current_index = this->QuadData16[current_index].GetChild2();
                bottom = centre_y;
            } else {
                left = centre_x;
                current_index = this->QuadData16[current_index].GetChild3();
                top = centre_y;
            }
        }
    }
}

void *ParameterMapLayer::GetFieldPointer(int set_index, int field_index) {
    if (!Header->NumberOfParameterSets || !Header->SizeOfParameterSet || !Header->NumberOfFields || (FieldTypes == nullptr) ||
        (FieldOffsets == nullptr) || (ParameterData == nullptr)) {
        return nullptr;
    }
    if (set_index >= Header->NumberOfParameterSets) {
        return nullptr;
    }
    if (field_index >= Header->NumberOfFields) {
        return nullptr;
    }
    int data_offset = Header->SizeOfParameterSet * set_index + FieldOffsets[field_index];
    return static_cast<char *>(ParameterData) + data_offset;
}

static const int EnableAutoParameterAccessorsListDump = 0;

ParameterMapsManager &GetParameterMapsManager() {
    static ParameterMapsManager TheParameterMapsManager;
    return TheParameterMapsManager;
}

bTList<ParameterAccessor> &GetAutoParameterAccessors() {
    static bTList<ParameterAccessor> AutoParameterAccessors;
    return AutoParameterAccessors;
}

void DumpAutoParameterAccessorsList() {}

ParameterAccessor::ParameterAccessor() {}

ParameterAccessor::ParameterAccessor(const char *layer_name)
    : Layer(nullptr),             //
      AutoAttachLayerNamehash(0), //
      DebugName(layer_name),      //
      CurrentParameterData(nullptr) {
    AutoAttachLayerNamehash = bStringHash(layer_name);
    if (!GetParameterMapsManager().GetDataForLayer(AutoAttachLayerNamehash, this, 0)) {
        GetAutoParameterAccessors().AddTail(this);
    }
}

ParameterAccessor::~ParameterAccessor() {
    ClearLayer();
    if (GetAutoParameterAccessors().IsInList(this)) {
        GetAutoParameterAccessors().Remove(this);
    }
}

void ParameterAccessor::SetLayer(ParameterMapLayer *layer) {
    this->ClearData();

    if (this->Layer != nullptr) {
        this->Layer->RemoveParameterAccessor(this);
    }

    this->Layer = layer;
    if (layer != nullptr) {
        this->SetUpForNewLayer();
        this->Layer->AddParameterAccessor(this);
    } else if (this->AutoAttachLayerNamehash) {
        GetAutoParameterAccessors().AddTail(this);
    }
}

void ParameterAccessor::ClearLayer() {
    this->SetLayer(nullptr);
}

void ParameterAccessor::CaptureData(float x, float y) {
    this->CurrentParameterData = this->Layer != nullptr ? this->Layer->GetParameterData(x, y) : this->Layer;
}

void ParameterAccessor::ClearData() {
    this->CurrentParameterData = nullptr;
}

float ParameterAccessor::GetDataFloat(int field_index) {
    return this->Layer != nullptr && this->CurrentParameterData != nullptr ? this->Layer->GetDataFloat(field_index, this->CurrentParameterData)
                                                                           : 0.0f;
}

int ParameterAccessor::GetDataInt(int field_index) {
    return this->Layer != nullptr && this->CurrentParameterData != nullptr ? this->Layer->GetDataInt(field_index, this->CurrentParameterData) : 0;
}

void ParameterAccessor::SetUpForNewLayer() {}

ParameterAccessorBlend::ParameterAccessorBlend(const char *layer_name)
    : ParameterAccessor(layer_name), //
      LastData(nullptr),             //
      HaveLastData(0) {}

ParameterAccessorBlend::~ParameterAccessorBlend() {}

void ParameterAccessorBlend::CaptureData(float x, float y, float ratio) {

    if (this->Layer != nullptr && this->LastData != nullptr) {
        void *current_data = this->Layer->GetParameterData(x, y);

        if (this->HaveLastData == 0) {
            memcpy(this->LastData, current_data, this->Layer->GetSizeOfParameterSet());
            this->HaveLastData = 1;
        } else {
            for (int i = 0; i < this->Layer->GetNumberOfFields(); i++) {
                int field_type = this->Layer->GetFieldType(i);
                int field_offset = this->Layer->GetFieldOffset(i);
                void *from_current;
                void *from_last;
                void *to;

                if (field_offset >= 0) {
                    from_current = static_cast<char *>(current_data) + field_offset;
                    from_last = static_cast<char *>(this->LastData) + field_offset;
                    to = from_last;

                    switch (field_type) {
                        case 0: {
                            float current_amount = *static_cast<float *>(from_current);
                            float last_amount = *static_cast<float *>(from_last);

                            *static_cast<float *>(to) = current_amount * ratio + last_amount * (1.0f - ratio);
                            break;
                        }
                        case 1: {
                            float current_amount = static_cast<float>(*static_cast<int *>(from_current));
                            float last_amount = static_cast<float>(*static_cast<int *>(from_last));

                            *static_cast<int *>(to) = static_cast<int>(current_amount * ratio + last_amount * (1.0f - ratio));
                            break;
                        }
                    }
                }
            }
        }

        this->CurrentParameterData = this->LastData;
    } else {
        this->CurrentParameterData = nullptr;
    }
}

void ParameterAccessorBlend::ClearData() {
    if (this->LastData != nullptr) {
        delete[] reinterpret_cast<char *>(this->LastData);
        this->LastData = nullptr;
    }
    this->HaveLastData = 0;
    ParameterAccessor::ClearData();
}

void ParameterAccessorBlend::SetUpForNewLayer() {
    if (Layer != nullptr) {
        int data_size = Layer->GetSizeOfParameterSet();
        if (data_size > 0) {
            LastData = new ("Parameter Accessor Blend Buffer", 0) char[data_size];
        }
    }
}

void ParameterAccessorBlend::CaptureData(float x, float y) {}

ParameterAccessorBlendByDistance::ParameterAccessorBlendByDistance(const char *layer_name)
    : ParameterAccessorBlend(layer_name), //
      last_x(0.0f),                       //
      last_y(0.0f),                       //
      HaveLastPosition(0) {}

ParameterAccessorBlendByDistance::~ParameterAccessorBlendByDistance() {}

void ParameterAccessorBlendByDistance::CaptureData(float x, float y, float full_blend_distance) {
    float ratio = 1.0f;

    if (this->HaveLastPosition != 0 && full_blend_distance != 0.0f) {
        float dx = x - this->last_x;
        float dy = y - this->last_y;
        float distance_traveled = bSqrt(dx * dx + dy * dy);
        if (distance_traveled < full_blend_distance) {
            ratio = distance_traveled / full_blend_distance;
        }
    }

    ParameterAccessorBlend::CaptureData(x, y, ratio);
    this->last_y = y;
    this->last_x = x;
    this->HaveLastPosition = 1;
}

void ParameterAccessorBlendByDistance::SetUpForNewLayer() {
    this->last_x = 0.0f;
    this->last_y = 0.0f;
    this->HaveLastPosition = 0;
    ParameterAccessorBlend::SetUpForNewLayer();
}

void ParameterAccessorBlendByDistance::CaptureData(float x, float y) {}

ParameterMapsManager::ParameterMapsManager() {}

ParameterMapsManager::~ParameterMapsManager() {
    while (!this->ParameterMapLayers.IsEmpty()) {
        delete this->ParameterMapLayers.RemoveHead();
    }
}

void ParameterMapsManager::AddLayer(ParameterMapLayer *new_layer) {
    this->ParameterMapLayers.AddTail(new_layer);
}

void ParameterMapsManager::UnloadAllLayers() {
    for (ParameterMapLayer *current_layer = this->ParameterMapLayers.GetHead(); current_layer != this->ParameterMapLayers.EndOfList();
         current_layer = current_layer->GetNext()) {
        current_layer->Unload();
    }
    while (!this->ParameterMapLayers.IsEmpty()) {
        delete this->ParameterMapLayers.RemoveHead();
    }
}

int ParameterMapsManager::GetDataForLayer(unsigned int layer_name_hash, ParameterAccessor *accessor, int warning_if_not_found) {
    for (ParameterMapLayer *layer = this->ParameterMapLayers.GetHead(); layer != this->ParameterMapLayers.EndOfList(); layer = layer->GetNext()) {
        if (layer->GetNameHash() == layer_name_hash) {
            accessor->SetLayer(layer);
            return 1;
        }
    }

    return 0;
}

int LoaderParameterMaps(bChunk *chunk) {
    if (chunk->GetID() == BCHUNK_PARAMETER_MAPS_DATA) {
        bChunk *last_chunk = chunk->GetLastChunk();
        chunk = chunk->GetFirstChunk();

        while (chunk < last_chunk) {
            if (chunk->GetID() == BCHUNK_PARAMETER_MAPS_LAYER_DATA) {
                ParameterMapLayer *new_layer = new ("ParameterMapLayer", 0) ParameterMapLayer;
                new_layer->Load(&chunk);
                GetParameterMapsManager().AddLayer(new_layer);
            }
        }

        ParameterAccessor *current_accessor = GetAutoParameterAccessors().GetHead();
        while (current_accessor != GetAutoParameterAccessors().EndOfList()) {
            DumpAutoParameterAccessorsList();
            ParameterAccessor *next_accessor = current_accessor->GetNext();
            unsigned int namehash = current_accessor->GetAutoAttachLayerNamehash();
            if (namehash) {
                current_accessor->Remove();
                if (!GetParameterMapsManager().GetDataForLayer(namehash, current_accessor, 0)) {
                    GetAutoParameterAccessors().AddHead(current_accessor);
                }
            }
            current_accessor = next_accessor;
        }

        DumpAutoParameterAccessorsList();
        return 1;
    }
    return 0;
}

int UnloaderParameterMaps(bChunk *chunk) {
    if (chunk->GetID() == BCHUNK_PARAMETER_MAPS_DATA) {
        DumpAutoParameterAccessorsList();
        GetParameterMapsManager().UnloadAllLayers();
        DumpAutoParameterAccessorsList();
        return 1;
    }
    return 0;
}

// STRIPPED
int GetXYviewCar(eView *view, float *x, float *y) {}
