//=====================================================================================================
//    Programme recoder.cpp qui permet l'enregistrement en fonction des paramètre passer dans le main 
//    Utilistation des fonctions k4a en c++: version 1.4
//=====================================================================================================

#include "recorder.h"

// =======================================================
//                    GLOBAL VARIABLE
// =======================================================
std::atomic_bool exiting(false);


// =============================================================================
// Function int do_recording(   uint16_t device_index,
//                              int recording_length,
//                              k4a_device_configuration_t *device_config,
//                              bool align_depth,
//                              bool face,
//                              string string_codec,
//                              int coef_resize,
//                              bool illimite,
//                              bool showtime
//                          )
//     Recorder with parameter of main.cpp
// =============================================================================


int do_recording(   uint16_t device_count_manual,
                    int recording_length,
                    k4a_device_configuration_t *device_config,
                    bool align_depth,
                    bool face,
                    string string_codec,
                    int coef_resize,
                    bool illimite,
                    bool showtime
                 )
{

// =======================================================
//                    VARIABLES
// =======================================================
    int color_width = 0, color_height = 0;
    int depth_width = 0, depth_height = 0;
    double fps;
    uint32_t device_count = k4a::device::get_installed_count();

    vector<string> availableDevices;
    string inputDevice;

    sf::SoundBufferRecorder rec_song; 

    string device_serialnumber[device_count];

// =======================================================
//                    K4A VARIABLES
// =======================================================
    
    k4a::device device [device_count];
    k4a::calibration calibration[device_count];
    k4a::transformation transformation[device_count];
    k4a::image depth_image_transformed[device_count], ir_image_transformed[device_count];

// =======================================================
//          Conversion of parameter of main.cpp
// =======================================================
    k4a_convert_color_resolution_to_int(device_config, color_width, color_height);
    k4a_convert_depth_mode_to_int(device_config, depth_width, depth_height);
    fps = k4a_convert_fps_to_double(device_config->camera_fps);
    
    cout << "Number of devices installed : " << device_count << "\n" << endl;

    if (device_count_manual != 0) device_count = device_count_manual;
 
    for(int i=0; i<device_count;i++)
    {   try
        {

            // =======================================================
            //              Open the device index
            // =======================================================
            device[i] = k4a::device::open(i);
            // =======================================================
            //              Serial number device index
            // =======================================================
            device_serialnumber[i] = device[i].get_serialnum();
            cout << "Open device serial number "<< device_serialnumber[i] << " in index " << i << endl;

            // ========================================================
            //             Start the device index  
            // ========================================================
            device[i].start_cameras(device_config);
            cout << "Start device index : " << i << "\n" <<endl;
        }
        catch (const std::exception & e)
        {
            cout << "\nDevice index " << i << " has problem\n" << endl;
            device_count = device_count - 1;
        }
    }
    if (device_count==0)
    {
        cout << "No device available" << endl;
        cout << "Closing program ..." << endl;    
        return 0; 
    }

    
     for(int i=0; i<device_count;i++)
    { 
        try
        {
        // =======================================================
        //              Calibration of the device open 
        // =======================================================
        calibration[i] = device[i].get_calibration(device_config->depth_mode, device_config->color_resolution);

        // ==============================================================
        // Preparation to the transforamation of depth and ir image 
        // ==============================================================
        transformation[i] = k4a::transformation(calibration[i]);
        depth_image_transformed[i] = k4a::image::create(
                                                K4A_IMAGE_FORMAT_DEPTH16,
                                                calibration[i].color_camera_calibration.resolution_width,
                                                calibration[i].color_camera_calibration.resolution_height,
                                                calibration[i].color_camera_calibration.resolution_width * (int)sizeof(uint16_t)
                                            );

        ir_image_transformed[i] = k4a::image::create(
                                                K4A_IMAGE_FORMAT_CUSTOM16,
                                                calibration[i].color_camera_calibration.resolution_width,
                                                calibration[i].color_camera_calibration.resolution_height,
                                                calibration[i].color_camera_calibration.resolution_width * (int)sizeof(uint16_t)
                                                );

        }
    

    catch (const std::exception & e)
    {
        cerr << e.what();
    }
    }

// ==================================================================
// Count number of microphone available and start device microphone  
// ==================================================================
    availableDevices = sf::SoundRecorder::getAvailableDevices();
    inputDevice = availableDevices[0];
    rec_song.setDevice(inputDevice);

// ==============================================================
//                      Do-While LOOP  
// Start recording with the given paramaeter and stop if CRTL+C
// is press or end of recording 
// ==============================================================
    do 
    {  
        // ==============================================================
        //                      VARIABLE Do-While LOOP  
        // ==============================================================

        std::time_t temps;
        string string_time, filename;

        std::stringstream stringstream_time;
        std::experimental::filesystem::v1::__cxx11::path dirname_time[device_count];

        char char_array[4];
        bool isColor = true;
        int codec;

        VideoWriter out_color[device_count];
        VideoWriter out_depth_small[device_count], out_depth_big[device_count];
        VideoWriter out_ir_small[device_count], out_ir_big[device_count];

        VideoWriter out_depth_transformed_small[device_count], out_depth_transformed_big[device_count];
        VideoWriter out_ir_transformed_small[device_count], out_ir_transformed_big[device_count];

        cv::Size framesize;

        ofstream frametime_file[device_count], looptime_file;

        int frame_number[device_count];

        typedef std::chrono::duration<float> float_seconds;
        float_seconds duration_recording;

        auto duration_write_color = duration_cast<milliseconds>(std::chrono::milliseconds(0));
        auto duration_write_depth = duration_cast<milliseconds>(std::chrono::milliseconds(0));
        auto duration_write_ir = duration_cast<milliseconds>(std::chrono::milliseconds(0));
        auto duration_write = duration_cast<milliseconds>(std::chrono::milliseconds(0));
        auto duration_capture = duration_cast<milliseconds>(std::chrono::milliseconds(0));
        auto duration_transf = duration_cast<milliseconds>(std::chrono::milliseconds(0));

        auto start_recording = high_resolution_clock::now();


        temps = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        stringstream_time << std::put_time(std::localtime(&temps), "%Y_%m_%d_%H_%M_%S");

        for (int i =0;i<device_count;i++)
        {     
            frame_number[i]= 1;

            string_time =  stringstream_time.str() + "_S9_C157_kinect_" + to_string(i);
            dirname_time[i] = fs::current_path()/"Data/Video"/string_time;
            create_directories(dirname_time[i]);
            cout << "Creating directory to save video : " << dirname_time[i] << "\n" << endl;
    
            strcpy(char_array, string_codec.c_str());

            codec = VideoWriter::fourcc(char_array[0], char_array[1], char_array[2], char_array[3]);  
            
            if (face == true)  fps = 5.0;

            filename = dirname_time[i] + "/" + string_time + "_color.mp4";    
            framesize = Size(color_width/coef_resize, color_height/coef_resize);
            isColor = true;
            out_color[i].open(filename, codec, fps, framesize, isColor);

            if (device_config->depth_mode != K4A_DEPTH_MODE_OFF && align_depth == true ) 
            {
                isColor = false;
                filename = dirname_time[i] + "/" + string_time + "_depth_transformed_smallbits.mp4"; 
                out_depth_transformed_small[i].open(filename, codec, fps, framesize, isColor);
                filename = dirname_time[i] + "/" + string_time + + "_depth_transformed_bigbits.mp4";           
                out_depth_transformed_big[i].open(filename, codec, fps, framesize, isColor);

                filename = dirname_time[i] + "/" + string_time + "_ir_transformed_smallbits.mp4"; 
                out_ir_transformed_small[i].open(filename, codec, fps, framesize, isColor);
                filename = dirname_time[i] + "/" + string_time + "_ir_transformed_bigbits.mp4";           
                out_ir_transformed_big[i].open(filename, codec, fps, framesize, isColor);
            }

            if (device_config->depth_mode != K4A_DEPTH_MODE_OFF  && align_depth != true) {
                isColor = false; 
                framesize = Size(depth_width,depth_width);                                
                filename = dirname_time[i] + "/" + string_time + "_ir_smallbits.mp4"; 
                out_ir_small[i].open(filename, codec, fps, framesize, isColor);
                filename = dirname_time[i] + "/" + string_time + "_ir_bigbits.mp4";    
                out_ir_big[i].open(filename, codec, fps, framesize, isColor);

                filename = dirname_time[i] + "/" + string_time + "_depth_smallbits.mp4";
                out_depth_small[i].open(filename, codec, fps, framesize, isColor); 
                filename = dirname_time[i] + "/" + string_time + "_depth_bigbits.mp4";           
                out_depth_big[i].open(filename, codec, fps, framesize, isColor);
            }                     
                
            
            frametime_file[i].open (dirname_time[i] + "/" + string_time + "_frame_time.csv");
            //looptime_file.open (dirname_time + "/" + string_time + "_loop_time.csv");
        }


        if (rec_song.isAvailable()==true)
        {    
            rec_song.start();
            cout << "Start recording soung of device " << "\n" << endl;
        }
        else cout << "Impossible to record soung of device " << "\n" << endl;

        if (recording_length <= 0)
        {
            cout << "Press Ctrl-C to stop recording.\n" << std::endl;
        }

        cout << "Start recording video ...\n" << endl;

        do 
        {  

            auto start = high_resolution_clock::now();

            for( int i=0; i<device_count; i++)
            
            {   
                k4a::image color_image;
                k4a::image depth_image;
                k4a::image ir_image;
                k4a::capture capture;
                bool result = device[i].get_capture(&capture);

                auto stop_capture = high_resolution_clock::now();
                duration_capture = duration_cast<milliseconds>(stop_capture - start);

                if(showtime == true)
                    cout << "Time taken by capture : "
                        << duration_capture.count() << " milliseconds" << endl;

                if (result == false)
                {   
                    cout << "Invalid capture"<< endl;
                    continue;
                }

                time_t now = time(0);
                tm *ltm = localtime(&now);

                frametime_file[i]   << frame_number[i] << "," 
                                    << 1900 + ltm->tm_year 
                                    << "," << (1+ltm->tm_mon) 
                                    << "," << ltm->tm_mday 
                                    << "," << ltm->tm_hour
                                    << "," << ltm->tm_min
                                    << "," << ltm->tm_sec
                                    << "\n";

                cout << "Kinect "<< i << " capture frame number " << frame_number[i] << endl;       
                frame_number[i]= frame_number[i] + 1;

                // 2.6.2 Probe for a color image
                auto start_color = high_resolution_clock::now();

                if (device_config->color_resolution != K4A_COLOR_RESOLUTION_OFF)
                {   
                    //auto start_write_color = high_resolution_clock::now();
                    if(device_config->color_format == K4A_IMAGE_FORMAT_COLOR_MJPG) 
                    {
                        k4a::image compressed_color_image = capture.get_color_image();                             
                        MJPG2BGRA(compressed_color_image,color_image);
                    }
                    else color_image = capture.get_color_image();

                    
                    uint8_t* color_buffer = color_image.get_buffer();
                    cv::Mat colorMat(color_image.get_height_pixels(), color_image.get_width_pixels(), CV_8UC4, color_buffer, cv::Mat::AUTO_STEP);
                
                    cvtColor(colorMat,colorMat, CV_BGRA2BGR);
                    colorMat.convertTo(colorMat,CV_8UC3);
                    resize(colorMat,colorMat,colorMat.size()/coef_resize);

                    auto start_write_color = high_resolution_clock::now();
                    out_color[i].write(colorMat);
                    
                    auto stop_write_color = high_resolution_clock::now();
                    auto duration_write_color = duration_cast<milliseconds>(stop_write_color - start_write_color);

                    if(showtime == true)
                    cout << "Time taken by write color loop : "
                         << duration_write_color.count() << " milliseconds" << endl;
                    
                } 
                auto stop_color = high_resolution_clock::now();
                auto duration_color = duration_cast<milliseconds>(stop_color - start_color);
                if(showtime == true)
                    cout << "Time taken by color loop : "
                         << duration_color.count() << " milliseconds" << endl;

                // 2.6.3 : Probe for a IR16 image
                if (device_config->depth_mode != K4A_DEPTH_MODE_OFF)
                {
                    auto start_ir = high_resolution_clock::now();
                    ir_image = capture.get_ir_image();

                    if (align_depth != true)
                    {
                        uint8_t* ir_buffer = ir_image.get_buffer();

                        Mat irMat(ir_image.get_height_pixels() , ir_image.get_width_pixels(), CV_16U, (void*)ir_buffer, cv::Mat::AUTO_STEP);  

                        Mat irMat_Bigbits,irMat_Smallbits;
                        irMat_Smallbits = Mat::zeros(ir_image.get_height_pixels(), ir_image.get_width_pixels(), CV_16U);
                        bitwise_and(irMat,255,irMat_Smallbits);
                        irMat_Smallbits.convertTo(irMat_Smallbits,CV_8U);

                        irMat_Bigbits = Mat::zeros(ir_image.get_height_pixels(), ir_image.get_width_pixels(), CV_16U);
                        bitwise_and(irMat,65280,irMat_Bigbits);
                        irMat_Bigbits.convertTo(irMat_Bigbits,CV_8U,1/256.0);
                        
                        auto start_write_ir = high_resolution_clock::now();

                        out_ir_small[i].write(irMat_Smallbits);
                        out_ir_big[i].write(irMat_Bigbits);

                        auto stop_write_ir = high_resolution_clock::now();
                        duration_write_ir = duration_cast<milliseconds>(stop_write_ir - start_write_ir);

                    }

                    auto stop_ir = high_resolution_clock::now();
                    auto duration_ir = duration_cast<milliseconds>(stop_ir - start_ir);
                    if(showtime == true)
                        cout << "Time taken by ir loop : "
                            << duration_ir.count() << " milliseconds" << endl;

                    // 2.6.4 : Probe for a depth16 image
                    auto start_depth = high_resolution_clock::now();
                    depth_image = capture.get_depth_image();

                    if (align_depth != true)
                    {
                        uint8_t* depth_buffer = depth_image.get_buffer();
                        cv::Mat depthMat(depth_image.get_height_pixels() , depth_image.get_width_pixels(), CV_16U, (void*)depth_buffer, Mat::AUTO_STEP);
                
                        Mat depthMat_Bigbits,depthMat_Smallbits;

                        depthMat_Smallbits = Mat::zeros(depth_image.get_height_pixels(), depth_image.get_width_pixels(), CV_16U);
                        bitwise_and(depthMat,255,depthMat_Smallbits);
                        depthMat_Smallbits.convertTo(depthMat_Smallbits,CV_8U);

                        depthMat_Bigbits = Mat::zeros(depth_image.get_height_pixels(), depth_image.get_width_pixels(), CV_16U);
                        bitwise_and(depthMat,65280,depthMat_Bigbits);
                        depthMat_Bigbits.convertTo(depthMat_Bigbits,CV_8U,1/256.0);

                        auto start_write_depth = high_resolution_clock::now();

                        out_depth_small[i].write(depthMat_Smallbits);
                        out_depth_big[i].write(depthMat_Bigbits);

                        auto stop_write_depth = high_resolution_clock::now();
                        duration_write_depth = duration_cast<milliseconds>(stop_write_depth - start_write_depth);
                        
                    }

                    auto stop_depth = high_resolution_clock::now();
                    auto duration_depth = duration_cast<milliseconds>(stop_depth - start_depth);
                    if(showtime == true)
                        cout << "Time taken by depth loop : "
                            << duration_depth.count() << " milliseconds" << endl;



                    if (align_depth == true && device_config->color_resolution != K4A_COLOR_RESOLUTION_OFF)
                    {
                        auto start_align = high_resolution_clock::now();
                        uint8_t *ir_image_buffer = ir_image.get_buffer();
                        void *buffer_release_cb_context ; 

                        auto start_transf = high_resolution_clock::now();
                        k4a::image ir_image_custom = k4a::image::create_from_buffer(K4A_IMAGE_FORMAT_CUSTOM16,
                                                                                    depth_width,
                                                                                    depth_height,
                                                                                    depth_width * (int)sizeof(uint16_t),
                                                                                    ir_image_buffer,
                                                                                    depth_height * depth_width * (int)sizeof(uint16_t),
                                                                                    NULL,                                                        
                                                                                    NULL);
                        
                        transformation[i].depth_image_to_color_camera_custom(  depth_image, 
                                                                            ir_image_custom,
                                                                            &depth_image_transformed[i],
                                                                            &ir_image_transformed[i],
                                                                            K4A_TRANSFORMATION_INTERPOLATION_TYPE_LINEAR,
                                                                            0 );
                        auto stop_transf = high_resolution_clock::now();
                        duration_transf = duration_cast<milliseconds>(stop_transf - start_transf);
                        if(showtime == true)
                            cout << "Time taken by transformation loop : "
                                << duration_transf.count() << " milliseconds" << endl;
                        
                        auto start_transf_depth = high_resolution_clock::now();
                        if (depth_image_transformed != nullptr)
                        {          
                            uint8_t* depth_image_transformed_buffer = depth_image_transformed[i].get_buffer();
                                            
                            cv::Mat depth_image_transformed_Mat(color_image.get_height_pixels() , color_image.get_width_pixels(), CV_16U, (void*)depth_image_transformed_buffer, cv::Mat::AUTO_STEP);
                            
                            Mat depthMat_Bigbits,depthMat_Smallbits;
                            depthMat_Smallbits = Mat::zeros(color_image.get_height_pixels(), color_image.get_width_pixels(), CV_16U);
                            bitwise_and(depth_image_transformed_Mat,255,depthMat_Smallbits);
                            depthMat_Smallbits.convertTo(depthMat_Smallbits,CV_8U);
                            resize(depthMat_Smallbits,depthMat_Smallbits,depthMat_Smallbits.size()/coef_resize);

                            depthMat_Bigbits = Mat::zeros(color_image.get_height_pixels(), color_image.get_width_pixels(), CV_16U);
                            bitwise_and(depth_image_transformed_Mat,65280,depthMat_Bigbits);
                            depthMat_Bigbits.convertTo(depthMat_Bigbits,CV_8U,1/256.0);
                            resize(depthMat_Bigbits,depthMat_Bigbits,depthMat_Bigbits.size()/coef_resize);
                            
                            auto start_write_transf_depth = high_resolution_clock::now();

                            out_depth_transformed_small[i].write(depthMat_Smallbits);
                            out_depth_transformed_big[i].write(depthMat_Bigbits);

                            auto stop_write_transf_depth = high_resolution_clock::now();
                            duration_write_depth = duration_cast<milliseconds>(stop_write_transf_depth - start_write_transf_depth);

                        }
                        auto stop_transf_depth = high_resolution_clock::now();
                        auto duration_transf_depth = duration_cast<milliseconds>(stop_transf_depth - start_transf_depth);

                
                        auto start_transf_ir = high_resolution_clock::now();
                        if (ir_image_transformed != nullptr)
                        {               
                            uint8_t* ir_image_transformed_buffer = ir_image_transformed[i].get_buffer();
                                            
                            cv::Mat ir_image_transformed_Mat(color_image.get_height_pixels(), color_image.get_width_pixels(), CV_16U, (void*)ir_image_transformed_buffer, cv::Mat::AUTO_STEP);

                            Mat irMat_Bigbits,irMat_Smallbits;
                            irMat_Smallbits = Mat::zeros(color_image.get_height_pixels(), color_image.get_width_pixels(), CV_16U);
                            bitwise_and(ir_image_transformed_Mat,255,irMat_Smallbits);
                            irMat_Smallbits.convertTo(irMat_Smallbits,CV_8U);
                            resize(irMat_Smallbits,irMat_Smallbits,irMat_Smallbits.size()/coef_resize);

                            irMat_Bigbits = Mat::zeros(color_image.get_height_pixels()/coef_resize, color_image.get_width_pixels()/coef_resize, CV_16U);
                            bitwise_and(ir_image_transformed_Mat,65280,irMat_Bigbits);
                            irMat_Bigbits.convertTo(irMat_Bigbits,CV_8U,1/256.0);
                            resize(irMat_Bigbits,irMat_Bigbits,irMat_Bigbits.size()/coef_resize);

                            auto start_write_transf_ir = high_resolution_clock::now();

                            out_ir_transformed_small[i].write(irMat_Smallbits);
                            out_ir_transformed_big[i].write(irMat_Bigbits);

                            auto stop_write_transf_ir = high_resolution_clock::now();

                            duration_write_depth = duration_cast<milliseconds>(stop_write_transf_ir - start_write_transf_ir);
                        }
                        auto stop_transf_ir = high_resolution_clock::now();
                        auto duration_transf_ir = duration_cast<milliseconds>(stop_transf_ir - start_transf_ir);

                        auto duration_align = duration_cast<milliseconds>(stop_transf_ir - start_align);
                        if(showtime == true)
                            cout << "Time taken by DEPTH an IR transformation loop : "
                                << duration_align.count() << " milliseconds" << endl;
                        
                    }

                }

            }

            auto stop = high_resolution_clock::now();
            auto duration_loop = duration_cast<milliseconds>(stop - start);
            if(showtime == true)
                cout << "Time taken by while loop: "
                    << duration_loop.count() << " milliseconds\n" << endl;
            
            looptime_file   << duration_loop.count()
             //               << "," << duration_capture.count()
             //               << "," << duration_transf.count()
             //               << "," << duration_write_color.count() + duration_write_ir.count() + duration_write_depth.count()
                            << "\n";           
            
            duration_recording = std::chrono::duration_cast<float_seconds>(stop - start_recording);
            
            /*
            if (face == true && duration_recording.count() < recording_length)
            {   
                std::this_thread::sleep_for(std::chrono::milliseconds(1000 - duration_loop.count()));
            }
            */
            
        } while (!exiting && (recording_length < 0 || duration_recording.count() < recording_length));
        
        if(showtime == true)
                cout << "Time taken by recording : "
                     << duration_recording.count()<< " seconds\n" << endl;

        looptime_file.close();

        cout << "\nSaving sound buffer ..." << endl;
        rec_song.stop();
        const sf::SoundBuffer& buffer = rec_song.getBuffer();
        string_time =  stringstream_time.str() + "_S9_C157_kinect_0_son.wav";
        filename = dirname_time[0] + "/" + string_time;
        buffer.saveToFile(filename);
        cout << "Sound saved\n" << endl;

        for(int i=0;i<device_count;i++)
        {
            frametime_file[i].close();
            ofstream readyfile;
            readyfile.open (dirname_time[i] + "/ready.txt");
        }

        if (!exiting && illimite)
            cout << "Starting new recording...\n" << endl;

        if (exiting || !illimite)
        {
            for(int i=0;i<device_count;i++)
            {
                device[i].close();
                cout << "Kinect " << i << " closed" << endl;
            }
        }

    } while (!exiting && illimite != false);

    cout << "Closing program ..." << endl;    
    return 0;
}