
#include <napi.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#include <iostream>
#include <unordered_map>
#include "AcPhysics.h"
#include "util.h"
using namespace std;

unordered_map<string, vector<int>*> *makeStructHashMap()
{
    ifstream fieldsFile("fields.txt");

    stringstream ss;

    if (!fieldsFile.is_open())
    {
        cerr << "Failed to open fields.txt" << endl;
        return nullptr;
    }

    //split the line by comma and store the values in a vector
    

    unordered_map<string,vector<int>*> *theMap = new unordered_map<string,vector<int>*>();
    vector<int> *fields;
    string line;

    while (getline(fieldsFile, line))
    {

    }


}


Napi::String GetValue(const Napi::CallbackInfo &args)
{
    //get the key and print the accordig value from the physics struct
    Napi::Env env = args.Env();

    if (args.Length() != 1)
    {
        Napi::TypeError::New(env, "Expected one argument").ThrowAsJavaScriptException();
        return Napi::String::New(env, "");
    }

    string pr = args[0].As<Napi::String>().ToString();
    return Napi::String::New(env, "");

}



Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    
    util.hMapFile = OpenFileMappingA(
        FILE_MAP_READ,
        FALSE,
        "Local\\acpmf_physics");
    
    util.physics = static_cast<SPageFilePhysics*>(
        MapViewOfFile(
            util.hMapFile,
            FILE_MAP_READ,
            0,
            0,
            sizeof(SPageFilePhysics)));

    
    
    exports.Set("GetValue",
    Napi::Function::New(env, GetValue)
    );

    return exports;
}
