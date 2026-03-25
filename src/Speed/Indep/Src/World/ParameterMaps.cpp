#include "./ParameterMaps.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

#include <cstring>

namespace {

enum ParameterMapChunkID {
    kPMCH_Header = 0x3B602,
    kPMCH_FieldTypes = 0x3B603,
    kPMCH_FieldOffsets = 0x3B604,
    kPMCH_ParameterData = 0x3B605,
    kPMCH_QuadData8 = 0x3B607,
    kPMCH_QuadData16 = 0x3B608,
};

} // namespace

ParameterMapsManager &GetParameterMapsManager() {
    static ParameterMapsManager TheParameterMapsManager;
    return TheParameterMapsManager;
}

bTList<ParameterAccessor> &GetAutoParameterAccessors() {
    static bTList<ParameterAccessor> AutoParameterAccessors;
    return AutoParameterAccessors;
}

ParameterMapLayer::ParameterMapLayer()
    : Header(0), //
      FieldTypes(0), //
      FieldOffsets(0), //
      ParameterData(0), //
      QuadData8(0), //
      QuadData16(0) {}

ParameterMapLayer::~ParameterMapLayer() {
    Unload();
}

void ParameterMapLayer::Load(bChunk **chunk) {
    bChunk *root = *chunk;
    *chunk = root->GetFirstChunk();

    while (*chunk < root->GetLastChunk()) {
        bChunk *current = *chunk;

        switch (current->GetID()) {
        case kPMCH_Header:
            this->Header = reinterpret_cast<ParameterMapLayerHeader *>(current->GetData());
            bEndianSwap32(&this->Header->NameHash);
            bEndianSwap32(&this->Header->QuadLeft);
            bEndianSwap32(&this->Header->QuadTop);
            bEndianSwap32(&this->Header->QuadRight);
            bEndianSwap32(&this->Header->QuadBottom);
            bEndianSwap32(&this->Header->NumberOfQuadNodes);
            bEndianSwap32(&this->Header->NumberOfParameterSets);
            bEndianSwap32(&this->Header->SizeOfParameterSet);
            bEndianSwap32(&this->Header->NumberOfFields);
            this->FieldTypes = 0;
            this->FieldOffsets = 0;
            this->ParameterData = 0;
            this->QuadData8 = 0;
            this->QuadData16 = 0;
            break;

        case kPMCH_FieldTypes:
            if (this->Header != 0 && current->Size == this->Header->NumberOfFields * 4) {
                this->FieldTypes = reinterpret_cast<int *>(current->GetData());
                for (int i = 0; i < this->Header->NumberOfFields; i++) {
                    bEndianSwap32(&this->FieldTypes[i]);
                }
            }
            break;

        case kPMCH_FieldOffsets:
            if (this->Header != 0 && current->Size == this->Header->NumberOfFields * 4) {
                this->FieldOffsets = reinterpret_cast<int *>(current->GetData());
                for (int i = 0; i < this->Header->NumberOfFields; i++) {
                    bEndianSwap32(&this->FieldOffsets[i]);
                }
            }
            break;

        case kPMCH_ParameterData:
            if (this->Header != 0 && this->FieldTypes != 0 && this->FieldOffsets != 0 &&
                current->Size == this->Header->NumberOfParameterSets * this->Header->SizeOfParameterSet) {
                this->ParameterData = current->GetData();
                for (int set_index = 0; set_index < this->Header->NumberOfParameterSets; set_index++) {
                    for (int field_index = 0; field_index < this->Header->NumberOfFields; field_index++) {
                        void *field = this->GetFieldPointer(set_index, field_index);
                        int field_type = this->FieldTypes[field_index];
                        if (field_type == 0 || field_type == 1) {
                            bEndianSwap32(field);
                        }
                    }
                }
            }
            break;

        case kPMCH_QuadData8:
            if (this->Header != 0 && current->Size == this->Header->NumberOfQuadNodes * 4) {
                this->QuadData8 = reinterpret_cast<ParameterMapQuad8 *>(current->GetData());
            }
            break;

        case kPMCH_QuadData16:
            if (this->Header != 0 && current->Size == this->Header->NumberOfQuadNodes * 8) {
                this->QuadData16 = reinterpret_cast<ParameterMapQuad16 *>(current->GetData());
                for (int i = 0; i < this->Header->NumberOfQuadNodes; i++) {
                    bEndianSwap16(&this->QuadData16[i].Children.Child0);
                    bEndianSwap16(&this->QuadData16[i].Children.Child1);
                    bEndianSwap16(&this->QuadData16[i].Children.Child2);
                    bEndianSwap16(&this->QuadData16[i].Children.Child3);
                }
            }
            break;
        }

        *chunk = current->GetNext();
    }
}

void ParameterMapLayer::Unload() {
    this->Header = 0;
    this->FieldTypes = 0;
    this->FieldOffsets = 0;
    this->ParameterData = 0;

    while (!this->ParameterAccessors.IsEmpty()) {
        this->ParameterAccessors.GetHead()->ClearLayer();
    }
}

void ParameterMapLayer::AddParameterAccessor(ParameterAccessor *accessor) {
    this->ParameterAccessors.AddTail(accessor);
}

void ParameterMapLayer::RemoveParameterAccessor(ParameterAccessor *accessor) {
    accessor->Remove();
}

void *ParameterMapLayer::GetParameterData(float x, float y) {
    int parameter_set_index = 0;

    if (this->QuadData8 != 0) {
        parameter_set_index = this->GetParameterSetIndexFromQuadData8(x, y);
    } else if (this->QuadData16 != 0) {
        parameter_set_index = this->GetParameterSetIndexFromQuadData16(x, y);
    }

    return reinterpret_cast<char *>(this->ParameterData) + this->Header->SizeOfParameterSet * parameter_set_index;
}

float ParameterMapLayer::GetDataFloat(int field_index, void *parameter_data) {
    float *data = reinterpret_cast<float *>(static_cast<char *>(parameter_data) + FieldOffsets[field_index]);
    if (!data) {
        return 0.0f;
    }
    return *data;
}

int ParameterMapLayer::GetDataInt(int field_index, void *parameter_data) {
    int *data = reinterpret_cast<int *>(static_cast<char *>(parameter_data) + FieldOffsets[field_index]);
    if (!data) {
        return 0;
    }
    return *data;
}

int ParameterMapLayer::GetParameterSetIndexFromMapData(float x, float y) {
    if (this->QuadData8 != 0) {
        return this->GetParameterSetIndexFromQuadData8(x, y);
    }
    if (this->QuadData16 != 0) {
        return this->GetParameterSetIndexFromQuadData16(x, y);
    }
    return 0;
}

int ParameterMapLayer::GetParameterSetIndexFromQuadData8(float x, float y) {
    float left = this->Header->QuadLeft;
    float top = this->Header->QuadTop;
    float right = this->Header->QuadRight;
    float bottom = this->Header->QuadBottom;
    unsigned int node_index = 0;

    if (x < left || right < x || y < top || bottom < y) {
        return 0;
    }

    while (true) {
        ParameterMapQuad8 *quad = &this->QuadData8[node_index];
        node_index = quad->Children.Child0;
        if (node_index == 0) {
            return quad->Children.Child1;
        }

        float middle_x = (left + right) * 0.5f;
        float middle_y = (top + bottom) * 0.5f;
        if (x < middle_x) {
            right = middle_x;
            if (middle_y <= y) {
                node_index = quad->Children.Child1;
                top = middle_y;
            }
        } else {
            left = middle_x;
            if (y < middle_y) {
                node_index = quad->Children.Child2;
            } else {
                node_index = quad->Children.Child3;
                top = middle_y;
            }
        }
    }
}

int ParameterMapLayer::GetParameterSetIndexFromQuadData16(float x, float y) {
    float left = this->Header->QuadLeft;
    float top = this->Header->QuadTop;
    float right = this->Header->QuadRight;
    float bottom = this->Header->QuadBottom;
    unsigned int node_index = 0;

    if (x < left || right < x || y < top || bottom < y) {
        return 0;
    }

    while (true) {
        ParameterMapQuad16 *quad = &this->QuadData16[node_index];
        node_index = quad->Children.Child0;
        if (node_index == 0) {
            return quad->Children.Child1;
        }

        float middle_x = (left + right) * 0.5f;
        float middle_y = (top + bottom) * 0.5f;
        if (x < middle_x) {
            right = middle_x;
            if (middle_y <= y) {
                node_index = quad->Children.Child1;
                top = middle_y;
            }
        } else {
            left = middle_x;
            if (y < middle_y) {
                node_index = quad->Children.Child2;
            } else {
                node_index = quad->Children.Child3;
                top = middle_y;
            }
        }
    }
}

void *ParameterMapLayer::GetFieldPointer(int set_index, int field_index) {
    if (!Header->NumberOfParameterSets || !Header->SizeOfParameterSet ||
        !Header->NumberOfFields || !FieldTypes || !FieldOffsets || !ParameterData) {
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

ParameterAccessor::ParameterAccessor(const char *layer_name)
    : Layer(0), //
      AutoAttachLayerNamehash(0), //
      DebugName(layer_name), //
      CurrentParameterData(0) {
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

    if (this->Layer != 0) {
        this->Layer->RemoveParameterAccessor(this);
    }

    this->Layer = layer;
    if (layer) {
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
    this->CurrentParameterData = this->Layer != 0 ? this->Layer->GetParameterData(x, y) : this->Layer;
}

void ParameterAccessor::ClearData() {
    this->CurrentParameterData = 0;
}

float ParameterAccessor::GetDataFloat(int field_index) {
    return this->Layer != 0 && this->CurrentParameterData != 0 ? this->Layer->GetDataFloat(field_index, this->CurrentParameterData) : 0.0f;
}

int ParameterAccessor::GetDataInt(int field_index) {
    return this->Layer != 0 && this->CurrentParameterData != 0 ? this->Layer->GetDataInt(field_index, this->CurrentParameterData) : 0;
}

void ParameterAccessor::SetUpForNewLayer() {}

ParameterMapsManager::ParameterMapsManager() {}

ParameterMapsManager::~ParameterMapsManager() {
    this->UnloadAllLayers();
}

void ParameterMapsManager::AddLayer(ParameterMapLayer *new_layer) {
    this->ParameterMapLayers.AddTail(new_layer);
}

void ParameterMapsManager::UnloadAllLayers() {
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

    if (warning_if_not_found) {
    }
    return 0;
}

int LoaderParameterMaps(bChunk *chunk) {
    bChunk *last_chunk = chunk->GetLastChunk();
    bChunk *current_chunk = chunk->GetFirstChunk();

    while (current_chunk < last_chunk) {
        ParameterMapLayer *new_layer = new ParameterMapLayer;
        new_layer->Load(&current_chunk);
        GetParameterMapsManager().AddLayer(new_layer);

        for (ParameterAccessor *accessor = GetAutoParameterAccessors().GetHead(); accessor != GetAutoParameterAccessors().EndOfList();) {
            ParameterAccessor *next_accessor = accessor->GetNext();
            if (accessor->GetAutoAttachLayerNamehash() == new_layer->GetNameHash()) {
                accessor->Remove();
                accessor->SetLayer(new_layer);
            }
            accessor = next_accessor;
        }
    }

    return 0;
}

int UnloaderParameterMaps(bChunk *chunk) {
    if (chunk->GetID() == 0x8003B600) {
        DumpAutoParameterAccessorsList();
        GetParameterMapsManager().UnloadAllLayers();
        DumpAutoParameterAccessorsList();
        return 1;
    }
    return 0;
}

void DumpAutoParameterAccessorsList() {}

ParameterAccessorBlend::ParameterAccessorBlend(const char *layer_name)
    : ParameterAccessor(layer_name), //
      LastData(0), //
      HaveLastData(0) {}

ParameterAccessorBlend::~ParameterAccessorBlend() {
}

void ParameterAccessorBlend::CaptureData(float x, float y, float ratio) {
    if (this->Layer == 0 || this->LastData == 0) {
        this->CurrentParameterData = 0;
        return;
    }

    void *current_data = this->Layer->GetParameterData(x, y);
    if (this->HaveLastData == 0) {
        std::memcpy(this->LastData, current_data, this->Layer->GetSizeOfParameterSet());
        this->HaveLastData = 1;
    } else {
        for (int i = 0; i < this->Layer->GetNumberOfFields(); i++) {
            int field_type = this->Layer->GetFieldType(i);
            int field_offset = this->Layer->GetFieldOffset(i);
            char *last_data = reinterpret_cast<char *>(this->LastData) + field_offset;
            char *new_data = reinterpret_cast<char *>(current_data) + field_offset;

            if (field_type == 0) {
                *reinterpret_cast<float *>(last_data) =
                    *reinterpret_cast<float *>(new_data) * ratio + *reinterpret_cast<float *>(last_data) * (1.0f - ratio);
            } else if (field_type == 1) {
                *reinterpret_cast<int *>(last_data) = static_cast<int>(static_cast<float>(*reinterpret_cast<int *>(new_data)) * ratio +
                                                                       static_cast<float>(*reinterpret_cast<int *>(last_data)) * (1.0f - ratio));
            }
        }
    }

    this->CurrentParameterData = this->LastData;
}

void ParameterAccessorBlend::ClearData() {
    if (this->LastData != 0) {
        delete[] reinterpret_cast<char *>(this->LastData);
        this->LastData = 0;
    }
    this->HaveLastData = 0;
    ParameterAccessor::ClearData();
}

void ParameterAccessorBlend::SetUpForNewLayer() {
    if (this->Layer != 0 && this->Layer->GetSizeOfParameterSet() > 0) {
        this->LastData = new char[this->Layer->GetSizeOfParameterSet()];
    }
}

void ParameterAccessorBlend::CaptureData(float x, float y) {
    (void)x;
    (void)y;
}

ParameterAccessorBlendByDistance::ParameterAccessorBlendByDistance(const char *layer_name)
    : ParameterAccessorBlend(layer_name), //
      last_x(0.0f), //
      last_y(0.0f), //
      HaveLastPosition(0) {}

ParameterAccessorBlendByDistance::~ParameterAccessorBlendByDistance() {}

void ParameterAccessorBlendByDistance::CaptureData(float x, float y, float full_blend_distance) {
    float ratio = 1.0f;

    if (this->HaveLastPosition != 0 && full_blend_distance != 0.0f) {
        float dy = y - this->last_y;
        float dx = x - this->last_x;
        float distance = bSqrt(dx * dx + dy * dy);
        if (distance < full_blend_distance) {
            ratio = distance / full_blend_distance;
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

void ParameterAccessorBlendByDistance::CaptureData(float x, float y) {
    (void)x;
    (void)y;
}
