#include "Speed/Indep/Libs/Support/Miscellaneous/PackedBinaryTree.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UGroup.hpp"
#include "Speed/Indep/Src/Sim/Simulation.h"

namespace Query {

struct StaticData {
    unsigned int fQueryID;
};

typedef bool (*QueryFunc)(const UTL::COM::IUnknown *, const UTL::COM::IUnknown *, const void *, unsigned int, void *);
typedef void (*QueryResolver)(void *, const UGroup *);
typedef void (*QueryByteSwapper)(void *);

bool QSimTime(const UTL::COM::IUnknown *_actee, const UTL::COM::IUnknown *_actor, const void *_params, unsigned int _length, void *_output);
void QSimTime_Resolver(void *staticData, const UGroup *context);
void QSimTime_ByteSwapper(void *staticData);

static const unsigned int gQueryKeyOrderTable[1] = {0xBF50744E};
static QueryFunc gQueryFunctionTable[1] = {QSimTime};
static QueryResolver gQueryResolverTable[1] = {QSimTime_Resolver};
static QueryByteSwapper gQueryByteSwapperTable[1] = {QSimTime_ByteSwapper};

bool QSimTime(const UTL::COM::IUnknown *_actee, const UTL::COM::IUnknown *_actor, const void *_params, unsigned int _length, void *_output) {
    *static_cast<float *>(_output) = Sim::GetTime();
    return true;
}

void QSimTime_Resolver(void *staticData, const UGroup *context) {}

void QSimTime_ByteSwapper(void *staticData) {}

bool Invoke(unsigned int funcName, const UTL::COM::IUnknown *acteeContext, const UTL::COM::IUnknown *actorContext, const void *params,
            unsigned int numResults, void *results) {
    unsigned int index = SearchPackedBinaryTree<const unsigned int, unsigned int>(1, gQueryKeyOrderTable, funcName);
    if (index == 0) {
        return gQueryFunctionTable[0](acteeContext, actorContext, params, numResults, results);
    }
    return false;
}

QueryFunc LookupQueryFunc(unsigned int funcName) {
    unsigned int index = SearchPackedBinaryTree<const unsigned int, unsigned int>(1, gQueryKeyOrderTable, funcName);

    if (index == 0) {
        return gQueryFunctionTable[0];
    }

    return NULL;
}

void ResolveStaticData(const void *staticData, const UGroup *context) {
    unsigned int queryid = *static_cast<const unsigned int *>(staticData);
    unsigned int index = SearchPackedBinaryTree<const unsigned int, unsigned int>(1, gQueryKeyOrderTable, queryid);
    if (index == 0) {
        gQueryResolverTable[0](const_cast<void *>(staticData), context);
    }
}

void ByteSwapStaticData(const void *staticData) {
    unsigned int queryid = *static_cast<const unsigned int *>(staticData);

    unsigned int index = SearchPackedBinaryTree<const unsigned int, unsigned int>(1, gQueryKeyOrderTable, queryid);

    if (index == 0) {
        gQueryByteSwapperTable[0](const_cast<void *>(staticData));
    }
}

} // namespace Query
