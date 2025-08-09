#define _CRT_SECURE_NO_WARNINGS // shut up msvc

#include "include/video_player.hpp"

#include <unordered_set>

bool handle_args(int argc, char* argv[], std::string& video_file_path, std::string& srt_file_path, std::string& font_file_path, std::string& ip, int& port, bool& high_performance) {

    if (argc < 3) {

        std::cerr << "Make sure you provide the video file path and srt file path"
        "\nCorrect Usage: " << argv[0] << " <video_file_path> <subtitle_file_path> [options]";
        std::cerr << "\nOptions:"
        "\n     -hpTrue:                Enables multi-threaded video decoding"
        "\n     -printinfo              Prints the file paths you passed in"
        "\n     -librehost <ip>         Sets LibreTranslate host"
        "\n     -libreport <port>       Sets LibreTranslate port"
        "\n     -fontfile <file_path>   Sets the path to the .ttf file that Freetype will use";

        std::this_thread::sleep_for(std::chrono::seconds(3)); // let user see the message in case window wants to close immediately if not ran from terminal

        return false;

    }

    constexpr const char* err_message = "\nFailed to parse argument. Check that the syntax you used was correct.";
    constexpr const char* fail_message = "\nError while parsing arguments";

    video_file_path = argv[1];
    srt_file_path = argv[2];

    std::string check_video = video_file_path.substr(video_file_path.size() - 4);
    std::string check_srt = srt_file_path.substr(srt_file_path.size() - 4);

    if (!(check_video == ".mp4" || check_video == ".mkv") || !(check_srt == ".srt" || check_srt == ".txt")) {

        std::cerr << "\nInvalid format in \n" << video_file_path << " OR " << srt_file_path;
        std::cerr << "\nSupported formats for video are .mp4 and .mkv"
        "\nSupported formats for srt files are .txt and .srt"
        "\nEnsure that the file paths you provided are valid";

        return false;

    }

    
    std::unordered_set<std::string> commands { 
        
    // this is here because if user forgets to add a value for a command then the next command is likely going to be the "value" so can easily check with this
    
        "-hpTrue",
        "-printinfo",
        "-librehost",
        "-libreport",
        "-fontfile"

    };

    // i = 3 because argv[0, 1 and 2] are known and if not there an error will be thrown
    for (int i = 3; i < argc; i++) {

        std::string arg = argv[i];

        if (arg == "-hpTrue") {

            high_performance = true;

            std::cout << "\nMulti-threaded video decoding enabled";

        }

        else if (arg == "-printinfo") {

            std::cout << "\nVideo File Path: " << video_file_path;
            std::cout << "\nSubtitle File Path: " << srt_file_path;

        }

        else if (arg == "-librehost") {

            // if no more arguments throw error

            if (i + 1 >= argc) {

                std::cerr << err_message << "\nArgument -librehost is missing a hostname";
                throw std::runtime_error(fail_message);

            }

            ip = argv[++i];

            // if ip is equal to a command(the user probably forgot to type the value) throw error

            if (commands.count(ip)) {

                std::cerr << "\nValue for librehost is invalid. Value: " << ip;
                throw std::runtime_error(fail_message);

            }

            std::cout << "\nSetting IP to: " << ip;

        }

        else if (arg == "-libreport") {

            if (i + 1 >= argc) {

                std::cerr << err_message << "\nArgument -libreport is missing a port number";
                throw std::runtime_error(fail_message);

            }

            std::string argplus = argv[++i];

            if (commands.count(argplus)) {

                std::cerr << "\nValue for libreport is invalid. Value: " << argplus;
                throw std::runtime_error(fail_message);

            }

            port = std::stoi(argplus);

            std::cout << "\nSetting port to: " << port;

        }

        else if (arg == "-fontfile") {

            if (i + 1 >= argc) {

                std::cerr << err_message << "\nArgument -fontfile is missing a file path";
                throw std::runtime_error(fail_message);

            }

            font_file_path = argv[++i];

            if (commands.count(font_file_path)) {

                std::cerr << "\nValue for fontfile is invalid. Value: " << font_file_path;
                throw std::runtime_error(fail_message);

            }

            std::string ttf_check = font_file_path.substr(font_file_path.size() - 4);

            if (ttf_check != ".ttf" && ttf_check != ".TTF") {

                std::cerr << "\nExtension is invalid. Must be .ttf OR .TTF. ERROR AT: " << arg << " " << font_file_path;
                throw std::runtime_error(fail_message);

            }

            std::cout << "\nSetting Font File Path to: " << font_file_path;

        } 
        
        else {

            std::cerr << "\nArgument: " << arg << " is invalid.";

        }

    }

    return true;

}

int main(int argc, char* argv[]) {

    userPreferences::init_default_user_preferences();

    GLutil::init_opengl_context();
    GLutil::init_texture_atlas();
    GLutil::init_overlay_textures();


    // guaranteed to be set or error thrown
    std::string video_file_path;
    std::string srt_file_path;

    //defaults
    std::string font_file_path = GET_RESOURCE_FOLDER_PATH() + "font.ttf";
    std::string ip = "localhost";
    int port = 5000;
    bool high_performance = false;


    bool check;

    try {
        // set vals
        check = handle_args(argc, argv, video_file_path, srt_file_path, font_file_path, ip, port, high_performance);

    } 
    catch (std::exception& e) {

        std::cerr << "\n" << e.what();

        return -1;

    }

    if (!check) {

        return -1; // handle args failed with no exception

    }

    subtitleUtil::init_httplib_client(ip, port);

    videoPlayer player(video_file_path, srt_file_path, font_file_path, high_performance);

    player.start();

    subtitleUtil::destroy_httplib_client();

}