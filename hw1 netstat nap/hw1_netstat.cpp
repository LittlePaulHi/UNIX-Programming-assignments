#include "hw1_netstat.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>
#include <unordered_map>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <error.h>
#include <getopt.h>
#include <limits.h>
#include <stdio.h>

using namespace std;

string filterStr = "";
unordered_map<u_long, ProcInfo> dictOfProcs;

void traversePidFds(string path, string pidStr)  // path = "/proc/[pidStr]/fd/"
{
    DIR *dirOfPid = opendir(path.c_str());
    struct dirent *dp;
    if(dirOfPid == NULL)
    {
        // cerr << "Open proc/[pid]/fd failed: " << strerror(errno) << endl;
    }
    else
    {
        u_long inodeIndex;
        string tmpPath, tmpInodeStr, procName = "";
        char buffer[PATH_MAX];
        ssize_t len;
        // int tmpInode;
        while ((dp = readdir(dirOfPid)) != NULL)
        {
            bzero(&buffer, PATH_MAX);
            tmpPath = path + (dp->d_name);
            len = readlink(tmpPath.c_str(), buffer, sizeof(buffer) - 1);
            if (len != -1)
            {
                buffer[len] = '\0';
                if (string(buffer).find("socket") != string::npos)
                {
                    regex reg("socket:\\[(\\d+)");
                    auto result_start = sregex_iterator(string(buffer).begin(), string(buffer).end(), reg);
                    if (result_start != sregex_iterator())
                    {
                        smatch match = *result_start;
                        inodeIndex = stoul(match.str(1), nullptr, 0);

                        ifstream fin;
                        fin.open(regex_replace(path, std::regex("fd/"), "comm"));
                        if (fin.is_open())
                        {
                            ostringstream ss;
                            ss << fin.rdbuf();
                            procName = ss.str();
                        }
                        ProcInfo procinfo = {atoi(pidStr.c_str()), procName};
                        dictOfProcs[inodeIndex] = procinfo;
                        fin.close();
                    }
                }
            }
        }
    }
    closedir(dirOfPid);
}

void printProcInfos(u_long inodeIndex)
{
    if (dictOfProcs.find(inodeIndex) == dictOfProcs.end())
    {
        cout << "-\n";
    }
    else
    {
        cout << dictOfProcs[inodeIndex].pid << '/' << dictOfProcs[inodeIndex].procName;
    }
}

string hexIp6Convert(string hexIp6)
{
    struct in6_addr addr6;
    char ip6Str[INET6_ADDRSTRLEN];
    sscanf(hexIp6.c_str(), "%08X%08X%08X%08X",
            &addr6.s6_addr32[0], &addr6.s6_addr32[1], &addr6.s6_addr32[2], &addr6.s6_addr32[3]);
    inet_ntop(AF_INET6, &addr6, ip6Str, INET6_ADDRSTRLEN);

    return ip6Str;
}

void readNet(string proto, string pathIP4, string pathIP6)
{
    ifstream fin;
    string line, redundant;
    u_long inodeIndex;

    // IPv4
    fin.open(pathIP4);
    if (fin.fail())
    {
        cerr << "Open " << pathIP4 << " failed: " << strerror(errno) << endl;
    }
    else  // format: Proto, Local Address, Foreign(Remote) Address, PID/Program name and arguments
    {
        char colon;
        u_int localAddr4, localPort4, remAddr4, remPort4;
        struct in_addr addr;

        cout << "Proto\tLocal Address\t\tForeign Address\t\tPID/Program name and arguments\n";
        while (getline(fin, line))
        {
            istringstream iss(line);
            if (iss >> redundant >> hex >> localAddr4 >> colon >> hex >> localPort4
                    >> hex >> remAddr4 >> colon >> hex >> remPort4
                    // st           tx/rx_queue  tr tm-<when  retrnsmt     uid          timeout
                    >> redundant >> redundant >> redundant >> redundant >> redundant >> redundant
                    >> dec >> inodeIndex)
            {
                if (!filterStr.empty() && dictOfProcs[inodeIndex].procName.find(filterStr) == string::npos) { continue; }

                char ipStr[INET_ADDRSTRLEN];
                addr.s_addr = localAddr4;  //htonl(localAddr4);
                inet_ntop(AF_INET, &(addr), ipStr, INET_ADDRSTRLEN);
                cout << proto << '\t' << ipStr << ':' << dec << localPort4 << "\t\t";
                addr.s_addr = remAddr4;  //htonl(remAddr4);
                stringstream ss;
                ss << dec << remPort4;
                cout << inet_ntoa(addr) << ':' << (ss.str() == "0" ? "*" : ss.str()) << "\t\t";
                printProcInfos(inodeIndex);
            }
        }
    }
    fin.close();
    fin.clear();

    // IPv6
    proto += '6';
    fin.open(pathIP6);
    if (fin.fail())
    {
        cerr << "Open " << pathIP6 << " failed: " << strerror(errno) << endl;
    }
    else  // format: Proto, Local Address, Foreign(Remote) Address, PID/Program name and arguments
    {
        string localAddr6withPort, localPort6, remAddr6withPort, remPort6;

        while (getline(fin, line))
        {
            istringstream iss(line);
            if (iss >> redundant >> localAddr6withPort >> remAddr6withPort
                    // st           tx/rx_queue  tr tm-<when  retrnsmt     uid          timeout
                    >> redundant >> redundant >> redundant >> redundant >> redundant >> redundant
                    >> dec >> inodeIndex)
            {
                if (!filterStr.empty() && dictOfProcs[inodeIndex].procName.find(filterStr) == string::npos) { continue; }

                localPort6 = localAddr6withPort.substr(localAddr6withPort.find(":") + 1);
                localAddr6withPort = localAddr6withPort.substr(0, localAddr6withPort.find(":"));
                cout << proto << '\t' << hexIp6Convert(localAddr6withPort) << ':' << stoul(localPort6, nullptr, 16) << "\t\t\t";

                remPort6 = remAddr6withPort.substr(remAddr6withPort.find(":") + 1);
                remAddr6withPort = remAddr6withPort.substr(0, remAddr6withPort.find(":"));
                stringstream ss;
                ss << stoul(remPort6, nullptr, 16);
                cout << hexIp6Convert(remAddr6withPort) << ':' << (ss.str() == "0" ? "*" : ss.str()) << "\t\t\t";  // TODO: convert '*'
                printProcInfos(inodeIndex);
            }
        }
    }
    cout << endl;
    fin.close();
    fin.clear();
}

int main(int argc, char *argv[])
{
    int opt;
    bool isTcp = false, isUdp = false;
    while ((opt = getopt_long(argc, argv, short_opts, long_opts, nullptr)) != -1)
    {
        switch(opt)
        {
            case 't':
                isTcp = true;
                break;
            case 'u':
                isUdp = true;
                break;
        }
    }
    if ((isTcp == isUdp) && (isTcp == false)) { isTcp = isUdp = true; }

    // filter string
    if (argc >= 2)
    {
        string cmdLine = argv[1];
        regex reg("([^\\-]\\w*\\d*)");
        for (int i = 1; i < argc; i++)
        {
            if (regex_match(argv[i], reg))
            {
                filterStr += argv[i];
                filterStr += ' ';
            }
        }
        if (filterStr.length() > 1) { filterStr.pop_back(); }
    }

    DIR *dir = opendir("/proc");
    struct dirent *dp;
    if(dir == NULL)
    {
        cerr << "Open /proc failed: " << strerror(errno) << endl;
    }
    else
    {
        while ((dp = readdir(dir)) != NULL)
        {
            if (isdigit(*dp->d_name))
            {
                traversePidFds("/proc/" + string(dp->d_name) + "/fd/", string(dp->d_name));
            }
        }
    }
    closedir(dir);

    const string filePath = "/proc/net/";
    if (isTcp)
    {
        cout << "List of TCP connections:" << endl;
        readNet(TcpStr, filePath + TcpStr, filePath + Tcp6Str);
    }
    if (isUdp)
    {
        cout << "List of UDP connections:" << endl;
        readNet(UdpStr, filePath + UdpStr, filePath + Udp6Str);
    }

    return 0;
}