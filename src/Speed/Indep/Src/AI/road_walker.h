#ifndef _ROAD_WALKER
#define _ROAD_WALKER

#include "Speed/Indep/Src/AI/path_spot.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"

DECLARE_CONTAINER_TYPE(_type_set);
DECLARE_CONTAINER_TYPE(start_vector);

// total size: 0x9C
class road_walker {
  public:
    // total size: 0x20
    struct start_record {
        float score;            // offset 0x0, size 0x4
        path_spot spot;         // offset 0x4, size 0x10
        UMath::Vector3 point;   // offset 0x14, size 0xC

        start_record(float s, WRoadNav &nav);

        bool operator<(const start_record &o) const {
            return score < o.score;
        }
    };

#ifdef EA_BUILD_A124
    typedef UTL::Std::vector<start_record, _type_start_vector> start_vector;
#else
    typedef _STL::vector<start_record, _STL::allocator<start_record> > start_vector;
#endif

    road_walker()
        : raceroutes(false) //
        , bestscore(1e30f) //
        , futurescale(0.0f) //
        , startscore(1e30f) //
        , numwalkallpaths(0) //
        , numevaluates(0) {}

    void set_race_routes(bool on) {
        raceroutes = on;
    }

    const path_spot &get_best_start_spot() const {
        return beststartspot;
    }

    const path_spot &get_best_future_spot() const {
        return bestfuturespot;
    }

    const path_spot &get_best_target_spot() const {
        return besttargetspot;
    }

    bool walk_road(const UMath::Vector3 &start, const UMath::Vector3 &dir, float futuredist,
                   float targetdist, short prevfuture, int prevnodeind);

  private:
    float node_find_radius() const {
        return raceroutes ? 20.0f : 50.0f;
    }

    void walk_all_paths(const path_spot &start, float futuredist, float targetdist,
                        bool coppenalty);
    void evaluate_end(const path_spot &targetspot, bool coppenalty);

    static const int walk_limit;
    static const int evaluate_limit;

    bool raceroutes;                 // offset 0x0, size 0x1
    float bestscore;                 // offset 0x4, size 0x4
    path_spot beststartspot;         // offset 0x8, size 0x10
    path_spot bestfuturespot;        // offset 0x18, size 0x10
    path_spot besttargetspot;        // offset 0x28, size 0x10
    UMath::Vector3 direction;        // offset 0x38, size 0xC
    UMath::Vector3 futurepoint;      // offset 0x44, size 0xC
    UMath::Vector3 targetpoint;      // offset 0x50, size 0xC
    short previousfutures[2];        // offset 0x5C, size 0x4
    float futurescale;               // offset 0x60, size 0x4
    path_spot futurespot;            // offset 0x64, size 0x10
    path_spot startspot;             // offset 0x74, size 0x10
    UMath::Vector3 startpoint;       // offset 0x84, size 0xC
    float startscore;                // offset 0x90, size 0x4
    int numwalkallpaths;             // offset 0x94, size 0x4
    int numevaluates;                // offset 0x98, size 0x4
};

#endif
