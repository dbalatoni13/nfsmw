// ITimer: acceso al tick del sistema para la libreria de input.

extern unsigned int TIMER_gettick();

namespace RealInput {

class ITimer {
  public:
    unsigned int GetTime();
};

} // namespace RealInput

unsigned int RealInput::ITimer::GetTime() {
    return TIMER_gettick();
}
