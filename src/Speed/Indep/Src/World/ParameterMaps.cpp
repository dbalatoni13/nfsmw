#include "./ParameterMaps.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

namespace {

enum ParameterMapChunkID {
    kPMCH_Header = 0x3B602,
    kPMCH_FieldTypes = 0x3B603,
    kPMCH_FieldOffsets = 0x3B604,
    kPMCH_ParameterData = 0x3B605,
    kPMCH_QuadData8 = 0x3B607,
    kPMCH_QuadData16 = 0x3B608,
};

ParameterMapsManager gParameterMapsManager;
bTList<ParameterAccessor> gAutoParameterAccessors;

} // namespace

ParameterMapsManager *GetParameterMapsManager() {
    return &gParameterMapsManager;
}

bTList<ParameterAccessor> *GetAutoParameterAccessors() {
    return &gAutoParameterAccessors;
}

ParameterMapLayer::ParameterMapLayer()
    : Header(0), //
      FieldTypes(0), //
      FieldOffsets(0), //
      ParameterData(0), //
      QuadData8(0), //
      QuadData16(0) {}

ParameterMapLayer::~ParameterMapLayer() {
    this->Unload();
    while (!this->ParameterAccessors.IsEmpty()) {
        delete this->ParameterAccessors.GetHead();
    }
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
    this->QuadData8 = 0;
    this->QuadData16 = 0;

    while (!this->ParameterAccessors.IsEmpty()) {
        this->ParameterAccessors.GetHead()->SetLayer(0);
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
    return *reinterpret_cast<float *>(reinterpret_cast<char *>(parameter_data) + this->GetFieldOffset(field_index));
}

int ParameterMapLayer::GetDataInt(int field_index, void *parameter_data) {
    return *reinterpret_cast<int *>(reinterpret_cast<char *>(parameter_data) + this->GetFieldOffset(field_index));
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
    int data_offset = set_index * this->GetSizeOfParameterSet() + this->GetFieldOffset(field_index);
    return reinterpret_cast<char *>(this->ParameterData) + data_offset;
}

ParameterAccessor::ParameterAccessor(const char *layer_name)
    : Layer(0), //
      AutoAttachLayerNamehash(layer_name != 0 ? UCrc32(layer_name).GetValue() : 0), //
      DebugName(layer_name), //
      CurrentParameterData(0) {
    this->Next = this;
    this->Prev = this;
}

ParameterAccessor::~ParameterAccessor() {
    this->ClearLayer();
}

void ParameterAccessor::SetLayer(ParameterMapLayer *layer) {
    this->ClearData();

    if (this->Layer != 0) {
        this->Layer->RemoveParameterAccessor(this);
    }

    this->Layer = layer;
    if (layer == 0) {
        if (this->AutoAttachLayerNamehash != 0) {
            GetAutoParameterAccessors()->AddHead(this);
        }
    } else {
        this->SetUpForNewLayer();
        this->Layer->AddParameterAccessor(this);
    }
}

void ParameterAccessor::ClearLayer() {
    if (this->Layer != 0) {
        this->Layer->RemoveParameterAccessor(this);
        this->Layer = 0;
    } else if (this->AutoAttachLayerNamehash != 0 && this->Next != this) {
        this->Remove();
    }
    this->ClearData();
}

void ParameterAccessor::CaptureData(float x, float y) {
    if (this->Layer != 0) {
        this->CurrentParameterData = this->Layer->GetParameterData(x, y);
    }
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
    accessor->SetLayer(0);
    return 0;
}

int ParameterMapsManager::GetDataForLayer(const char *layer_name, ParameterAccessor *accessor, int warning_if_not_found) {
    return this->GetDataForLayer(UCrc32(layer_name).GetValue(), accessor, warning_if_not_found);
}
