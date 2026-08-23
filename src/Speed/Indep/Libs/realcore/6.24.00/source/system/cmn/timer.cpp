extern volatile int ticks;

int TIMER_gettick() {
    return ticks;
}
