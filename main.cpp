//===========================================================================================================================================================================================================================
//      Programme main.cpp contient le main de la maquette d'enregistrement
//      Pour compiler le programme : 
//      g++  main.cpp recorder.cpp  functions.cpp -lopencv_core -lopencv_imgcodecs -lopencv_highgui -lopencv_videoio -lopencv_imgproc -lk4a -lstdc++fs -L/opt/libjpeg-turbo/lib64/ -lturbojpeg -lsfml-audio -o recorder
//      g++  main.cpp recorder.cpp  functions.cpp -lopencv_core -lopencv_imgcodecs -lopencv_highgui -lopencv_videoio -lopencv_imgproc -lk4a -lstdc++fs -lturbojpeg -lsfml-audio -o recorder
//===========================================================================================================================================================================================================================

// =============================================================================
//                                  INCLUDES
// =============================================================================
#include "cmdparser.h"
#include "recorder.h"
#include "functions.h"

int main(int argc, char **argv)
{
    uint16_t device_count_manual = 0;
    uint16_t device_index = 255;
    int recording_length = -1;

    k4a_image_format_t recording_color_format = K4A_IMAGE_FORMAT_COLOR_MJPG;
    k4a_color_resolution_t recording_color_resolution = K4A_COLOR_RESOLUTION_720P;
    k4a_depth_mode_t recording_depth_mode = K4A_DEPTH_MODE_WFOV_2X2BINNED;
    k4a_fps_t recording_rate = K4A_FRAMES_PER_SECOND_5;
    bool sync = true;
      
    bool align_depth = false;
    bool show_time = false;
    bool face = false;
    
    bool illimite = false;
    int coef_resize = 1;
    std::string codec = "MJPG";


    CmdParser::OptionParser cmd_parser;
    
    cmd_parser.RegisterOption("-h|--help", "Prints this help", [&]() {
        std::cout << "\nRecorder [options]" << std::endl << std::endl;
        cmd_parser.PrintOptions();
        exit(0);
    });

    cmd_parser.RegisterOption("-d|--device-index",
                              "Specify the index number of device to use ",
                              1,
                              [&](const std::vector<char *> &args) {
                                  device_index = std::stoi(args[0]);
                                  if (device_index < 0 || device_count_manual > 5)
                                      throw std::runtime_error("Device index must 0-5");
                              });
    cmd_parser.RegisterOption("-nd|--number-device",
                              "Specify the number of device to use to do multi-kinect recording",
                              1,
                              [&](const std::vector<char *> &args) {
                                  device_count_manual = std::stoi(args[0]);
                                  if (device_count_manual < 0 || device_count_manual > 5)
                                      throw std::runtime_error("Device count must 0-5");
                              });

    cmd_parser.RegisterOption("-l|--record-length",
                              "Limit the recording to N seconds (default: infinite)",
                              1,
                              [&](const std::vector<char *> &args) {
                                  recording_length = std::stoi(args[0]);
                                  if (recording_length < 0)
                                      throw std::runtime_error("Recording length must be positive");
                              });

    cmd_parser.RegisterOption("-i|--record-illimite",
                              "Recording illimite with files of record length seconds",
                              [&]() {illimite = true;
                              });

    cmd_parser.RegisterOption("-c|--color-mode",
                              "Set the color sensor mode (default: 1080p), Available options:\n"
                              "3072p, 2160p, 1536p, 1440p, 1080p, 720p, OFF",
                              1,
                              [&](const std::vector<char *> &args) {
                                  if (string_compare(args[0], "3072p") == 0)
                                  {
                                      recording_color_resolution = K4A_COLOR_RESOLUTION_3072P;
                                  }
                                  else if (string_compare(args[0], "2160p") == 0)
                                  {
                                      recording_color_resolution = K4A_COLOR_RESOLUTION_2160P;
                                  }
                                  else if (string_compare(args[0], "1536p") == 0)
                                  {
                                      recording_color_resolution = K4A_COLOR_RESOLUTION_1536P;
                                  }
                                  else if (string_compare(args[0], "1440p") == 0)
                                  {
                                      recording_color_resolution = K4A_COLOR_RESOLUTION_1440P;
                                  }
                                  else if (string_compare(args[0], "1080p") == 0)
                                  {
                                      recording_color_resolution = K4A_COLOR_RESOLUTION_1080P;
                                  }
                                  else if (string_compare(args[0], "720p") == 0)
                                  {
                                      recording_color_resolution = K4A_COLOR_RESOLUTION_720P;
                                  }
                                  else if (string_compare(args[0], "off") == 0)
                                  {
                                      recording_color_resolution = K4A_COLOR_RESOLUTION_OFF;
                                  }
                                  else
                                  {
                                      recording_color_resolution = K4A_COLOR_RESOLUTION_OFF;

                                      std::ostringstream str;
                                      str << "Unknown color mode specified: " << args[0];
                                      throw std::runtime_error(str.str());
                                  }
                              });

    cmd_parser.RegisterOption("-dm|--depth-mode",
                              "Set the depth sensor mode (default: WFOV_2X2BINNED), Available options:\n"
                              "NFOV_2X2BINNED, NFOV_UNBINNED, WFOV_2X2BINNED, WFOV_UNBINNED, OFF",
                              1,
                              [&](const std::vector<char *> &args) {
                                  if (string_compare(args[0], "NFOV_2X2BINNED") == 0)
                                  {
                                      recording_depth_mode = K4A_DEPTH_MODE_NFOV_2X2BINNED;
                                  }
                                  else if (string_compare(args[0], "NFOV_UNBINNED") == 0)
                                  {
                                      recording_depth_mode = K4A_DEPTH_MODE_NFOV_UNBINNED;
                                  }
                                  else if (string_compare(args[0], "WFOV_2X2BINNED") == 0)
                                  {
                                      recording_depth_mode = K4A_DEPTH_MODE_WFOV_2X2BINNED;
                                  }
                                  else if (string_compare(args[0], "WFOV_UNBINNED") == 0)
                                  {
                                      recording_depth_mode = K4A_DEPTH_MODE_WFOV_UNBINNED;
                                  }
                                  else if (string_compare(args[0], "off") == 0)
                                  {
                                      recording_depth_mode = K4A_DEPTH_MODE_OFF;
                                      sync = false;
                                      
                                  }
                                  else
                                  {
                                      std::ostringstream str;
                                      str << "Unknown depth mode specified: " << args[0];
                                      throw std::runtime_error(str.str());
                                  }

                              });
   
    cmd_parser.RegisterOption("-fps|--frameperseconds",
                              "Set the camera frame rate in Frames per Second\n"
                              "Default is the maximum rate supported by the camera modes.\n"
                              "Available options: 30, 15, 5",
                              1,
                              [&](const std::vector<char *> &args) {
                                  if (string_compare(args[0], "30") == 0)
                                  {
                                      recording_rate = K4A_FRAMES_PER_SECOND_30;
                                  }
                                  else if (string_compare(args[0], "15") == 0)
                                  {
                                      recording_rate = K4A_FRAMES_PER_SECOND_15;
                                  }
                                  else if (string_compare(args[0], "5") == 0)
                                  {
                                      recording_rate = K4A_FRAMES_PER_SECOND_5;
                                  }
                                  else
                                  {
                                      std::ostringstream str;
                                      str << "Unknown frame rate specified: " << args[0];
                                      throw std::runtime_error(str.str());
                                  }
                              });

    cmd_parser.RegisterOption("-cd|--codec",
                              "Set the codec mode (default: MJPG), Available options:\n"
                              "XVID, X264 et MJPG",
                              1,
                              [&](const std::vector<char *> &args) {
                                  if (string_compare(args[0], "MJPG") == 0)
                                  {
                                      codec = "MJPG";
                                  }
                                  else if (string_compare(args[0], "XVID") == 0)
                                  {
                                      codec = "XVID";
                                  }
                                  else if (string_compare(args[0], "X264") == 0)
                                  {
                                      codec = "X264";
                                  }
                              }); 

    cmd_parser.RegisterOption(  "-a|--align", 
                                "Set align depth image and ir image to color camera. ", 
                                [&]() {align_depth = true;
                            });

    cmd_parser.RegisterOption(  "-f|--face", 
                                "To stream the face of the pig. ", 
                                [&]() { face = true;
                                        recording_rate = K4A_FRAMES_PER_SECOND_5;
                        
                            });


    cmd_parser.RegisterOption("-cr|--coef_resize",
                              "Resize the size of the image",
                              1,
                              [&](const std::vector<char *> &args) {
                                  coef_resize = std::stoi(args[0]);
                                  if (coef_resize < 0)
                                      throw std::runtime_error("Coefficient resize must be positive");
                              });

    cmd_parser.RegisterOption(  "-st|--showtime",
                                "Show time of each function in the console.",
                                [&]() {show_time = true;}  
                            ); 
 
    int args_left = 0;

    try
    {
        args_left = cmd_parser.ParseCmd(argc, argv);
    }
    catch (CmdParser::ArgumentError &e)
    {
        std::cerr << e.option() << ": " << e.what() << std::endl;
        return 1;
    }

    if (recording_rate == K4A_FRAMES_PER_SECOND_30 && (recording_depth_mode == K4A_DEPTH_MODE_WFOV_UNBINNED ||
                                                       recording_color_resolution == K4A_COLOR_RESOLUTION_3072P))
    {
       
        std::cerr << "Error: 30 Frames per second is not supported by this camera mode." << std::endl;
            return 1;
        
    }
   
    struct sigaction act;
    act.sa_handler = signal_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGINT, &act, 0);

    
    k4a_device_configuration_t device_config = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
    device_config.color_format = recording_color_format;
    device_config.color_resolution = recording_color_resolution;
    device_config.depth_mode = recording_depth_mode;
    device_config.camera_fps = recording_rate;
    device_config.synchronized_images_only = sync;
    
    return do_recording(    (uint16_t)device_index,
                            (uint16_t)device_count_manual,
                            recording_length,
                            &device_config,
                            align_depth,
                            face,
                            codec,
                            coef_resize,
                            illimite,
                            show_time                       
                        );

                    
}