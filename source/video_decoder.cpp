
#include "include/video_decoder.hpp"

videoDecoder::~videoDecoder() {

    read_frame_thread.condition.notify_all();
    video_thread.condition.notify_all();
    audio_thread.condition.notify_all();
    GLOBAL_STATES::VIDEO_PAUSED_CONDITION.notify_all();

    if (audio_thread.thread.joinable()) {

        audio_thread.thread.join();

    }

    if (video_thread.thread.joinable()) {

        video_thread.thread.join(); 

    }

    if (read_frame_thread.thread.joinable()) {

        read_frame_thread.thread.join();

    }

    flush_queues();

    if (sws_context) {

        sws_freeContext(sws_context);

    }
    if (swr_context) {

        swr_close(swr_context);

    }
    
    av_frame_free(&frame_rgb);
    av_frame_free(&video_frame);
    av_frame_free(&audio_frame);

    avformat_close_input(&format_context);

}

videoDecoder::videoDecoder(const std::string& video_file_path, const int& threads) :

format_context { nullptr },
video_frame { nullptr },
frame_rgb { nullptr },
audio_frame { nullptr },

sws_context { nullptr },
swr_context { nullptr },

num_bytes { 0 },
video_duration { 0 },
current_audio_pts { 0 },
current_video_pts { 0 },

video_stream { -1 },
audio_stream { -1 }


{

    // in case of exception
    std::string error_str = "\nEnsure that the video you provided is a valid video file which FFmpeg can decode."
    "\nEnsure that the argument for the video file paths do not look like --path or -path."
    "\nTry running the program with './MovieTranslatorApp filepath1 filepath2 printinfo'"
    "\nThis will print the file paths you pass in."
    "\nIf this looks correct and the mp4 file is valid, then try reinstalling FFmpeg and re-run the program.";

    AVCodecContext* video_codec_context = nullptr;
    AVCodecContext* audio_codec_context = nullptr;
    AVCodecParameters* video_codec_params = nullptr;
    AVCodecParameters* audio_codec_params = nullptr;
    const AVCodec* video_codec = nullptr;
    const AVCodec* audio_codec = nullptr;

    if (avformat_open_input(&format_context, video_file_path.c_str(), nullptr, nullptr) != 0) {

        std::cerr << "\nERROR: Failed to open the video file path." << error_str;
        throw std::runtime_error("Failed to open video file");

    }

    if (avformat_find_stream_info(format_context, nullptr) < 0) {

        std::cerr << "\nERROR: Failed to find stream info for video file." << error_str;
        throw std::runtime_error("Failed to find video stream info");

    }

    video_duration = format_context->duration;
    video_duration = (video_duration / AV_TIME_BASE) * 1000;

    for (unsigned int i = 0; i < format_context->nb_streams; i++) {

        if (format_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO && video_stream < 0) {

            video_stream = i;

        }
        if (format_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO && audio_stream < 0) {

            audio_stream = i;

        }

    }

    if (video_stream == -1) {

        std::cerr << "\nERROR: Failed to find the video stream for the video file." << error_str;
        throw std::runtime_error("Failed to find video stream");

    }

    if (audio_stream == -1) {

        std::cerr << "\nERROR: Failed to find audio stream for the video file." << error_str;
        throw std::runtime_error("Failed to find audio stream");

    }

    /*  VIDEO CODEC INITIALIZATION  */

    video_codec_params = format_context->streams[video_stream]->codecpar;
    video_codec = avcodec_find_decoder(video_codec_params->codec_id);

    if (!video_codec) {
    
        std::cerr << "\nERROR: Failed to find video codec." << error_str;
        throw std::runtime_error("Failed to find video codec");

    }

    video_codec_context = avcodec_alloc_context3(video_codec);

    if (threads > 1) {

        video_codec_context->thread_count = threads;
        video_codec_context->thread_type = FF_THREAD_FRAME;

    }

    if (avcodec_parameters_to_context(video_codec_context, video_codec_params) < 0) {

        std::cerr << "\nERROR: Failed to copy video codec params to context." << error_str;
        throw std::runtime_error("Failed to copy video codec params to context");

    }

    if (avcodec_open2(video_codec_context, video_codec, nullptr) != 0) {

        std::cerr << "\nERROR: Failed to open video codec." << error_str;
        throw std::runtime_error("Failed to open video codec");

    }

    /*  AUDIO CODEC INITIALIZATION  */

    audio_codec_params = format_context->streams[audio_stream]->codecpar;
    audio_codec = avcodec_find_decoder(audio_codec_params->codec_id);

    if (!audio_codec) {

        std::cerr << "\nERROR: Failed to find audio codec." << error_str;
        throw std::runtime_error("Failed to find audio codec");

    }

    audio_codec_context = avcodec_alloc_context3(audio_codec);

    if (avcodec_parameters_to_context(audio_codec_context, audio_codec_params)) {

        std::cerr << "\nERROR: Failed to copy audio codec params to context." << error_str;
        throw std::runtime_error("Failed to copy audio codec params to context");

    }

    if (avcodec_open2(audio_codec_context, audio_codec, nullptr) != 0) {

        std::cerr << "\nERROR: Failed to open audio codec." << error_str;
        throw std::runtime_error("Failed to open audio codec");

    }

    video_context.codec_context = video_codec_context;
    video_context.codec_params = video_codec_params;
    video_context.codec = video_codec;

    audio_context.codec_context = audio_codec_context;
    audio_context.codec_params = audio_codec_params;
    audio_context.codec = audio_codec;

    audio_frame = av_frame_alloc();
    video_frame = av_frame_alloc();
    frame_rgb = av_frame_alloc();

    if (frame_rgb == NULL || video_frame == NULL || audio_frame == NULL) {

        std::cerr << "\nFailed to allocate video frames." << error_str;
        throw std::runtime_error("Failed to allocate frames");

    }

    sws_context = sws_getContext(

        video_context.codec_context->width,
        video_context.codec_context->height,
        video_context.codec_context->pix_fmt,
        video_context.codec_context->width,
        video_context.codec_context->height,
        AV_PIX_FMT_RGB24,
        SWS_BILINEAR,
        nullptr, nullptr, nullptr

    );

    AVChannelLayout layout;

    av_channel_layout_default(&layout, 2);

    int ret = swr_alloc_set_opts2(

        &swr_context,
        &layout,
        AV_SAMPLE_FMT_S16,
        CONSTANTS::AUDIO::SAMPLE_RATE,
        &audio_context.codec_context->ch_layout,
        audio_context.codec_context->sample_fmt,
        audio_context.codec_context->sample_rate,
        0, 
        nullptr

    );

    if (!swr_context || swr_init(swr_context) < 0 || ret < 0) {

        av_channel_layout_uninit(&layout);

        std::cerr << "\nERROR: Failed to init SWR context." << error_str;
        throw std::runtime_error("Failed to initialize swr context");

    }

    av_channel_layout_uninit(&layout);

    num_bytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, video_context.codec_context->width, video_context.codec_context->height, 1);
    uint8_t* buffer = (uint8_t*)av_malloc(num_bytes * sizeof(uint8_t));

    av_image_fill_arrays(frame_rgb->data, frame_rgb->linesize, buffer, AV_PIX_FMT_RGB24, video_context.codec_context->width, video_context.codec_context->height, 1);

}

template<typename Func>
void videoDecoder::packet_decode_loop(Func decode_func, std::mutex& mtx, std::condition_variable& pkt_con, std::condition_variable& read_con, std::queue<AVPacket*>& packet_queue) {

    {

        std::unique_lock<std::mutex> lock(mtx);
        pkt_con.wait(lock, [&] { return !packet_queue.empty() || GLOBAL_STATES::VIDEO_SEEK_REQUESTED; });

    }

    AVPacket* packet = decoder::get_next_t(packet_queue, read_con);

    decode_func(packet);
    av_packet_unref(packet);
    av_packet_free(&packet);

}

template<typename Func, typename T>
void videoDecoder::decode_t(Func prepare_data_func, AVCodecContext* cdc_ctxt, std::queue<T>& queue, AVFrame* frame, std::mutex& mtx, std::condition_variable& cond, AVPacket* pkt) {

    {

        std::unique_lock<std::mutex> lock(mtx);
        cond.wait(lock, [&] { return queue.size() < decoder::MAX_DATA_QUEUE_LIMIT || GLOBAL_STATES::VIDEO_SEEK_REQUESTED; });

    }

    int ret = avcodec_send_packet(cdc_ctxt, pkt);

    while (ret >= 0) {

        ret = avcodec_receive_frame(cdc_ctxt, frame);

        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {

            return;

        }

        prepare_data_func(pkt);

    }

}

void videoDecoder::audio_decode_loop() {

    while (!GLOBAL_STATES::VIDEO_SEEK_REQUESTED) {

        packet_decode_loop([this](AVPacket* pkt) { decode_audio(pkt); }, audio_thread.mutex, audio_thread.condition, read_frame_thread.condition, audio_packet_queue);

    }

}

void videoDecoder::video_decode_loop() {

    while (!GLOBAL_STATES::VIDEO_SEEK_REQUESTED) {

        packet_decode_loop([this](AVPacket* pkt) { decode_video(pkt); }, video_thread.mutex, video_thread.condition, read_frame_thread.condition, video_packet_queue);

    }

}

void videoDecoder::decode_audio(AVPacket* packet) {

    decode_t(
        [this](AVPacket* pkt) {

            uint8_t* output;
            int out_samples = static_cast<int>(av_rescale_rnd(
                swr_get_delay(swr_context, audio_context.codec_context->sample_rate) + audio_frame->nb_samples, 
                CONSTANTS::AUDIO::SAMPLE_RATE, 
                audio_context.codec_context->sample_rate, 
                AV_ROUND_UP
            ));

            av_samples_alloc(&output, nullptr, 2, out_samples, AV_SAMPLE_FMT_S16, 0);

            out_samples = swr_convert(swr_context, &output, out_samples, (const uint8_t**)audio_frame->extended_data, audio_frame->nb_samples);

            int buf_size = av_samples_get_buffer_size(nullptr, 2, out_samples, AV_SAMPLE_FMT_S16, 1);

            std::vector<int16_t> pcm_data(buf_size / sizeof(int16_t));
            std::memcpy(pcm_data.data(), output, buf_size);

            uint64_t audio_pts = AV_NOPTS_VALUE;

            if (pkt->pts != audio_pts) {

                audio_pts = av_rescale_q(pkt->pts, format_context->streams[audio_stream]->time_base, { 1, 1000 });

            }

            if (audio_pts == AV_NOPTS_VALUE) {

                audio_pts = current_audio_pts++;

            }

            if (buf_size < 0) {

                av_freep(&output);

                return;

            }

            audioData data;

            data.pcm_data = pcm_data;
            data.pts = audio_pts;

            audio_thread.mutex.lock();
            audio_queue.push(data);
            audio_thread.mutex.unlock();
        
            av_freep(&output);
            av_frame_unref(audio_frame);
            
        },

        audio_context.codec_context,
        audio_queue,
        audio_frame,
        audio_thread.mutex,
        audio_thread.condition,
        packet

    );

}

void videoDecoder::decode_video(AVPacket* packet) {

    decode_t(
        [this](AVPacket* pkt) {

            sws_scale(sws_context, video_frame->data, video_frame->linesize, 0, video_context.codec_context->height, frame_rgb->data, frame_rgb->linesize);

            int buf_size = av_image_get_buffer_size(AV_PIX_FMT_RGB24, video_context.codec_context->width, video_context.codec_context->height, 1);
            uint64_t video_pts = av_rescale_q(video_frame->pts, format_context->streams[video_stream]->time_base, { 1, 1000 });

            videoData data;

            data.rgb_pixels.resize(buf_size);
            std::memcpy(data.rgb_pixels.data(), frame_rgb->data[0], buf_size);

            data.width = video_context.codec_context->width;
            data.height = video_context.codec_context->height;
            data.pts = video_pts;

            video_thread.mutex.lock();
            video_frame_queue.push(data);
            video_thread.mutex.unlock();

            av_frame_unref(video_frame);

        },

        video_context.codec_context,
        video_frame_queue,
        video_frame,
        video_thread.mutex,
        video_thread.condition,
        packet
        
    );

}

void videoDecoder::read_frame_loop() {

    AVPacket packet;

    while (!GLOBAL_STATES::VIDEO_SEEK_REQUESTED && av_read_frame(format_context, &packet) >= 0) {

        if (GLOBAL_STATES::VIDEO_PAUSED) {

            std::unique_lock<std::mutex> lock(GLOBAL_STATES::VIDEO_PAUSED_MUTEX);
            GLOBAL_STATES::VIDEO_PAUSED_CONDITION.wait(lock, [&] { return !GLOBAL_STATES::VIDEO_PAUSED; });

        }

        {

            std::unique_lock<std::mutex> lock(read_frame_thread.mutex);

            // if video packet or audio packet queue full or video seek requested wait. 
            read_frame_thread.condition.wait
            (
                lock, 
                [&] 
                { 
                    return video_packet_queue.size() < decoder::MAX_PACKET_QUEUE_LIMIT ||
                           audio_packet_queue.size() < decoder::MAX_PACKET_QUEUE_LIMIT ||
                           GLOBAL_STATES::VIDEO_SEEK_REQUESTED; 
                }
            );

        }

        if (packet.stream_index == video_stream && !GLOBAL_STATES::VIDEO_SEEK_REQUESTED) {

            AVPacket* new_packet = av_packet_alloc();
            av_packet_ref(new_packet, &packet);

            video_packet_queue.push(new_packet);

            video_thread.condition.notify_one();

        }

        else if (packet.stream_index == audio_stream && !GLOBAL_STATES::VIDEO_SEEK_REQUESTED) {

            AVPacket* new_packet = av_packet_alloc();
            av_packet_ref(new_packet, &packet);

            audio_packet_queue.push(new_packet);

            audio_thread.condition.notify_one();

        }

        else { av_packet_unref(&packet); } 

        av_packet_unref(&packet);

    }

}

void videoDecoder::seek_t(int64_t& target, AVCodecContext* cdc_ctx, const int& stream, const int& flag) {

    if (av_seek_frame(format_context, stream, target, flag | AVSEEK_FLAG_FRAME) < 0) {

        throw std::runtime_error("error seeking video");

    } else {

        avcodec_flush_buffers(cdc_ctx);

    }

}

void videoDecoder::seek_video(int64_t& seek_target_seconds) {

    int64_t target = seek_target_seconds * 1000;

    current_audio_pts = target;
    current_video_pts = target;

    read_frame_thread.condition.notify_all();
    video_thread.condition.notify_all();
    audio_thread.condition.notify_all();
    GLOBAL_STATES::VIDEO_PAUSED_CONDITION.notify_all();

    if (audio_thread.thread.joinable()) {

        audio_thread.thread.join();

    }

    if (video_thread.thread.joinable()) {

        video_thread.thread.join(); 

    }

    if (read_frame_thread.thread.joinable()) {

        read_frame_thread.thread.join();

    }

    flush_queues();

    int64_t scaled_timestamp = 0;
    int flag = 0; 

    scaled_timestamp = av_rescale_q(seek_target_seconds * AV_TIME_BASE, AV_TIME_BASE_Q, format_context->streams[video_stream]->time_base);
    flag = (scaled_timestamp < 0) ? AVSEEK_FLAG_BACKWARD : 0;
    seek_t(scaled_timestamp, video_context.codec_context, video_stream, flag);

    scaled_timestamp = av_rescale_q(seek_target_seconds * AV_TIME_BASE, AV_TIME_BASE_Q, format_context->streams[audio_stream]->time_base);
    flag = (scaled_timestamp < 0) ? AVSEEK_FLAG_BACKWARD : 0;
    seek_t(scaled_timestamp, audio_context.codec_context, audio_stream, flag);

}

void videoDecoder::flush_queues() {

    AVPacket* pkt = nullptr;
    
    read_frame_thread.mutex.lock();

    if (video_packet_queue.size() > 0) {

        while(!video_packet_queue.empty()) {

            pkt = video_packet_queue.front();
            video_packet_queue.pop();

            av_packet_free(&pkt);

        }

    }

    if (audio_packet_queue.size() > 0) {

        while(!audio_packet_queue.empty()) {

            pkt = audio_packet_queue.front();
            audio_packet_queue.pop();

            av_packet_free(&pkt);

        }       

    }

    if (video_frame_queue.size() > 0) {

        video_thread.mutex.lock();

        while(!video_frame_queue.empty()) video_frame_queue.pop();

        video_thread.mutex.unlock();

    }

    if (audio_queue.size() > 0) {

        audio_thread.mutex.lock();

        while(!audio_queue.empty()) audio_queue.pop();

        audio_thread.mutex.unlock();

    }
    
    read_frame_thread.mutex.unlock();
    
}

void videoDecoder::start_decoder_threads() {

    read_frame_thread.thread = std::thread(&videoDecoder::read_frame_loop, this);
    video_thread.thread = std::thread(&videoDecoder::video_decode_loop, this);
    audio_thread.thread = std::thread(&videoDecoder::audio_decode_loop, this);

}

double videoDecoder::get_video_fps() const {

    AVRational avg_fps = format_context->streams[video_stream]->avg_frame_rate;

    if (avg_fps.num != 0 && avg_fps.den != 0) {

        return av_q2d(avg_fps);

    } 

    return 0.0;

}

uint64_t videoDecoder::get_video_length() const {

    return video_duration; 

}

int64_t videoDecoder::get_current_audio_pts_val() const {

    return current_audio_pts;

}

int64_t videoDecoder::get_current_video_pts_val() const {

    return current_video_pts;

}

bool videoDecoder::has_video_data() {

    video_thread.mutex.lock();

    bool ret = !video_frame_queue.empty();

    video_thread.mutex.unlock();

    return ret;

}

bool videoDecoder::has_audio() {

    audio_thread.mutex.lock();

    bool ret = !audio_queue.empty();

    audio_thread.mutex.unlock();

    return ret;

}

// These should probably be abstracted into a function that does this but since it is only done twice I don't want to complicate it further
videoData videoDecoder::get_next_video_data() {

    video_thread.mutex.lock();

    videoData next = video_frame_queue.front();

    video_frame_queue.pop();

    video_thread.mutex.unlock();

    current_video_pts = next.pts;

    video_thread.condition.notify_one();

    return next;

}

std::vector<int16_t> videoDecoder::get_next_audio_pcm_data() {

    audio_thread.mutex.lock();

    audioData next = audio_queue.front();

    audio_queue.pop();

    audio_thread.mutex.unlock();

    current_audio_pts = next.pts;

    audio_thread.condition.notify_one();

    return next.pcm_data;

}