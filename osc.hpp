
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
#define CHANNELS 16

struct MetronomeEvent {
    int bpm;
    int beat;
    float t;
    bool handled;
    MetronomeEvent(int _bpm, int _beat) {
        bpm = _bpm;
        beat = _beat;
        long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        handled = false;
    }
};

struct MidiNoteEvent {
    int note;
    int velocity;
    bool handled;

    MidiNoteEvent(int n, int v) {
        note = n;
        velocity = v;
        long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        handled = false;
    }
};

struct EnvelopeEvent {
    float value;
    bool handled;

    EnvelopeEvent(float v) {
        value = v;
        long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        handled = false;
    }
};

class OSCServer {

    private:
        lo::ServerThread st;

    public:

    std::vector<MetronomeEvent> metronomeQueue;
    std::vector<MidiNoteEvent> midiNoteQueue[CHANNELS];
    std::vector<EnvelopeEvent> envelopeQueue[CHANNELS];

    OSCServer(int port): st(port) {};

    void start() {

        if (!st.is_valid()) {
            std::cout << "couldn't start OSC server" << std::endl;
            return ;
        }

        // set up lambda callbacks
        st.set_callbacks([this]() { printf("OSC THREAD INIT: %p\n", this);},
            []() {printf("OSC THREAD CLEANUP\n");});

        std::cout << "OSC URL: " << st.url() << std::endl;

        // number of msgs received
        std::atomic<unsigned int> received(0);

        st.add_method("/midi/note", "iii",
            [&received, this](lo_arg **argv, int)
            {
                int note = argv[0]->i;
                int velocity = argv[1]->i;
                int channel = argv[2]->i;

                MidiNoteEvent event(note, velocity);
                this->midiNoteQueue[channel].push_back(event);

                // std::cout << "/midi/note (" << (++received) << "): "
                // << " note: " << note
                // << " velocity: " << velocity
                // << " channel: " << channel
                // << std::endl;

                });

        st.add_method("/metronome", "ii",
            [&received, this](lo_arg **argv, int)
            {
                int bpm = argv[0]->i;
                int beat = argv[1]->i;

                // std::cout << "/metronome (" << (++received) << "): "
                // << " bpm: " << bpm
                // << " beat: " << beat
                // << std::endl;

                MetronomeEvent event(bpm, beat);
                this->metronomeQueue.push_back(event);

            });

        st.add_method("/audio/envelope", "fi",
            [&received, this](lo_arg **argv, int)
            {
                float val = argv[0]->f;
                int channel = argv[1]->i;

                EnvelopeEvent event(val);
                envelopeQueue[channel].push_back(event);
            });

        st.start();
    }
};

