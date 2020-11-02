// **** Include file for recorder.cpp ****

// =============================================================================
//                                  INCLUDES
// =============================================================================
#include <thread>
#include <experimental/filesystem>

#include <k4a/k4a.hpp>
#include <turbojpeg.h>
#include <SFML/Audio.hpp>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "functions.h"

// =============================================================================
//                                  NAMESPACES
// =============================================================================
using namespace std::chrono; 
using namespace cv;
using namespace std;
namespace fs = std::experimental::filesystem;

// =============================================================================
//                                  DEFINES
// =============================================================================
#ifndef RECORDER_H
#define RECORDER_H

// =============================================================================
//                             FOWARD DECLARATIONS
// =============================================================================
int do_recording(   uint16_t device_index,
                    uint16_t device_count_manual,
                    int recording_length,
                    k4a_device_configuration_t *device_config,
                    bool align_depth,
                    bool face,
                    std::string string_codec,
                    int coef_resize,
                    bool illimite,
                    bool showtime
                );

#endif