// Simple Windows performance manager (Windows Performance Counter API) // 
#include <iostream>
#include <windows.h>
#include <pdh.h>
#pragma comment(lib, "pdh.lib")

#define ERROR_BUFFER_SIZE 256

// Retrieve CPU usage
double GetCPUUsage() {
    PDH_HQUERY query;
    PDH_HCOUNTER counter;
    PDH_FMT_COUNTERVALUE value;

    PDH_STATUS status = PdhOpenQuery(NULL, 0, &query);
    if (status != ERROR_SUCCESS) {
        std::cerr << "Failed to open query: " << status << std::endl;
        return -1.0;
    }

    status = PdhAddCounter(query, L"\\Processor(_Total)\\% Processor Time", 0, &counter);
    if (status != ERROR_SUCCESS) {
        PdhCloseQuery(query);
        std::cerr << "Failed to add counter: " << status << std::endl;
        return -1.0;
    }

    status = PdhCollectQueryData(query);
    if (status != ERROR_SUCCESS) {
        PdhCloseQuery(query);
        std::cerr << "Failed to collect query data: " << status << std::endl;
        return -1.0;
    }

    Sleep(1000);

    status = PdhCollectQueryData(query);
    if (status != ERROR_SUCCESS) {
        PdhCloseQuery(query);
        std::cerr << "Failed to collect query data: " << status << std::endl;
        return -1.0;
    }

    status = PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, NULL, &value);
    if (status != ERROR_SUCCESS) {
        PdhCloseQuery(query);
        std::cerr << "Failed to get formatted counter value: " << status << std::endl;
        return -1.0;
    }

    PdhCloseQuery(query);
    return value.doubleValue;
}

// Retrieve memory usage
MEMORYSTATUSEX GetMemoryUsage() {
    MEMORYSTATUSEX memoryStatus;
    memoryStatus.dwLength = sizeof(memoryStatus);
    GlobalMemoryStatusEx(&memoryStatus);
    return memoryStatus;
}

int main() {
    // Buffer for error messages
    char errorBuffer[ERROR_BUFFER_SIZE];

    // COM library for certain APIs
    CoInitializeEx(NULL, COINIT_MULTITHREADED);

    double cpuUsage = GetCPUUsage();
    if (cpuUsage != -1.0) {
        std::cout << "CPU -> " << cpuUsage << "%" << std::endl;
    }
    else {
        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 0, errorBuffer, ERROR_BUFFER_SIZE, NULL);
        std::cerr << "Failed to retrieve CPU usage: " << errorBuffer << std::endl;
    }

    MEMORYSTATUSEX memoryStatus = GetMemoryUsage();
    std::cout << "Memory ->" << std::endl;
    std::cout << "  Total Physical Memory -> " << memoryStatus.ullTotalPhys / (1024 * 1024) << " MB" << std::endl;
    std::cout << "  Available Physical Memory -> " << memoryStatus.ullAvailPhys / (1024 * 1024) << " MB" << std::endl;
    std::cout << "  Total Virtual Memory -> " << memoryStatus.ullTotalVirtual / (1024 * 1024) << " MB" << std::endl;
    std::cout << "  Available Virtual Memory -> " << memoryStatus.ullAvailVirtual / (1024 * 1024) << " MB" << std::endl;

    CoUninitialize();

    return 0;
}
