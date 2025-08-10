#define _CRT_SECURE_NO_WARNINGS // shut up msvc

#include "include/video_player.hpp"
#include "include/arg_parser.hpp"


int main(int argc, char* argv[]) {

    userPreferences::init_default_user_preferences();

    GLutil::init_opengl_context();
    GLutil::init_texture_atlas();
    GLutil::init_overlay_textures();

    argParser arg_handler;

    try {
        
        arg_handler.handle_args(argc, argv);

    } 
    catch (std::exception& e) {

        std::cerr << "\n" << e.what();

        return -1; // handle args failed

    }

    subtitleUtil::init_httplib_client(arg_handler.vars.ip, arg_handler.vars.port);

    videoPlayer player(arg_handler.vars.video_file_path, arg_handler.vars.srt_file_path, arg_handler.vars.font_file_path, arg_handler.vars.threads);

    player.start();

}