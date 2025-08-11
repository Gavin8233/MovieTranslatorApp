
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
// allowed extensions for each file type. extension is mapped to extension character length
std::unordered_set<std::string> video_extensions {
".mp4",
".mkv",
".avi",
".mov",
".flv",
".webm",
".wmv",
".mpg",
".mpeg",
".m4v",
".3gp",
".3g2",
".mts",
".m2ts",
".vob"
};

std::unordered_set<std::string> srt_extensions {
".srt",
".txt",
".sub",
".sbv",
".scr",
".caption",
".vtt",
".dfxp",
".ttml",
".cap",
".ssa",
".rt"
};

std::unordered_set<std::string> font_extensions {
".ttf",
".ttc",
".otf",
".woff",
".woff2",
".pfa",
".pfb",
".pcf",
".bdf",
".fnt",
".pfr",
".afm",
".pfm",
".cid"
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

        if (!check_file(video_extensions, vars.video_file_path) || !check_file(srt_extensions, vars.srt_file_path)) {

            throw std::runtime_error(fail_message);

        }

        // i = 3 because argv[0, 1 and 2] are known and if not there an error will be thrown
        for (int i = 3; i < argc; i++) {

            std::string arg = argv[i];

            if (arg == "-printinfo") {

                std::cout << "Video File Path: " << vars.video_file_path << std::endl;
                std::cout << "Subtitle File Path: " << vars.srt_file_path << std::endl;

            }

            else if (arg == "-hpTrue") {

                if (!handle_int(argc, argv, vars.threads, i, arg)) {

                    throw std::runtime_error(fail_message);

                }

                std::cout << "Multi-threaded video decoding enabled. Thread count: " << vars.threads << std::endl;

            }

            else if (arg == "-libreport") {

                if (!handle_int(argc, argv, vars.port, i, arg)) {

                    throw std::runtime_error(fail_message);

                }

                std::cout << "Setting port to: " << vars.port << std::endl;

            }

            else if (arg == "-librehost") {

                if (!handle_str(argc, argv, vars.ip, i, arg)) {

                    throw std::runtime_error(fail_message);

                }

                std::cout << "Setting IP to: " << vars.ip << std::endl;

            }

            else if (arg == "-fontfile") {

                if (!handle_str(argc, argv, vars.font_file_path, i, arg)) {

                    throw std::runtime_error(fail_message);

                }

                if (!check_file(font_extensions, vars.font_file_path)) {

                    throw std::runtime_error(fail_message);

                }

                std::cout << "Setting Font File Path to: " << vars.font_file_path << std::endl;

            } 
        
            else {

                std::cerr << "Argument: " << arg << " is invalid." << std::endl;

            }

        }

    }

private:

    bool handle_int(const int& argc, char* argv[], int& val, int& i, const std::string& argument) {

        if (i + 1 >= argc) {

            std::cerr << "\n" << err_message << "\nArgument " << argument << " is missing a value." << std::endl;
            return false;

        }

        std::string argplus = argv[++i];

        if (commands.count(argplus)) {

            std::cerr << "\nInvalid value passed into " << argument << "."
            "\nValue passed in was: " << argplus << "."
            "\nMake sure you pass in a value after " << argument << "." << std::endl;
            return false;

        }

        try {

            val = std::stoi(argplus);

        }
        catch (std::exception& e) {

            std::cerr << e.what() << std::endl;
            std::cerr << "\nValue for command " << argument << " is not an integer."
            "\nValue set for " << argument << " was " << argplus << "."
            "\nType a valid integer and try again." << std::endl;
            return false;

        }

        return true; 

    }

    bool handle_str(const int& argc, char* argv[], std::string& val, int& i, const std::string& argument) {

        if (i + 1 >= argc) {

            std::cerr << "\n" << err_message << "\nArgument " << argument << " is missing a value." << std::endl;
            return false;

        }

        val = argv[++i];

        if (commands.count(val)) {

            std::cerr << "\nValue for " << argument << " is invalid."
            "\nValue passed in was: " << val << "."
            "\nMake sure you add a value after " << argument << "." << std::endl;
            return false;

        }

        return true;

    }

    bool check_file(const std::unordered_set<std::string>& allowed_extensions, const std::string& val) {

        std::string check;
        std::size_t slash = val.find_last_of("/\\");
        std::string extension = (slash == std::string::npos) ? val : val.substr(slash + 1);

        std::size_t dot = extension.find_last_of('.');
        if (dot == std::string::npos) {

            std::cerr << "\nNo extension found in " << val << "." << std::endl;

            return false;

        }

        extension = extension.substr(dot);

        if (!allowed_extensions.count(extension)) {

            std::cerr << "\nInvalid extension in " << val << "."
            "\nEXPECTED: " << std::endl;

            for (const std::string& ext : allowed_extensions) {

                std::cerr << ext << std::endl;

            }

            return false;

        }

        return true;

    }


};


#endif