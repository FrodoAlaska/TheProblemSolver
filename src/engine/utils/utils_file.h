#pragma once

#include "defines.h"

#include <string>
#include <fstream>

// Public functions
/////////////////////////////////////////////////////////////////////////////////
// NOTE: The functions where the file handle is passed, do not close the file. 
// You would need to do that yourself.
// However, wherever the path is passed, the file does get closed since the function is 
// the one who created the file in the first place.

// Get the total size in bytes of the file.
// NOTE: This function will only seek to the end to tell the size and 
// seek to the beginning once again. Meaning, it will not mess with it whatsoever.
const usizei file_get_size(std::ifstream& file);
const usizei file_get_size(const std::string& path);

// Returns 'true' if the file is empty and 'false' otherwise.
const bool file_is_empty(std::ifstream& file);
const bool file_is_empty(const std::string& path);

// Reads the whole file as a string and writes to 'out_str'
bool file_read_string(const std::ifstream& file, std::string* out_str);
bool file_read_string(const std::string& path, std::string* out_str);

// Reads the whole file as binary and writes to 'out_buff'
// NOTE: If you pass the file, you need to make sure the file is opened in binary mode. 
// NOTE: Also, 'out_buff' is your complete responsability. You will have to allocate and deallocate it.
// The function will not write to the buffer if it is a 'nullptr'
bool file_read_binary(std::ifstream& file, void* out_buff, const usizei buff_size);
bool file_read_binary(const std::string& path, void* out_buff, const usizei buff_size);

// Write the 'contents' string into the file. 
bool file_write_string(std::ofstream& file, const std::string& contents);
bool file_write_string(const std::string& path, const std::string& contents);

// Write the 'in_buff' buffer into the file. 
// NOTE: Like the 'file_read_binary' function, the file passed needs to have the 'std::ios::binary' flag. 
// NOTE: The 'in_buff' needs to be an allocated or valid pointer. The function will NOT write to the 
// file if the buffer is a 'nullptr'.
bool file_write_binary(std::ofstream& file, const void* in_buff, const usizei buff_size);
bool file_write_binary(const std::string& path, const void* in_buff, const usizei buff_size);
/////////////////////////////////////////////////////////////////////////////////
