#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <cstdio> 

int FileReadToBuffer(std::vector<uint8_t>& outBuffer, const std::string& fileName);
int FileReadToString(std::string& outString, const std::string& filename);
bool FileWriteFromBuffer(std::vector<uint8_t> const& buffer, std::string const& filePathName);
bool CreateDirectoryA(std::string const& folderPathName);