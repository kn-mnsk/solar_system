// root_directory.h.in
#ifndef ROOT_DIRECTORY_H
#define ROOT_DIRECTORY_H

const char * project_root = "E:/MyDevelopment/01_Solar/solar05_newStructure/02_ReadIn_CPP/";
const std::string localdata_root = "E:/MyDevelopment/01_Solar/solar05_newStructure/02_ReadIn_CPP/localdata/";
const std::string globaldata_root = "E:/MyDevelopment/01_Solar/solar05_newStructure/02_ReadIn_CPP/../.globaldata/";
const std::string shader_root = "E:/MyDevelopment/01_Solar/solar05_newStructure/02_ReadIn_CPP/shader/";

const std::string get_localdata_root(const std::string file){
return (localdata_root + file);
}
const std::string get_globaldata_root(const std::string file){
return (globaldata_root + file);
}
const std::string get_shader_root(const std::string file){
return (shader_root + file);
}

// ROOT_DIRECTORY_H
#endif
