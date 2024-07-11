#include "Engine/Core/FileUtils.hpp"

// Function to read file into a buffer
int FileReadToBuffer(std::vector<uint8_t>& outBuffer, const std::string& fileName) {
	FILE* file;
	errno_t err = fopen_s(&file, fileName.c_str(), "rb"); // Open file in binary mode

	if (err != 0 || file == nullptr) {
		std::cerr << "Error opening file: " << fileName << std::endl;
		return -1; // File opening failed
	}

	// Seek to the end of the file to determine its size
	fseek(file, 0, SEEK_END);
	long fileSize = ftell(file);
	rewind(file); // Go back to the start of the file

	// Resize the buffer to fit the file size
	outBuffer.resize(fileSize);

	// Read the file into the buffer
	size_t bytesRead = fread(outBuffer.data(), sizeof(uint8_t), fileSize, file);
	fclose(file); // Close the file

	// Return number of bytes read or -1 if read was incomplete
	if (bytesRead > static_cast<size_t>(std::numeric_limits<int>::max())) {
		std::cerr << "File size exceeds the maximum value that can be returned as int." << std::endl;
		return -1;
	}

	return static_cast<int>(bytesRead);
}

// Function to read file contents into a string
int FileReadToString(std::string& outString, const std::string& fileName) {
	std::vector<uint8_t> buffer;
	int bytesRead = FileReadToBuffer(buffer, fileName);

	if (bytesRead <= 0) {
		return -1; // Reading file into buffer failed
	}

	// Convert buffer to string
	outString.assign(buffer.begin(), buffer.end());
	return bytesRead;
}

bool FileWriteFromBuffer(std::vector<uint8_t> const& buffer, std::string const& filePathName)
{
	FILE* file = nullptr;
	errno_t result = fopen_s(&file, filePathName.c_str(), "wb");
	if (result != 0 || file == nullptr)
	{
		return false;
	}
	fwrite(buffer.data(), 1, buffer.size(), file);
	fclose(file);
	return true;
}

//bool CreateDirectoryA(std::string const& folderPathName)
//{
//	return CreateDirectoryA(folderPathName.c_str(), nullptr);
//}

