static bool gInGameMoviePlaying;

struct InGameAnyMovieScreen {
    static bool IsPlaying();
};

bool InGameAnyMovieScreen::IsPlaying() {
    return gInGameMoviePlaying;
}