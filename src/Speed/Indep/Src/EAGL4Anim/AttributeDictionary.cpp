#include "Speed/Indep/Libs/Support/Utility/UVectorMath.h"

#include "AttributeDictionary.h"

namespace EAGL4Anim {

AttributeMetaData AttributeDictionary::mReservedAttributeMetaData[] = {
    AttributeMetaData(),
    AttributeMetaData("FPS", AttributeMetaData::AT_UNSIGNEDCHAR, 1, AttributeId::ID_FPS),
};

};
