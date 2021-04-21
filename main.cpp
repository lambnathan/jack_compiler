#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem; //for reading all files in a directory

string get_last_dir(string dir);

int main(int argc, char* argv[]){

    /*
     * program accepts one input: either a file with the .jack extension,
     * or a directory name containing one or more .jack files
     */
    if(argc != 2){
        cerr << "Error. Please provide either a .jack file or the name of a directory with .jack files inside" << endl;
        exit(-1);
    }
    
    vector<string> filenames;
    string output_file_path; //this is based on the input, and will have a .xml extension for now
    string arg = argv[1];
    if(arg.find(".jack") != string::npos){
        //user entered a .jack filename instead of a directory
        ifstream fin;
        fin.open(arg);
        if(fin.fail()){
            cerr << "Error. Either something went wrong opening the file, or the file does not exist." << endl;
            exit(-1);
        }
        fin.close();
        filenames.push_back(arg); //add to the collection of filenames
        output_file_path = arg.substr(0, arg.find(".jack")) + ".xml";
    }
    else{
        //user entered a directory name, so get all .jack files inside
        if(arg[arg.length() - 1] == '/'){
            arg = arg.substr(0, arg.length() - 1); //remove the last slash if present
        }
        string last_dir = "";
        if(arg.find("/") != string::npos){ //if the argument has nested folders, get the last one
            last_dir = get_last_dir(arg);
        }
        output_file_path = arg + "/" + last_dir + ".xml"; //is xml for now
        cout << "output file path: " << output_file_path << endl;
    }

    for(const auto &entry: fs::directory_iterator(arg)){ //go through directory, get all filenames, add them to vector 
        //cout << "file: " << entry.path() << endl;
        string f_name = "";
        for(char c: entry.path().string()){
            if(c != '"'){
                f_name += c;
            }
        }
        if(f_name.find(".jack") == string::npos){ //skip non .jack files
            continue;
        }
        filenames.push_back(f_name); //add to the list of filenames
        f_name = f_name.substr(f_name.find_last_of("/") + 1); //get only the filename, not path
    }

    //now go through each .jack file
    

    return 0;
}

//takes a path and gets the last directory or file in the path,
//stripping out '/'
string get_last_dir(string dir){
    string last_dir = dir.substr(dir.find_last_of("/"));
    if(last_dir.length() > 1){
        last_dir = last_dir.substr(1);
    }
    else{
        cerr << "Something is wrong with the argument name." << endl;
        exit(-1);
    }
    return last_dir;
}