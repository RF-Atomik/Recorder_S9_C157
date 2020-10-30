// **** Include file for function.cpp ****

// =============================================================================
//                                  INCLUDES
// =============================================================================
#include <csignal>
#include <atomic>
#include <iostream>
#include <assert.h>

#include <k4a/k4a.hpp>
#include <turbojpeg.h>

// =============================================================================
//                               GLOBAL VARIABLES
// =============================================================================
extern std::atomic_bool exiting; //define in recorder.cpp 


// =============================================================================
//                             FOWARD DECLARATIONS
// =============================================================================
void signal_handler(int s);

int string_compare(const char *s1, const char *s2);

bool MJPG2BGRA (const k4a::image &mjpgImage,k4a::image &bgraImage );

double k4a_convert_fps_to_double(k4a_fps_t fps);

void k4a_convert_color_resolution_to_int (  k4a_device_configuration_t *config, 
                                            int &width_color, 
                                            int &height_color
                                        );

void k4a_convert_depth_mode_to_int (    k4a_device_configuration_t *config, 
                                        int &width_depth, 
                                        int &height_depth
                                    );
