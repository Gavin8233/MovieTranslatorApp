
#ifndef ARGPARSER_H
#define ARGPARSER_H

#include <unordered_set>
#include <iostream>

constexpr const char* err_message = "\nFailed to parse argument. Check that the syntax you used was correct.";
constexpr const char* fail_message = "\nError while parsing arguments";

std::unordered_set<std::string> commands { 
// this is here because if user forgets to add a value for a command then the next command is likely going to be the "value" so can easily check with this
"-hpTrue",
"-printinfo",
"-librehost",
"-libreport",
"-fontfile"
};
// allowed extensions for each file type
std::unordered_set<std::string> video_extensions {

".mp4",
".mkv"

};
std::unordered_set<std::string> srt_extensions {

".srt",
".txt"

};
std::unordered_set<std::string> font_extensions {

".ttf",
".TTF"

};

struct Vars {

    std::string video_file_path;
    std::string srt_file_path;
    std::string font_file_path;
    std::string ip;
    int port;
    int threads;

};

class argParser {

public:

    Vars vars;

    argParser() {
        // set defaults, ignore video file path and srt file path because guaranteed to be set or error thrown
        vars.font_file_path = GET_RESOURCE_FOLDER_PATH() + "font.ttf";
        vars.ip = "localhost";
        vars.port = 5000;
        vars.threads = 1;

    }

    void handle_args(const int& argc, char* argv[]) {

        if (argc < 3) {

            std::cerr << "Make sure you provide the video file path and srt file path"
            "\nCorrect Usage: " << argv[0] << " <video_file_path> <subtitle_file_path> [options]";
            std::cerr << "\nOptions:"
            "\n     -hpTrue <thread_count>      Enables multi-threaded video decoding"
            "\n     -printinfo                  Prints the file paths you passed in"
            "\n     -librehost <ip>             Sets LibreTranslate host"
            "\n     -libreport <port>           Sets LibreTranslate port"
            "\n     -fontfile <file_path>       Sets the path to the .ttf file that Freetype will use";

            std::this_thread::sleep_for(std::chrono::seconds(3)); // let user see the message in case window wants to close immediately if not ran from terminal

            throw std::runtime_error(fail_message);

        }

        vars.video_file_path = argv[1];
        vars.srt_file_path = argv[2];

        if (!check_file(video_extensions, 4, vars.video_file_path) || !check_file(srt_extensions, 4, vars.srt_file_path)) {

            throw std::runtime_error(fail_message);

        }

        // i = 3 because argv[0, 1 and 2] are known and if not there an error will be thrown
        for (int i = 3; i < argc; i++) {

            std::string arg = argv[i];

            if (arg == "-printinfo") {

                std::cout << "\nVideo File Path: " << vars.video_file_path;
                std::cout << "\nSubtitle File Path: " << vars.srt_file_path;

            }

            else if (arg == "-hpTrue") {

                if (!handle_int(argc, argv, vars.threads, i, arg)) {

                    throw std::runtime_error(fail_message);

                }

                std::cout << "\nMulti-threaded video decoding enabled. Thread count: " << vars.threads;

            }

            else if (arg == "-libreport") {

                if (!handle_int(argc, argv, vars.port, i, arg)) {

                    throw std::runtime_error(fail_message);

                }

                std::cout << "\nSetting port to: " << vars.port;

            }

            else if (arg == "-librehost") {

                if (!handle_str(argc, argv, vars.ip, i, arg)) {

                    throw std::runtime_error(fail_message);

                }

                std::cout << "\nSetting IP to: " << vars.ip;

            }

            else if (arg == "-fontfile") {

                if (!handle_str(argc, argv, vars.font_file_path, i, arg)) {

                    throw std::runtime_error(fail_message);

                }

                if (!check_file(font_extensions, 4, vars.font_file_path)) {

                    throw std::runtime_error(fail_message);

                }

                std::cout << "\nSetting Font File Path to: " << vars.font_file_path;

            } 
        
            else {

                std::cerr << "\nArgument: " << arg << " is invalid.";

            }

        }

    }

private:

    bool handle_int(const int& argc, char* argv[], int& val, int& i, const std::string& argument) {

        if (i + 1 >= argc) {

            std::cerr << "\n" << err_message << "\nArgument " << argument << " is missing a value.";
            return false;

        }

        std::string argplus = argv[++i];

        if (commands.count(argplus)) {

            std::cerr << "\nInvalid value passed into " << argument << "."
            "\nValue passed in was: " << argplus << "."
            "\nMake sure you pass in a value after " << argument << ".";
            return false;

        }

        try {

            val = std::stoi(argplus);

        }
        catch (std::exception& e) {

            std::cerr << "\nValue for command " << argument << " is not an integer."
            "\nValue set for " << argument << " was " << argplus << "."
            "\nType a valid integer and try again.";
            return false;

        }

        return true; 

    }

    bool handle_str(const int& argc, char* argv[], std::string& val, int& i, const std::string& argument) {

        if (i + 1 >= argc) {

            std::cerr << "\n" << err_message << "\nArgument " << argument << " is missing a value.";
            return false;

        }

        val = argv[++i];

        if (commands.count(val)) {

            std::cerr << "\nValue for " << argument << " is invalid."
            "\nValue passed in was: " << val << "."
            "\nMake sure you add a value after " << argument << ".";
            return false;

        }

        return true;

    }

    bool check_file(const std::unordered_set<std::string>& allowed_extensions, const int extension_length, const std::string& val) {

        std::string check = val.substr(val.size() - extension_length);

        if (!allowed_extensions.count(check)) {

            std::cerr << "\nInvalid extension in " << val << "."
            "\nEXPECTED: ";

            for (const std::string& ext : allowed_extensions) {

                std::cerr << "\n" << ext;

            }

            return false;

        }

        return true;

    }


};


#endif