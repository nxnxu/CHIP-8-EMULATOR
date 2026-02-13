#ifndef _SOUND_H
#define _SOUND_H


/*
    source:
        https://stackoverflow.com/questions/10110905/simple-sound-wave-generator-with-sdl-in-c
*/


#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <queue>
#include <cmath>

struct BeepObject
{
    double freq;
    int samplesLeft;
};

class Beeper
{
private:
    double v;
    std::queue<BeepObject> beeps;
public:
    Beeper();
    ~Beeper();
    void beep(double freq, int duration);
    void generateSamples(Sint16 *stream, int length);
    void wait();
};

#endif