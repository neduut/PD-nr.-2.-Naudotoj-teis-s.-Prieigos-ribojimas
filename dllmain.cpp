#include "pch.h"
#include <string>
#include <cstdlib>
#include <fstream>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <vector>
#include <algorithm>


// Timing variables
static std::chrono::high_resolution_clock::time_point g_startTime;
static std::chrono::high_resolution_clock::time_point g_endTime;


extern "C" __declspec(dllexport) void StartTimer()
{
    g_startTime = std::chrono::high_resolution_clock::now();
}

extern "C" __declspec(dllexport) void StopTimer()
{
    g_endTime = std::chrono::high_resolution_clock::now();
}

extern "C" __declspec(dllexport) double GetElapsedSeconds()
{
    std::chrono::duration<double> elapsed = g_endTime - g_startTime;
    return elapsed.count();
}

extern "C" __declspec(dllexport) int SetUserTimeRestriction(const char* username, const char* schedule)
{
    if (username == nullptr || schedule == nullptr)
        return -1;

    std::string cmd = "net user ";
    cmd += username;
    cmd += " /time:";
    cmd += schedule;

    return system(cmd.c_str());
}

extern "C" __declspec(dllexport) int CreateWorkFolders()
{
    int result = 0;

    result += system("mkdir Davidaviciute");
    result += system("mkdir Davidaviciute\\Neda1");
    result += system("mkdir Davidaviciute\\Neda2");
    result += system("mkdir Davidaviciute\\Neda3");

    result += system("mkdir Davidaviciute\\Neda1\\Neda1Davidaviciute1");
    result += system("mkdir Davidaviciute\\Neda1\\Neda1Davidaviciute2");
    result += system("mkdir Davidaviciute\\Neda1\\Neda1Davidaviciute3");

    result += system("mkdir Davidaviciute\\Neda2\\Neda2Davidaviciute1");
    result += system("mkdir Davidaviciute\\Neda2\\Neda2Davidaviciute2");
    result += system("mkdir Davidaviciute\\Neda2\\Neda2Davidaviciute3");

    result += system("mkdir Davidaviciute\\Neda3\\Neda3Davidaviciute1");
    result += system("mkdir Davidaviciute\\Neda3\\Neda3Davidaviciute2");
    result += system("mkdir Davidaviciute\\Neda3\\Neda3Davidaviciute3");

    return result;
}

extern "C" __declspec(dllexport) int CreateWorkFiles()
{
    const char* files[] =
    {
        "Davidaviciute\\Neda1\\Neda1Davidaviciute1\\data.txt",
        "Davidaviciute\\Neda1\\Neda1Davidaviciute2\\data.txt",
        "Davidaviciute\\Neda1\\Neda1Davidaviciute3\\data.txt",

        "Davidaviciute\\Neda2\\Neda2Davidaviciute1\\data.txt",
        "Davidaviciute\\Neda2\\Neda2Davidaviciute2\\data.txt",
        "Davidaviciute\\Neda2\\Neda2Davidaviciute3\\data.txt",

        "Davidaviciute\\Neda3\\Neda3Davidaviciute1\\data.txt",
        "Davidaviciute\\Neda3\\Neda3Davidaviciute2\\data.txt",
        "Davidaviciute\\Neda3\\Neda3Davidaviciute3\\data.txt"
    };

    int created = 0;

    for (int i = 0; i < 9; i++)
    {
        std::ofstream file(files[i], std::ios::trunc);
        if (!file.is_open())
            return -1;

        file.close();
        created++;
    }

    return created;
}

extern "C" __declspec(dllexport) int ClearWorkFiles()
{
    const char* files[] =
    {
        "Davidaviciute\\Neda1\\Neda1Davidaviciute1\\data.txt",
        "Davidaviciute\\Neda1\\Neda1Davidaviciute2\\data.txt",
        "Davidaviciute\\Neda1\\Neda1Davidaviciute3\\data.txt",

        "Davidaviciute\\Neda2\\Neda2Davidaviciute1\\data.txt",
        "Davidaviciute\\Neda2\\Neda2Davidaviciute2\\data.txt",
        "Davidaviciute\\Neda2\\Neda2Davidaviciute3\\data.txt",

        "Davidaviciute\\Neda3\\Neda3Davidaviciute1\\data.txt",
        "Davidaviciute\\Neda3\\Neda3Davidaviciute2\\data.txt",
        "Davidaviciute\\Neda3\\Neda3Davidaviciute3\\data.txt"
    };

    for (int i = 0; i < 9; i++)
    {
        std::ofstream file(files[i], std::ios::trunc);
        if (!file.is_open())
            return -1;
    }

    return 9;
}

extern "C" __declspec(dllexport) int DeleteWorkFolders()
{
    return system("rmdir /s /q Davidaviciute");
}

extern "C" __declspec(dllexport)
long long CalculateTschirnhausenAndWrite(
    double F,
    double x_start,
    double x_end,
    double x_step
)
{
    const char* files[] =
    {
        "Davidaviciute\\Neda1\\Neda1Davidaviciute1\\data.txt",
        "Davidaviciute\\Neda1\\Neda1Davidaviciute2\\data.txt",
        "Davidaviciute\\Neda1\\Neda1Davidaviciute3\\data.txt",

        "Davidaviciute\\Neda2\\Neda2Davidaviciute1\\data.txt",
        "Davidaviciute\\Neda2\\Neda2Davidaviciute2\\data.txt",
        "Davidaviciute\\Neda2\\Neda2Davidaviciute3\\data.txt",

        "Davidaviciute\\Neda3\\Neda3Davidaviciute1\\data.txt",
        "Davidaviciute\\Neda3\\Neda3Davidaviciute2\\data.txt",
        "Davidaviciute\\Neda3\\Neda3Davidaviciute3\\data.txt"
    };

    std::ofstream ofs[9];
    for (int i = 0; i < 9; ++i)
    {
        ofs[i].open(files[i], std::ios::app);
        if (!ofs[i].is_open())
            return -1;
        ofs[i].setf(std::ios::fixed);
        ofs[i].precision(8); // or 12 if you need more digits
    }

    std::string buffers[9];
    const size_t FLUSH_SIZE = 1 << 20; // ~1MB

    long long pointCount = 0;
    int fileIndex = 0;
    long long steps = static_cast<long long>((x_end - x_start) / x_step);

    for (long long i = 0; i <= steps; i++)
    {
        double x = x_start + i * x_step;
        double expr = x * x * x + 3.0 * x * x - F;

        if (expr < 0.0)
            continue;

        double y = std::sqrt(expr);

        int idx = fileIndex % 9;
        {
            std::ostringstream oss;
            oss << x << " " << y << "\n";
            buffers[idx] += oss.str();
            if (buffers[idx].size() > FLUSH_SIZE)
            {
                ofs[idx] << buffers[idx];
                buffers[idx].clear();
            }
        }
        pointCount++;
        fileIndex++;

        if (std::abs(y) > 1e-12)
        {
            idx = fileIndex % 9;
            std::ostringstream oss;
            oss << x << " " << -y << "\n";
            buffers[idx] += oss.str();
            if (buffers[idx].size() > FLUSH_SIZE)
            {
                ofs[idx] << buffers[idx];
                buffers[idx].clear();
            }
            pointCount++;
            fileIndex++;
        }
    }

    // Flush remaining buffers
    for (int i = 0; i < 9; ++i)
    {
        if (!buffers[i].empty())
            ofs[i] << buffers[i];
        ofs[i].close();
    }

    return pointCount;
}

// Helper structure to store and sort the points
struct Point {
    double x;
    double y;

    // Sorting logic: prioritize x ascending, then y ascending if x is equal
    bool operator<(const Point& other) const {
        if (x != other.x) {
            return x < other.x;
        }
        return y < other.y;
    }
};

extern "C" __declspec(dllexport) int MergeFilesForF(double F)
{
    const char* files[] =
    {
        "Davidaviciute\\Neda1\\Neda1Davidaviciute1\\data.txt",
        "Davidaviciute\\Neda1\\Neda1Davidaviciute2\\data.txt",
        "Davidaviciute\\Neda1\\Neda1Davidaviciute3\\data.txt",

        "Davidaviciute\\Neda2\\Neda2Davidaviciute1\\data.txt",
        "Davidaviciute\\Neda2\\Neda2Davidaviciute2\\data.txt",
        "Davidaviciute\\Neda2\\Neda2Davidaviciute3\\data.txt",

        "Davidaviciute\\Neda3\\Neda3Davidaviciute1\\data.txt",
        "Davidaviciute\\Neda3\\Neda3Davidaviciute2\\data.txt",
        "Davidaviciute\\Neda3\\Neda3Davidaviciute3\\data.txt"
    };

    std::vector<Point> allPoints;

    // 1 & 2. Read all files and store the points
    for (int i = 0; i < 9; ++i)
    {
        std::ifstream ifs(files[i]);
        if (!ifs.is_open())
            continue;

        double x, y;
        while (ifs >> x >> y)
        {
            allPoints.push_back({ x, y });
        }
        ifs.close();
    }

    // 3. Sort points according to requirements
    std::sort(allPoints.begin(), allPoints.end());

    // 4. Construct output filename based on F
    std::string outFilename = "result_F_";
    if (F < 0.0)
    {
        outFilename += "minus_";
        outFilename += std::to_string(static_cast<int>(std::round(-F)));
    }
    else
    {
        outFilename += std::to_string(static_cast<int>(std::round(F)));
    }
    outFilename += ".txt";

    // Write all sorted points into the output file
    std::ofstream ofs(outFilename, std::ios::trunc);
    if (!ofs.is_open())
        return -1;

    ofs.setf(std::ios::fixed);
    ofs.precision(8); // keeping consistency with write function

    for (const auto& pt : allPoints)
    {
        ofs << pt.x << " " << pt.y << "\n";
    }

    ofs.close();

    // 5. Return the number of merged points
    return static_cast<int>(allPoints.size());
}