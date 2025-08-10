
#ifndef SUBTITLEDECODER_H
#define SUBTITLEDECODER_H

#include "include/decoder.hpp"
#include "include/utility.hpp"
#include "include/texture_atlas.hpp"

class subtitleDecoder {

    public:

        subtitleDecoder(const std::string& srt_file_path);
        ~subtitleDecoder();
        
        Subtitle get_next_subtitle();
        void start_decoder_thread();

        bool has_subtitle() const;

        // If seek, since SRT files are not that big we just find the correct timestamp and seek to that location in the file. 
        // This works because when the decoder thread is started it seeks to the start_position variable, which is set to file beginning in the constructor and when first ran and whenever the 
        // seek subtitle function is ran at the end seek_location will be set and when the decoder loop is restarted it will seek to the correct location. 
        void seek_subtitle_location(const int64_t& seek_to);

        // When subtitle delay is altered the queued delays need to reflect that 
        void adjust_queued_subtitle_delays(const int& delay);

    private:

        void decode_loop();

        Thread decoder_thread;
        std::vector<Subtitle> subtitle_queue;

        std::ifstream file;

        std::streampos start_position;

};


#endif