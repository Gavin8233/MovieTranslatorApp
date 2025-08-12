
#include "include/subtitle_decoder.hpp"

subtitleDecoder::~subtitleDecoder() {

    GLOBAL_STATES::VIDEO_SEEK_REQUESTED = true; // to allow thread to join

    decoder_thread.condition.notify_all();

    if (decoder_thread.thread.joinable()) {

        decoder_thread.thread.join();

    }

    file.close();

    std::cout << "\nStopping HTTPLib Client";
    subtitleUtil::destroy_httplib_client();

}

subtitleDecoder::subtitleDecoder(const std::string& srt_file_path) :

start_position { 0 },
local_seek_requested { false }

{

    file = std::ifstream(srt_file_path.c_str());

}

void subtitleDecoder::decode_loop() {

    std::string line;
    std::string subtitle_text, translated_text;
    std::vector<uint32_t> text_codepoints, translated_text_codepoints;
    srtTimestamp timestamp;

    file.clear();
    file.seekg(start_position);

    while (!GLOBAL_STATES::VIDEO_SEEK_REQUESTED && !local_seek_requested) {

        std::unique_lock<std::mutex> lock(decoder_thread.mutex);

        decoder_thread.condition.wait(lock, [&]() { return subtitle_queue.size() < 10 || GLOBAL_STATES::VIDEO_SEEK_REQUESTED || local_seek_requested; }); 

        lock.unlock();

        std::getline(file, line); // Subtitle ID, like 1, 2, 3. OR empty line, which just means we need to continue to the next line, until the subtitle ID line is found.

        if (line.empty()) continue;

        std::getline(file, line); // Timestamp line

        timestamp = subtitleUtil::get_srt_timestamp(line);

        std::getline(file, line); // Subtitle text 

        if (line.empty()) continue; 

        subtitle_text = line;

        // Refer to subtitleUtil to see what function does in depth
        if (!subtitleUtil::decode_subtitle_text(file, subtitle_text, line)) { // File ended

            try {

                translated_text = subtitleUtil::translate_text(subtitle_text);

            }
            catch(std::exception& e) {

                std::cerr << e.what() << std::endl;
                translated_text = "failed";

            }

            try {

                text_codepoints = Utility::get_codepoints_from_string(subtitle_text);
                translated_text_codepoints = Utility::get_codepoints_from_string(translated_text);

            }
            catch (std::exception& e) {

                std::cerr << e.what() << std::endl;

            }

            Subtitle final_sub = subtitleUtil::make_subtitle(timestamp, subtitle_text, translated_text, text_codepoints, translated_text_codepoints);

            decoder_thread.mutex.lock();
            subtitle_queue.emplace_back(final_sub);
            decoder_thread.mutex.unlock();

            break;

        }

        try {

            translated_text = subtitleUtil::translate_text(subtitle_text);

        }
        catch (std::exception& e) {

            std::cerr << e.what() << std::endl;
            translated_text = "failed";

        }

        try {

            text_codepoints = Utility::get_codepoints_from_string(subtitle_text);
            translated_text_codepoints = Utility::get_codepoints_from_string(translated_text);

        }
        catch (std::exception& e) {

            std::cerr << e.what() << std::endl;

        }

        Subtitle sub = subtitleUtil::make_subtitle(timestamp, subtitle_text, translated_text, text_codepoints, translated_text_codepoints);

        decoder_thread.mutex.lock();
        subtitle_queue.emplace_back(sub);
        decoder_thread.mutex.unlock();

    }

}

void subtitleDecoder::seek_subtitle_location(const int64_t& seek_to, bool local_seek) {

    if (local_seek) {

        local_seek_requested = true;

    }

    decoder_thread.condition.notify_all();

    if (decoder_thread.thread.joinable()) {

        decoder_thread.thread.join();

    }

    file.clear();
    file.seekg(0);

    std::string line;
    srtTimestamp timestamp;

    while (true) {

        start_position = file.tellg();

        std::getline(file, line);

        if (line.empty()) continue;

        std::getline(file, line);

        timestamp = subtitleUtil::get_srt_timestamp(line);

        if ((timestamp.start < seek_to && seek_to < timestamp.end) || timestamp.start > seek_to) { // Location found

            break;

        }

        std::getline(file, line);

        while (std::getline(file, line)) {

            if (line.empty()) break;

        }

    }

    decoder_thread.mutex.lock();
    subtitle_queue.clear();
    decoder_thread.mutex.unlock();

    if (local_seek) {

        local_seek_requested = false;
        
    }

}

void subtitleDecoder::adjust_queued_subtitle_delays(const int& delay) {

    for (Subtitle& s : subtitle_queue) {

        s.display_timestamp.start = s.original_timestamp.start + userPreferences::user_video_preferences.subtitle_delay_offset_ms;
        s.display_timestamp.end = s.original_timestamp.end + userPreferences::user_video_preferences.subtitle_delay_offset_ms;

    }

}

bool subtitleDecoder::has_subtitle() const {

    return !subtitle_queue.empty();

}

Subtitle subtitleDecoder::get_next_subtitle() {

    decoder_thread.mutex.lock();

    Subtitle subtitle = subtitle_queue.front();

    subtitle_queue.erase(subtitle_queue.begin() + 0);

    decoder_thread.mutex.unlock();

    decoder_thread.condition.notify_one();

    return subtitle;

}

void subtitleDecoder::start_decoder_thread() {

    decoder_thread.thread = std::thread(&subtitleDecoder::decode_loop, this);

}