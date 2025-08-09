
#ifndef VIDEODECODER_H
#define VIDEODECODER_H

#include "include/utility.hpp"
#include "include/decoder.hpp"

struct videoData {

    int width, height;
    std::vector<uint8_t> rgb_pixels;
    int64_t pts;

};

struct audioData {

    std::vector<int16_t> pcm_data;
    int64_t pts;

};

struct decoderContext {

    AVCodecContext* codec_context;
    AVCodecParameters* codec_params;
    const AVCodec* codec;

};

class videoDecoder {

    public:

        videoDecoder(const std::string& video_file_path, const bool& high_performance);
        ~videoDecoder();

        void start_decoder_threads();

        void seek_video(int64_t& seek_target_seconds);

        double get_video_fps() const;
        uint64_t get_video_length() const;

        int64_t get_current_audio_pts_val() const;
        int64_t get_current_video_pts_val() const;

        bool has_video_data();
        bool has_audio();

        videoData get_next_video_data();
        std::vector<int16_t> get_next_audio_pcm_data();

    private:

        template<typename Func>
        void packet_decode_loop(Func decode_func, std::mutex& mtx, std::condition_variable& pkt_con, std::condition_variable& read_con, std::queue<AVPacket*>& packet_queue);
        template<typename Func, typename T>
        void decode_t(Func prepare_data_func, AVCodecContext* cdc_ctxt, std::queue<T>& queue, AVFrame* frame, std::mutex& mtx, std::condition_variable& cond, AVPacket* pkt);

        void seek_t(int64_t& target, AVCodecContext* cdc_ctx, const int& stream, const int& flag);

        void decode_video(AVPacket* packet);
        void decode_audio(AVPacket* packet);

        void video_decode_loop();
        void audio_decode_loop();
        void read_frame_loop();

        uint64_t video_duration;

        uint64_t current_audio_pts;
        uint64_t current_video_pts;

        Thread read_frame_thread;
        Thread video_thread;
        Thread audio_thread;

        struct SwsContext* sws_context;
        struct SwrContext* swr_context;

        AVFormatContext* format_context;
        decoderContext video_context;
        decoderContext audio_context;

        AVFrame* frame_rgb;
        AVFrame* video_frame;
        AVFrame* audio_frame;

        int video_stream;
        int audio_stream;
        int num_bytes;

        void flush_queues();
    
        std::queue<AVPacket*> video_packet_queue;
        std::queue<AVPacket*> audio_packet_queue;
        
        std::queue<videoData> video_frame_queue;
        std::queue<audioData> audio_queue;

};



#endif