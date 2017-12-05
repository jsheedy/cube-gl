
// struct MetronomeEvent;

// class OSCServer(int);


#include <chrono>
#include <iostream>
#include <vector>

// OSC stuff
#include <atomic>
#include <lo/lo.h>
#include <lo/lo_cpp.h>

#define PORT 37341
#define CHANNELS 16;

struct MetronomeEvent {
    int bpm;
    float t;
    bool handled;
    MetronomeEvent(int b) {
        bpm = b;
        long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        handled = false;
    }
};

class OSCServer {

    private:
        lo::ServerThread st;

    public:

    std::vector<int> metronomeQueue;
    // std::vector<float>[16] envQueue;
    std::vector<float> midiNoteQueue[16];

    OSCServer(int port): st(port) {};

    void start() {

        // OSC megahack
        if (!st.is_valid()) {
            std::cout << "couldn't start OSC server" << std::endl;
            return ;
        }

        // set up lambda callbacks
        // st.set_callbacks([&st]() { printf("OSC THREAD INIT: %p\n", &st);},
        //     []() {printf("OSC THREAD CLEANUP\n");});

        std::cout << "URL: " << st.url() << std::endl;

        // number of msgs received. use atomic
        std::atomic<int> received(0);

        st.add_method("/midi/note", "iii",
            [&received, this](lo_arg **argv, int)
            {
                int note = argv[0]->i;
                int velocity = argv[1]->i;
                int channel = argv[2]->i;

                if (argv[2]->i == 1 && argv[0]->i == 36) {
                    this->midiNoteQueue[channel].push_back(velocity);
                }
                // std::cout << "/midi/note (" << (++received) << "): "
                // << std::endl;
                });

        st.add_method("/metronome", "ii",
            [&received, this](lo_arg **argv, int)
            {
                int bpm = argv[0]->i;
                std::cout << "/metronome (" << (++received) << "): "
                << " bpm: " << bpm
                << " beat: " << argv[1]->i
                << std::endl;
                this->metronomeQueue.push_back(bpm);
            });

        // st.add_method("/audio/envelope", "ff",
        //     [&received, &this.oscEnvQueue](lo_arg **argv, int)
        //     {
        //         float val = argv[0]->f;
        //         int channel = argv[0]->i;
        //         // std::cout << "/metronome (" << (++received) << "): "
        //         // << " bpm: " << bpm
        //         // << " beat: " << argv[1]->i
        //         // << std::endl;
        //         oscEnvQueue.push_back(val);
        //     });

        st.start();
        // OSC megahack
    }
};

