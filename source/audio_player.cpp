
#include "include/audio_player.hpp"

audioPlayer::~audioPlayer() {

    alcMakeContextCurrent(NULL);

    alcDestroyContext(alc_context);
    alc_context = nullptr;

    alcCloseDevice(alc_device);
    alc_device = nullptr;

    GLOBAL_STATES::VIDEO_SEEK_REQUESTED = true; // to allow thread to join

    audio_player_thread.condition.notify_all();

    if (audio_player_thread.thread.joinable()) {

        audio_player_thread.thread.join();

    }

    delete_buffers_and_source();

}

audioPlayer::audioPlayer() :

alc_device { nullptr },
alc_context { nullptr }

{

    alc_device = alcOpenDevice(nullptr);

    if (alc_device) {

        alc_context = alcCreateContext(alc_device, nullptr);
        alcMakeContextCurrent(alc_context);

    } else {

        std::cerr << "\nERROR: Failed to create an OpenAL device for audio playback, this is likely due to a misconfigured setup."
        "\nTo fix this, uninstall all instances of OpenAL Soft on your device if you had it installed prior to downloading this project, if not, you can proceed."
        "\n1.) Uninstall the project folders from /usr/local/bin/MovieTranslatorApp and /usr/local/share/MovieTranslatorApp."
        "\n2.) Uninstall the folder where you originally copied the project."
        "\n3.) run '<your package manager> install libpulse-dev libasound2-dev'"
        "\nAfter all this is complete, you can re-clone the repository and run the build commands, then you shouldn't see this error again on startup."
        "\nIf you do see this error again, check your system's audio configuration to ensure audio output devices are available(PulseAudio, ALSA, or PipeWire).";
        throw std::runtime_error("Failed to Create an OpenAL Device");

    }

    alGetError();

    init_buffers_and_source(false);

}

void audioPlayer::init_buffers_and_source(bool delete_previous) {

    if (delete_previous) {

        delete_buffers_and_source();

    }

    alGenBuffers(CONSTANTS::AUDIO::BUFFER_QUEUE_SIZE, bufs);
    alGenSources(1, source);

}

void audioPlayer::delete_buffers_and_source() {

    ALint queued = 0;

    alGetSourcei(source[0], AL_BUFFERS_QUEUED, &queued);

    if (queued > 0) {

        for (int i = 0; i < queued; i++) {

            ALuint freed;
            alSourceUnqueueBuffers(source[0], 1, &freed);

        }

    }

    alDeleteBuffers(CONSTANTS::AUDIO::BUFFER_QUEUE_SIZE, bufs);
    alDeleteSources(1, &source[0]);

}

void audioPlayer::play_audio_loop() {

    ALint processed = 0, state = 0, queued = 0;

    {

        std::unique_lock<std::mutex> lock(audio_player_thread.mutex);
        audio_player_thread.condition.wait(lock, [&] { return is_queue_full(); }); // wait for enough audio to fill buffers

    }

    for (int i = 0; i < CONSTANTS::AUDIO::BUFFER_QUEUE_SIZE; i++) { // queue it

        audio_player_thread.mutex.lock();
        std::vector<int16_t> data = pcm_data_queue.front();
        pcm_data_queue.pop();
        audio_player_thread.mutex.unlock();

        ALsizei data_size = static_cast<ALsizei>(data.size() * sizeof(int16_t));

        alBufferData(bufs[i], AL_FORMAT_STEREO16, data.data(), data_size, CONSTANTS::AUDIO::SAMPLE_RATE);

        alSourceQueueBuffers(source[0], 1, &bufs[i]);

    }

    alSourcePlay(source[0]);

    alGetSourcei(source[0], AL_BUFFERS_QUEUED, &queued);

    while (!GLOBAL_STATES::VIDEO_SEEK_REQUESTED) {

        {

            std::unique_lock<std::mutex> lock(audio_player_thread.mutex);
            audio_player_thread.condition.wait(lock, [&] { return !pcm_data_queue.empty() || GLOBAL_STATES::VIDEO_SEEK_REQUESTED; });

            if (GLOBAL_STATES::VIDEO_PAUSED) {

                GLOBAL_STATES::VIDEO_PAUSED_CONDITION.wait(lock, [&] { return !GLOBAL_STATES::VIDEO_PAUSED; });

            }

        }

        alGetSourcei(source[0], AL_BUFFERS_PROCESSED, &processed);

        while (processed-- > 0 && !pcm_data_queue.empty() && !GLOBAL_STATES::VIDEO_SEEK_REQUESTED) {

            alGetSourcei(source[0], AL_BUFFERS_QUEUED, &queued);

            ALuint freed_buf;
            alSourceUnqueueBuffers(source[0], 1, &freed_buf);

            audio_player_thread.mutex.lock();
            
            std::vector<int16_t> pcm_data = pcm_data_queue.front();
            pcm_data_queue.pop();

            audio_player_thread.mutex.unlock();

            ALsizei data_size = static_cast<ALsizei>(pcm_data.size() * sizeof(int16_t));

            alBufferData(freed_buf, AL_FORMAT_STEREO16, pcm_data.data(), data_size, CONSTANTS::AUDIO::SAMPLE_RATE);

            alSourceQueueBuffers(source[0], 1, &freed_buf);

        }

        alGetSourcei(source[0], AL_SOURCE_STATE, &state);

        if (state != AL_PLAYING) {
            
            alSourcePlay(source[0]);

        }

    }

}

bool audioPlayer::is_queue_full() {

    return pcm_data_queue.size() >= CONSTANTS::AUDIO::BUFFER_QUEUE_SIZE;

}

void audioPlayer::push_to_queue(const std::vector<int16_t>& pcm_data) {

    audio_player_thread.mutex.lock();

    pcm_data_accumulator.insert(pcm_data_accumulator.end(), pcm_data.begin(), pcm_data.end());

    if (pcm_data_accumulator.size() * sizeof(int16_t) >= CONSTANTS::AUDIO::BYTES_NEEDED_PER_AUDIO_PACKET) {

        pcm_data_queue.push(pcm_data_accumulator);

        pcm_data_accumulator.clear();

        audio_player_thread.condition.notify_one();

    }

    audio_player_thread.mutex.unlock();

}

void audioPlayer::clear_queue_and_restart() {

    audio_player_thread.mutex.lock();

    audio_player_thread.condition.notify_all();

    if (audio_player_thread.thread.joinable()) {

        audio_player_thread.thread.join();

    }

    while (!pcm_data_queue.empty()) {

        pcm_data_queue.pop();

    }

    audio_player_thread.mutex.unlock();

    init_buffers_and_source(true);

}

void audioPlayer::startup_thread() {

    audio_player_thread.thread = std::thread(&audioPlayer::play_audio_loop, this);

}