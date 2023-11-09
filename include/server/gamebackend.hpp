#ifndef GAMEBACKEND_HPP
#define GAMEBACKEND_HPP

#include <atomic>

class GameBackend
{
public:
    static GameBackend* create();

    GameBackend();
    ~GameBackend();

    void init();
    void update(double dt);
    void run();

    void stop();

    static GameBackend *corePtr;
private:
    std::atomic_flag m_doUpdate;
    double m_deltaTime;
    std::atomic<bool> m_shouldRun;
};

#endif // GAMEBACKEND_HPP
