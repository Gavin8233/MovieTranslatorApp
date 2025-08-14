#define _CRT_SECURE_NO_WARNINGS // shut up msvc

#include "include/video_player.hpp"
#include "include/arg_parser.hpp"


int main(int argc, char* argv[]) {

    userPreferences::init_default_user_preferences();

    try {

        GLutil::init_opengl_context();

    }
    catch (std::exception& e) {

        std::cerr << e.what() << std::endl;

    }
    
    GLutil::init_texture_atlas();
    GLutil::init_overlay_textures();

    argParser arg_handler;

    try {
        
        arg_handler.handle_args(argc, argv);

    } 
    catch (std::exception& e) {

        std::cerr << e.what() << std::endl;

        return -1; // handle args failed

    }

    if (arg_handler.vars.translate_to_override != "false") {

        userPreferences::alter_translate_to(arg_handler.vars.translate_to_override);

    }

    subtitleUtil::init_httplib_client(arg_handler.vars.ip, arg_handler.vars.port);

    videoPlayer player(arg_handler.vars.video_file_path, arg_handler.vars.srt_file_path, arg_handler.vars.font_file_path, arg_handler.vars.threads);

    player.start();

}