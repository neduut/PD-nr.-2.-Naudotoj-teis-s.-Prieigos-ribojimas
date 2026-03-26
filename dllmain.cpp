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


// timing variables
static std::chrono::high_resolution_clock::time_point g_startTime;
static std::chrono::high_resolution_clock::time_point g_endTime;

static const std::string SURNAME = "Davidaviciute";
static const std::string NAME = "Neda";


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

// helper functions to generate folder and file paths
static std::vector<std::string> GetWorkFolderPaths()
{
    std::vector<std::string> folders;

    folders.push_back(SURNAME);

    for (int i = 1; i <= 3; i++)
    {
        std::string level1 = SURNAME + "\\" + NAME + std::to_string(i);
        folders.push_back(level1);

        for (int j = 1; j <= 3; j++)
        {
            std::string level2 = level1 + "\\" + NAME + std::to_string(i) + SURNAME + std::to_string(j);
            folders.push_back(level2);
        }
    }

    return folders;
}

static std::vector<std::string> GetWorkFilePaths()
{
    std::vector<std::string> files;

    for (int i = 1; i <= 3; i++)
    {
        std::string level1 = SURNAME + "\\" + NAME + std::to_string(i);

        for (int j = 1; j <= 3; j++)
        {
            std::string path = level1 + "\\" + NAME + std::to_string(i) + SURNAME + std::to_string(j) + "\\data.txt";
            files.push_back(path);
        }
    }

    return files;
}

extern "C" __declspec(dllexport) int CreateWorkFolders()
{
    int result = 0;
    std::vector<std::string> folders = GetWorkFolderPaths();

    for (const auto& folder : folders)
    {
        std::string cmd = "mkdir \"" + folder + "\"";
        result += system(cmd.c_str());
    }

    return result;
}

extern "C" __declspec(dllexport) int CreateWorkFiles()
{
    std::vector<std::string> files = GetWorkFilePaths();
    int created = 0;

    for (const auto& path : files)
    {
        std::ofstream file(path, std::ios::trunc);
        if (!file.is_open())
            return -1;

        file.close();
        created++;
    }

    return created;
}

extern "C" __declspec(dllexport) int ClearWorkFiles()
{
    std::vector<std::string> files = GetWorkFilePaths();

    for (const auto& path : files)
    {
        std::ofstream file(path, std::ios::trunc);
        if (!file.is_open())
            return -1;
    }

    return static_cast<int>(files.size());
}

extern "C" __declspec(dllexport) int DeleteWorkFolders()
{
    std::string cmd = "rmdir /s /q \"" + SURNAME + "\"";
    return system(cmd.c_str());
}

extern "C" __declspec(dllexport)
long long CalculateTschirnhausenAndWrite(
    double F,
    double x_start,
    double x_end,
    double x_step
)
{
    std::vector<std::string> files = GetWorkFilePaths();

    std::ofstream ofs[9];
    for (int i = 0; i < 9; ++i)
    {
        ofs[i].open(files[i], std::ios::app);
        if (!ofs[i].is_open())
            return -1;

        ofs[i].setf(std::ios::fixed);
        ofs[i].precision(8);
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
    std::vector<Point> allPoints;
    std::vector<std::string> files = GetWorkFilePaths();

    // 1 & 2 read all files and store the points
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

    // 3.sort points according to requirements
    std::sort(allPoints.begin(), allPoints.end());

    // 4. construct output filename based on F
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

    // write all sorted points into the output file
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

    // clear intermediate files
    ClearWorkFiles();

    // 5. return the number of merged points
    return static_cast<int>(allPoints.size());
}