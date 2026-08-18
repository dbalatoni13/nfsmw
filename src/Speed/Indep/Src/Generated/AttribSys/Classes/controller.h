#ifndef _attrib_gen_controller_h
#define _attrib_gen_controller_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct controller : Instance {
    struct _LayoutStruct {
        EA::Reflection::Text CollectionName; // offset 0x0, size 0x4
        EA::Reflection::Bool Pauseable;      // offset 0x4, size 0x1
    };

    typedef ControllerDataRecord TypeOf_CAMERAACTION_CHANGE;
    typedef ControllerDataRecord TypeOf_CAMERAACTION_DEBUG;
    typedef ControllerDataRecord TypeOf_CAMERAACTION_ENABLE_ICE;
    typedef ControllerDataRecord TypeOf_CAMERAACTION_LOOKBACK;
    typedef ControllerDataRecord TypeOf_CAMERAACTION_PULLBACK;
    typedef EA::Reflection::Text TypeOf_CollectionName;
    typedef ControllerDataRecord TypeOf_DEBUGACTION_DROPCAR;
    typedef ControllerDataRecord TypeOf_DEBUGACTION_LOOK_DOWN;
    typedef ControllerDataRecord TypeOf_DEBUGACTION_LOOK_D_DOWN;
    typedef ControllerDataRecord TypeOf_DEBUGACTION_LOOK_D_LEFT;
    typedef ControllerDataRecord TypeOf_DEBUGACTION_LOOK_D_RIGHT;
    typedef ControllerDataRecord TypeOf_DEBUGACTION_LOOK_D_UP;
    typedef ControllerDataRecord TypeOf_DEBUGACTION_LOOK_LEFT;
    typedef ControllerDataRecord TypeOf_DEBUGACTION_LOOK_RIGHT;
    typedef ControllerDataRecord TypeOf_DEBUGACTION_LOOK_UP;
    typedef ControllerDataRecord TypeOf_DEBUGACTION_MOVE_BACK;
    typedef ControllerDataRecord TypeOf_DEBUGACTION_MOVE_DOWN;
    typedef ControllerDataRecord TypeOf_DEBUGACTION_MOVE_D_BACK;
    typedef ControllerDataRecord TypeOf_DEBUGACTION_MOVE_D_FORWARD;
    typedef ControllerDataRecord TypeOf_DEBUGACTION_MOVE_D_LEFT;
    typedef ControllerDataRecord TypeOf_DEBUGACTION_MOVE_D_RIGHT;
    typedef ControllerDataRecord TypeOf_DEBUGACTION_MOVE_FORWARD;
    typedef ControllerDataRecord TypeOf_DEBUGACTION_MOVE_LEFT;
    typedef ControllerDataRecord TypeOf_DEBUGACTION_MOVE_RIGHT;
    typedef ControllerDataRecord TypeOf_DEBUGACTION_MOVE_UP;
    typedef ControllerDataRecord TypeOf_DEBUGACTION_SIMSTEP;
    typedef ControllerDataRecord TypeOf_DEBUGACTION_SUPER_TURBO;
    typedef ControllerDataRecord TypeOf_DEBUGACTION_TOGGLEAI;
    typedef ControllerDataRecord TypeOf_DEBUGACTION_TOGGLECARCOLOUR;
    typedef ControllerDataRecord TypeOf_DEBUGACTION_TOGGLEDEMOCAMERAS;
    typedef ControllerDataRecord TypeOf_DEBUGACTION_TOGGLESIMSTEP;
    typedef ControllerDataRecord TypeOf_DEBUGACTION_TURBO;
    typedef ControllerDataRecord TypeOf_FRONTENDACTION_ACCEPT;
    typedef ControllerDataRecord TypeOf_FRONTENDACTION_BUTTON0;
    typedef ControllerDataRecord TypeOf_FRONTENDACTION_BUTTON1;
    typedef ControllerDataRecord TypeOf_FRONTENDACTION_BUTTON2;
    typedef ControllerDataRecord TypeOf_FRONTENDACTION_BUTTON3;
    typedef ControllerDataRecord TypeOf_FRONTENDACTION_BUTTON4;
    typedef ControllerDataRecord TypeOf_FRONTENDACTION_BUTTON5;
    typedef ControllerDataRecord TypeOf_FRONTENDACTION_CANCEL;
    typedef ControllerDataRecord TypeOf_FRONTENDACTION_CANCEL_ALT;
    typedef ControllerDataRecord TypeOf_FRONTENDACTION_DOWN;
    typedef ControllerDataRecord TypeOf_FRONTENDACTION_LEFT;
    typedef ControllerDataRecord TypeOf_FRONTENDACTION_LTRIGGER;
    typedef ControllerDataRecord TypeOf_FRONTENDACTION_RDOWN;
    typedef ControllerDataRecord TypeOf_FRONTENDACTION_RIGHT;
    typedef ControllerDataRecord TypeOf_FRONTENDACTION_RLEFT;
    typedef ControllerDataRecord TypeOf_FRONTENDACTION_RRIGHT;
    typedef ControllerDataRecord TypeOf_FRONTENDACTION_RTRIGGER;
    typedef ControllerDataRecord TypeOf_FRONTENDACTION_RUP;
    typedef ControllerDataRecord TypeOf_FRONTENDACTION_START;
    typedef ControllerDataRecord TypeOf_FRONTENDACTION_UP;
    typedef ControllerDataRecord TypeOf_GAMEACTION_BACK;
    typedef ControllerDataRecord TypeOf_GAMEACTION_BRAKE;
    typedef ControllerDataRecord TypeOf_GAMEACTION_DEBUGHUMAN1;
    typedef ControllerDataRecord TypeOf_GAMEACTION_DEBUGHUMAN2;
    typedef ControllerDataRecord TypeOf_GAMEACTION_DEBUGHUMAN3;
    typedef ControllerDataRecord TypeOf_GAMEACTION_DEBUGHUMAN4;
    typedef ControllerDataRecord TypeOf_GAMEACTION_FORWARD;
    typedef ControllerDataRecord TypeOf_GAMEACTION_GAMEBREAKER;
    typedef ControllerDataRecord TypeOf_GAMEACTION_GAS;
    typedef ControllerDataRecord TypeOf_GAMEACTION_HANDBRAKE;
    typedef ControllerDataRecord TypeOf_GAMEACTION_JUMP;
    typedef ControllerDataRecord TypeOf_GAMEACTION_NOS;
    typedef ControllerDataRecord TypeOf_GAMEACTION_RESET;
    typedef ControllerDataRecord TypeOf_GAMEACTION_SHIFTDOWN;
    typedef ControllerDataRecord TypeOf_GAMEACTION_SHIFTUP;
    typedef ControllerDataRecord TypeOf_GAMEACTION_STEERLEFT;
    typedef ControllerDataRecord TypeOf_GAMEACTION_STEERRIGHT;
    typedef ControllerDataRecord TypeOf_GAMEACTION_TURNLEFT;
    typedef ControllerDataRecord TypeOf_GAMEACTION_TURNRIGHT;
    typedef ControllerDataRecord TypeOf_HUDACTION_ENGAGE_EVENT;
    typedef ControllerDataRecord TypeOf_HUDACTION_NEXTSONG;
    typedef ControllerDataRecord TypeOf_HUDACTION_PAD_DOWN;
    typedef ControllerDataRecord TypeOf_HUDACTION_PAD_LEFT;
    typedef ControllerDataRecord TypeOf_HUDACTION_PAD_RIGHT;
    typedef ControllerDataRecord TypeOf_HUDACTION_PAUSEREQUEST;
    typedef ControllerDataRecord TypeOf_HUDACTION_SKIPNIS;
    typedef ControllerDataRecord TypeOf_ICEACTION_ALT_1;
    typedef ControllerDataRecord TypeOf_ICEACTION_BUBBLE_BACK;
    typedef ControllerDataRecord TypeOf_ICEACTION_BUBBLE_DOWN;
    typedef ControllerDataRecord TypeOf_ICEACTION_BUBBLE_FORTH;
    typedef ControllerDataRecord TypeOf_ICEACTION_BUBBLE_IN;
    typedef ControllerDataRecord TypeOf_ICEACTION_BUBBLE_LEFT;
    typedef ControllerDataRecord TypeOf_ICEACTION_BUBBLE_OUT;
    typedef ControllerDataRecord TypeOf_ICEACTION_BUBBLE_RIGHT;
    typedef ControllerDataRecord TypeOf_ICEACTION_BUBBLE_SPIN_LEFT;
    typedef ControllerDataRecord TypeOf_ICEACTION_BUBBLE_SPIN_RIGHT;
    typedef ControllerDataRecord TypeOf_ICEACTION_BUBBLE_TILT_DOWN;
    typedef ControllerDataRecord TypeOf_ICEACTION_BUBBLE_TILT_UP;
    typedef ControllerDataRecord TypeOf_ICEACTION_BUBBLE_UP;
    typedef ControllerDataRecord TypeOf_ICEACTION_CANCEL;
    typedef ControllerDataRecord TypeOf_ICEACTION_CLIP_IN;
    typedef ControllerDataRecord TypeOf_ICEACTION_CLIP_OUT;
    typedef ControllerDataRecord TypeOf_ICEACTION_COPY;
    typedef ControllerDataRecord TypeOf_ICEACTION_CUT;
    typedef ControllerDataRecord TypeOf_ICEACTION_DOWN;
    typedef ControllerDataRecord TypeOf_ICEACTION_DUTCH_LEFT;
    typedef ControllerDataRecord TypeOf_ICEACTION_DUTCH_RIGHT;
    typedef ControllerDataRecord TypeOf_ICEACTION_FAST_LEFT;
    typedef ControllerDataRecord TypeOf_ICEACTION_FAST_RIGHT;
    typedef ControllerDataRecord TypeOf_ICEACTION_GRAB_LEFT;
    typedef ControllerDataRecord TypeOf_ICEACTION_GRAB_RIGHT;
    typedef ControllerDataRecord TypeOf_ICEACTION_HELP;
    typedef ControllerDataRecord TypeOf_ICEACTION_HIDE;
    typedef ControllerDataRecord TypeOf_ICEACTION_INSERT;
    typedef ControllerDataRecord TypeOf_ICEACTION_LEFT;
    typedef ControllerDataRecord TypeOf_ICEACTION_LETTERBOX_IN;
    typedef ControllerDataRecord TypeOf_ICEACTION_LETTERBOX_OUT;
    typedef ControllerDataRecord TypeOf_ICEACTION_PASTE;
    typedef ControllerDataRecord TypeOf_ICEACTION_PLAY;
    typedef ControllerDataRecord TypeOf_ICEACTION_RIGHT;
    typedef ControllerDataRecord TypeOf_ICEACTION_SCREENSHOT;
    typedef ControllerDataRecord TypeOf_ICEACTION_SCREENSHOT_STREAM;
    typedef ControllerDataRecord TypeOf_ICEACTION_SELECT;
    typedef ControllerDataRecord TypeOf_ICEACTION_SHAKE_FRQ_DEC;
    typedef ControllerDataRecord TypeOf_ICEACTION_SHAKE_FRQ_INC;
    typedef ControllerDataRecord TypeOf_ICEACTION_SHAKE_MAG_DEC;
    typedef ControllerDataRecord TypeOf_ICEACTION_SHAKE_MAG_INC;
    typedef ControllerDataRecord TypeOf_ICEACTION_SHUTTLE_DOWN;
    typedef ControllerDataRecord TypeOf_ICEACTION_SHUTTLE_LEFT;
    typedef ControllerDataRecord TypeOf_ICEACTION_SHUTTLE_RIGHT;
    typedef ControllerDataRecord TypeOf_ICEACTION_SHUTTLE_SPEED_DEC;
    typedef ControllerDataRecord TypeOf_ICEACTION_SHUTTLE_SPEED_INC;
    typedef ControllerDataRecord TypeOf_ICEACTION_SHUTTLE_UP;
    typedef ControllerDataRecord TypeOf_ICEACTION_SIMSPEED_DEC;
    typedef ControllerDataRecord TypeOf_ICEACTION_SIMSPEED_INC;
    typedef ControllerDataRecord TypeOf_ICEACTION_UNDO;
    typedef ControllerDataRecord TypeOf_ICEACTION_UP;
    typedef ControllerDataRecord TypeOf_ICEACTION_ZOOM_IN;
    typedef ControllerDataRecord TypeOf_ICEACTION_ZOOM_OUT;
    typedef EA::Reflection::Bool TypeOf_Pauseable;
    typedef ControllerDataRecord TypeOf_REACTION_ACTIVATE_EDIT;
    typedef ControllerDataRecord TypeOf_REACTION_APPLY_TO_NEXT;
    typedef ControllerDataRecord TypeOf_REACTION_AUTOFIT;
    typedef ControllerDataRecord TypeOf_REACTION_EDIT_NODE;
    typedef ControllerDataRecord TypeOf_REACTION_LOOKDOWN;
    typedef ControllerDataRecord TypeOf_REACTION_LOOKLEFT;
    typedef ControllerDataRecord TypeOf_REACTION_LOOKRIGHT;
    typedef ControllerDataRecord TypeOf_REACTION_LOOKUP;
    typedef ControllerDataRecord TypeOf_REACTION_MOVE_BACK;
    typedef ControllerDataRecord TypeOf_REACTION_MOVE_DOWN;
    typedef ControllerDataRecord TypeOf_REACTION_MOVE_FORWARD;
    typedef ControllerDataRecord TypeOf_REACTION_MOVE_LEFT;
    typedef ControllerDataRecord TypeOf_REACTION_MOVE_RIGHT;
    typedef ControllerDataRecord TypeOf_REACTION_MOVE_UP;
    typedef ControllerDataRecord TypeOf_REACTION_NEXTLANE;
    typedef ControllerDataRecord TypeOf_REACTION_PICK;
    typedef ControllerDataRecord TypeOf_REACTION_PREVIOUSLANE;
    typedef ControllerDataRecord TypeOf_REACTION_SELECTNEXT;
    typedef ControllerDataRecord TypeOf_REACTION_TURBO;
    typedef ControllerDataRecord TypeOf_VOIPACTION_PUSHTOTALK;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("controller");
    controller(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    controller(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    controller(const Instance &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    controller(const controller &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    controller(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ~controller() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0x2dee1998;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0x2dee1998, dynamicCollectionKey, spaceForAdditionalAttributes);
    }
    Key GenerateUniqueKey(const char *name, bool registerName) const {
        return GenerateUniqueKey(name, registerName);
    }
    void Change(const Collection *c) {
        Instance::Change(c);
    }
    void Change(const RefSpec &refspec) {
        Instance::Change(refspec);
    }
    void Change(Key collectionkey) {
        Change(FindCollection(ClassKey(), collectionkey));
    }
    void ChangeWithDefault(const RefSpec &refspec) {
        Instance::ChangeWithDefault(refspec);
    }
    void ChangeWithDefault(Key collectionkey) {
        Change(FindCollectionWithDefault(ClassKey(), collectionkey));
    }
    const controller &operator=(const controller &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const controller &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool CAMERAACTION_CHANGE(TAttrib<ControllerDataRecord> &result) const;
    bool CAMERAACTION_CHANGE(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &CAMERAACTION_CHANGE(unsigned int index) const;
    unsigned int Num_CAMERAACTION_CHANGE() const;
    bool SET_CAMERAACTION_CHANGE(const ControllerDataRecord &input, unsigned int index);

    bool CAMERAACTION_DEBUG(TAttrib<ControllerDataRecord> &result) const;
    bool CAMERAACTION_DEBUG(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &CAMERAACTION_DEBUG(unsigned int index) const;
    unsigned int Num_CAMERAACTION_DEBUG() const;
    bool SET_CAMERAACTION_DEBUG(const ControllerDataRecord &input, unsigned int index);

    bool CAMERAACTION_ENABLE_ICE(TAttrib<ControllerDataRecord> &result) const;
    bool CAMERAACTION_ENABLE_ICE(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &CAMERAACTION_ENABLE_ICE(unsigned int index) const;
    unsigned int Num_CAMERAACTION_ENABLE_ICE() const;
    bool SET_CAMERAACTION_ENABLE_ICE(const ControllerDataRecord &input, unsigned int index);

    bool CAMERAACTION_LOOKBACK(TAttrib<ControllerDataRecord> &result) const;
    bool CAMERAACTION_LOOKBACK(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &CAMERAACTION_LOOKBACK(unsigned int index) const;
    unsigned int Num_CAMERAACTION_LOOKBACK() const;
    bool SET_CAMERAACTION_LOOKBACK(const ControllerDataRecord &input, unsigned int index);

    bool CAMERAACTION_PULLBACK(TAttrib<ControllerDataRecord> &result) const;
    bool CAMERAACTION_PULLBACK(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &CAMERAACTION_PULLBACK(unsigned int index) const;
    unsigned int Num_CAMERAACTION_PULLBACK() const;
    bool SET_CAMERAACTION_PULLBACK(const ControllerDataRecord &input, unsigned int index);

    bool CollectionName(TAttrib<EA::Reflection::Text> &result) const;
    bool CollectionName(EA::Reflection::Text &result) const;
    const EA::Reflection::Text &CollectionName() const;
    bool SET_CollectionName(const EA::Reflection::Text &input);

    bool DEBUGACTION_DROPCAR(TAttrib<ControllerDataRecord> &result) const;
    bool DEBUGACTION_DROPCAR(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &DEBUGACTION_DROPCAR(unsigned int index) const;
    unsigned int Num_DEBUGACTION_DROPCAR() const;
    bool SET_DEBUGACTION_DROPCAR(const ControllerDataRecord &input, unsigned int index);

    bool DEBUGACTION_LOOK_DOWN(TAttrib<ControllerDataRecord> &result) const;
    bool DEBUGACTION_LOOK_DOWN(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &DEBUGACTION_LOOK_DOWN(unsigned int index) const;
    unsigned int Num_DEBUGACTION_LOOK_DOWN() const;
    bool SET_DEBUGACTION_LOOK_DOWN(const ControllerDataRecord &input, unsigned int index);

    bool DEBUGACTION_LOOK_D_DOWN(TAttrib<ControllerDataRecord> &result) const;
    bool DEBUGACTION_LOOK_D_DOWN(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &DEBUGACTION_LOOK_D_DOWN(unsigned int index) const;
    unsigned int Num_DEBUGACTION_LOOK_D_DOWN() const;
    bool SET_DEBUGACTION_LOOK_D_DOWN(const ControllerDataRecord &input, unsigned int index);

    bool DEBUGACTION_LOOK_D_LEFT(TAttrib<ControllerDataRecord> &result) const;
    bool DEBUGACTION_LOOK_D_LEFT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &DEBUGACTION_LOOK_D_LEFT(unsigned int index) const;
    unsigned int Num_DEBUGACTION_LOOK_D_LEFT() const;
    bool SET_DEBUGACTION_LOOK_D_LEFT(const ControllerDataRecord &input, unsigned int index);

    bool DEBUGACTION_LOOK_D_RIGHT(TAttrib<ControllerDataRecord> &result) const;
    bool DEBUGACTION_LOOK_D_RIGHT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &DEBUGACTION_LOOK_D_RIGHT(unsigned int index) const;
    unsigned int Num_DEBUGACTION_LOOK_D_RIGHT() const;
    bool SET_DEBUGACTION_LOOK_D_RIGHT(const ControllerDataRecord &input, unsigned int index);

    bool DEBUGACTION_LOOK_D_UP(TAttrib<ControllerDataRecord> &result) const;
    bool DEBUGACTION_LOOK_D_UP(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &DEBUGACTION_LOOK_D_UP(unsigned int index) const;
    unsigned int Num_DEBUGACTION_LOOK_D_UP() const;
    bool SET_DEBUGACTION_LOOK_D_UP(const ControllerDataRecord &input, unsigned int index);

    bool DEBUGACTION_LOOK_LEFT(TAttrib<ControllerDataRecord> &result) const;
    bool DEBUGACTION_LOOK_LEFT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &DEBUGACTION_LOOK_LEFT(unsigned int index) const;
    unsigned int Num_DEBUGACTION_LOOK_LEFT() const;
    bool SET_DEBUGACTION_LOOK_LEFT(const ControllerDataRecord &input, unsigned int index);

    bool DEBUGACTION_LOOK_RIGHT(TAttrib<ControllerDataRecord> &result) const;
    bool DEBUGACTION_LOOK_RIGHT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &DEBUGACTION_LOOK_RIGHT(unsigned int index) const;
    unsigned int Num_DEBUGACTION_LOOK_RIGHT() const;
    bool SET_DEBUGACTION_LOOK_RIGHT(const ControllerDataRecord &input, unsigned int index);

    bool DEBUGACTION_LOOK_UP(TAttrib<ControllerDataRecord> &result) const;
    bool DEBUGACTION_LOOK_UP(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &DEBUGACTION_LOOK_UP(unsigned int index) const;
    unsigned int Num_DEBUGACTION_LOOK_UP() const;
    bool SET_DEBUGACTION_LOOK_UP(const ControllerDataRecord &input, unsigned int index);

    bool DEBUGACTION_MOVE_BACK(TAttrib<ControllerDataRecord> &result) const;
    bool DEBUGACTION_MOVE_BACK(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &DEBUGACTION_MOVE_BACK(unsigned int index) const;
    unsigned int Num_DEBUGACTION_MOVE_BACK() const;
    bool SET_DEBUGACTION_MOVE_BACK(const ControllerDataRecord &input, unsigned int index);

    bool DEBUGACTION_MOVE_DOWN(TAttrib<ControllerDataRecord> &result) const;
    bool DEBUGACTION_MOVE_DOWN(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &DEBUGACTION_MOVE_DOWN(unsigned int index) const;
    unsigned int Num_DEBUGACTION_MOVE_DOWN() const;
    bool SET_DEBUGACTION_MOVE_DOWN(const ControllerDataRecord &input, unsigned int index);

    bool DEBUGACTION_MOVE_D_BACK(TAttrib<ControllerDataRecord> &result) const;
    bool DEBUGACTION_MOVE_D_BACK(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &DEBUGACTION_MOVE_D_BACK(unsigned int index) const;
    unsigned int Num_DEBUGACTION_MOVE_D_BACK() const;
    bool SET_DEBUGACTION_MOVE_D_BACK(const ControllerDataRecord &input, unsigned int index);

    bool DEBUGACTION_MOVE_D_FORWARD(TAttrib<ControllerDataRecord> &result) const;
    bool DEBUGACTION_MOVE_D_FORWARD(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &DEBUGACTION_MOVE_D_FORWARD(unsigned int index) const;
    unsigned int Num_DEBUGACTION_MOVE_D_FORWARD() const;
    bool SET_DEBUGACTION_MOVE_D_FORWARD(const ControllerDataRecord &input, unsigned int index);

    bool DEBUGACTION_MOVE_D_LEFT(TAttrib<ControllerDataRecord> &result) const;
    bool DEBUGACTION_MOVE_D_LEFT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &DEBUGACTION_MOVE_D_LEFT(unsigned int index) const;
    unsigned int Num_DEBUGACTION_MOVE_D_LEFT() const;
    bool SET_DEBUGACTION_MOVE_D_LEFT(const ControllerDataRecord &input, unsigned int index);

    bool DEBUGACTION_MOVE_D_RIGHT(TAttrib<ControllerDataRecord> &result) const;
    bool DEBUGACTION_MOVE_D_RIGHT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &DEBUGACTION_MOVE_D_RIGHT(unsigned int index) const;
    unsigned int Num_DEBUGACTION_MOVE_D_RIGHT() const;
    bool SET_DEBUGACTION_MOVE_D_RIGHT(const ControllerDataRecord &input, unsigned int index);

    bool DEBUGACTION_MOVE_FORWARD(TAttrib<ControllerDataRecord> &result) const;
    bool DEBUGACTION_MOVE_FORWARD(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &DEBUGACTION_MOVE_FORWARD(unsigned int index) const;
    unsigned int Num_DEBUGACTION_MOVE_FORWARD() const;
    bool SET_DEBUGACTION_MOVE_FORWARD(const ControllerDataRecord &input, unsigned int index);

    bool DEBUGACTION_MOVE_LEFT(TAttrib<ControllerDataRecord> &result) const;
    bool DEBUGACTION_MOVE_LEFT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &DEBUGACTION_MOVE_LEFT(unsigned int index) const;
    unsigned int Num_DEBUGACTION_MOVE_LEFT() const;
    bool SET_DEBUGACTION_MOVE_LEFT(const ControllerDataRecord &input, unsigned int index);

    bool DEBUGACTION_MOVE_RIGHT(TAttrib<ControllerDataRecord> &result) const;
    bool DEBUGACTION_MOVE_RIGHT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &DEBUGACTION_MOVE_RIGHT(unsigned int index) const;
    unsigned int Num_DEBUGACTION_MOVE_RIGHT() const;
    bool SET_DEBUGACTION_MOVE_RIGHT(const ControllerDataRecord &input, unsigned int index);

    bool DEBUGACTION_MOVE_UP(TAttrib<ControllerDataRecord> &result) const;
    bool DEBUGACTION_MOVE_UP(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &DEBUGACTION_MOVE_UP(unsigned int index) const;
    unsigned int Num_DEBUGACTION_MOVE_UP() const;
    bool SET_DEBUGACTION_MOVE_UP(const ControllerDataRecord &input, unsigned int index);

    bool DEBUGACTION_SIMSTEP(TAttrib<ControllerDataRecord> &result) const;
    bool DEBUGACTION_SIMSTEP(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &DEBUGACTION_SIMSTEP(unsigned int index) const;
    unsigned int Num_DEBUGACTION_SIMSTEP() const;
    bool SET_DEBUGACTION_SIMSTEP(const ControllerDataRecord &input, unsigned int index);

    bool DEBUGACTION_SUPER_TURBO(TAttrib<ControllerDataRecord> &result) const;
    bool DEBUGACTION_SUPER_TURBO(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &DEBUGACTION_SUPER_TURBO(unsigned int index) const;
    unsigned int Num_DEBUGACTION_SUPER_TURBO() const;
    bool SET_DEBUGACTION_SUPER_TURBO(const ControllerDataRecord &input, unsigned int index);

    bool DEBUGACTION_TOGGLEAI(TAttrib<ControllerDataRecord> &result) const;
    bool DEBUGACTION_TOGGLEAI(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &DEBUGACTION_TOGGLEAI(unsigned int index) const;
    unsigned int Num_DEBUGACTION_TOGGLEAI() const;
    bool SET_DEBUGACTION_TOGGLEAI(const ControllerDataRecord &input, unsigned int index);

    bool DEBUGACTION_TOGGLECARCOLOUR(TAttrib<ControllerDataRecord> &result) const;
    bool DEBUGACTION_TOGGLECARCOLOUR(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &DEBUGACTION_TOGGLECARCOLOUR(unsigned int index) const;
    unsigned int Num_DEBUGACTION_TOGGLECARCOLOUR() const;
    bool SET_DEBUGACTION_TOGGLECARCOLOUR(const ControllerDataRecord &input, unsigned int index);

    bool DEBUGACTION_TOGGLEDEMOCAMERAS(TAttrib<ControllerDataRecord> &result) const;
    bool DEBUGACTION_TOGGLEDEMOCAMERAS(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &DEBUGACTION_TOGGLEDEMOCAMERAS(unsigned int index) const;
    unsigned int Num_DEBUGACTION_TOGGLEDEMOCAMERAS() const;
    bool SET_DEBUGACTION_TOGGLEDEMOCAMERAS(const ControllerDataRecord &input, unsigned int index);

    bool DEBUGACTION_TOGGLESIMSTEP(TAttrib<ControllerDataRecord> &result) const;
    bool DEBUGACTION_TOGGLESIMSTEP(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &DEBUGACTION_TOGGLESIMSTEP(unsigned int index) const;
    unsigned int Num_DEBUGACTION_TOGGLESIMSTEP() const;
    bool SET_DEBUGACTION_TOGGLESIMSTEP(const ControllerDataRecord &input, unsigned int index);

    bool DEBUGACTION_TURBO(TAttrib<ControllerDataRecord> &result) const;
    bool DEBUGACTION_TURBO(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &DEBUGACTION_TURBO(unsigned int index) const;
    unsigned int Num_DEBUGACTION_TURBO() const;
    bool SET_DEBUGACTION_TURBO(const ControllerDataRecord &input, unsigned int index);

    bool FRONTENDACTION_ACCEPT(TAttrib<ControllerDataRecord> &result) const;
    bool FRONTENDACTION_ACCEPT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &FRONTENDACTION_ACCEPT(unsigned int index) const;
    unsigned int Num_FRONTENDACTION_ACCEPT() const;
    bool SET_FRONTENDACTION_ACCEPT(const ControllerDataRecord &input, unsigned int index);

    bool FRONTENDACTION_BUTTON0(TAttrib<ControllerDataRecord> &result) const;
    bool FRONTENDACTION_BUTTON0(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &FRONTENDACTION_BUTTON0(unsigned int index) const;
    unsigned int Num_FRONTENDACTION_BUTTON0() const;
    bool SET_FRONTENDACTION_BUTTON0(const ControllerDataRecord &input, unsigned int index);

    bool FRONTENDACTION_BUTTON1(TAttrib<ControllerDataRecord> &result) const;
    bool FRONTENDACTION_BUTTON1(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &FRONTENDACTION_BUTTON1(unsigned int index) const;
    unsigned int Num_FRONTENDACTION_BUTTON1() const;
    bool SET_FRONTENDACTION_BUTTON1(const ControllerDataRecord &input, unsigned int index);

    bool FRONTENDACTION_BUTTON2(TAttrib<ControllerDataRecord> &result) const;
    bool FRONTENDACTION_BUTTON2(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &FRONTENDACTION_BUTTON2(unsigned int index) const;
    unsigned int Num_FRONTENDACTION_BUTTON2() const;
    bool SET_FRONTENDACTION_BUTTON2(const ControllerDataRecord &input, unsigned int index);

    bool FRONTENDACTION_BUTTON3(TAttrib<ControllerDataRecord> &result) const;
    bool FRONTENDACTION_BUTTON3(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &FRONTENDACTION_BUTTON3(unsigned int index) const;
    unsigned int Num_FRONTENDACTION_BUTTON3() const;
    bool SET_FRONTENDACTION_BUTTON3(const ControllerDataRecord &input, unsigned int index);

    bool FRONTENDACTION_BUTTON4(TAttrib<ControllerDataRecord> &result) const;
    bool FRONTENDACTION_BUTTON4(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &FRONTENDACTION_BUTTON4(unsigned int index) const;
    unsigned int Num_FRONTENDACTION_BUTTON4() const;
    bool SET_FRONTENDACTION_BUTTON4(const ControllerDataRecord &input, unsigned int index);

    bool FRONTENDACTION_BUTTON5(TAttrib<ControllerDataRecord> &result) const;
    bool FRONTENDACTION_BUTTON5(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &FRONTENDACTION_BUTTON5(unsigned int index) const;
    unsigned int Num_FRONTENDACTION_BUTTON5() const;
    bool SET_FRONTENDACTION_BUTTON5(const ControllerDataRecord &input, unsigned int index);

    bool FRONTENDACTION_CANCEL(TAttrib<ControllerDataRecord> &result) const;
    bool FRONTENDACTION_CANCEL(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &FRONTENDACTION_CANCEL(unsigned int index) const;
    unsigned int Num_FRONTENDACTION_CANCEL() const;
    bool SET_FRONTENDACTION_CANCEL(const ControllerDataRecord &input, unsigned int index);

    bool FRONTENDACTION_CANCEL_ALT(TAttrib<ControllerDataRecord> &result) const;
    bool FRONTENDACTION_CANCEL_ALT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &FRONTENDACTION_CANCEL_ALT(unsigned int index) const;
    unsigned int Num_FRONTENDACTION_CANCEL_ALT() const;
    bool SET_FRONTENDACTION_CANCEL_ALT(const ControllerDataRecord &input, unsigned int index);

    bool FRONTENDACTION_DOWN(TAttrib<ControllerDataRecord> &result) const;
    bool FRONTENDACTION_DOWN(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &FRONTENDACTION_DOWN(unsigned int index) const;
    unsigned int Num_FRONTENDACTION_DOWN() const;
    bool SET_FRONTENDACTION_DOWN(const ControllerDataRecord &input, unsigned int index);

    bool FRONTENDACTION_LEFT(TAttrib<ControllerDataRecord> &result) const;
    bool FRONTENDACTION_LEFT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &FRONTENDACTION_LEFT(unsigned int index) const;
    unsigned int Num_FRONTENDACTION_LEFT() const;
    bool SET_FRONTENDACTION_LEFT(const ControllerDataRecord &input, unsigned int index);

    bool FRONTENDACTION_LTRIGGER(TAttrib<ControllerDataRecord> &result) const;
    bool FRONTENDACTION_LTRIGGER(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &FRONTENDACTION_LTRIGGER(unsigned int index) const;
    unsigned int Num_FRONTENDACTION_LTRIGGER() const;
    bool SET_FRONTENDACTION_LTRIGGER(const ControllerDataRecord &input, unsigned int index);

    bool FRONTENDACTION_RDOWN(TAttrib<ControllerDataRecord> &result) const;
    bool FRONTENDACTION_RDOWN(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &FRONTENDACTION_RDOWN(unsigned int index) const;
    unsigned int Num_FRONTENDACTION_RDOWN() const;
    bool SET_FRONTENDACTION_RDOWN(const ControllerDataRecord &input, unsigned int index);

    bool FRONTENDACTION_RIGHT(TAttrib<ControllerDataRecord> &result) const;
    bool FRONTENDACTION_RIGHT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &FRONTENDACTION_RIGHT(unsigned int index) const;
    unsigned int Num_FRONTENDACTION_RIGHT() const;
    bool SET_FRONTENDACTION_RIGHT(const ControllerDataRecord &input, unsigned int index);

    bool FRONTENDACTION_RLEFT(TAttrib<ControllerDataRecord> &result) const;
    bool FRONTENDACTION_RLEFT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &FRONTENDACTION_RLEFT(unsigned int index) const;
    unsigned int Num_FRONTENDACTION_RLEFT() const;
    bool SET_FRONTENDACTION_RLEFT(const ControllerDataRecord &input, unsigned int index);

    bool FRONTENDACTION_RRIGHT(TAttrib<ControllerDataRecord> &result) const;
    bool FRONTENDACTION_RRIGHT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &FRONTENDACTION_RRIGHT(unsigned int index) const;
    unsigned int Num_FRONTENDACTION_RRIGHT() const;
    bool SET_FRONTENDACTION_RRIGHT(const ControllerDataRecord &input, unsigned int index);

    bool FRONTENDACTION_RTRIGGER(TAttrib<ControllerDataRecord> &result) const;
    bool FRONTENDACTION_RTRIGGER(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &FRONTENDACTION_RTRIGGER(unsigned int index) const;
    unsigned int Num_FRONTENDACTION_RTRIGGER() const;
    bool SET_FRONTENDACTION_RTRIGGER(const ControllerDataRecord &input, unsigned int index);

    bool FRONTENDACTION_RUP(TAttrib<ControllerDataRecord> &result) const;
    bool FRONTENDACTION_RUP(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &FRONTENDACTION_RUP(unsigned int index) const;
    unsigned int Num_FRONTENDACTION_RUP() const;
    bool SET_FRONTENDACTION_RUP(const ControllerDataRecord &input, unsigned int index);

    bool FRONTENDACTION_START(TAttrib<ControllerDataRecord> &result) const;
    bool FRONTENDACTION_START(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &FRONTENDACTION_START(unsigned int index) const;
    unsigned int Num_FRONTENDACTION_START() const;
    bool SET_FRONTENDACTION_START(const ControllerDataRecord &input, unsigned int index);

    bool FRONTENDACTION_UP(TAttrib<ControllerDataRecord> &result) const;
    bool FRONTENDACTION_UP(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &FRONTENDACTION_UP(unsigned int index) const;
    unsigned int Num_FRONTENDACTION_UP() const;
    bool SET_FRONTENDACTION_UP(const ControllerDataRecord &input, unsigned int index);

    bool GAMEACTION_BACK(TAttrib<ControllerDataRecord> &result) const;
    bool GAMEACTION_BACK(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &GAMEACTION_BACK(unsigned int index) const;
    unsigned int Num_GAMEACTION_BACK() const;
    bool SET_GAMEACTION_BACK(const ControllerDataRecord &input, unsigned int index);

    bool GAMEACTION_BRAKE(TAttrib<ControllerDataRecord> &result) const;
    bool GAMEACTION_BRAKE(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &GAMEACTION_BRAKE(unsigned int index) const;
    unsigned int Num_GAMEACTION_BRAKE() const;
    bool SET_GAMEACTION_BRAKE(const ControllerDataRecord &input, unsigned int index);

    bool GAMEACTION_DEBUGHUMAN1(TAttrib<ControllerDataRecord> &result) const;
    bool GAMEACTION_DEBUGHUMAN1(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &GAMEACTION_DEBUGHUMAN1(unsigned int index) const;
    unsigned int Num_GAMEACTION_DEBUGHUMAN1() const;
    bool SET_GAMEACTION_DEBUGHUMAN1(const ControllerDataRecord &input, unsigned int index);

    bool GAMEACTION_DEBUGHUMAN2(TAttrib<ControllerDataRecord> &result) const;
    bool GAMEACTION_DEBUGHUMAN2(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &GAMEACTION_DEBUGHUMAN2(unsigned int index) const;
    unsigned int Num_GAMEACTION_DEBUGHUMAN2() const;
    bool SET_GAMEACTION_DEBUGHUMAN2(const ControllerDataRecord &input, unsigned int index);

    bool GAMEACTION_DEBUGHUMAN3(TAttrib<ControllerDataRecord> &result) const;
    bool GAMEACTION_DEBUGHUMAN3(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &GAMEACTION_DEBUGHUMAN3(unsigned int index) const;
    unsigned int Num_GAMEACTION_DEBUGHUMAN3() const;
    bool SET_GAMEACTION_DEBUGHUMAN3(const ControllerDataRecord &input, unsigned int index);

    bool GAMEACTION_DEBUGHUMAN4(TAttrib<ControllerDataRecord> &result) const;
    bool GAMEACTION_DEBUGHUMAN4(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &GAMEACTION_DEBUGHUMAN4(unsigned int index) const;
    unsigned int Num_GAMEACTION_DEBUGHUMAN4() const;
    bool SET_GAMEACTION_DEBUGHUMAN4(const ControllerDataRecord &input, unsigned int index);

    bool GAMEACTION_FORWARD(TAttrib<ControllerDataRecord> &result) const;
    bool GAMEACTION_FORWARD(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &GAMEACTION_FORWARD(unsigned int index) const;
    unsigned int Num_GAMEACTION_FORWARD() const;
    bool SET_GAMEACTION_FORWARD(const ControllerDataRecord &input, unsigned int index);

    bool GAMEACTION_GAMEBREAKER(TAttrib<ControllerDataRecord> &result) const;
    bool GAMEACTION_GAMEBREAKER(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &GAMEACTION_GAMEBREAKER(unsigned int index) const;
    unsigned int Num_GAMEACTION_GAMEBREAKER() const;
    bool SET_GAMEACTION_GAMEBREAKER(const ControllerDataRecord &input, unsigned int index);

    bool GAMEACTION_GAS(TAttrib<ControllerDataRecord> &result) const;
    bool GAMEACTION_GAS(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &GAMEACTION_GAS(unsigned int index) const;
    unsigned int Num_GAMEACTION_GAS() const;
    bool SET_GAMEACTION_GAS(const ControllerDataRecord &input, unsigned int index);

    bool GAMEACTION_HANDBRAKE(TAttrib<ControllerDataRecord> &result) const;
    bool GAMEACTION_HANDBRAKE(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &GAMEACTION_HANDBRAKE(unsigned int index) const;
    unsigned int Num_GAMEACTION_HANDBRAKE() const;
    bool SET_GAMEACTION_HANDBRAKE(const ControllerDataRecord &input, unsigned int index);

    bool GAMEACTION_JUMP(TAttrib<ControllerDataRecord> &result) const;
    bool GAMEACTION_JUMP(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &GAMEACTION_JUMP(unsigned int index) const;
    unsigned int Num_GAMEACTION_JUMP() const;
    bool SET_GAMEACTION_JUMP(const ControllerDataRecord &input, unsigned int index);

    bool GAMEACTION_NOS(TAttrib<ControllerDataRecord> &result) const;
    bool GAMEACTION_NOS(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &GAMEACTION_NOS(unsigned int index) const;
    unsigned int Num_GAMEACTION_NOS() const;
    bool SET_GAMEACTION_NOS(const ControllerDataRecord &input, unsigned int index);

    bool GAMEACTION_RESET(TAttrib<ControllerDataRecord> &result) const;
    bool GAMEACTION_RESET(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &GAMEACTION_RESET(unsigned int index) const;
    unsigned int Num_GAMEACTION_RESET() const;
    bool SET_GAMEACTION_RESET(const ControllerDataRecord &input, unsigned int index);

    bool GAMEACTION_SHIFTDOWN(TAttrib<ControllerDataRecord> &result) const;
    bool GAMEACTION_SHIFTDOWN(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &GAMEACTION_SHIFTDOWN(unsigned int index) const;
    unsigned int Num_GAMEACTION_SHIFTDOWN() const;
    bool SET_GAMEACTION_SHIFTDOWN(const ControllerDataRecord &input, unsigned int index);

    bool GAMEACTION_SHIFTUP(TAttrib<ControllerDataRecord> &result) const;
    bool GAMEACTION_SHIFTUP(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &GAMEACTION_SHIFTUP(unsigned int index) const;
    unsigned int Num_GAMEACTION_SHIFTUP() const;
    bool SET_GAMEACTION_SHIFTUP(const ControllerDataRecord &input, unsigned int index);

    bool GAMEACTION_STEERLEFT(TAttrib<ControllerDataRecord> &result) const;
    bool GAMEACTION_STEERLEFT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &GAMEACTION_STEERLEFT(unsigned int index) const;
    unsigned int Num_GAMEACTION_STEERLEFT() const;
    bool SET_GAMEACTION_STEERLEFT(const ControllerDataRecord &input, unsigned int index);

    bool GAMEACTION_STEERRIGHT(TAttrib<ControllerDataRecord> &result) const;
    bool GAMEACTION_STEERRIGHT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &GAMEACTION_STEERRIGHT(unsigned int index) const;
    unsigned int Num_GAMEACTION_STEERRIGHT() const;
    bool SET_GAMEACTION_STEERRIGHT(const ControllerDataRecord &input, unsigned int index);

    bool GAMEACTION_TURNLEFT(TAttrib<ControllerDataRecord> &result) const;
    bool GAMEACTION_TURNLEFT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &GAMEACTION_TURNLEFT(unsigned int index) const;
    unsigned int Num_GAMEACTION_TURNLEFT() const;
    bool SET_GAMEACTION_TURNLEFT(const ControllerDataRecord &input, unsigned int index);

    bool GAMEACTION_TURNRIGHT(TAttrib<ControllerDataRecord> &result) const;
    bool GAMEACTION_TURNRIGHT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &GAMEACTION_TURNRIGHT(unsigned int index) const;
    unsigned int Num_GAMEACTION_TURNRIGHT() const;
    bool SET_GAMEACTION_TURNRIGHT(const ControllerDataRecord &input, unsigned int index);

    bool HUDACTION_ENGAGE_EVENT(TAttrib<ControllerDataRecord> &result) const;
    bool HUDACTION_ENGAGE_EVENT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &HUDACTION_ENGAGE_EVENT(unsigned int index) const;
    unsigned int Num_HUDACTION_ENGAGE_EVENT() const;
    bool SET_HUDACTION_ENGAGE_EVENT(const ControllerDataRecord &input, unsigned int index);

    bool HUDACTION_NEXTSONG(TAttrib<ControllerDataRecord> &result) const;
    bool HUDACTION_NEXTSONG(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &HUDACTION_NEXTSONG(unsigned int index) const;
    unsigned int Num_HUDACTION_NEXTSONG() const;
    bool SET_HUDACTION_NEXTSONG(const ControllerDataRecord &input, unsigned int index);

    bool HUDACTION_PAD_DOWN(TAttrib<ControllerDataRecord> &result) const;
    bool HUDACTION_PAD_DOWN(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &HUDACTION_PAD_DOWN(unsigned int index) const;
    unsigned int Num_HUDACTION_PAD_DOWN() const;
    bool SET_HUDACTION_PAD_DOWN(const ControllerDataRecord &input, unsigned int index);

    bool HUDACTION_PAD_LEFT(TAttrib<ControllerDataRecord> &result) const;
    bool HUDACTION_PAD_LEFT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &HUDACTION_PAD_LEFT(unsigned int index) const;
    unsigned int Num_HUDACTION_PAD_LEFT() const;
    bool SET_HUDACTION_PAD_LEFT(const ControllerDataRecord &input, unsigned int index);

    bool HUDACTION_PAD_RIGHT(TAttrib<ControllerDataRecord> &result) const;
    bool HUDACTION_PAD_RIGHT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &HUDACTION_PAD_RIGHT(unsigned int index) const;
    unsigned int Num_HUDACTION_PAD_RIGHT() const;
    bool SET_HUDACTION_PAD_RIGHT(const ControllerDataRecord &input, unsigned int index);

    bool HUDACTION_PAUSEREQUEST(TAttrib<ControllerDataRecord> &result) const;
    bool HUDACTION_PAUSEREQUEST(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &HUDACTION_PAUSEREQUEST(unsigned int index) const;
    unsigned int Num_HUDACTION_PAUSEREQUEST() const;
    bool SET_HUDACTION_PAUSEREQUEST(const ControllerDataRecord &input, unsigned int index);

    bool HUDACTION_SKIPNIS(TAttrib<ControllerDataRecord> &result) const;
    bool HUDACTION_SKIPNIS(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &HUDACTION_SKIPNIS(unsigned int index) const;
    unsigned int Num_HUDACTION_SKIPNIS() const;
    bool SET_HUDACTION_SKIPNIS(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_ALT_1(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_ALT_1(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_ALT_1(unsigned int index) const;
    unsigned int Num_ICEACTION_ALT_1() const;
    bool SET_ICEACTION_ALT_1(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_BUBBLE_BACK(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_BUBBLE_BACK(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_BUBBLE_BACK(unsigned int index) const;
    unsigned int Num_ICEACTION_BUBBLE_BACK() const;
    bool SET_ICEACTION_BUBBLE_BACK(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_BUBBLE_DOWN(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_BUBBLE_DOWN(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_BUBBLE_DOWN(unsigned int index) const;
    unsigned int Num_ICEACTION_BUBBLE_DOWN() const;
    bool SET_ICEACTION_BUBBLE_DOWN(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_BUBBLE_FORTH(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_BUBBLE_FORTH(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_BUBBLE_FORTH(unsigned int index) const;
    unsigned int Num_ICEACTION_BUBBLE_FORTH() const;
    bool SET_ICEACTION_BUBBLE_FORTH(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_BUBBLE_IN(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_BUBBLE_IN(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_BUBBLE_IN(unsigned int index) const;
    unsigned int Num_ICEACTION_BUBBLE_IN() const;
    bool SET_ICEACTION_BUBBLE_IN(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_BUBBLE_LEFT(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_BUBBLE_LEFT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_BUBBLE_LEFT(unsigned int index) const;
    unsigned int Num_ICEACTION_BUBBLE_LEFT() const;
    bool SET_ICEACTION_BUBBLE_LEFT(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_BUBBLE_OUT(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_BUBBLE_OUT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_BUBBLE_OUT(unsigned int index) const;
    unsigned int Num_ICEACTION_BUBBLE_OUT() const;
    bool SET_ICEACTION_BUBBLE_OUT(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_BUBBLE_RIGHT(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_BUBBLE_RIGHT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_BUBBLE_RIGHT(unsigned int index) const;
    unsigned int Num_ICEACTION_BUBBLE_RIGHT() const;
    bool SET_ICEACTION_BUBBLE_RIGHT(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_BUBBLE_SPIN_LEFT(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_BUBBLE_SPIN_LEFT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_BUBBLE_SPIN_LEFT(unsigned int index) const;
    unsigned int Num_ICEACTION_BUBBLE_SPIN_LEFT() const;
    bool SET_ICEACTION_BUBBLE_SPIN_LEFT(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_BUBBLE_SPIN_RIGHT(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_BUBBLE_SPIN_RIGHT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_BUBBLE_SPIN_RIGHT(unsigned int index) const;
    unsigned int Num_ICEACTION_BUBBLE_SPIN_RIGHT() const;
    bool SET_ICEACTION_BUBBLE_SPIN_RIGHT(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_BUBBLE_TILT_DOWN(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_BUBBLE_TILT_DOWN(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_BUBBLE_TILT_DOWN(unsigned int index) const;
    unsigned int Num_ICEACTION_BUBBLE_TILT_DOWN() const;
    bool SET_ICEACTION_BUBBLE_TILT_DOWN(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_BUBBLE_TILT_UP(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_BUBBLE_TILT_UP(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_BUBBLE_TILT_UP(unsigned int index) const;
    unsigned int Num_ICEACTION_BUBBLE_TILT_UP() const;
    bool SET_ICEACTION_BUBBLE_TILT_UP(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_BUBBLE_UP(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_BUBBLE_UP(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_BUBBLE_UP(unsigned int index) const;
    unsigned int Num_ICEACTION_BUBBLE_UP() const;
    bool SET_ICEACTION_BUBBLE_UP(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_CANCEL(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_CANCEL(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_CANCEL(unsigned int index) const;
    unsigned int Num_ICEACTION_CANCEL() const;
    bool SET_ICEACTION_CANCEL(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_CLIP_IN(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_CLIP_IN(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_CLIP_IN(unsigned int index) const;
    unsigned int Num_ICEACTION_CLIP_IN() const;
    bool SET_ICEACTION_CLIP_IN(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_CLIP_OUT(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_CLIP_OUT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_CLIP_OUT(unsigned int index) const;
    unsigned int Num_ICEACTION_CLIP_OUT() const;
    bool SET_ICEACTION_CLIP_OUT(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_COPY(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_COPY(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_COPY(unsigned int index) const;
    unsigned int Num_ICEACTION_COPY() const;
    bool SET_ICEACTION_COPY(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_CUT(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_CUT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_CUT(unsigned int index) const;
    unsigned int Num_ICEACTION_CUT() const;
    bool SET_ICEACTION_CUT(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_DOWN(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_DOWN(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_DOWN(unsigned int index) const;
    unsigned int Num_ICEACTION_DOWN() const;
    bool SET_ICEACTION_DOWN(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_DUTCH_LEFT(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_DUTCH_LEFT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_DUTCH_LEFT(unsigned int index) const;
    unsigned int Num_ICEACTION_DUTCH_LEFT() const;
    bool SET_ICEACTION_DUTCH_LEFT(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_DUTCH_RIGHT(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_DUTCH_RIGHT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_DUTCH_RIGHT(unsigned int index) const;
    unsigned int Num_ICEACTION_DUTCH_RIGHT() const;
    bool SET_ICEACTION_DUTCH_RIGHT(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_FAST_LEFT(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_FAST_LEFT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_FAST_LEFT(unsigned int index) const;
    unsigned int Num_ICEACTION_FAST_LEFT() const;
    bool SET_ICEACTION_FAST_LEFT(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_FAST_RIGHT(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_FAST_RIGHT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_FAST_RIGHT(unsigned int index) const;
    unsigned int Num_ICEACTION_FAST_RIGHT() const;
    bool SET_ICEACTION_FAST_RIGHT(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_GRAB_LEFT(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_GRAB_LEFT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_GRAB_LEFT(unsigned int index) const;
    unsigned int Num_ICEACTION_GRAB_LEFT() const;
    bool SET_ICEACTION_GRAB_LEFT(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_GRAB_RIGHT(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_GRAB_RIGHT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_GRAB_RIGHT(unsigned int index) const;
    unsigned int Num_ICEACTION_GRAB_RIGHT() const;
    bool SET_ICEACTION_GRAB_RIGHT(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_HELP(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_HELP(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_HELP(unsigned int index) const;
    unsigned int Num_ICEACTION_HELP() const;
    bool SET_ICEACTION_HELP(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_HIDE(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_HIDE(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_HIDE(unsigned int index) const;
    unsigned int Num_ICEACTION_HIDE() const;
    bool SET_ICEACTION_HIDE(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_INSERT(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_INSERT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_INSERT(unsigned int index) const;
    unsigned int Num_ICEACTION_INSERT() const;
    bool SET_ICEACTION_INSERT(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_LEFT(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_LEFT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_LEFT(unsigned int index) const;
    unsigned int Num_ICEACTION_LEFT() const;
    bool SET_ICEACTION_LEFT(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_LETTERBOX_IN(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_LETTERBOX_IN(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_LETTERBOX_IN(unsigned int index) const;
    unsigned int Num_ICEACTION_LETTERBOX_IN() const;
    bool SET_ICEACTION_LETTERBOX_IN(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_LETTERBOX_OUT(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_LETTERBOX_OUT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_LETTERBOX_OUT(unsigned int index) const;
    unsigned int Num_ICEACTION_LETTERBOX_OUT() const;
    bool SET_ICEACTION_LETTERBOX_OUT(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_PASTE(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_PASTE(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_PASTE(unsigned int index) const;
    unsigned int Num_ICEACTION_PASTE() const;
    bool SET_ICEACTION_PASTE(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_PLAY(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_PLAY(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_PLAY(unsigned int index) const;
    unsigned int Num_ICEACTION_PLAY() const;
    bool SET_ICEACTION_PLAY(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_RIGHT(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_RIGHT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_RIGHT(unsigned int index) const;
    unsigned int Num_ICEACTION_RIGHT() const;
    bool SET_ICEACTION_RIGHT(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_SCREENSHOT(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_SCREENSHOT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_SCREENSHOT(unsigned int index) const;
    unsigned int Num_ICEACTION_SCREENSHOT() const;
    bool SET_ICEACTION_SCREENSHOT(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_SCREENSHOT_STREAM(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_SCREENSHOT_STREAM(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_SCREENSHOT_STREAM(unsigned int index) const;
    unsigned int Num_ICEACTION_SCREENSHOT_STREAM() const;
    bool SET_ICEACTION_SCREENSHOT_STREAM(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_SELECT(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_SELECT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_SELECT(unsigned int index) const;
    unsigned int Num_ICEACTION_SELECT() const;
    bool SET_ICEACTION_SELECT(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_SHAKE_FRQ_DEC(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_SHAKE_FRQ_DEC(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_SHAKE_FRQ_DEC(unsigned int index) const;
    unsigned int Num_ICEACTION_SHAKE_FRQ_DEC() const;
    bool SET_ICEACTION_SHAKE_FRQ_DEC(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_SHAKE_FRQ_INC(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_SHAKE_FRQ_INC(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_SHAKE_FRQ_INC(unsigned int index) const;
    unsigned int Num_ICEACTION_SHAKE_FRQ_INC() const;
    bool SET_ICEACTION_SHAKE_FRQ_INC(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_SHAKE_MAG_DEC(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_SHAKE_MAG_DEC(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_SHAKE_MAG_DEC(unsigned int index) const;
    unsigned int Num_ICEACTION_SHAKE_MAG_DEC() const;
    bool SET_ICEACTION_SHAKE_MAG_DEC(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_SHAKE_MAG_INC(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_SHAKE_MAG_INC(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_SHAKE_MAG_INC(unsigned int index) const;
    unsigned int Num_ICEACTION_SHAKE_MAG_INC() const;
    bool SET_ICEACTION_SHAKE_MAG_INC(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_SHUTTLE_DOWN(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_SHUTTLE_DOWN(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_SHUTTLE_DOWN(unsigned int index) const;
    unsigned int Num_ICEACTION_SHUTTLE_DOWN() const;
    bool SET_ICEACTION_SHUTTLE_DOWN(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_SHUTTLE_LEFT(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_SHUTTLE_LEFT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_SHUTTLE_LEFT(unsigned int index) const;
    unsigned int Num_ICEACTION_SHUTTLE_LEFT() const;
    bool SET_ICEACTION_SHUTTLE_LEFT(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_SHUTTLE_RIGHT(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_SHUTTLE_RIGHT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_SHUTTLE_RIGHT(unsigned int index) const;
    unsigned int Num_ICEACTION_SHUTTLE_RIGHT() const;
    bool SET_ICEACTION_SHUTTLE_RIGHT(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_SHUTTLE_SPEED_DEC(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_SHUTTLE_SPEED_DEC(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_SHUTTLE_SPEED_DEC(unsigned int index) const;
    unsigned int Num_ICEACTION_SHUTTLE_SPEED_DEC() const;
    bool SET_ICEACTION_SHUTTLE_SPEED_DEC(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_SHUTTLE_SPEED_INC(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_SHUTTLE_SPEED_INC(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_SHUTTLE_SPEED_INC(unsigned int index) const;
    unsigned int Num_ICEACTION_SHUTTLE_SPEED_INC() const;
    bool SET_ICEACTION_SHUTTLE_SPEED_INC(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_SHUTTLE_UP(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_SHUTTLE_UP(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_SHUTTLE_UP(unsigned int index) const;
    unsigned int Num_ICEACTION_SHUTTLE_UP() const;
    bool SET_ICEACTION_SHUTTLE_UP(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_SIMSPEED_DEC(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_SIMSPEED_DEC(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_SIMSPEED_DEC(unsigned int index) const;
    unsigned int Num_ICEACTION_SIMSPEED_DEC() const;
    bool SET_ICEACTION_SIMSPEED_DEC(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_SIMSPEED_INC(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_SIMSPEED_INC(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_SIMSPEED_INC(unsigned int index) const;
    unsigned int Num_ICEACTION_SIMSPEED_INC() const;
    bool SET_ICEACTION_SIMSPEED_INC(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_UNDO(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_UNDO(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_UNDO(unsigned int index) const;
    unsigned int Num_ICEACTION_UNDO() const;
    bool SET_ICEACTION_UNDO(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_UP(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_UP(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_UP(unsigned int index) const;
    unsigned int Num_ICEACTION_UP() const;
    bool SET_ICEACTION_UP(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_ZOOM_IN(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_ZOOM_IN(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_ZOOM_IN(unsigned int index) const;
    unsigned int Num_ICEACTION_ZOOM_IN() const;
    bool SET_ICEACTION_ZOOM_IN(const ControllerDataRecord &input, unsigned int index);

    bool ICEACTION_ZOOM_OUT(TAttrib<ControllerDataRecord> &result) const;
    bool ICEACTION_ZOOM_OUT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &ICEACTION_ZOOM_OUT(unsigned int index) const;
    unsigned int Num_ICEACTION_ZOOM_OUT() const;
    bool SET_ICEACTION_ZOOM_OUT(const ControllerDataRecord &input, unsigned int index);

    bool Pauseable(TAttrib<EA::Reflection::Bool> &result) const;
    bool Pauseable(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &Pauseable() const;
    bool SET_Pauseable(const EA::Reflection::Bool &input);

    bool REACTION_ACTIVATE_EDIT(TAttrib<ControllerDataRecord> &result) const;
    bool REACTION_ACTIVATE_EDIT(ControllerDataRecord &result) const;
    const ControllerDataRecord &REACTION_ACTIVATE_EDIT() const;
    bool SET_REACTION_ACTIVATE_EDIT(const ControllerDataRecord &input);

    bool REACTION_APPLY_TO_NEXT(TAttrib<ControllerDataRecord> &result) const;
    bool REACTION_APPLY_TO_NEXT(ControllerDataRecord &result) const;
    const ControllerDataRecord &REACTION_APPLY_TO_NEXT() const;
    bool SET_REACTION_APPLY_TO_NEXT(const ControllerDataRecord &input);

    bool REACTION_AUTOFIT(TAttrib<ControllerDataRecord> &result) const;
    bool REACTION_AUTOFIT(ControllerDataRecord &result) const;
    const ControllerDataRecord &REACTION_AUTOFIT() const;
    bool SET_REACTION_AUTOFIT(const ControllerDataRecord &input);

    bool REACTION_EDIT_NODE(TAttrib<ControllerDataRecord> &result) const;
    bool REACTION_EDIT_NODE(ControllerDataRecord &result) const;
    const ControllerDataRecord &REACTION_EDIT_NODE() const;
    bool SET_REACTION_EDIT_NODE(const ControllerDataRecord &input);

    bool REACTION_LOOKDOWN(TAttrib<ControllerDataRecord> &result) const;
    bool REACTION_LOOKDOWN(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &REACTION_LOOKDOWN(unsigned int index) const;
    unsigned int Num_REACTION_LOOKDOWN() const;
    bool SET_REACTION_LOOKDOWN(const ControllerDataRecord &input, unsigned int index);

    bool REACTION_LOOKLEFT(TAttrib<ControllerDataRecord> &result) const;
    bool REACTION_LOOKLEFT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &REACTION_LOOKLEFT(unsigned int index) const;
    unsigned int Num_REACTION_LOOKLEFT() const;
    bool SET_REACTION_LOOKLEFT(const ControllerDataRecord &input, unsigned int index);

    bool REACTION_LOOKRIGHT(TAttrib<ControllerDataRecord> &result) const;
    bool REACTION_LOOKRIGHT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &REACTION_LOOKRIGHT(unsigned int index) const;
    unsigned int Num_REACTION_LOOKRIGHT() const;
    bool SET_REACTION_LOOKRIGHT(const ControllerDataRecord &input, unsigned int index);

    bool REACTION_LOOKUP(TAttrib<ControllerDataRecord> &result) const;
    bool REACTION_LOOKUP(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &REACTION_LOOKUP(unsigned int index) const;
    unsigned int Num_REACTION_LOOKUP() const;
    bool SET_REACTION_LOOKUP(const ControllerDataRecord &input, unsigned int index);

    bool REACTION_MOVE_BACK(TAttrib<ControllerDataRecord> &result) const;
    bool REACTION_MOVE_BACK(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &REACTION_MOVE_BACK(unsigned int index) const;
    unsigned int Num_REACTION_MOVE_BACK() const;
    bool SET_REACTION_MOVE_BACK(const ControllerDataRecord &input, unsigned int index);

    bool REACTION_MOVE_DOWN(TAttrib<ControllerDataRecord> &result) const;
    bool REACTION_MOVE_DOWN(ControllerDataRecord &result) const;
    const ControllerDataRecord &REACTION_MOVE_DOWN() const;
    bool SET_REACTION_MOVE_DOWN(const ControllerDataRecord &input);

    bool REACTION_MOVE_FORWARD(TAttrib<ControllerDataRecord> &result) const;
    bool REACTION_MOVE_FORWARD(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &REACTION_MOVE_FORWARD(unsigned int index) const;
    unsigned int Num_REACTION_MOVE_FORWARD() const;
    bool SET_REACTION_MOVE_FORWARD(const ControllerDataRecord &input, unsigned int index);

    bool REACTION_MOVE_LEFT(TAttrib<ControllerDataRecord> &result) const;
    bool REACTION_MOVE_LEFT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &REACTION_MOVE_LEFT(unsigned int index) const;
    unsigned int Num_REACTION_MOVE_LEFT() const;
    bool SET_REACTION_MOVE_LEFT(const ControllerDataRecord &input, unsigned int index);

    bool REACTION_MOVE_RIGHT(TAttrib<ControllerDataRecord> &result) const;
    bool REACTION_MOVE_RIGHT(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &REACTION_MOVE_RIGHT(unsigned int index) const;
    unsigned int Num_REACTION_MOVE_RIGHT() const;
    bool SET_REACTION_MOVE_RIGHT(const ControllerDataRecord &input, unsigned int index);

    bool REACTION_MOVE_UP(TAttrib<ControllerDataRecord> &result) const;
    bool REACTION_MOVE_UP(ControllerDataRecord &result) const;
    const ControllerDataRecord &REACTION_MOVE_UP() const;
    bool SET_REACTION_MOVE_UP(const ControllerDataRecord &input);

    bool REACTION_NEXTLANE(TAttrib<ControllerDataRecord> &result) const;
    bool REACTION_NEXTLANE(ControllerDataRecord &result) const;
    const ControllerDataRecord &REACTION_NEXTLANE() const;
    bool SET_REACTION_NEXTLANE(const ControllerDataRecord &input);

    bool REACTION_PICK(TAttrib<ControllerDataRecord> &result) const;
    bool REACTION_PICK(ControllerDataRecord &result) const;
    const ControllerDataRecord &REACTION_PICK() const;
    bool SET_REACTION_PICK(const ControllerDataRecord &input);

    bool REACTION_PREVIOUSLANE(TAttrib<ControllerDataRecord> &result) const;
    bool REACTION_PREVIOUSLANE(ControllerDataRecord &result) const;
    const ControllerDataRecord &REACTION_PREVIOUSLANE() const;
    bool SET_REACTION_PREVIOUSLANE(const ControllerDataRecord &input);

    bool REACTION_SELECTNEXT(TAttrib<ControllerDataRecord> &result) const;
    bool REACTION_SELECTNEXT(ControllerDataRecord &result) const;
    const ControllerDataRecord &REACTION_SELECTNEXT() const;
    bool SET_REACTION_SELECTNEXT(const ControllerDataRecord &input);

    bool REACTION_TURBO(TAttrib<ControllerDataRecord> &result) const;
    bool REACTION_TURBO(ControllerDataRecord &result) const;
    const ControllerDataRecord &REACTION_TURBO() const;
    bool SET_REACTION_TURBO(const ControllerDataRecord &input);

    bool VOIPACTION_PUSHTOTALK(TAttrib<ControllerDataRecord> &result) const;
    bool VOIPACTION_PUSHTOTALK(ControllerDataRecord &result, unsigned int index) const;
    const ControllerDataRecord &VOIPACTION_PUSHTOTALK(unsigned int index) const;
    unsigned int Num_VOIPACTION_PUSHTOTALK() const;
    bool SET_VOIPACTION_PUSHTOTALK(const ControllerDataRecord &input, unsigned int index);

  private:
    unsigned int GetLayoutSize() {
        return sizeof(_LayoutStruct);
    }
    controller &ConvertFromInstance(Instance &src) {}
    const controller &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key controller = 0x2dee1998;

}; // namespace ClassName

namespace Hash {
namespace controller {

static const Key CAMERAACTION_CHANGE = 0x2f478795;
static const Key CAMERAACTION_DEBUG = 0xacc223c6;
static const Key CAMERAACTION_ENABLE_ICE = 0xbac6c7d7;
static const Key CAMERAACTION_LOOKBACK = 0x8dc18faf;
static const Key CAMERAACTION_PULLBACK = 0x7f189eec;
static const Key CollectionName = 0x9ca1c8f9;
static const Key DEBUGACTION_DROPCAR = 0xbdf62d8e;
static const Key DEBUGACTION_LOOK_DOWN = 0x136435ac;
static const Key DEBUGACTION_LOOK_D_DOWN = 0x88e9f301;
static const Key DEBUGACTION_LOOK_D_LEFT = 0xf3c5ed4e;
static const Key DEBUGACTION_LOOK_D_RIGHT = 0x49eade84;
static const Key DEBUGACTION_LOOK_D_UP = 0x8bb25bd4;
static const Key DEBUGACTION_LOOK_LEFT = 0x8afced7a;
static const Key DEBUGACTION_LOOK_RIGHT = 0x5c36897f;
static const Key DEBUGACTION_LOOK_UP = 0xc0c95361;
static const Key DEBUGACTION_MOVE_BACK = 0xb9db7418;
static const Key DEBUGACTION_MOVE_DOWN = 0x5eb6f87f;
static const Key DEBUGACTION_MOVE_D_BACK = 0x94227bad;
static const Key DEBUGACTION_MOVE_D_FORWARD = 0x76354cb7;
static const Key DEBUGACTION_MOVE_D_LEFT = 0x3ae35a4c;
static const Key DEBUGACTION_MOVE_D_RIGHT = 0xa449a027;
static const Key DEBUGACTION_MOVE_FORWARD = 0x3391934b;
static const Key DEBUGACTION_MOVE_LEFT = 0x98a510c3;
static const Key DEBUGACTION_MOVE_RIGHT = 0x64a558de;
static const Key DEBUGACTION_MOVE_UP = 0x65f33c23;
static const Key DEBUGACTION_SIMSTEP = 0x1f903fec;
static const Key DEBUGACTION_SUPER_TURBO = 0x7d1b500d;
static const Key DEBUGACTION_TOGGLEAI = 0x1ff07aef;
static const Key DEBUGACTION_TOGGLECARCOLOUR = 0x25e38fdf;
static const Key DEBUGACTION_TOGGLEDEMOCAMERAS = 0xedd2a2f3;
static const Key DEBUGACTION_TOGGLESIMSTEP = 0x770e0d16;
static const Key DEBUGACTION_TURBO = 0x2cf3d7d7;
static const Key FRONTENDACTION_ACCEPT = 0xc18193fd;
static const Key FRONTENDACTION_BUTTON0 = 0x6eb8dedc;
static const Key FRONTENDACTION_BUTTON1 = 0x5954d9c3;
static const Key FRONTENDACTION_BUTTON2 = 0x02c68d30;
static const Key FRONTENDACTION_BUTTON3 = 0x4178fc35;
static const Key FRONTENDACTION_BUTTON4 = 0xf31f71c7;
static const Key FRONTENDACTION_BUTTON5 = 0xa28033b2;
static const Key FRONTENDACTION_CANCEL = 0x01079014;
static const Key FRONTENDACTION_CANCEL_ALT = 0xf6e6bad9;
static const Key FRONTENDACTION_DOWN = 0xef3fac67;
static const Key FRONTENDACTION_LEFT = 0x97011fc1;
static const Key FRONTENDACTION_LTRIGGER = 0xeda51002;
static const Key FRONTENDACTION_RDOWN = 0x06645bd6;
static const Key FRONTENDACTION_RIGHT = 0xa968b63a;
static const Key FRONTENDACTION_RLEFT = 0x376803fa;
static const Key FRONTENDACTION_RRIGHT = 0xde24df25;
static const Key FRONTENDACTION_RTRIGGER = 0x8d005b54;
static const Key FRONTENDACTION_RUP = 0x2b8b4b6f;
static const Key FRONTENDACTION_START = 0x67eebe3f;
static const Key FRONTENDACTION_UP = 0xc455d61d;
static const Key GAMEACTION_BACK = 0xdc0e7b73;
static const Key GAMEACTION_BRAKE = 0x82a9b7d8;
static const Key GAMEACTION_DEBUGHUMAN1 = 0xe82d180f;
static const Key GAMEACTION_DEBUGHUMAN2 = 0x37eb8ea4;
static const Key GAMEACTION_DEBUGHUMAN3 = 0x15024c88;
static const Key GAMEACTION_DEBUGHUMAN4 = 0x6b76f3be;
static const Key GAMEACTION_FORWARD = 0xdc1ab9ea;
static const Key GAMEACTION_GAMEBREAKER = 0x98d605b8;
static const Key GAMEACTION_GAS = 0x06525329;
static const Key GAMEACTION_HANDBRAKE = 0x1aaf2c13;
static const Key GAMEACTION_JUMP = 0xba48a502;
static const Key GAMEACTION_NOS = 0xabc46a0f;
static const Key GAMEACTION_RESET = 0xb488445a;
static const Key GAMEACTION_SHIFTDOWN = 0x50638735;
static const Key GAMEACTION_SHIFTUP = 0x06dbd3c3;
static const Key GAMEACTION_STEERLEFT = 0x37b1b6a8;
static const Key GAMEACTION_STEERRIGHT = 0xb3a37c41;
static const Key GAMEACTION_TURNLEFT = 0x77570455;
static const Key GAMEACTION_TURNRIGHT = 0xfa647f8f;
static const Key HUDACTION_ENGAGE_EVENT = 0xbbab56ef;
static const Key HUDACTION_NEXTSONG = 0x5a1e0eb7;
static const Key HUDACTION_PAD_DOWN = 0x112b5d2d;
static const Key HUDACTION_PAD_LEFT = 0xe2d51bdc;
static const Key HUDACTION_PAD_RIGHT = 0x713c8f5c;
static const Key HUDACTION_PAUSEREQUEST = 0xdaaf60cd;
static const Key HUDACTION_SKIPNIS = 0x8053361a;
static const Key ICEACTION_ALT_1 = 0xc6658aef;
static const Key ICEACTION_BUBBLE_BACK = 0x90b590f9;
static const Key ICEACTION_BUBBLE_DOWN = 0x8573245a;
static const Key ICEACTION_BUBBLE_FORTH = 0xc667a7c0;
static const Key ICEACTION_BUBBLE_IN = 0x71e24c87;
static const Key ICEACTION_BUBBLE_LEFT = 0x83d67a7b;
static const Key ICEACTION_BUBBLE_OUT = 0x535f828a;
static const Key ICEACTION_BUBBLE_RIGHT = 0xdd43339a;
static const Key ICEACTION_BUBBLE_SPIN_LEFT = 0x5bcd578b;
static const Key ICEACTION_BUBBLE_SPIN_RIGHT = 0x8784bb00;
static const Key ICEACTION_BUBBLE_TILT_DOWN = 0x58482016;
static const Key ICEACTION_BUBBLE_TILT_UP = 0x2cfdf83b;
static const Key ICEACTION_BUBBLE_UP = 0x0fd8110f;
static const Key ICEACTION_CANCEL = 0xd9e3873b;
static const Key ICEACTION_CLIP_IN = 0x85c7f9ff;
static const Key ICEACTION_CLIP_OUT = 0xa4f85f2d;
static const Key ICEACTION_COPY = 0x469a29a2;
static const Key ICEACTION_CUT = 0x7be61732;
static const Key ICEACTION_DOWN = 0x80ff983c;
static const Key ICEACTION_DUTCH_LEFT = 0x34e03ed7;
static const Key ICEACTION_DUTCH_RIGHT = 0x005f9d1d;
static const Key ICEACTION_FAST_LEFT = 0x021ecbba;
static const Key ICEACTION_FAST_RIGHT = 0x1cf73674;
static const Key ICEACTION_GRAB_LEFT = 0x0d6d7fb4;
static const Key ICEACTION_GRAB_RIGHT = 0xfcbd5969;
static const Key ICEACTION_HELP = 0xcbb4ccbf;
static const Key ICEACTION_HIDE = 0x9b1e3f1b;
static const Key ICEACTION_INSERT = 0x0bec5a35;
static const Key ICEACTION_LEFT = 0xeb1ce242;
static const Key ICEACTION_LETTERBOX_IN = 0x509346cd;
static const Key ICEACTION_LETTERBOX_OUT = 0x9d6dea3c;
static const Key ICEACTION_PASTE = 0xcf996f30;
static const Key ICEACTION_PLAY = 0x5404dd6d;
static const Key ICEACTION_RIGHT = 0x5bd28819;
static const Key ICEACTION_SCREENSHOT = 0x68af00cb;
static const Key ICEACTION_SCREENSHOT_STREAM = 0x915b5e15;
static const Key ICEACTION_SELECT = 0xa3b4828b;
static const Key ICEACTION_SHAKE_FRQ_DEC = 0x68306931;
static const Key ICEACTION_SHAKE_FRQ_INC = 0xd3f858ca;
static const Key ICEACTION_SHAKE_MAG_DEC = 0x8a9ee6ef;
static const Key ICEACTION_SHAKE_MAG_INC = 0xda3fa888;
static const Key ICEACTION_SHUTTLE_DOWN = 0xdc4ccb3c;
static const Key ICEACTION_SHUTTLE_LEFT = 0x9b870f45;
static const Key ICEACTION_SHUTTLE_RIGHT = 0x591cf93a;
static const Key ICEACTION_SHUTTLE_SPEED_DEC = 0xf08628de;
static const Key ICEACTION_SHUTTLE_SPEED_INC = 0x82fd8328;
static const Key ICEACTION_SHUTTLE_UP = 0xdcfe9685;
static const Key ICEACTION_SIMSPEED_DEC = 0xb5625552;
static const Key ICEACTION_SIMSPEED_INC = 0x25ee8e66;
static const Key ICEACTION_UNDO = 0xc9aca929;
static const Key ICEACTION_UP = 0x5383adea;
static const Key ICEACTION_ZOOM_IN = 0x46b49ed9;
static const Key ICEACTION_ZOOM_OUT = 0xf036026a;
static const Key Pauseable = 0xbe84a0d8;
static const Key REACTION_ACTIVATE_EDIT = 0xaa96159c;
static const Key REACTION_APPLY_TO_NEXT = 0x06bb1fc4;
static const Key REACTION_AUTOFIT = 0xc604e4c8;
static const Key REACTION_EDIT_NODE = 0x87c357b2;
static const Key REACTION_LOOKDOWN = 0x8a90bda5;
static const Key REACTION_LOOKLEFT = 0xc1097c28;
static const Key REACTION_LOOKRIGHT = 0x2e183c63;
static const Key REACTION_LOOKUP = 0xccc3ad00;
static const Key REACTION_MOVE_BACK = 0x521d0a7c;
static const Key REACTION_MOVE_DOWN = 0x16dab024;
static const Key REACTION_MOVE_FORWARD = 0x395f1ceb;
static const Key REACTION_MOVE_LEFT = 0xa914965d;
static const Key REACTION_MOVE_RIGHT = 0xd19432c0;
static const Key REACTION_MOVE_UP = 0xbdcd877e;
static const Key REACTION_NEXTLANE = 0xedd1f872;
static const Key REACTION_PICK = 0x67490952;
static const Key REACTION_PREVIOUSLANE = 0x64bf8411;
static const Key REACTION_SELECTNEXT = 0x291bda5d;
static const Key REACTION_TURBO = 0xda7d1177;
static const Key VOIPACTION_PUSHTOTALK = 0xe5c50a17;

}; // namespace controller
}; // namespace Hash

inline Key Gen::controller::ClassKey() {
    return ClassName::controller;
}

inline bool Gen::controller::CAMERAACTION_CHANGE(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x2f478795);
}
inline const ControllerDataRecord &Gen::controller::CAMERAACTION_CHANGE(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x2f478795, index);
}
inline bool Gen::controller::CAMERAACTION_CHANGE(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x2f478795, result, index);
}
inline unsigned int Gen::controller::Num_CAMERAACTION_CHANGE() const {
    ATTRIB_CODEGEN_GETLENGTH(0x2f478795);
}
inline bool Gen::controller::SET_CAMERAACTION_CHANGE(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x2f478795, input, index);
}

inline bool Gen::controller::CAMERAACTION_DEBUG(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xacc223c6);
}
inline const ControllerDataRecord &Gen::controller::CAMERAACTION_DEBUG(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xacc223c6, index);
}
inline bool Gen::controller::CAMERAACTION_DEBUG(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xacc223c6, result, index);
}
inline unsigned int Gen::controller::Num_CAMERAACTION_DEBUG() const {
    ATTRIB_CODEGEN_GETLENGTH(0xacc223c6);
}
inline bool Gen::controller::SET_CAMERAACTION_DEBUG(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xacc223c6, input, index);
}

inline bool Gen::controller::CAMERAACTION_ENABLE_ICE(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xbac6c7d7);
}
inline const ControllerDataRecord &Gen::controller::CAMERAACTION_ENABLE_ICE(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xbac6c7d7, index);
}
inline bool Gen::controller::CAMERAACTION_ENABLE_ICE(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xbac6c7d7, result, index);
}
inline unsigned int Gen::controller::Num_CAMERAACTION_ENABLE_ICE() const {
    ATTRIB_CODEGEN_GETLENGTH(0xbac6c7d7);
}
inline bool Gen::controller::SET_CAMERAACTION_ENABLE_ICE(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xbac6c7d7, input, index);
}

inline bool Gen::controller::CAMERAACTION_LOOKBACK(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x8dc18faf);
}
inline const ControllerDataRecord &Gen::controller::CAMERAACTION_LOOKBACK(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x8dc18faf, index);
}
inline bool Gen::controller::CAMERAACTION_LOOKBACK(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x8dc18faf, result, index);
}
inline unsigned int Gen::controller::Num_CAMERAACTION_LOOKBACK() const {
    ATTRIB_CODEGEN_GETLENGTH(0x8dc18faf);
}
inline bool Gen::controller::SET_CAMERAACTION_LOOKBACK(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x8dc18faf, input, index);
}

inline bool Gen::controller::CAMERAACTION_PULLBACK(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x7f189eec);
}
inline const ControllerDataRecord &Gen::controller::CAMERAACTION_PULLBACK(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x7f189eec, index);
}
inline bool Gen::controller::CAMERAACTION_PULLBACK(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x7f189eec, result, index);
}
inline unsigned int Gen::controller::Num_CAMERAACTION_PULLBACK() const {
    ATTRIB_CODEGEN_GETLENGTH(0x7f189eec);
}
inline bool Gen::controller::SET_CAMERAACTION_PULLBACK(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x7f189eec, input, index);
}

inline bool Gen::controller::CollectionName(TAttrib<EA::Reflection::Text> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x9ca1c8f9);
}
inline const EA::Reflection::Text &Gen::controller::CollectionName() const {
    ATTRIB_CODEGEN_GETLAYOUT(CollectionName);
}
inline bool Gen::controller::CollectionName(EA::Reflection::Text &result) const {
    result = CollectionName();
    return true;
}
inline bool Gen::controller::SET_CollectionName(const EA::Reflection::Text &input) {
    ATTRIB_CODEGEN_SETLAYOUT(CollectionName, input);
}

inline bool Gen::controller::DEBUGACTION_DROPCAR(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xbdf62d8e);
}
inline const ControllerDataRecord &Gen::controller::DEBUGACTION_DROPCAR(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xbdf62d8e, index);
}
inline bool Gen::controller::DEBUGACTION_DROPCAR(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xbdf62d8e, result, index);
}
inline unsigned int Gen::controller::Num_DEBUGACTION_DROPCAR() const {
    ATTRIB_CODEGEN_GETLENGTH(0xbdf62d8e);
}
inline bool Gen::controller::SET_DEBUGACTION_DROPCAR(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xbdf62d8e, input, index);
}

inline bool Gen::controller::DEBUGACTION_LOOK_DOWN(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x136435ac);
}
inline const ControllerDataRecord &Gen::controller::DEBUGACTION_LOOK_DOWN(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x136435ac, index);
}
inline bool Gen::controller::DEBUGACTION_LOOK_DOWN(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x136435ac, result, index);
}
inline unsigned int Gen::controller::Num_DEBUGACTION_LOOK_DOWN() const {
    ATTRIB_CODEGEN_GETLENGTH(0x136435ac);
}
inline bool Gen::controller::SET_DEBUGACTION_LOOK_DOWN(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x136435ac, input, index);
}

inline bool Gen::controller::DEBUGACTION_LOOK_D_DOWN(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x88e9f301);
}
inline const ControllerDataRecord &Gen::controller::DEBUGACTION_LOOK_D_DOWN(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x88e9f301, index);
}
inline bool Gen::controller::DEBUGACTION_LOOK_D_DOWN(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x88e9f301, result, index);
}
inline unsigned int Gen::controller::Num_DEBUGACTION_LOOK_D_DOWN() const {
    ATTRIB_CODEGEN_GETLENGTH(0x88e9f301);
}
inline bool Gen::controller::SET_DEBUGACTION_LOOK_D_DOWN(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x88e9f301, input, index);
}

inline bool Gen::controller::DEBUGACTION_LOOK_D_LEFT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xf3c5ed4e);
}
inline const ControllerDataRecord &Gen::controller::DEBUGACTION_LOOK_D_LEFT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xf3c5ed4e, index);
}
inline bool Gen::controller::DEBUGACTION_LOOK_D_LEFT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xf3c5ed4e, result, index);
}
inline unsigned int Gen::controller::Num_DEBUGACTION_LOOK_D_LEFT() const {
    ATTRIB_CODEGEN_GETLENGTH(0xf3c5ed4e);
}
inline bool Gen::controller::SET_DEBUGACTION_LOOK_D_LEFT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xf3c5ed4e, input, index);
}

inline bool Gen::controller::DEBUGACTION_LOOK_D_RIGHT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x49eade84);
}
inline const ControllerDataRecord &Gen::controller::DEBUGACTION_LOOK_D_RIGHT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x49eade84, index);
}
inline bool Gen::controller::DEBUGACTION_LOOK_D_RIGHT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x49eade84, result, index);
}
inline unsigned int Gen::controller::Num_DEBUGACTION_LOOK_D_RIGHT() const {
    ATTRIB_CODEGEN_GETLENGTH(0x49eade84);
}
inline bool Gen::controller::SET_DEBUGACTION_LOOK_D_RIGHT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x49eade84, input, index);
}

inline bool Gen::controller::DEBUGACTION_LOOK_D_UP(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x8bb25bd4);
}
inline const ControllerDataRecord &Gen::controller::DEBUGACTION_LOOK_D_UP(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x8bb25bd4, index);
}
inline bool Gen::controller::DEBUGACTION_LOOK_D_UP(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x8bb25bd4, result, index);
}
inline unsigned int Gen::controller::Num_DEBUGACTION_LOOK_D_UP() const {
    ATTRIB_CODEGEN_GETLENGTH(0x8bb25bd4);
}
inline bool Gen::controller::SET_DEBUGACTION_LOOK_D_UP(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x8bb25bd4, input, index);
}

inline bool Gen::controller::DEBUGACTION_LOOK_LEFT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x8afced7a);
}
inline const ControllerDataRecord &Gen::controller::DEBUGACTION_LOOK_LEFT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x8afced7a, index);
}
inline bool Gen::controller::DEBUGACTION_LOOK_LEFT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x8afced7a, result, index);
}
inline unsigned int Gen::controller::Num_DEBUGACTION_LOOK_LEFT() const {
    ATTRIB_CODEGEN_GETLENGTH(0x8afced7a);
}
inline bool Gen::controller::SET_DEBUGACTION_LOOK_LEFT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x8afced7a, input, index);
}

inline bool Gen::controller::DEBUGACTION_LOOK_RIGHT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x5c36897f);
}
inline const ControllerDataRecord &Gen::controller::DEBUGACTION_LOOK_RIGHT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x5c36897f, index);
}
inline bool Gen::controller::DEBUGACTION_LOOK_RIGHT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x5c36897f, result, index);
}
inline unsigned int Gen::controller::Num_DEBUGACTION_LOOK_RIGHT() const {
    ATTRIB_CODEGEN_GETLENGTH(0x5c36897f);
}
inline bool Gen::controller::SET_DEBUGACTION_LOOK_RIGHT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x5c36897f, input, index);
}

inline bool Gen::controller::DEBUGACTION_LOOK_UP(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xc0c95361);
}
inline const ControllerDataRecord &Gen::controller::DEBUGACTION_LOOK_UP(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xc0c95361, index);
}
inline bool Gen::controller::DEBUGACTION_LOOK_UP(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xc0c95361, result, index);
}
inline unsigned int Gen::controller::Num_DEBUGACTION_LOOK_UP() const {
    ATTRIB_CODEGEN_GETLENGTH(0xc0c95361);
}
inline bool Gen::controller::SET_DEBUGACTION_LOOK_UP(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xc0c95361, input, index);
}

inline bool Gen::controller::DEBUGACTION_MOVE_BACK(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xb9db7418);
}
inline const ControllerDataRecord &Gen::controller::DEBUGACTION_MOVE_BACK(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xb9db7418, index);
}
inline bool Gen::controller::DEBUGACTION_MOVE_BACK(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xb9db7418, result, index);
}
inline unsigned int Gen::controller::Num_DEBUGACTION_MOVE_BACK() const {
    ATTRIB_CODEGEN_GETLENGTH(0xb9db7418);
}
inline bool Gen::controller::SET_DEBUGACTION_MOVE_BACK(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xb9db7418, input, index);
}

inline bool Gen::controller::DEBUGACTION_MOVE_DOWN(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x5eb6f87f);
}
inline const ControllerDataRecord &Gen::controller::DEBUGACTION_MOVE_DOWN(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x5eb6f87f, index);
}
inline bool Gen::controller::DEBUGACTION_MOVE_DOWN(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x5eb6f87f, result, index);
}
inline unsigned int Gen::controller::Num_DEBUGACTION_MOVE_DOWN() const {
    ATTRIB_CODEGEN_GETLENGTH(0x5eb6f87f);
}
inline bool Gen::controller::SET_DEBUGACTION_MOVE_DOWN(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x5eb6f87f, input, index);
}

inline bool Gen::controller::DEBUGACTION_MOVE_D_BACK(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x94227bad);
}
inline const ControllerDataRecord &Gen::controller::DEBUGACTION_MOVE_D_BACK(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x94227bad, index);
}
inline bool Gen::controller::DEBUGACTION_MOVE_D_BACK(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x94227bad, result, index);
}
inline unsigned int Gen::controller::Num_DEBUGACTION_MOVE_D_BACK() const {
    ATTRIB_CODEGEN_GETLENGTH(0x94227bad);
}
inline bool Gen::controller::SET_DEBUGACTION_MOVE_D_BACK(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x94227bad, input, index);
}

inline bool Gen::controller::DEBUGACTION_MOVE_D_FORWARD(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x76354cb7);
}
inline const ControllerDataRecord &Gen::controller::DEBUGACTION_MOVE_D_FORWARD(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x76354cb7, index);
}
inline bool Gen::controller::DEBUGACTION_MOVE_D_FORWARD(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x76354cb7, result, index);
}
inline unsigned int Gen::controller::Num_DEBUGACTION_MOVE_D_FORWARD() const {
    ATTRIB_CODEGEN_GETLENGTH(0x76354cb7);
}
inline bool Gen::controller::SET_DEBUGACTION_MOVE_D_FORWARD(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x76354cb7, input, index);
}

inline bool Gen::controller::DEBUGACTION_MOVE_D_LEFT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x3ae35a4c);
}
inline const ControllerDataRecord &Gen::controller::DEBUGACTION_MOVE_D_LEFT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x3ae35a4c, index);
}
inline bool Gen::controller::DEBUGACTION_MOVE_D_LEFT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x3ae35a4c, result, index);
}
inline unsigned int Gen::controller::Num_DEBUGACTION_MOVE_D_LEFT() const {
    ATTRIB_CODEGEN_GETLENGTH(0x3ae35a4c);
}
inline bool Gen::controller::SET_DEBUGACTION_MOVE_D_LEFT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x3ae35a4c, input, index);
}

inline bool Gen::controller::DEBUGACTION_MOVE_D_RIGHT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xa449a027);
}
inline const ControllerDataRecord &Gen::controller::DEBUGACTION_MOVE_D_RIGHT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xa449a027, index);
}
inline bool Gen::controller::DEBUGACTION_MOVE_D_RIGHT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xa449a027, result, index);
}
inline unsigned int Gen::controller::Num_DEBUGACTION_MOVE_D_RIGHT() const {
    ATTRIB_CODEGEN_GETLENGTH(0xa449a027);
}
inline bool Gen::controller::SET_DEBUGACTION_MOVE_D_RIGHT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xa449a027, input, index);
}

inline bool Gen::controller::DEBUGACTION_MOVE_FORWARD(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x3391934b);
}
inline const ControllerDataRecord &Gen::controller::DEBUGACTION_MOVE_FORWARD(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x3391934b, index);
}
inline bool Gen::controller::DEBUGACTION_MOVE_FORWARD(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x3391934b, result, index);
}
inline unsigned int Gen::controller::Num_DEBUGACTION_MOVE_FORWARD() const {
    ATTRIB_CODEGEN_GETLENGTH(0x3391934b);
}
inline bool Gen::controller::SET_DEBUGACTION_MOVE_FORWARD(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x3391934b, input, index);
}

inline bool Gen::controller::DEBUGACTION_MOVE_LEFT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x98a510c3);
}
inline const ControllerDataRecord &Gen::controller::DEBUGACTION_MOVE_LEFT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x98a510c3, index);
}
inline bool Gen::controller::DEBUGACTION_MOVE_LEFT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x98a510c3, result, index);
}
inline unsigned int Gen::controller::Num_DEBUGACTION_MOVE_LEFT() const {
    ATTRIB_CODEGEN_GETLENGTH(0x98a510c3);
}
inline bool Gen::controller::SET_DEBUGACTION_MOVE_LEFT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x98a510c3, input, index);
}

inline bool Gen::controller::DEBUGACTION_MOVE_RIGHT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x64a558de);
}
inline const ControllerDataRecord &Gen::controller::DEBUGACTION_MOVE_RIGHT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x64a558de, index);
}
inline bool Gen::controller::DEBUGACTION_MOVE_RIGHT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x64a558de, result, index);
}
inline unsigned int Gen::controller::Num_DEBUGACTION_MOVE_RIGHT() const {
    ATTRIB_CODEGEN_GETLENGTH(0x64a558de);
}
inline bool Gen::controller::SET_DEBUGACTION_MOVE_RIGHT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x64a558de, input, index);
}

inline bool Gen::controller::DEBUGACTION_MOVE_UP(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x65f33c23);
}
inline const ControllerDataRecord &Gen::controller::DEBUGACTION_MOVE_UP(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x65f33c23, index);
}
inline bool Gen::controller::DEBUGACTION_MOVE_UP(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x65f33c23, result, index);
}
inline unsigned int Gen::controller::Num_DEBUGACTION_MOVE_UP() const {
    ATTRIB_CODEGEN_GETLENGTH(0x65f33c23);
}
inline bool Gen::controller::SET_DEBUGACTION_MOVE_UP(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x65f33c23, input, index);
}

inline bool Gen::controller::DEBUGACTION_SIMSTEP(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x1f903fec);
}
inline const ControllerDataRecord &Gen::controller::DEBUGACTION_SIMSTEP(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x1f903fec, index);
}
inline bool Gen::controller::DEBUGACTION_SIMSTEP(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x1f903fec, result, index);
}
inline unsigned int Gen::controller::Num_DEBUGACTION_SIMSTEP() const {
    ATTRIB_CODEGEN_GETLENGTH(0x1f903fec);
}
inline bool Gen::controller::SET_DEBUGACTION_SIMSTEP(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x1f903fec, input, index);
}

inline bool Gen::controller::DEBUGACTION_SUPER_TURBO(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x7d1b500d);
}
inline const ControllerDataRecord &Gen::controller::DEBUGACTION_SUPER_TURBO(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x7d1b500d, index);
}
inline bool Gen::controller::DEBUGACTION_SUPER_TURBO(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x7d1b500d, result, index);
}
inline unsigned int Gen::controller::Num_DEBUGACTION_SUPER_TURBO() const {
    ATTRIB_CODEGEN_GETLENGTH(0x7d1b500d);
}
inline bool Gen::controller::SET_DEBUGACTION_SUPER_TURBO(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x7d1b500d, input, index);
}

inline bool Gen::controller::DEBUGACTION_TOGGLEAI(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x1ff07aef);
}
inline const ControllerDataRecord &Gen::controller::DEBUGACTION_TOGGLEAI(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x1ff07aef, index);
}
inline bool Gen::controller::DEBUGACTION_TOGGLEAI(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x1ff07aef, result, index);
}
inline unsigned int Gen::controller::Num_DEBUGACTION_TOGGLEAI() const {
    ATTRIB_CODEGEN_GETLENGTH(0x1ff07aef);
}
inline bool Gen::controller::SET_DEBUGACTION_TOGGLEAI(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x1ff07aef, input, index);
}

inline bool Gen::controller::DEBUGACTION_TOGGLECARCOLOUR(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x25e38fdf);
}
inline const ControllerDataRecord &Gen::controller::DEBUGACTION_TOGGLECARCOLOUR(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x25e38fdf, index);
}
inline bool Gen::controller::DEBUGACTION_TOGGLECARCOLOUR(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x25e38fdf, result, index);
}
inline unsigned int Gen::controller::Num_DEBUGACTION_TOGGLECARCOLOUR() const {
    ATTRIB_CODEGEN_GETLENGTH(0x25e38fdf);
}
inline bool Gen::controller::SET_DEBUGACTION_TOGGLECARCOLOUR(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x25e38fdf, input, index);
}

inline bool Gen::controller::DEBUGACTION_TOGGLEDEMOCAMERAS(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xedd2a2f3);
}
inline const ControllerDataRecord &Gen::controller::DEBUGACTION_TOGGLEDEMOCAMERAS(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xedd2a2f3, index);
}
inline bool Gen::controller::DEBUGACTION_TOGGLEDEMOCAMERAS(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xedd2a2f3, result, index);
}
inline unsigned int Gen::controller::Num_DEBUGACTION_TOGGLEDEMOCAMERAS() const {
    ATTRIB_CODEGEN_GETLENGTH(0xedd2a2f3);
}
inline bool Gen::controller::SET_DEBUGACTION_TOGGLEDEMOCAMERAS(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xedd2a2f3, input, index);
}

inline bool Gen::controller::DEBUGACTION_TOGGLESIMSTEP(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x770e0d16);
}
inline const ControllerDataRecord &Gen::controller::DEBUGACTION_TOGGLESIMSTEP(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x770e0d16, index);
}
inline bool Gen::controller::DEBUGACTION_TOGGLESIMSTEP(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x770e0d16, result, index);
}
inline unsigned int Gen::controller::Num_DEBUGACTION_TOGGLESIMSTEP() const {
    ATTRIB_CODEGEN_GETLENGTH(0x770e0d16);
}
inline bool Gen::controller::SET_DEBUGACTION_TOGGLESIMSTEP(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x770e0d16, input, index);
}

inline bool Gen::controller::DEBUGACTION_TURBO(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x2cf3d7d7);
}
inline const ControllerDataRecord &Gen::controller::DEBUGACTION_TURBO(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x2cf3d7d7, index);
}
inline bool Gen::controller::DEBUGACTION_TURBO(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x2cf3d7d7, result, index);
}
inline unsigned int Gen::controller::Num_DEBUGACTION_TURBO() const {
    ATTRIB_CODEGEN_GETLENGTH(0x2cf3d7d7);
}
inline bool Gen::controller::SET_DEBUGACTION_TURBO(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x2cf3d7d7, input, index);
}

inline bool Gen::controller::FRONTENDACTION_ACCEPT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xc18193fd);
}
inline const ControllerDataRecord &Gen::controller::FRONTENDACTION_ACCEPT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xc18193fd, index);
}
inline bool Gen::controller::FRONTENDACTION_ACCEPT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xc18193fd, result, index);
}
inline unsigned int Gen::controller::Num_FRONTENDACTION_ACCEPT() const {
    ATTRIB_CODEGEN_GETLENGTH(0xc18193fd);
}
inline bool Gen::controller::SET_FRONTENDACTION_ACCEPT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xc18193fd, input, index);
}

inline bool Gen::controller::FRONTENDACTION_BUTTON0(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x6eb8dedc);
}
inline const ControllerDataRecord &Gen::controller::FRONTENDACTION_BUTTON0(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x6eb8dedc, index);
}
inline bool Gen::controller::FRONTENDACTION_BUTTON0(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x6eb8dedc, result, index);
}
inline unsigned int Gen::controller::Num_FRONTENDACTION_BUTTON0() const {
    ATTRIB_CODEGEN_GETLENGTH(0x6eb8dedc);
}
inline bool Gen::controller::SET_FRONTENDACTION_BUTTON0(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x6eb8dedc, input, index);
}

inline bool Gen::controller::FRONTENDACTION_BUTTON1(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x5954d9c3);
}
inline const ControllerDataRecord &Gen::controller::FRONTENDACTION_BUTTON1(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x5954d9c3, index);
}
inline bool Gen::controller::FRONTENDACTION_BUTTON1(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x5954d9c3, result, index);
}
inline unsigned int Gen::controller::Num_FRONTENDACTION_BUTTON1() const {
    ATTRIB_CODEGEN_GETLENGTH(0x5954d9c3);
}
inline bool Gen::controller::SET_FRONTENDACTION_BUTTON1(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x5954d9c3, input, index);
}

inline bool Gen::controller::FRONTENDACTION_BUTTON2(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x02c68d30);
}
inline const ControllerDataRecord &Gen::controller::FRONTENDACTION_BUTTON2(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x02c68d30, index);
}
inline bool Gen::controller::FRONTENDACTION_BUTTON2(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x02c68d30, result, index);
}
inline unsigned int Gen::controller::Num_FRONTENDACTION_BUTTON2() const {
    ATTRIB_CODEGEN_GETLENGTH(0x02c68d30);
}
inline bool Gen::controller::SET_FRONTENDACTION_BUTTON2(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x02c68d30, input, index);
}

inline bool Gen::controller::FRONTENDACTION_BUTTON3(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x4178fc35);
}
inline const ControllerDataRecord &Gen::controller::FRONTENDACTION_BUTTON3(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x4178fc35, index);
}
inline bool Gen::controller::FRONTENDACTION_BUTTON3(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x4178fc35, result, index);
}
inline unsigned int Gen::controller::Num_FRONTENDACTION_BUTTON3() const {
    ATTRIB_CODEGEN_GETLENGTH(0x4178fc35);
}
inline bool Gen::controller::SET_FRONTENDACTION_BUTTON3(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x4178fc35, input, index);
}

inline bool Gen::controller::FRONTENDACTION_BUTTON4(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xf31f71c7);
}
inline const ControllerDataRecord &Gen::controller::FRONTENDACTION_BUTTON4(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xf31f71c7, index);
}
inline bool Gen::controller::FRONTENDACTION_BUTTON4(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xf31f71c7, result, index);
}
inline unsigned int Gen::controller::Num_FRONTENDACTION_BUTTON4() const {
    ATTRIB_CODEGEN_GETLENGTH(0xf31f71c7);
}
inline bool Gen::controller::SET_FRONTENDACTION_BUTTON4(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xf31f71c7, input, index);
}

inline bool Gen::controller::FRONTENDACTION_BUTTON5(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xa28033b2);
}
inline const ControllerDataRecord &Gen::controller::FRONTENDACTION_BUTTON5(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xa28033b2, index);
}
inline bool Gen::controller::FRONTENDACTION_BUTTON5(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xa28033b2, result, index);
}
inline unsigned int Gen::controller::Num_FRONTENDACTION_BUTTON5() const {
    ATTRIB_CODEGEN_GETLENGTH(0xa28033b2);
}
inline bool Gen::controller::SET_FRONTENDACTION_BUTTON5(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xa28033b2, input, index);
}

inline bool Gen::controller::FRONTENDACTION_CANCEL(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x01079014);
}
inline const ControllerDataRecord &Gen::controller::FRONTENDACTION_CANCEL(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x01079014, index);
}
inline bool Gen::controller::FRONTENDACTION_CANCEL(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x01079014, result, index);
}
inline unsigned int Gen::controller::Num_FRONTENDACTION_CANCEL() const {
    ATTRIB_CODEGEN_GETLENGTH(0x01079014);
}
inline bool Gen::controller::SET_FRONTENDACTION_CANCEL(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x01079014, input, index);
}

inline bool Gen::controller::FRONTENDACTION_CANCEL_ALT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xf6e6bad9);
}
inline const ControllerDataRecord &Gen::controller::FRONTENDACTION_CANCEL_ALT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xf6e6bad9, index);
}
inline bool Gen::controller::FRONTENDACTION_CANCEL_ALT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xf6e6bad9, result, index);
}
inline unsigned int Gen::controller::Num_FRONTENDACTION_CANCEL_ALT() const {
    ATTRIB_CODEGEN_GETLENGTH(0xf6e6bad9);
}
inline bool Gen::controller::SET_FRONTENDACTION_CANCEL_ALT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xf6e6bad9, input, index);
}

inline bool Gen::controller::FRONTENDACTION_DOWN(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xef3fac67);
}
inline const ControllerDataRecord &Gen::controller::FRONTENDACTION_DOWN(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xef3fac67, index);
}
inline bool Gen::controller::FRONTENDACTION_DOWN(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xef3fac67, result, index);
}
inline unsigned int Gen::controller::Num_FRONTENDACTION_DOWN() const {
    ATTRIB_CODEGEN_GETLENGTH(0xef3fac67);
}
inline bool Gen::controller::SET_FRONTENDACTION_DOWN(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xef3fac67, input, index);
}

inline bool Gen::controller::FRONTENDACTION_LEFT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x97011fc1);
}
inline const ControllerDataRecord &Gen::controller::FRONTENDACTION_LEFT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x97011fc1, index);
}
inline bool Gen::controller::FRONTENDACTION_LEFT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x97011fc1, result, index);
}
inline unsigned int Gen::controller::Num_FRONTENDACTION_LEFT() const {
    ATTRIB_CODEGEN_GETLENGTH(0x97011fc1);
}
inline bool Gen::controller::SET_FRONTENDACTION_LEFT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x97011fc1, input, index);
}

inline bool Gen::controller::FRONTENDACTION_LTRIGGER(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xeda51002);
}
inline const ControllerDataRecord &Gen::controller::FRONTENDACTION_LTRIGGER(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xeda51002, index);
}
inline bool Gen::controller::FRONTENDACTION_LTRIGGER(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xeda51002, result, index);
}
inline unsigned int Gen::controller::Num_FRONTENDACTION_LTRIGGER() const {
    ATTRIB_CODEGEN_GETLENGTH(0xeda51002);
}
inline bool Gen::controller::SET_FRONTENDACTION_LTRIGGER(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xeda51002, input, index);
}

inline bool Gen::controller::FRONTENDACTION_RDOWN(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x06645bd6);
}
inline const ControllerDataRecord &Gen::controller::FRONTENDACTION_RDOWN(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x06645bd6, index);
}
inline bool Gen::controller::FRONTENDACTION_RDOWN(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x06645bd6, result, index);
}
inline unsigned int Gen::controller::Num_FRONTENDACTION_RDOWN() const {
    ATTRIB_CODEGEN_GETLENGTH(0x06645bd6);
}
inline bool Gen::controller::SET_FRONTENDACTION_RDOWN(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x06645bd6, input, index);
}

inline bool Gen::controller::FRONTENDACTION_RIGHT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xa968b63a);
}
inline const ControllerDataRecord &Gen::controller::FRONTENDACTION_RIGHT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xa968b63a, index);
}
inline bool Gen::controller::FRONTENDACTION_RIGHT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xa968b63a, result, index);
}
inline unsigned int Gen::controller::Num_FRONTENDACTION_RIGHT() const {
    ATTRIB_CODEGEN_GETLENGTH(0xa968b63a);
}
inline bool Gen::controller::SET_FRONTENDACTION_RIGHT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xa968b63a, input, index);
}

inline bool Gen::controller::FRONTENDACTION_RLEFT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x376803fa);
}
inline const ControllerDataRecord &Gen::controller::FRONTENDACTION_RLEFT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x376803fa, index);
}
inline bool Gen::controller::FRONTENDACTION_RLEFT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x376803fa, result, index);
}
inline unsigned int Gen::controller::Num_FRONTENDACTION_RLEFT() const {
    ATTRIB_CODEGEN_GETLENGTH(0x376803fa);
}
inline bool Gen::controller::SET_FRONTENDACTION_RLEFT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x376803fa, input, index);
}

inline bool Gen::controller::FRONTENDACTION_RRIGHT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xde24df25);
}
inline const ControllerDataRecord &Gen::controller::FRONTENDACTION_RRIGHT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xde24df25, index);
}
inline bool Gen::controller::FRONTENDACTION_RRIGHT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xde24df25, result, index);
}
inline unsigned int Gen::controller::Num_FRONTENDACTION_RRIGHT() const {
    ATTRIB_CODEGEN_GETLENGTH(0xde24df25);
}
inline bool Gen::controller::SET_FRONTENDACTION_RRIGHT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xde24df25, input, index);
}

inline bool Gen::controller::FRONTENDACTION_RTRIGGER(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x8d005b54);
}
inline const ControllerDataRecord &Gen::controller::FRONTENDACTION_RTRIGGER(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x8d005b54, index);
}
inline bool Gen::controller::FRONTENDACTION_RTRIGGER(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x8d005b54, result, index);
}
inline unsigned int Gen::controller::Num_FRONTENDACTION_RTRIGGER() const {
    ATTRIB_CODEGEN_GETLENGTH(0x8d005b54);
}
inline bool Gen::controller::SET_FRONTENDACTION_RTRIGGER(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x8d005b54, input, index);
}

inline bool Gen::controller::FRONTENDACTION_RUP(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x2b8b4b6f);
}
inline const ControllerDataRecord &Gen::controller::FRONTENDACTION_RUP(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x2b8b4b6f, index);
}
inline bool Gen::controller::FRONTENDACTION_RUP(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x2b8b4b6f, result, index);
}
inline unsigned int Gen::controller::Num_FRONTENDACTION_RUP() const {
    ATTRIB_CODEGEN_GETLENGTH(0x2b8b4b6f);
}
inline bool Gen::controller::SET_FRONTENDACTION_RUP(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x2b8b4b6f, input, index);
}

inline bool Gen::controller::FRONTENDACTION_START(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x67eebe3f);
}
inline const ControllerDataRecord &Gen::controller::FRONTENDACTION_START(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x67eebe3f, index);
}
inline bool Gen::controller::FRONTENDACTION_START(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x67eebe3f, result, index);
}
inline unsigned int Gen::controller::Num_FRONTENDACTION_START() const {
    ATTRIB_CODEGEN_GETLENGTH(0x67eebe3f);
}
inline bool Gen::controller::SET_FRONTENDACTION_START(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x67eebe3f, input, index);
}

inline bool Gen::controller::FRONTENDACTION_UP(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xc455d61d);
}
inline const ControllerDataRecord &Gen::controller::FRONTENDACTION_UP(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xc455d61d, index);
}
inline bool Gen::controller::FRONTENDACTION_UP(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xc455d61d, result, index);
}
inline unsigned int Gen::controller::Num_FRONTENDACTION_UP() const {
    ATTRIB_CODEGEN_GETLENGTH(0xc455d61d);
}
inline bool Gen::controller::SET_FRONTENDACTION_UP(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xc455d61d, input, index);
}

inline bool Gen::controller::GAMEACTION_BACK(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xdc0e7b73);
}
inline const ControllerDataRecord &Gen::controller::GAMEACTION_BACK(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xdc0e7b73, index);
}
inline bool Gen::controller::GAMEACTION_BACK(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xdc0e7b73, result, index);
}
inline unsigned int Gen::controller::Num_GAMEACTION_BACK() const {
    ATTRIB_CODEGEN_GETLENGTH(0xdc0e7b73);
}
inline bool Gen::controller::SET_GAMEACTION_BACK(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xdc0e7b73, input, index);
}

inline bool Gen::controller::GAMEACTION_BRAKE(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x82a9b7d8);
}
inline const ControllerDataRecord &Gen::controller::GAMEACTION_BRAKE(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x82a9b7d8, index);
}
inline bool Gen::controller::GAMEACTION_BRAKE(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x82a9b7d8, result, index);
}
inline unsigned int Gen::controller::Num_GAMEACTION_BRAKE() const {
    ATTRIB_CODEGEN_GETLENGTH(0x82a9b7d8);
}
inline bool Gen::controller::SET_GAMEACTION_BRAKE(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x82a9b7d8, input, index);
}

inline bool Gen::controller::GAMEACTION_DEBUGHUMAN1(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xe82d180f);
}
inline const ControllerDataRecord &Gen::controller::GAMEACTION_DEBUGHUMAN1(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xe82d180f, index);
}
inline bool Gen::controller::GAMEACTION_DEBUGHUMAN1(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xe82d180f, result, index);
}
inline unsigned int Gen::controller::Num_GAMEACTION_DEBUGHUMAN1() const {
    ATTRIB_CODEGEN_GETLENGTH(0xe82d180f);
}
inline bool Gen::controller::SET_GAMEACTION_DEBUGHUMAN1(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xe82d180f, input, index);
}

inline bool Gen::controller::GAMEACTION_DEBUGHUMAN2(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x37eb8ea4);
}
inline const ControllerDataRecord &Gen::controller::GAMEACTION_DEBUGHUMAN2(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x37eb8ea4, index);
}
inline bool Gen::controller::GAMEACTION_DEBUGHUMAN2(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x37eb8ea4, result, index);
}
inline unsigned int Gen::controller::Num_GAMEACTION_DEBUGHUMAN2() const {
    ATTRIB_CODEGEN_GETLENGTH(0x37eb8ea4);
}
inline bool Gen::controller::SET_GAMEACTION_DEBUGHUMAN2(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x37eb8ea4, input, index);
}

inline bool Gen::controller::GAMEACTION_DEBUGHUMAN3(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x15024c88);
}
inline const ControllerDataRecord &Gen::controller::GAMEACTION_DEBUGHUMAN3(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x15024c88, index);
}
inline bool Gen::controller::GAMEACTION_DEBUGHUMAN3(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x15024c88, result, index);
}
inline unsigned int Gen::controller::Num_GAMEACTION_DEBUGHUMAN3() const {
    ATTRIB_CODEGEN_GETLENGTH(0x15024c88);
}
inline bool Gen::controller::SET_GAMEACTION_DEBUGHUMAN3(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x15024c88, input, index);
}

inline bool Gen::controller::GAMEACTION_DEBUGHUMAN4(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x6b76f3be);
}
inline const ControllerDataRecord &Gen::controller::GAMEACTION_DEBUGHUMAN4(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x6b76f3be, index);
}
inline bool Gen::controller::GAMEACTION_DEBUGHUMAN4(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x6b76f3be, result, index);
}
inline unsigned int Gen::controller::Num_GAMEACTION_DEBUGHUMAN4() const {
    ATTRIB_CODEGEN_GETLENGTH(0x6b76f3be);
}
inline bool Gen::controller::SET_GAMEACTION_DEBUGHUMAN4(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x6b76f3be, input, index);
}

inline bool Gen::controller::GAMEACTION_FORWARD(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xdc1ab9ea);
}
inline const ControllerDataRecord &Gen::controller::GAMEACTION_FORWARD(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xdc1ab9ea, index);
}
inline bool Gen::controller::GAMEACTION_FORWARD(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xdc1ab9ea, result, index);
}
inline unsigned int Gen::controller::Num_GAMEACTION_FORWARD() const {
    ATTRIB_CODEGEN_GETLENGTH(0xdc1ab9ea);
}
inline bool Gen::controller::SET_GAMEACTION_FORWARD(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xdc1ab9ea, input, index);
}

inline bool Gen::controller::GAMEACTION_GAMEBREAKER(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x98d605b8);
}
inline const ControllerDataRecord &Gen::controller::GAMEACTION_GAMEBREAKER(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x98d605b8, index);
}
inline bool Gen::controller::GAMEACTION_GAMEBREAKER(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x98d605b8, result, index);
}
inline unsigned int Gen::controller::Num_GAMEACTION_GAMEBREAKER() const {
    ATTRIB_CODEGEN_GETLENGTH(0x98d605b8);
}
inline bool Gen::controller::SET_GAMEACTION_GAMEBREAKER(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x98d605b8, input, index);
}

inline bool Gen::controller::GAMEACTION_GAS(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x06525329);
}
inline const ControllerDataRecord &Gen::controller::GAMEACTION_GAS(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x06525329, index);
}
inline bool Gen::controller::GAMEACTION_GAS(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x06525329, result, index);
}
inline unsigned int Gen::controller::Num_GAMEACTION_GAS() const {
    ATTRIB_CODEGEN_GETLENGTH(0x06525329);
}
inline bool Gen::controller::SET_GAMEACTION_GAS(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x06525329, input, index);
}

inline bool Gen::controller::GAMEACTION_HANDBRAKE(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x1aaf2c13);
}
inline const ControllerDataRecord &Gen::controller::GAMEACTION_HANDBRAKE(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x1aaf2c13, index);
}
inline bool Gen::controller::GAMEACTION_HANDBRAKE(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x1aaf2c13, result, index);
}
inline unsigned int Gen::controller::Num_GAMEACTION_HANDBRAKE() const {
    ATTRIB_CODEGEN_GETLENGTH(0x1aaf2c13);
}
inline bool Gen::controller::SET_GAMEACTION_HANDBRAKE(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x1aaf2c13, input, index);
}

inline bool Gen::controller::GAMEACTION_JUMP(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xba48a502);
}
inline const ControllerDataRecord &Gen::controller::GAMEACTION_JUMP(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xba48a502, index);
}
inline bool Gen::controller::GAMEACTION_JUMP(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xba48a502, result, index);
}
inline unsigned int Gen::controller::Num_GAMEACTION_JUMP() const {
    ATTRIB_CODEGEN_GETLENGTH(0xba48a502);
}
inline bool Gen::controller::SET_GAMEACTION_JUMP(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xba48a502, input, index);
}

inline bool Gen::controller::GAMEACTION_NOS(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xabc46a0f);
}
inline const ControllerDataRecord &Gen::controller::GAMEACTION_NOS(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xabc46a0f, index);
}
inline bool Gen::controller::GAMEACTION_NOS(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xabc46a0f, result, index);
}
inline unsigned int Gen::controller::Num_GAMEACTION_NOS() const {
    ATTRIB_CODEGEN_GETLENGTH(0xabc46a0f);
}
inline bool Gen::controller::SET_GAMEACTION_NOS(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xabc46a0f, input, index);
}

inline bool Gen::controller::GAMEACTION_RESET(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xb488445a);
}
inline const ControllerDataRecord &Gen::controller::GAMEACTION_RESET(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xb488445a, index);
}
inline bool Gen::controller::GAMEACTION_RESET(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xb488445a, result, index);
}
inline unsigned int Gen::controller::Num_GAMEACTION_RESET() const {
    ATTRIB_CODEGEN_GETLENGTH(0xb488445a);
}
inline bool Gen::controller::SET_GAMEACTION_RESET(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xb488445a, input, index);
}

inline bool Gen::controller::GAMEACTION_SHIFTDOWN(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x50638735);
}
inline const ControllerDataRecord &Gen::controller::GAMEACTION_SHIFTDOWN(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x50638735, index);
}
inline bool Gen::controller::GAMEACTION_SHIFTDOWN(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x50638735, result, index);
}
inline unsigned int Gen::controller::Num_GAMEACTION_SHIFTDOWN() const {
    ATTRIB_CODEGEN_GETLENGTH(0x50638735);
}
inline bool Gen::controller::SET_GAMEACTION_SHIFTDOWN(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x50638735, input, index);
}

inline bool Gen::controller::GAMEACTION_SHIFTUP(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x06dbd3c3);
}
inline const ControllerDataRecord &Gen::controller::GAMEACTION_SHIFTUP(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x06dbd3c3, index);
}
inline bool Gen::controller::GAMEACTION_SHIFTUP(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x06dbd3c3, result, index);
}
inline unsigned int Gen::controller::Num_GAMEACTION_SHIFTUP() const {
    ATTRIB_CODEGEN_GETLENGTH(0x06dbd3c3);
}
inline bool Gen::controller::SET_GAMEACTION_SHIFTUP(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x06dbd3c3, input, index);
}

inline bool Gen::controller::GAMEACTION_STEERLEFT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x37b1b6a8);
}
inline const ControllerDataRecord &Gen::controller::GAMEACTION_STEERLEFT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x37b1b6a8, index);
}
inline bool Gen::controller::GAMEACTION_STEERLEFT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x37b1b6a8, result, index);
}
inline unsigned int Gen::controller::Num_GAMEACTION_STEERLEFT() const {
    ATTRIB_CODEGEN_GETLENGTH(0x37b1b6a8);
}
inline bool Gen::controller::SET_GAMEACTION_STEERLEFT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x37b1b6a8, input, index);
}

inline bool Gen::controller::GAMEACTION_STEERRIGHT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xb3a37c41);
}
inline const ControllerDataRecord &Gen::controller::GAMEACTION_STEERRIGHT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xb3a37c41, index);
}
inline bool Gen::controller::GAMEACTION_STEERRIGHT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xb3a37c41, result, index);
}
inline unsigned int Gen::controller::Num_GAMEACTION_STEERRIGHT() const {
    ATTRIB_CODEGEN_GETLENGTH(0xb3a37c41);
}
inline bool Gen::controller::SET_GAMEACTION_STEERRIGHT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xb3a37c41, input, index);
}

inline bool Gen::controller::GAMEACTION_TURNLEFT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x77570455);
}
inline const ControllerDataRecord &Gen::controller::GAMEACTION_TURNLEFT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x77570455, index);
}
inline bool Gen::controller::GAMEACTION_TURNLEFT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x77570455, result, index);
}
inline unsigned int Gen::controller::Num_GAMEACTION_TURNLEFT() const {
    ATTRIB_CODEGEN_GETLENGTH(0x77570455);
}
inline bool Gen::controller::SET_GAMEACTION_TURNLEFT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x77570455, input, index);
}

inline bool Gen::controller::GAMEACTION_TURNRIGHT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xfa647f8f);
}
inline const ControllerDataRecord &Gen::controller::GAMEACTION_TURNRIGHT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xfa647f8f, index);
}
inline bool Gen::controller::GAMEACTION_TURNRIGHT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xfa647f8f, result, index);
}
inline unsigned int Gen::controller::Num_GAMEACTION_TURNRIGHT() const {
    ATTRIB_CODEGEN_GETLENGTH(0xfa647f8f);
}
inline bool Gen::controller::SET_GAMEACTION_TURNRIGHT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xfa647f8f, input, index);
}

inline bool Gen::controller::HUDACTION_ENGAGE_EVENT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xbbab56ef);
}
inline const ControllerDataRecord &Gen::controller::HUDACTION_ENGAGE_EVENT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xbbab56ef, index);
}
inline bool Gen::controller::HUDACTION_ENGAGE_EVENT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xbbab56ef, result, index);
}
inline unsigned int Gen::controller::Num_HUDACTION_ENGAGE_EVENT() const {
    ATTRIB_CODEGEN_GETLENGTH(0xbbab56ef);
}
inline bool Gen::controller::SET_HUDACTION_ENGAGE_EVENT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xbbab56ef, input, index);
}

inline bool Gen::controller::HUDACTION_NEXTSONG(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x5a1e0eb7);
}
inline const ControllerDataRecord &Gen::controller::HUDACTION_NEXTSONG(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x5a1e0eb7, index);
}
inline bool Gen::controller::HUDACTION_NEXTSONG(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x5a1e0eb7, result, index);
}
inline unsigned int Gen::controller::Num_HUDACTION_NEXTSONG() const {
    ATTRIB_CODEGEN_GETLENGTH(0x5a1e0eb7);
}
inline bool Gen::controller::SET_HUDACTION_NEXTSONG(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x5a1e0eb7, input, index);
}

inline bool Gen::controller::HUDACTION_PAD_DOWN(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x112b5d2d);
}
inline const ControllerDataRecord &Gen::controller::HUDACTION_PAD_DOWN(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x112b5d2d, index);
}
inline bool Gen::controller::HUDACTION_PAD_DOWN(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x112b5d2d, result, index);
}
inline unsigned int Gen::controller::Num_HUDACTION_PAD_DOWN() const {
    ATTRIB_CODEGEN_GETLENGTH(0x112b5d2d);
}
inline bool Gen::controller::SET_HUDACTION_PAD_DOWN(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x112b5d2d, input, index);
}

inline bool Gen::controller::HUDACTION_PAD_LEFT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xe2d51bdc);
}
inline const ControllerDataRecord &Gen::controller::HUDACTION_PAD_LEFT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xe2d51bdc, index);
}
inline bool Gen::controller::HUDACTION_PAD_LEFT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xe2d51bdc, result, index);
}
inline unsigned int Gen::controller::Num_HUDACTION_PAD_LEFT() const {
    ATTRIB_CODEGEN_GETLENGTH(0xe2d51bdc);
}
inline bool Gen::controller::SET_HUDACTION_PAD_LEFT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xe2d51bdc, input, index);
}

inline bool Gen::controller::HUDACTION_PAD_RIGHT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x713c8f5c);
}
inline const ControllerDataRecord &Gen::controller::HUDACTION_PAD_RIGHT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x713c8f5c, index);
}
inline bool Gen::controller::HUDACTION_PAD_RIGHT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x713c8f5c, result, index);
}
inline unsigned int Gen::controller::Num_HUDACTION_PAD_RIGHT() const {
    ATTRIB_CODEGEN_GETLENGTH(0x713c8f5c);
}
inline bool Gen::controller::SET_HUDACTION_PAD_RIGHT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x713c8f5c, input, index);
}

inline bool Gen::controller::HUDACTION_PAUSEREQUEST(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xdaaf60cd);
}
inline const ControllerDataRecord &Gen::controller::HUDACTION_PAUSEREQUEST(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xdaaf60cd, index);
}
inline bool Gen::controller::HUDACTION_PAUSEREQUEST(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xdaaf60cd, result, index);
}
inline unsigned int Gen::controller::Num_HUDACTION_PAUSEREQUEST() const {
    ATTRIB_CODEGEN_GETLENGTH(0xdaaf60cd);
}
inline bool Gen::controller::SET_HUDACTION_PAUSEREQUEST(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xdaaf60cd, input, index);
}

inline bool Gen::controller::HUDACTION_SKIPNIS(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x8053361a);
}
inline const ControllerDataRecord &Gen::controller::HUDACTION_SKIPNIS(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x8053361a, index);
}
inline bool Gen::controller::HUDACTION_SKIPNIS(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x8053361a, result, index);
}
inline unsigned int Gen::controller::Num_HUDACTION_SKIPNIS() const {
    ATTRIB_CODEGEN_GETLENGTH(0x8053361a);
}
inline bool Gen::controller::SET_HUDACTION_SKIPNIS(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x8053361a, input, index);
}

inline bool Gen::controller::ICEACTION_ALT_1(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xc6658aef);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_ALT_1(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xc6658aef, index);
}
inline bool Gen::controller::ICEACTION_ALT_1(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xc6658aef, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_ALT_1() const {
    ATTRIB_CODEGEN_GETLENGTH(0xc6658aef);
}
inline bool Gen::controller::SET_ICEACTION_ALT_1(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xc6658aef, input, index);
}

inline bool Gen::controller::ICEACTION_BUBBLE_BACK(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x90b590f9);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_BUBBLE_BACK(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x90b590f9, index);
}
inline bool Gen::controller::ICEACTION_BUBBLE_BACK(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x90b590f9, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_BUBBLE_BACK() const {
    ATTRIB_CODEGEN_GETLENGTH(0x90b590f9);
}
inline bool Gen::controller::SET_ICEACTION_BUBBLE_BACK(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x90b590f9, input, index);
}

inline bool Gen::controller::ICEACTION_BUBBLE_DOWN(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x8573245a);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_BUBBLE_DOWN(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x8573245a, index);
}
inline bool Gen::controller::ICEACTION_BUBBLE_DOWN(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x8573245a, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_BUBBLE_DOWN() const {
    ATTRIB_CODEGEN_GETLENGTH(0x8573245a);
}
inline bool Gen::controller::SET_ICEACTION_BUBBLE_DOWN(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x8573245a, input, index);
}

inline bool Gen::controller::ICEACTION_BUBBLE_FORTH(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xc667a7c0);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_BUBBLE_FORTH(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xc667a7c0, index);
}
inline bool Gen::controller::ICEACTION_BUBBLE_FORTH(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xc667a7c0, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_BUBBLE_FORTH() const {
    ATTRIB_CODEGEN_GETLENGTH(0xc667a7c0);
}
inline bool Gen::controller::SET_ICEACTION_BUBBLE_FORTH(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xc667a7c0, input, index);
}

inline bool Gen::controller::ICEACTION_BUBBLE_IN(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x71e24c87);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_BUBBLE_IN(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x71e24c87, index);
}
inline bool Gen::controller::ICEACTION_BUBBLE_IN(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x71e24c87, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_BUBBLE_IN() const {
    ATTRIB_CODEGEN_GETLENGTH(0x71e24c87);
}
inline bool Gen::controller::SET_ICEACTION_BUBBLE_IN(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x71e24c87, input, index);
}

inline bool Gen::controller::ICEACTION_BUBBLE_LEFT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x83d67a7b);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_BUBBLE_LEFT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x83d67a7b, index);
}
inline bool Gen::controller::ICEACTION_BUBBLE_LEFT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x83d67a7b, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_BUBBLE_LEFT() const {
    ATTRIB_CODEGEN_GETLENGTH(0x83d67a7b);
}
inline bool Gen::controller::SET_ICEACTION_BUBBLE_LEFT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x83d67a7b, input, index);
}

inline bool Gen::controller::ICEACTION_BUBBLE_OUT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x535f828a);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_BUBBLE_OUT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x535f828a, index);
}
inline bool Gen::controller::ICEACTION_BUBBLE_OUT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x535f828a, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_BUBBLE_OUT() const {
    ATTRIB_CODEGEN_GETLENGTH(0x535f828a);
}
inline bool Gen::controller::SET_ICEACTION_BUBBLE_OUT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x535f828a, input, index);
}

inline bool Gen::controller::ICEACTION_BUBBLE_RIGHT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xdd43339a);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_BUBBLE_RIGHT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xdd43339a, index);
}
inline bool Gen::controller::ICEACTION_BUBBLE_RIGHT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xdd43339a, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_BUBBLE_RIGHT() const {
    ATTRIB_CODEGEN_GETLENGTH(0xdd43339a);
}
inline bool Gen::controller::SET_ICEACTION_BUBBLE_RIGHT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xdd43339a, input, index);
}

inline bool Gen::controller::ICEACTION_BUBBLE_SPIN_LEFT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x5bcd578b);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_BUBBLE_SPIN_LEFT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x5bcd578b, index);
}
inline bool Gen::controller::ICEACTION_BUBBLE_SPIN_LEFT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x5bcd578b, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_BUBBLE_SPIN_LEFT() const {
    ATTRIB_CODEGEN_GETLENGTH(0x5bcd578b);
}
inline bool Gen::controller::SET_ICEACTION_BUBBLE_SPIN_LEFT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x5bcd578b, input, index);
}

inline bool Gen::controller::ICEACTION_BUBBLE_SPIN_RIGHT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x8784bb00);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_BUBBLE_SPIN_RIGHT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x8784bb00, index);
}
inline bool Gen::controller::ICEACTION_BUBBLE_SPIN_RIGHT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x8784bb00, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_BUBBLE_SPIN_RIGHT() const {
    ATTRIB_CODEGEN_GETLENGTH(0x8784bb00);
}
inline bool Gen::controller::SET_ICEACTION_BUBBLE_SPIN_RIGHT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x8784bb00, input, index);
}

inline bool Gen::controller::ICEACTION_BUBBLE_TILT_DOWN(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x58482016);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_BUBBLE_TILT_DOWN(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x58482016, index);
}
inline bool Gen::controller::ICEACTION_BUBBLE_TILT_DOWN(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x58482016, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_BUBBLE_TILT_DOWN() const {
    ATTRIB_CODEGEN_GETLENGTH(0x58482016);
}
inline bool Gen::controller::SET_ICEACTION_BUBBLE_TILT_DOWN(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x58482016, input, index);
}

inline bool Gen::controller::ICEACTION_BUBBLE_TILT_UP(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x2cfdf83b);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_BUBBLE_TILT_UP(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x2cfdf83b, index);
}
inline bool Gen::controller::ICEACTION_BUBBLE_TILT_UP(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x2cfdf83b, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_BUBBLE_TILT_UP() const {
    ATTRIB_CODEGEN_GETLENGTH(0x2cfdf83b);
}
inline bool Gen::controller::SET_ICEACTION_BUBBLE_TILT_UP(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x2cfdf83b, input, index);
}

inline bool Gen::controller::ICEACTION_BUBBLE_UP(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x0fd8110f);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_BUBBLE_UP(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x0fd8110f, index);
}
inline bool Gen::controller::ICEACTION_BUBBLE_UP(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x0fd8110f, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_BUBBLE_UP() const {
    ATTRIB_CODEGEN_GETLENGTH(0x0fd8110f);
}
inline bool Gen::controller::SET_ICEACTION_BUBBLE_UP(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x0fd8110f, input, index);
}

inline bool Gen::controller::ICEACTION_CANCEL(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xd9e3873b);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_CANCEL(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xd9e3873b, index);
}
inline bool Gen::controller::ICEACTION_CANCEL(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xd9e3873b, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_CANCEL() const {
    ATTRIB_CODEGEN_GETLENGTH(0xd9e3873b);
}
inline bool Gen::controller::SET_ICEACTION_CANCEL(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xd9e3873b, input, index);
}

inline bool Gen::controller::ICEACTION_CLIP_IN(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x85c7f9ff);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_CLIP_IN(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x85c7f9ff, index);
}
inline bool Gen::controller::ICEACTION_CLIP_IN(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x85c7f9ff, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_CLIP_IN() const {
    ATTRIB_CODEGEN_GETLENGTH(0x85c7f9ff);
}
inline bool Gen::controller::SET_ICEACTION_CLIP_IN(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x85c7f9ff, input, index);
}

inline bool Gen::controller::ICEACTION_CLIP_OUT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xa4f85f2d);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_CLIP_OUT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xa4f85f2d, index);
}
inline bool Gen::controller::ICEACTION_CLIP_OUT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xa4f85f2d, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_CLIP_OUT() const {
    ATTRIB_CODEGEN_GETLENGTH(0xa4f85f2d);
}
inline bool Gen::controller::SET_ICEACTION_CLIP_OUT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xa4f85f2d, input, index);
}

inline bool Gen::controller::ICEACTION_COPY(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x469a29a2);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_COPY(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x469a29a2, index);
}
inline bool Gen::controller::ICEACTION_COPY(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x469a29a2, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_COPY() const {
    ATTRIB_CODEGEN_GETLENGTH(0x469a29a2);
}
inline bool Gen::controller::SET_ICEACTION_COPY(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x469a29a2, input, index);
}

inline bool Gen::controller::ICEACTION_CUT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x7be61732);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_CUT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x7be61732, index);
}
inline bool Gen::controller::ICEACTION_CUT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x7be61732, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_CUT() const {
    ATTRIB_CODEGEN_GETLENGTH(0x7be61732);
}
inline bool Gen::controller::SET_ICEACTION_CUT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x7be61732, input, index);
}

inline bool Gen::controller::ICEACTION_DOWN(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x80ff983c);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_DOWN(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x80ff983c, index);
}
inline bool Gen::controller::ICEACTION_DOWN(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x80ff983c, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_DOWN() const {
    ATTRIB_CODEGEN_GETLENGTH(0x80ff983c);
}
inline bool Gen::controller::SET_ICEACTION_DOWN(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x80ff983c, input, index);
}

inline bool Gen::controller::ICEACTION_DUTCH_LEFT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x34e03ed7);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_DUTCH_LEFT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x34e03ed7, index);
}
inline bool Gen::controller::ICEACTION_DUTCH_LEFT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x34e03ed7, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_DUTCH_LEFT() const {
    ATTRIB_CODEGEN_GETLENGTH(0x34e03ed7);
}
inline bool Gen::controller::SET_ICEACTION_DUTCH_LEFT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x34e03ed7, input, index);
}

inline bool Gen::controller::ICEACTION_DUTCH_RIGHT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x005f9d1d);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_DUTCH_RIGHT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x005f9d1d, index);
}
inline bool Gen::controller::ICEACTION_DUTCH_RIGHT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x005f9d1d, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_DUTCH_RIGHT() const {
    ATTRIB_CODEGEN_GETLENGTH(0x005f9d1d);
}
inline bool Gen::controller::SET_ICEACTION_DUTCH_RIGHT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x005f9d1d, input, index);
}

inline bool Gen::controller::ICEACTION_FAST_LEFT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x021ecbba);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_FAST_LEFT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x021ecbba, index);
}
inline bool Gen::controller::ICEACTION_FAST_LEFT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x021ecbba, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_FAST_LEFT() const {
    ATTRIB_CODEGEN_GETLENGTH(0x021ecbba);
}
inline bool Gen::controller::SET_ICEACTION_FAST_LEFT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x021ecbba, input, index);
}

inline bool Gen::controller::ICEACTION_FAST_RIGHT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x1cf73674);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_FAST_RIGHT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x1cf73674, index);
}
inline bool Gen::controller::ICEACTION_FAST_RIGHT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x1cf73674, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_FAST_RIGHT() const {
    ATTRIB_CODEGEN_GETLENGTH(0x1cf73674);
}
inline bool Gen::controller::SET_ICEACTION_FAST_RIGHT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x1cf73674, input, index);
}

inline bool Gen::controller::ICEACTION_GRAB_LEFT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x0d6d7fb4);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_GRAB_LEFT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x0d6d7fb4, index);
}
inline bool Gen::controller::ICEACTION_GRAB_LEFT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x0d6d7fb4, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_GRAB_LEFT() const {
    ATTRIB_CODEGEN_GETLENGTH(0x0d6d7fb4);
}
inline bool Gen::controller::SET_ICEACTION_GRAB_LEFT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x0d6d7fb4, input, index);
}

inline bool Gen::controller::ICEACTION_GRAB_RIGHT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xfcbd5969);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_GRAB_RIGHT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xfcbd5969, index);
}
inline bool Gen::controller::ICEACTION_GRAB_RIGHT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xfcbd5969, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_GRAB_RIGHT() const {
    ATTRIB_CODEGEN_GETLENGTH(0xfcbd5969);
}
inline bool Gen::controller::SET_ICEACTION_GRAB_RIGHT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xfcbd5969, input, index);
}

inline bool Gen::controller::ICEACTION_HELP(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xcbb4ccbf);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_HELP(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xcbb4ccbf, index);
}
inline bool Gen::controller::ICEACTION_HELP(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xcbb4ccbf, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_HELP() const {
    ATTRIB_CODEGEN_GETLENGTH(0xcbb4ccbf);
}
inline bool Gen::controller::SET_ICEACTION_HELP(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xcbb4ccbf, input, index);
}

inline bool Gen::controller::ICEACTION_HIDE(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x9b1e3f1b);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_HIDE(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x9b1e3f1b, index);
}
inline bool Gen::controller::ICEACTION_HIDE(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x9b1e3f1b, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_HIDE() const {
    ATTRIB_CODEGEN_GETLENGTH(0x9b1e3f1b);
}
inline bool Gen::controller::SET_ICEACTION_HIDE(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x9b1e3f1b, input, index);
}

inline bool Gen::controller::ICEACTION_INSERT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x0bec5a35);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_INSERT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x0bec5a35, index);
}
inline bool Gen::controller::ICEACTION_INSERT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x0bec5a35, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_INSERT() const {
    ATTRIB_CODEGEN_GETLENGTH(0x0bec5a35);
}
inline bool Gen::controller::SET_ICEACTION_INSERT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x0bec5a35, input, index);
}

inline bool Gen::controller::ICEACTION_LEFT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xeb1ce242);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_LEFT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xeb1ce242, index);
}
inline bool Gen::controller::ICEACTION_LEFT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xeb1ce242, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_LEFT() const {
    ATTRIB_CODEGEN_GETLENGTH(0xeb1ce242);
}
inline bool Gen::controller::SET_ICEACTION_LEFT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xeb1ce242, input, index);
}

inline bool Gen::controller::ICEACTION_LETTERBOX_IN(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x509346cd);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_LETTERBOX_IN(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x509346cd, index);
}
inline bool Gen::controller::ICEACTION_LETTERBOX_IN(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x509346cd, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_LETTERBOX_IN() const {
    ATTRIB_CODEGEN_GETLENGTH(0x509346cd);
}
inline bool Gen::controller::SET_ICEACTION_LETTERBOX_IN(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x509346cd, input, index);
}

inline bool Gen::controller::ICEACTION_LETTERBOX_OUT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x9d6dea3c);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_LETTERBOX_OUT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x9d6dea3c, index);
}
inline bool Gen::controller::ICEACTION_LETTERBOX_OUT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x9d6dea3c, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_LETTERBOX_OUT() const {
    ATTRIB_CODEGEN_GETLENGTH(0x9d6dea3c);
}
inline bool Gen::controller::SET_ICEACTION_LETTERBOX_OUT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x9d6dea3c, input, index);
}

inline bool Gen::controller::ICEACTION_PASTE(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xcf996f30);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_PASTE(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xcf996f30, index);
}
inline bool Gen::controller::ICEACTION_PASTE(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xcf996f30, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_PASTE() const {
    ATTRIB_CODEGEN_GETLENGTH(0xcf996f30);
}
inline bool Gen::controller::SET_ICEACTION_PASTE(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xcf996f30, input, index);
}

inline bool Gen::controller::ICEACTION_PLAY(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x5404dd6d);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_PLAY(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x5404dd6d, index);
}
inline bool Gen::controller::ICEACTION_PLAY(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x5404dd6d, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_PLAY() const {
    ATTRIB_CODEGEN_GETLENGTH(0x5404dd6d);
}
inline bool Gen::controller::SET_ICEACTION_PLAY(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x5404dd6d, input, index);
}

inline bool Gen::controller::ICEACTION_RIGHT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x5bd28819);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_RIGHT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x5bd28819, index);
}
inline bool Gen::controller::ICEACTION_RIGHT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x5bd28819, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_RIGHT() const {
    ATTRIB_CODEGEN_GETLENGTH(0x5bd28819);
}
inline bool Gen::controller::SET_ICEACTION_RIGHT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x5bd28819, input, index);
}

inline bool Gen::controller::ICEACTION_SCREENSHOT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x68af00cb);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_SCREENSHOT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x68af00cb, index);
}
inline bool Gen::controller::ICEACTION_SCREENSHOT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x68af00cb, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_SCREENSHOT() const {
    ATTRIB_CODEGEN_GETLENGTH(0x68af00cb);
}
inline bool Gen::controller::SET_ICEACTION_SCREENSHOT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x68af00cb, input, index);
}

inline bool Gen::controller::ICEACTION_SCREENSHOT_STREAM(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x915b5e15);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_SCREENSHOT_STREAM(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x915b5e15, index);
}
inline bool Gen::controller::ICEACTION_SCREENSHOT_STREAM(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x915b5e15, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_SCREENSHOT_STREAM() const {
    ATTRIB_CODEGEN_GETLENGTH(0x915b5e15);
}
inline bool Gen::controller::SET_ICEACTION_SCREENSHOT_STREAM(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x915b5e15, input, index);
}

inline bool Gen::controller::ICEACTION_SELECT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xa3b4828b);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_SELECT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xa3b4828b, index);
}
inline bool Gen::controller::ICEACTION_SELECT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xa3b4828b, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_SELECT() const {
    ATTRIB_CODEGEN_GETLENGTH(0xa3b4828b);
}
inline bool Gen::controller::SET_ICEACTION_SELECT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xa3b4828b, input, index);
}

inline bool Gen::controller::ICEACTION_SHAKE_FRQ_DEC(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x68306931);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_SHAKE_FRQ_DEC(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x68306931, index);
}
inline bool Gen::controller::ICEACTION_SHAKE_FRQ_DEC(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x68306931, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_SHAKE_FRQ_DEC() const {
    ATTRIB_CODEGEN_GETLENGTH(0x68306931);
}
inline bool Gen::controller::SET_ICEACTION_SHAKE_FRQ_DEC(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x68306931, input, index);
}

inline bool Gen::controller::ICEACTION_SHAKE_FRQ_INC(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xd3f858ca);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_SHAKE_FRQ_INC(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xd3f858ca, index);
}
inline bool Gen::controller::ICEACTION_SHAKE_FRQ_INC(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xd3f858ca, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_SHAKE_FRQ_INC() const {
    ATTRIB_CODEGEN_GETLENGTH(0xd3f858ca);
}
inline bool Gen::controller::SET_ICEACTION_SHAKE_FRQ_INC(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xd3f858ca, input, index);
}

inline bool Gen::controller::ICEACTION_SHAKE_MAG_DEC(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x8a9ee6ef);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_SHAKE_MAG_DEC(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x8a9ee6ef, index);
}
inline bool Gen::controller::ICEACTION_SHAKE_MAG_DEC(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x8a9ee6ef, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_SHAKE_MAG_DEC() const {
    ATTRIB_CODEGEN_GETLENGTH(0x8a9ee6ef);
}
inline bool Gen::controller::SET_ICEACTION_SHAKE_MAG_DEC(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x8a9ee6ef, input, index);
}

inline bool Gen::controller::ICEACTION_SHAKE_MAG_INC(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xda3fa888);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_SHAKE_MAG_INC(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xda3fa888, index);
}
inline bool Gen::controller::ICEACTION_SHAKE_MAG_INC(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xda3fa888, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_SHAKE_MAG_INC() const {
    ATTRIB_CODEGEN_GETLENGTH(0xda3fa888);
}
inline bool Gen::controller::SET_ICEACTION_SHAKE_MAG_INC(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xda3fa888, input, index);
}

inline bool Gen::controller::ICEACTION_SHUTTLE_DOWN(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xdc4ccb3c);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_SHUTTLE_DOWN(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xdc4ccb3c, index);
}
inline bool Gen::controller::ICEACTION_SHUTTLE_DOWN(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xdc4ccb3c, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_SHUTTLE_DOWN() const {
    ATTRIB_CODEGEN_GETLENGTH(0xdc4ccb3c);
}
inline bool Gen::controller::SET_ICEACTION_SHUTTLE_DOWN(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xdc4ccb3c, input, index);
}

inline bool Gen::controller::ICEACTION_SHUTTLE_LEFT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x9b870f45);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_SHUTTLE_LEFT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x9b870f45, index);
}
inline bool Gen::controller::ICEACTION_SHUTTLE_LEFT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x9b870f45, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_SHUTTLE_LEFT() const {
    ATTRIB_CODEGEN_GETLENGTH(0x9b870f45);
}
inline bool Gen::controller::SET_ICEACTION_SHUTTLE_LEFT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x9b870f45, input, index);
}

inline bool Gen::controller::ICEACTION_SHUTTLE_RIGHT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x591cf93a);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_SHUTTLE_RIGHT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x591cf93a, index);
}
inline bool Gen::controller::ICEACTION_SHUTTLE_RIGHT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x591cf93a, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_SHUTTLE_RIGHT() const {
    ATTRIB_CODEGEN_GETLENGTH(0x591cf93a);
}
inline bool Gen::controller::SET_ICEACTION_SHUTTLE_RIGHT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x591cf93a, input, index);
}

inline bool Gen::controller::ICEACTION_SHUTTLE_SPEED_DEC(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xf08628de);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_SHUTTLE_SPEED_DEC(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xf08628de, index);
}
inline bool Gen::controller::ICEACTION_SHUTTLE_SPEED_DEC(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xf08628de, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_SHUTTLE_SPEED_DEC() const {
    ATTRIB_CODEGEN_GETLENGTH(0xf08628de);
}
inline bool Gen::controller::SET_ICEACTION_SHUTTLE_SPEED_DEC(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xf08628de, input, index);
}

inline bool Gen::controller::ICEACTION_SHUTTLE_SPEED_INC(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x82fd8328);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_SHUTTLE_SPEED_INC(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x82fd8328, index);
}
inline bool Gen::controller::ICEACTION_SHUTTLE_SPEED_INC(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x82fd8328, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_SHUTTLE_SPEED_INC() const {
    ATTRIB_CODEGEN_GETLENGTH(0x82fd8328);
}
inline bool Gen::controller::SET_ICEACTION_SHUTTLE_SPEED_INC(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x82fd8328, input, index);
}

inline bool Gen::controller::ICEACTION_SHUTTLE_UP(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xdcfe9685);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_SHUTTLE_UP(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xdcfe9685, index);
}
inline bool Gen::controller::ICEACTION_SHUTTLE_UP(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xdcfe9685, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_SHUTTLE_UP() const {
    ATTRIB_CODEGEN_GETLENGTH(0xdcfe9685);
}
inline bool Gen::controller::SET_ICEACTION_SHUTTLE_UP(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xdcfe9685, input, index);
}

inline bool Gen::controller::ICEACTION_SIMSPEED_DEC(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xb5625552);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_SIMSPEED_DEC(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xb5625552, index);
}
inline bool Gen::controller::ICEACTION_SIMSPEED_DEC(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xb5625552, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_SIMSPEED_DEC() const {
    ATTRIB_CODEGEN_GETLENGTH(0xb5625552);
}
inline bool Gen::controller::SET_ICEACTION_SIMSPEED_DEC(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xb5625552, input, index);
}

inline bool Gen::controller::ICEACTION_SIMSPEED_INC(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x25ee8e66);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_SIMSPEED_INC(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x25ee8e66, index);
}
inline bool Gen::controller::ICEACTION_SIMSPEED_INC(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x25ee8e66, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_SIMSPEED_INC() const {
    ATTRIB_CODEGEN_GETLENGTH(0x25ee8e66);
}
inline bool Gen::controller::SET_ICEACTION_SIMSPEED_INC(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x25ee8e66, input, index);
}

inline bool Gen::controller::ICEACTION_UNDO(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xc9aca929);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_UNDO(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xc9aca929, index);
}
inline bool Gen::controller::ICEACTION_UNDO(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xc9aca929, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_UNDO() const {
    ATTRIB_CODEGEN_GETLENGTH(0xc9aca929);
}
inline bool Gen::controller::SET_ICEACTION_UNDO(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xc9aca929, input, index);
}

inline bool Gen::controller::ICEACTION_UP(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x5383adea);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_UP(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x5383adea, index);
}
inline bool Gen::controller::ICEACTION_UP(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x5383adea, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_UP() const {
    ATTRIB_CODEGEN_GETLENGTH(0x5383adea);
}
inline bool Gen::controller::SET_ICEACTION_UP(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x5383adea, input, index);
}

inline bool Gen::controller::ICEACTION_ZOOM_IN(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x46b49ed9);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_ZOOM_IN(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x46b49ed9, index);
}
inline bool Gen::controller::ICEACTION_ZOOM_IN(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x46b49ed9, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_ZOOM_IN() const {
    ATTRIB_CODEGEN_GETLENGTH(0x46b49ed9);
}
inline bool Gen::controller::SET_ICEACTION_ZOOM_IN(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x46b49ed9, input, index);
}

inline bool Gen::controller::ICEACTION_ZOOM_OUT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xf036026a);
}
inline const ControllerDataRecord &Gen::controller::ICEACTION_ZOOM_OUT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xf036026a, index);
}
inline bool Gen::controller::ICEACTION_ZOOM_OUT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xf036026a, result, index);
}
inline unsigned int Gen::controller::Num_ICEACTION_ZOOM_OUT() const {
    ATTRIB_CODEGEN_GETLENGTH(0xf036026a);
}
inline bool Gen::controller::SET_ICEACTION_ZOOM_OUT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xf036026a, input, index);
}

inline bool Gen::controller::Pauseable(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xbe84a0d8);
}
inline const EA::Reflection::Bool &Gen::controller::Pauseable() const {
    ATTRIB_CODEGEN_GETLAYOUT(Pauseable);
}
inline bool Gen::controller::Pauseable(EA::Reflection::Bool &result) const {
    result = Pauseable();
    return true;
}
inline bool Gen::controller::SET_Pauseable(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETLAYOUT(Pauseable, input);
}

inline bool Gen::controller::REACTION_ACTIVATE_EDIT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xaa96159c);
}
inline const ControllerDataRecord &Gen::controller::REACTION_ACTIVATE_EDIT() const {
    ATTRIB_CODEGEN_GETVALUE(ControllerDataRecord, 0xaa96159c);
}
inline bool Gen::controller::REACTION_ACTIVATE_EDIT(ControllerDataRecord &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(ControllerDataRecord, 0xaa96159c, result);
}
inline bool Gen::controller::SET_REACTION_ACTIVATE_EDIT(const ControllerDataRecord &input) {
    ATTRIB_CODEGEN_SETVALUE(ControllerDataRecord, 0xaa96159c, input);
}

inline bool Gen::controller::REACTION_APPLY_TO_NEXT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x06bb1fc4);
}
inline const ControllerDataRecord &Gen::controller::REACTION_APPLY_TO_NEXT() const {
    ATTRIB_CODEGEN_GETVALUE(ControllerDataRecord, 0x06bb1fc4);
}
inline bool Gen::controller::REACTION_APPLY_TO_NEXT(ControllerDataRecord &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(ControllerDataRecord, 0x06bb1fc4, result);
}
inline bool Gen::controller::SET_REACTION_APPLY_TO_NEXT(const ControllerDataRecord &input) {
    ATTRIB_CODEGEN_SETVALUE(ControllerDataRecord, 0x06bb1fc4, input);
}

inline bool Gen::controller::REACTION_AUTOFIT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xc604e4c8);
}
inline const ControllerDataRecord &Gen::controller::REACTION_AUTOFIT() const {
    ATTRIB_CODEGEN_GETVALUE(ControllerDataRecord, 0xc604e4c8);
}
inline bool Gen::controller::REACTION_AUTOFIT(ControllerDataRecord &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(ControllerDataRecord, 0xc604e4c8, result);
}
inline bool Gen::controller::SET_REACTION_AUTOFIT(const ControllerDataRecord &input) {
    ATTRIB_CODEGEN_SETVALUE(ControllerDataRecord, 0xc604e4c8, input);
}

inline bool Gen::controller::REACTION_EDIT_NODE(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x87c357b2);
}
inline const ControllerDataRecord &Gen::controller::REACTION_EDIT_NODE() const {
    ATTRIB_CODEGEN_GETVALUE(ControllerDataRecord, 0x87c357b2);
}
inline bool Gen::controller::REACTION_EDIT_NODE(ControllerDataRecord &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(ControllerDataRecord, 0x87c357b2, result);
}
inline bool Gen::controller::SET_REACTION_EDIT_NODE(const ControllerDataRecord &input) {
    ATTRIB_CODEGEN_SETVALUE(ControllerDataRecord, 0x87c357b2, input);
}

inline bool Gen::controller::REACTION_LOOKDOWN(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x8a90bda5);
}
inline const ControllerDataRecord &Gen::controller::REACTION_LOOKDOWN(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x8a90bda5, index);
}
inline bool Gen::controller::REACTION_LOOKDOWN(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x8a90bda5, result, index);
}
inline unsigned int Gen::controller::Num_REACTION_LOOKDOWN() const {
    ATTRIB_CODEGEN_GETLENGTH(0x8a90bda5);
}
inline bool Gen::controller::SET_REACTION_LOOKDOWN(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x8a90bda5, input, index);
}

inline bool Gen::controller::REACTION_LOOKLEFT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xc1097c28);
}
inline const ControllerDataRecord &Gen::controller::REACTION_LOOKLEFT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xc1097c28, index);
}
inline bool Gen::controller::REACTION_LOOKLEFT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xc1097c28, result, index);
}
inline unsigned int Gen::controller::Num_REACTION_LOOKLEFT() const {
    ATTRIB_CODEGEN_GETLENGTH(0xc1097c28);
}
inline bool Gen::controller::SET_REACTION_LOOKLEFT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xc1097c28, input, index);
}

inline bool Gen::controller::REACTION_LOOKRIGHT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x2e183c63);
}
inline const ControllerDataRecord &Gen::controller::REACTION_LOOKRIGHT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x2e183c63, index);
}
inline bool Gen::controller::REACTION_LOOKRIGHT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x2e183c63, result, index);
}
inline unsigned int Gen::controller::Num_REACTION_LOOKRIGHT() const {
    ATTRIB_CODEGEN_GETLENGTH(0x2e183c63);
}
inline bool Gen::controller::SET_REACTION_LOOKRIGHT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x2e183c63, input, index);
}

inline bool Gen::controller::REACTION_LOOKUP(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xccc3ad00);
}
inline const ControllerDataRecord &Gen::controller::REACTION_LOOKUP(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xccc3ad00, index);
}
inline bool Gen::controller::REACTION_LOOKUP(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xccc3ad00, result, index);
}
inline unsigned int Gen::controller::Num_REACTION_LOOKUP() const {
    ATTRIB_CODEGEN_GETLENGTH(0xccc3ad00);
}
inline bool Gen::controller::SET_REACTION_LOOKUP(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xccc3ad00, input, index);
}

inline bool Gen::controller::REACTION_MOVE_BACK(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x521d0a7c);
}
inline const ControllerDataRecord &Gen::controller::REACTION_MOVE_BACK(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x521d0a7c, index);
}
inline bool Gen::controller::REACTION_MOVE_BACK(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x521d0a7c, result, index);
}
inline unsigned int Gen::controller::Num_REACTION_MOVE_BACK() const {
    ATTRIB_CODEGEN_GETLENGTH(0x521d0a7c);
}
inline bool Gen::controller::SET_REACTION_MOVE_BACK(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x521d0a7c, input, index);
}

inline bool Gen::controller::REACTION_MOVE_DOWN(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x16dab024);
}
inline const ControllerDataRecord &Gen::controller::REACTION_MOVE_DOWN() const {
    ATTRIB_CODEGEN_GETVALUE(ControllerDataRecord, 0x16dab024);
}
inline bool Gen::controller::REACTION_MOVE_DOWN(ControllerDataRecord &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(ControllerDataRecord, 0x16dab024, result);
}
inline bool Gen::controller::SET_REACTION_MOVE_DOWN(const ControllerDataRecord &input) {
    ATTRIB_CODEGEN_SETVALUE(ControllerDataRecord, 0x16dab024, input);
}

inline bool Gen::controller::REACTION_MOVE_FORWARD(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x395f1ceb);
}
inline const ControllerDataRecord &Gen::controller::REACTION_MOVE_FORWARD(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0x395f1ceb, index);
}
inline bool Gen::controller::REACTION_MOVE_FORWARD(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0x395f1ceb, result, index);
}
inline unsigned int Gen::controller::Num_REACTION_MOVE_FORWARD() const {
    ATTRIB_CODEGEN_GETLENGTH(0x395f1ceb);
}
inline bool Gen::controller::SET_REACTION_MOVE_FORWARD(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0x395f1ceb, input, index);
}

inline bool Gen::controller::REACTION_MOVE_LEFT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xa914965d);
}
inline const ControllerDataRecord &Gen::controller::REACTION_MOVE_LEFT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xa914965d, index);
}
inline bool Gen::controller::REACTION_MOVE_LEFT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xa914965d, result, index);
}
inline unsigned int Gen::controller::Num_REACTION_MOVE_LEFT() const {
    ATTRIB_CODEGEN_GETLENGTH(0xa914965d);
}
inline bool Gen::controller::SET_REACTION_MOVE_LEFT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xa914965d, input, index);
}

inline bool Gen::controller::REACTION_MOVE_RIGHT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xd19432c0);
}
inline const ControllerDataRecord &Gen::controller::REACTION_MOVE_RIGHT(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xd19432c0, index);
}
inline bool Gen::controller::REACTION_MOVE_RIGHT(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xd19432c0, result, index);
}
inline unsigned int Gen::controller::Num_REACTION_MOVE_RIGHT() const {
    ATTRIB_CODEGEN_GETLENGTH(0xd19432c0);
}
inline bool Gen::controller::SET_REACTION_MOVE_RIGHT(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xd19432c0, input, index);
}

inline bool Gen::controller::REACTION_MOVE_UP(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xbdcd877e);
}
inline const ControllerDataRecord &Gen::controller::REACTION_MOVE_UP() const {
    ATTRIB_CODEGEN_GETVALUE(ControllerDataRecord, 0xbdcd877e);
}
inline bool Gen::controller::REACTION_MOVE_UP(ControllerDataRecord &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(ControllerDataRecord, 0xbdcd877e, result);
}
inline bool Gen::controller::SET_REACTION_MOVE_UP(const ControllerDataRecord &input) {
    ATTRIB_CODEGEN_SETVALUE(ControllerDataRecord, 0xbdcd877e, input);
}

inline bool Gen::controller::REACTION_NEXTLANE(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xedd1f872);
}
inline const ControllerDataRecord &Gen::controller::REACTION_NEXTLANE() const {
    ATTRIB_CODEGEN_GETVALUE(ControllerDataRecord, 0xedd1f872);
}
inline bool Gen::controller::REACTION_NEXTLANE(ControllerDataRecord &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(ControllerDataRecord, 0xedd1f872, result);
}
inline bool Gen::controller::SET_REACTION_NEXTLANE(const ControllerDataRecord &input) {
    ATTRIB_CODEGEN_SETVALUE(ControllerDataRecord, 0xedd1f872, input);
}

inline bool Gen::controller::REACTION_PICK(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x67490952);
}
inline const ControllerDataRecord &Gen::controller::REACTION_PICK() const {
    ATTRIB_CODEGEN_GETVALUE(ControllerDataRecord, 0x67490952);
}
inline bool Gen::controller::REACTION_PICK(ControllerDataRecord &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(ControllerDataRecord, 0x67490952, result);
}
inline bool Gen::controller::SET_REACTION_PICK(const ControllerDataRecord &input) {
    ATTRIB_CODEGEN_SETVALUE(ControllerDataRecord, 0x67490952, input);
}

inline bool Gen::controller::REACTION_PREVIOUSLANE(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x64bf8411);
}
inline const ControllerDataRecord &Gen::controller::REACTION_PREVIOUSLANE() const {
    ATTRIB_CODEGEN_GETVALUE(ControllerDataRecord, 0x64bf8411);
}
inline bool Gen::controller::REACTION_PREVIOUSLANE(ControllerDataRecord &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(ControllerDataRecord, 0x64bf8411, result);
}
inline bool Gen::controller::SET_REACTION_PREVIOUSLANE(const ControllerDataRecord &input) {
    ATTRIB_CODEGEN_SETVALUE(ControllerDataRecord, 0x64bf8411, input);
}

inline bool Gen::controller::REACTION_SELECTNEXT(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0x291bda5d);
}
inline const ControllerDataRecord &Gen::controller::REACTION_SELECTNEXT() const {
    ATTRIB_CODEGEN_GETVALUE(ControllerDataRecord, 0x291bda5d);
}
inline bool Gen::controller::REACTION_SELECTNEXT(ControllerDataRecord &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(ControllerDataRecord, 0x291bda5d, result);
}
inline bool Gen::controller::SET_REACTION_SELECTNEXT(const ControllerDataRecord &input) {
    ATTRIB_CODEGEN_SETVALUE(ControllerDataRecord, 0x291bda5d, input);
}

inline bool Gen::controller::REACTION_TURBO(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xda7d1177);
}
inline const ControllerDataRecord &Gen::controller::REACTION_TURBO() const {
    ATTRIB_CODEGEN_GETVALUE(ControllerDataRecord, 0xda7d1177);
}
inline bool Gen::controller::REACTION_TURBO(ControllerDataRecord &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(ControllerDataRecord, 0xda7d1177, result);
}
inline bool Gen::controller::SET_REACTION_TURBO(const ControllerDataRecord &input) {
    ATTRIB_CODEGEN_SETVALUE(ControllerDataRecord, 0xda7d1177, input);
}

inline bool Gen::controller::VOIPACTION_PUSHTOTALK(TAttrib<ControllerDataRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(ControllerDataRecord, 0xe5c50a17);
}
inline const ControllerDataRecord &Gen::controller::VOIPACTION_PUSHTOTALK(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(ControllerDataRecord, 0xe5c50a17, index);
}
inline bool Gen::controller::VOIPACTION_PUSHTOTALK(ControllerDataRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(ControllerDataRecord, 0xe5c50a17, result, index);
}
inline unsigned int Gen::controller::Num_VOIPACTION_PUSHTOTALK() const {
    ATTRIB_CODEGEN_GETLENGTH(0xe5c50a17);
}
inline bool Gen::controller::SET_VOIPACTION_PUSHTOTALK(const ControllerDataRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(ControllerDataRecord, 0xe5c50a17, input, index);
}

}; // namespace Attrib

#endif
