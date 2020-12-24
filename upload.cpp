// g++ upload.cpp -lsfml-system -lsfml-network -lstdc++fs -o upload

#include <chrono>
#include <iostream>
#include <unistd.h>
#include <SFML/Network.hpp>
#include <experimental/filesystem>

using namespace std;
using namespace std::chrono; 
namespace fs = std::experimental::filesystem;

void upload_sleep(sf::Ftp &ftp)
 {
    std::cout << "\nThe program is currently sleeping 5 minutes\n" << endl;
    for (int i=0 ;i<5;i++)
            {
                sleep(60);
                ftp.keepAlive();

            }
 }

int main ()
{

    bool flag=false;
    string year,month, day,hour;
    string dir_nas = "CALIPRO/Data_VP_S9_C157/";
    string video_nas = "Video_S9_C157/";
    string spic_nas = "SPIC_S9_C157";
    
    while(1)
    {   
        sf::Ftp ftp;
        const sf::IpAddress ip_adresse ("10.0.10.41");
        sf::Ftp::Response response = ftp.connect(ip_adresse, 21, sf::seconds(30));

        if (!response.isOk()) upload_sleep(ftp);
        std::cout << "Response status: " << response.getStatus() << std::endl;
        std::cout << "Response message: " << response.getMessage() << std::endl;

        response = ftp.login("generique1", "2pwx%J");
        if (!response.isOk()) cout << "Login error" << endl;
        std::cout << "Response status: " << response.getStatus() << std::endl;
        std::cout << "Response message: " << response.getMessage() << std::endl;
        
        response = ftp.getWorkingDirectory();
        std::cout << "Response status: " << response.getStatus() << std::endl;
        std::cout << "Response message: " << response.getMessage() << std::endl;
        
	    if (!response.isOk()) cout << "Login error" << endl;

        else
        {
            string video = "Data/Video";

            fs::path path = fs::current_path()/video;
	    
            if (fs::is_empty(path) != true)
            {  
                cout << "\n****** Looking for video folder *****" << endl;  
                for (const auto & entry : fs::directory_iterator(path))
                {   
                    string p = entry.path().string();
                    std::size_t FolderPos = p.find_last_of("/");
                    string folder = p.substr(FolderPos+1,19);
                    year = folder.substr(0,4);
                    month = year+"/"+ folder.substr(0,7);
                    ftp.createDirectory(dir_nas + video_nas + month);
                    
                    day = month + "/" + folder.substr(0,10);
                    ftp.createDirectory(dir_nas + video_nas + day);

                    fs::path newpath = entry.path();
                                       
                    if (fs::exists(p+"/ready.txt") == true)
                    {   
                        hour = day + "/" + folder.substr(0,13);
                        ftp.createDirectory(dir_nas + video_nas + hour);

                        for (const auto & sub_entry : fs::directory_iterator(newpath))
                        {   
                            auto start_transf = high_resolution_clock::now();
                            cout << "Transferring " << sub_entry.path().string() << " to " << dir_nas + video_nas + hour << endl;
                            ftp.upload(sub_entry.path().string(),dir_nas + video_nas + hour,sf::Ftp::Binary);
                            auto stop_transf = high_resolution_clock::now();
                            auto duration_transf = duration_cast<seconds>(stop_transf - start_transf);
                            cout << "Transfer finished " << "in " << duration_transf.count() << " s" << endl; 
                            
                        }
                        fs::remove_all(newpath);
                    }
                    else 
                    {
                        cout << newpath << " is not ready to be upload !" << endl ;
                    }
                }
            }
            else 
            {
                cout << "**** Video directory is empty *****\n" << endl;
            }
            /*
            string spic = "Data/SPIC";
	        path = fs::current_path()/spic;
                
            if (fs::is_empty(path) != true)
            {   
                cout << "\n******Looking for spic files*****" << endl; 
                for (const auto & entry : fs::directory_iterator(path))
                {
                    string p = entry.path().string();
                    std::size_t FolderPos = p.find_last_of("/");
                    string folder = p.substr(FolderPos+1,p.length());
                    fs::path newpath = entry.path();
                    
                    if (fs::exists(p+"/ready.txt") == true)
                    {   
                        ftp.createDirectory(dir_nas + spic_nas + folder);
                        for (const auto & entry2 : fs::directory_iterator(newpath))
                        {   
                            cout << "Transferring " << entry2.path().string() << endl; 

                            string file = entry2.path().string();
                            ftp.upload(file,dir_nas + spic_nas + folder,sf::Ftp::Binary);
                            cout << "Transfer finished" << endl; 
                        }

                        fs::remove_all(newpath);
                    }
                    else 
                    {
                        cout << newpath << " is not ready to be upload !" << endl ;
                    }
                }
            }
            else 
            {
                cout << "SPIC directory is empty" << endl;  
            }
            */
            cout << "\n" <<"All files was transferred to the NAS" << endl;
            upload_sleep(ftp);  
        }
        
    }
}
