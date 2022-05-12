#ifndef AUDIOMANAGER_HPP
#define AUDIOMANAGER_HPP

#include <fmod.hpp>
#include <vector>

class AudioManager
{
public:
    AudioManager();
    ~AudioManager();

    void init();
    void terminate();

    void update(double dt=1/60.);

    void testInit();
private:
    // TEMP
    FMOD::System *m_audioSystem;
};

#endif // AUDIOMANAGER_HPP
