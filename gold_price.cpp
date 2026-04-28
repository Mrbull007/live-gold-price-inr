#include <iostream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <iomanip>
#include <chrono>
#include <thread>

#ifdef _WIN32
    #include <windows.h>
#endif

using json = nlohmann::json;

// Callback function to handle response data
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void setColor(int color) {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
#endif
}

int main() {
    std::cout << "\n";
    setColor(14); // Yellow
    std::cout << "========================================\n";
    std::cout << "     LIVE GOLD PRICE IN INDIA (INR)     \n";
    std::cout << "========================================\n\n";

    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    // Replace with your free API key from https://www.goldapi.io/
    std::string api_key = "YOUR_API_KEY_HERE";   // ←←← CHANGE THIS

    if (api_key == "YOUR_API_KEY_HERE") {
        setColor(12); // Red
        std::cout << "❌ Please replace YOUR_API_KEY_HERE with actual key!\n";
        std::cout << "Get free key: https://www.goldapi.io/\n";
        return 1;
    }

    curl = curl_easy_init();
    if (curl) {
        std::string url = "https://www.goldapi.io/api/XAU/INR";
        
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        
        // Headers
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, ("x-access-token: " + api_key).c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        setColor(11); // Cyan
        std::cout << "Fetching live gold price...\n\n";

        res = curl_easy_perform(curl);

        if (res == CURLE_OK) {
            try {
                json data = json::parse(readBuffer);
                
                double price_24k = data["price_gram_24k"];  // per gram
                
                double price_22k = price_24k * 22.0 / 24.0;
                double price_20k = price_24k * 20.0 / 24.0;
                double price_18k = price_24k * 18.0 / 24.0;

                auto now = std::chrono::system_clock::now();
                time_t now_time = std::chrono::system_clock::to_time_t(now);

                setColor(10); // Green
                std::cout << "✅ Updated: " << ctime(&now_time);
                std::cout << "======================================\n";

                setColor(14); // Yellow
                std::cout << std::fixed << std::setprecision(2);
                
                std::cout << "24K Gold : ₹" << price_24k << " / gram\n";
                std::cout << "22K Gold : ₹" << price_22k << " / gram\n";
                std::cout << "20K Gold : ₹" << price_20k << " / gram\n";
                std::cout << "18K Gold : ₹" << price_18k << " / gram\n\n";

                std::cout << "======================================\n";
                setColor(7); // White
                std::cout << "Data from GoldAPI.io\n";

            } catch (...) {
                setColor(12);
                std::cout << "❌ Error parsing JSON. Check your API key.\n";
            }
        } else {
            setColor(12);
            std::cout << "❌ Failed to fetch data. Error: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
    }

    setColor(7);
    std::cout << "\nPress Enter to exit...";
    std::cin.get();
    return 0;
}
