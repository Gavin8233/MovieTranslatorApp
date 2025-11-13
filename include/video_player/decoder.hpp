
#ifndef DECODER_H
#define DECODER_H

extern "C" {

    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
    #include <libavutil/imgutils.h>
    #include <libswresample/swresample.h>

}

#include <thread>
#include <mutex>
#include <condition_variable>

#include <queue>

namespace decoder {

    constexpr int MAX_DATA_QUEUE_LIMIT = 20;
    constexpr int MAX_PACKET_QUEUE_LIMIT = 40;

    template<typename T> 
    T get_next_t(std::queue<T>& queue, std::condition_variable& condition) {

        T t = queue.front();

        queue.pop();

        condition.notify_one();

        return t;

    }

};


#endif