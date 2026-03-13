#include "Speed/Indep/Src/Frontend/HUD/FeGenericMessage.hpp"

GenericMessage::GenericMessage(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0) //
    , IGenericMessage(pOutter)
{
}

void GenericMessage::Update(IPlayer *player) {
}

GenericMessage_Priority GenericMessage::GetCurrentGenericMessagePriority() {
    return mPriority;
}
