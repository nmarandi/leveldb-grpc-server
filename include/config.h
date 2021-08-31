#ifndef CONFIG_H
#define CONFIG_H
#include "string"

namespace leveldb
{
namespace config
{
#if defined(_WIN64) || defined(WIN64) || defined(_WIN32) || defined(WIN32)
const std::string db_file_base_path = "c:\\Users\\Public\\";
#else
const std::string db_file_base_path = "/tmp/";
#endif
}
}

#endif // CONFIG_H
