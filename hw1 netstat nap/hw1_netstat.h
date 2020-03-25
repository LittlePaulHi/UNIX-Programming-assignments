#include <string.h>
#include <getopt.h>
#include <string>

typedef struct
{
    pid_t pid;
    std::string procName;
} ProcInfo;

const char* const short_opts = "tu";
const option long_opts[] =
{
    {"tcp", no_argument, nullptr, 't'},
    {"udp", no_argument, nullptr, 'u'},
    {nullptr, no_argument, nullptr, 0}
};

const std::string TcpStr = "tcp";
const std::string Tcp6Str = "tcp6";
const std::string UdpStr = "udp";
const std::string Udp6Str = "udp6";
