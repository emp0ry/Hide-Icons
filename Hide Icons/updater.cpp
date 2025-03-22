#include "updater.h"
#include <fstream>
#include <sstream>
#include <windows.h>
#include <wininet.h>
#include <nlohmann/json.hpp>

#pragma comment(lib, "wininet.lib")

Updater::Updater(const std::string& currentVersion, const std::string& githubAuthor, const std::string& repoName, const std::string& downloadExeName) :
    currentVersion(currentVersion),
    currentExeName(getCurrentExeName()),
    downloadExeName(downloadExeName + ".exe"),
    releasesUrl("https://api.github.com/repos/" + githubAuthor + "/" + repoName + "/releases/latest"),
    githubAuthor(githubAuthor),
    repoName(repoName) {
    if (currentExeName.empty()) {
        currentExeName = downloadExeName;
    }
}

std::string Updater::getCurrentExeName() {
    char exePath[MAX_PATH];
    DWORD pathLength = GetModuleFileNameA(NULL, exePath, MAX_PATH);
    if (pathLength == 0) {
        return "";
    }

    std::string fullPath(exePath, pathLength);
    size_t lastSlash = fullPath.find_last_of("\\/");
    if (lastSlash != std::string::npos) {
        return fullPath.substr(lastSlash + 1);
    }
    return fullPath;
}

std::wstring Updater::stringToWstring(const std::string& str) {
    int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    std::wstring wstr(size, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size);
    return wstr;
}

std::string Updater::downloadJsonData(const std::string& url) {
    std::string jsonData;
    std::wstring wideUrl = stringToWstring(url);

    HINTERNET hInternet = InternetOpenW(L"ProgramUpdater", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (hInternet) {
        HINTERNET hConnect = InternetOpenUrlW(hInternet, wideUrl.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
        if (hConnect) {
            std::ostringstream ss;
            char buffer[4096];
            DWORD bytesRead;
            while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
                ss.write(buffer, bytesRead);
            }
            jsonData = ss.str();
            InternetCloseHandle(hConnect);
        }
        InternetCloseHandle(hInternet);
    }
    return jsonData;
}

bool Updater::downloadFile(const std::string& url, const std::string& outputPath) {
    std::wstring wideUrl = stringToWstring(url);

    HINTERNET hInternet = InternetOpenW(L"ProgramUpdater", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) {
        return false;
    }

    HINTERNET hConnect = InternetOpenUrlW(hInternet, wideUrl.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (!hConnect) {
        InternetCloseHandle(hInternet);
        return false;
    }

    DWORD contentLength = 0;
    DWORD bufferLength = sizeof(contentLength);
    DWORD index = 0;

    HttpQueryInfoW(hConnect, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, &contentLength, &bufferLength, &index);

    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile) {
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return false;
    }

    char buffer[4096];
    DWORD bytesRead;
    size_t totalBytes = 0;
    while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead)) {
        if (bytesRead == 0) break;
        outFile.write(buffer, bytesRead);
        totalBytes += bytesRead;
    }

    outFile.close();
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    if (contentLength > 0 && totalBytes != contentLength) {
        std::remove(outputPath.c_str());
        return false;
    }

    return true;
}

bool Updater::isValidExecutable(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        return false;
    }

    char header[2];
    file.read(header, 2);
    file.close();
    bool isValid = (header[0] == 'M' && header[1] == 'Z');

    return isValid;
}

bool Updater::replaceExecutable(const std::string& newFilePath) {
    if (!isValidExecutable(newFilePath)) {
        return false;
    }

    std::string backupPath = currentExeName + "_old.exe";
    std::string currentPath = currentExeName;
    std::string batPath = "update.bat";

    std::ofstream batFile(batPath);
    if (!batFile) {
        return false;
    }

    batFile << "@echo off\n";
    batFile << "timeout /t 1 >nul\n";
    batFile << "move \"" << currentPath << "\" \"" << backupPath << "\"\n";
    batFile << "move \"" << newFilePath << "\" \"" << currentPath << "\"\n";
    batFile << "start \"\" \"" << currentPath << "\"\n";
    batFile << "del \"" << backupPath << "\"\n";
    batFile << "del \"%~f0\"\n";
    batFile.close();

    ShellExecuteA(NULL, "open", batPath.c_str(), NULL, NULL, SW_HIDE);
    return true;
}

bool Updater::checkAndUpdate() {
    std::string jsonData = downloadJsonData(releasesUrl);
    if (jsonData.empty()) {
        return false;
    }

    try {
        auto json = nlohmann::json::parse(jsonData);
        std::string latestVersion = json["tag_name"].get<std::string>().substr(1);

        if (latestVersion > currentVersion) {

            std::string downloadUrl = "https://github.com/" + githubAuthor + "/" + repoName + "/releases/download/v" + latestVersion + "/" + downloadExeName;
            std::string tempPath = currentExeName + "_new.exe";

            if (downloadFile(downloadUrl, tempPath)) {
                if (replaceExecutable(tempPath)) {
                    return true;
                }
            }
            return false;
        }
        else {
            return false;
        }
    }
    catch (const nlohmann::json::exception& e) {
        return false;
    }
}