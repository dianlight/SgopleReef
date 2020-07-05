#pragma once

class EvoStartable {
    public:
        virtual ~EvoStartable() {}
        virtual bool start() = 0;
        bool isRunning(){ return _running;};
        bool _running = false;
};

class EvoStopable: public EvoStartable {
    public:
        virtual ~EvoStopable() {}
        virtual bool stop() = 0;
};