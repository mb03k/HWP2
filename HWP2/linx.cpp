
// C++ Program to illustrate the use of filesystem classes 
// and features 
#include <filesystem> 
#include <fstream> 
#include <iostream> 
  
// for simplicity 
using namespace std; 
using namespace std::filesystem; 
  
int main() 
{ 
    // Define the path to create directory 
    path directorypath = "mydirectory"; 
  
    // To check if the directory exist or not, create it if 
    // doesn't exist 
    if (!exists(directorypath)) { 
        create_directory(directorypath); 
        cout << "Directory created: " << directorypath 
             << endl; 
    } 
  
    // Define the file path within the directory and 
    // combining the directory 
    path filepath = directorypath / "my_file.txt"; 
  
    // Create and open the file for writing using 
    // std::ofstream 
    ofstream file(filepath); 
    if (file.is_open()) { 
        // Write data to the file 
        file << "Hello, FileSystem!"; 
        file.close(); 
        cout << "File created: " << filepath << endl; 
    } 
    else { 
        // Handle the case if any error occured 
        cerr << "Failed to create file: " << filepath 
             << endl; 
    } 
  
    return 0; 
}
