
#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

extern "C" {

    #include <al.h>
    #include <alc.h>

}

#include <vector>
#include <queue>

#include "include/utility.hpp"

class audioPlayer {

    public:

        audioPlayer();
        ~audioPlayer();

        void startup_thread();
        bool is_queue_full();
        void push_to_queue(const std::vector<int16_t>& pcm_data);
        void clear_queue_and_restart();

    private:

        std::vector<int16_t> pcm_data_accumulator;

        // if delete previous is set to true then the bufs and source will be cleared and deleted before re-initializing them 
        void init_buffers_and_source(bool delete_previous);
        void delete_buffers_and_source();
        ALuint bufs[3];
        ALuint source[1];

        void play_audio_loop();

        Thread audio_player_thread;

        std::queue<std::vector<int16_t>> pcm_data_queue;

        ALCdevice* alc_device;
        ALCcontext* alc_context;
    
};


#endif