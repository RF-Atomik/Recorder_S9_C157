// g++ upload.cpp -lsfml-system -lsfml-network -lstdc++fs -o upload

#include <chrono>
#include <iostream>
#include <unistd.h>
#include <SFML/Network.hpp>
#include <experimental/filesystem>

using namespace std;
using namespace std::chrono; 
namespace fs = std::experimental::filesystem;


int main ()
{

    bool flag=false;
    string year = "2020";
    string month, day,hour;
    
    while(1)
    {   
        sf::Ftp ftp;
        const sf::IpAddress ip_adresse ("10.0.10.41");
        sf::Ftp::Response response = ftp.connect(ip_adresse, 21, sf::seconds(10));

        if (!response.isOk()) flag =true;
        
        std::cout << "Response status: " << response.getStatus() << std::endl;
        std::cout << "Response message: " << response.getMessage() << std::endl;

        response = ftp.login("generique1", "2pwx%J");
        if (!response.isOk()) flag =true;

        std::cout << "Response status: " << response.getStatus() << std::endl;
        std::cout << "Response message: " << response.getMessage() << std::endl;
        
        ftp.getWorkingDirectory();

        if (!flag)
        {
            fs::path path = "/home/felin/Documents/Recorder_S9_C157/Data/Video";

            if (fs::is_empty(path) != true)
            {  
                for (const auto & entry : fs::directory_iterator(path))
                {   
                    string p = entry.path().string();
                    std::size_t FolderPos = p.find_last_of("/");
                    string folder = p.substr(FolderPos+1,19);
                    
                    month = year+"/"+ folder.substr(0,7);
                    ftp.createDirectory("CALIPRO/Data_VP_S9_C157/Video_S9_C157/" + month);
                    
                    day = month + "/" + folder.substr(0,10);
                    ftp.createDirectory("CALIPRO/Data_VP_S9_C157/Video_S9_C157/" + day);

                    fs::path newpath = entry.path();
                                       
                    if (fs::exists(p+"/ready.txt") == true)
                    {   
                        hour = day + "/" + folder.substr(0,13);
                        ftp.createDirectory("CALIPRO/Data_VP_S9_C157/Video_S9_C157/" + hour);

                        for (const auto & sub_entry : fs::directory_iterator(newpath))
                        {   
                            auto start_transf = high_resolution_clock::now();
                            cout << "Transferring " << sub_entry.path().string() << " to " << "CALIPRO/Data_VP_S9_C157/Video_S9_C157/"+hour << endl;
                            ftp.upload(sub_entry.path().string(),"CALIPRO/Data_VP_S9_C157/Video_S9_C157/"+hour,sf::Ftp::Binary);
                            auto stop_transf = high_resolution_clock::now();
                            auto duration_transf = duration_cast<seconds>(stop_transf - start_transf);
                            cout << "Transfer finished " << "in " << duration_transf.count() << " s" << endl; 
                            
                        }
                        fs::remove_all(newpath);
                    }
                    else {
                            cout << newpath << " is empty " << endl ;
                    }
                }
            }
            
            /*
            path = "/home/odroidh2/Documents/Recorder_S9_C157/Data/SPIC";
                
            if (fs::is_empty(path) != true)
            {   
                for (const auto & entry : fs::directory_iterator(path))
                {
                    string p = entry.path().string();
                    std::size_t FolderPos = p.find_last_of("/");
                    string folder = p.substr(FolderPos+1,p.length());
                    fs::path newpath = entry.path();
                    
                    if (fs::exists(p+"/ready.txt") == true)
                    {   
                        ftp.createDirectory("CALIPRO/Data_VP_S9_C157/SPIC_S9_C157/"+folder);
                        for (const auto & entry2 : fs::directory_iterator(newpath))
                        {   
                            cout << "Transferring " << entry2.path().string() << endl; 

                            string file = entry2.path().string();
                            ftp.upload(file,"CALIPRO/Data_VP_S9_C157/SPIC_S9_C157/"+folder,sf::Ftp::Binary);
                            cout << "Transfer finished" << endl; 

                        }

                        fs::remove_all(newpath);
                    }
                    else {
                            cout << newpath << " is empty " << endl ;
                    }
                }
            }

            else {
                cout << "All files was transferred to the NAS" << endl;  
            }*/

            for (int i=0 ;i<5;i++)
            {
                sleep(60);
                ftp.keepAlive();
            }
        }
        
    }

}
