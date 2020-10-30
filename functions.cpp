// =======================================================
// Program with functions of :
//      signal_handler to stop the program correctly
//      string_compare to compare two strings each other
//      convert MJPG to BGRA image
//      convert k4a_fps to uint
//      convert k4a_color_resoution to uint               
//      convert k4a_depth_resoution to uint
// =======================================================

// =============================================================================
//                                  INCLUDES
// =============================================================================
#include "functions.h"

// =============================================================================
//                               GLOBAL VARIABLES
// =============================================================================
static time_t exiting_timestamp;

// =============================================================================
//                     Function void signal_handler(int s)
//     If a CTRL+C is catch from the keyboard the program is stop
// =============================================================================
void signal_handler(int s)
{
    (void)s;

    // exiting variable
    if (!exiting) 
    {
        std::cout << "\nStopping recording...\n" << std::endl;
        exiting_timestamp = clock();
        exiting = true;
    }
    // If Ctrl-C is received again after 1 second, force-stop the application since it's not responding.
    else if (exiting_timestamp != 0 && clock() - exiting_timestamp > CLOCKS_PER_SEC)
    {
        std::cout << "Forcing stop." << std::endl;
        exit(1);
    }
}

// =============================================================================
//         Function int string_compare(const char *s1, const char *s2)
//              Compare 2 strings if they are equal or not
// =============================================================================
int string_compare(const char *s1, const char *s2)
{
    assert(s1 != NULL);
    assert(s2 != NULL);

    while (tolower((unsigned char)*s1) == tolower((unsigned char)*s2))
    {
        if (*s1 == '\0')
        {
            return 0;
        }
        s1++;
        s2++;
    }
    // The return value shows the relations between s1 and s2.
    // Return value   Description
    //     < 0        s1 less than s2
    //       0        s1 identical to s2
    //     > 0        s1 greater than s2
    return (int)tolower((unsigned char)*s1) - (int)tolower((unsigned char)*s2);
}

// =============================================================================
//  Function bool MJPG2BGRA (const k4a::image &mjpgImage,k4a::image &bgraImage)
//          Decompress a MJPG image and convert to BGRA image
// =============================================================================
bool MJPG2BGRA (const k4a::image &mjpgImage,k4a::image &bgraImage)
{
    bgraImage = k4a::image::create  (   K4A_IMAGE_FORMAT_COLOR_BGRA32,
                                        mjpgImage.get_width_pixels(),
                                        mjpgImage.get_height_pixels(),
                                        mjpgImage.get_width_pixels() * 4 * (int)sizeof(uint8_t)
                                    );

    tjhandle m_decompressor;
    m_decompressor = tjInitDecompress ();
    
    const int decompressStatus = tjDecompress2 (    m_decompressor,
                                                    mjpgImage.get_buffer(),
                                                    static_cast<unsigned long>(mjpgImage.get_size()),
                                                    bgraImage.get_buffer(),
                                                    mjpgImage.get_width_pixels(),
                                                    0,
                                                    mjpgImage.get_height_pixels(),
                                                    TJPF_BGRA,
                                                    TJFLAG_FASTDCT | TJFLAG_FASTUPSAMPLE
                                                );
            
    (void)tjDestroy(m_decompressor);
    return true;
}

// =============================================================================
//           Function double k4a_convert_fps_to_uint(k4a_fps_t fps)
//                          Convert k4a_fps to uint 
// =============================================================================
double k4a_convert_fps_to_double(k4a_fps_t fps)
{
    double conv_fps;
    switch (fps)
    {
    case K4A_FRAMES_PER_SECOND_5:
        conv_fps = 5.0;
        break;
    case K4A_FRAMES_PER_SECOND_15:
        conv_fps = 15.0;
        break;
    case K4A_FRAMES_PER_SECOND_30:
        conv_fps = 30.0;
        break;
    default:
        conv_fps = 0.0;
        break;
    }
    return conv_fps;
}

// ============================================================================================================================
// Function void k4a_convert_color_resolution_to_int(k4a_device_configuration_t *config, int &width_color, int &height_color)
//             Convert k4a color resolution to int 
//             Get color resolution from k4a device configuration 
// ============================================================================================================================
void k4a_convert_color_resolution_to_int(k4a_device_configuration_t *config, int &width_color, int &height_color)
{   
    
    switch (config->color_resolution)
    {
    case K4A_COLOR_RESOLUTION_720P:
        width_color = 1280;
        height_color = 720;
        break;
    case K4A_COLOR_RESOLUTION_1080P:
        width_color = 1920;
        height_color = 1080;
        break;
    case K4A_COLOR_RESOLUTION_1440P:
        width_color  = 2560;
        height_color = 1440;
        break;
    case K4A_COLOR_RESOLUTION_1536P:
        width_color = 2048;
        height_color = 1536;
        break;
    case K4A_COLOR_RESOLUTION_2160P:
        width_color = 3840;
        height_color = 2160;
        break;
    case K4A_COLOR_RESOLUTION_3072P:
        width_color = 4096;
        height_color = 3072;
        break;
    
    default:
        width_color = 1280;
        height_color = 720;
        break;
    }
}

// ============================================================================================================================
// Function void k4a_convert_depth_mode_to_int(k4a_device_configuration_t *config, int &width_depth, int &height_depth)
//             Convert k4a depth mode to int 
//             Get depth mode from k4a device configuration 
// ============================================================================================================================
void k4a_convert_depth_mode_to_int(k4a_device_configuration_t *config, int &width_depth, int &height_depth)
{  
    switch (config->depth_mode)
    {
    case K4A_DEPTH_MODE_NFOV_UNBINNED:
        width_depth = 640;
        height_depth = 576;
        break;
    case K4A_DEPTH_MODE_NFOV_2X2BINNED:
        width_depth = 320;
        height_depth = 288;
        break;
    case K4A_DEPTH_MODE_WFOV_2X2BINNED:
        width_depth  = 512;
        height_depth = 512;
        break;
    case K4A_DEPTH_MODE_WFOV_UNBINNED:
        width_depth = 1024;
        height_depth = 1024;
        break;
    case K4A_DEPTH_MODE_OFF:
        width_depth = 0;
        height_depth = 0;
        break;
    
    default:
        width_depth = 512;
        height_depth = 512;
        break;
    }
}