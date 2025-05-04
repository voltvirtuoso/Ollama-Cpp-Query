#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

struct ModelInfo {
    std::string name;
    uint64_t size;
};

// Callback for libcurl to write response data into a string
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    output->append(static_cast<char*>(contents), size * nmemb);
    return size * nmemb;
}

// Fetch JSON from a URL
json fetchJsonFromUrl(const std::string& url) {
    std::string response;
    CURL* curl = curl_easy_init();
    json result = nullptr;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "❌ Error fetching JSON: " << curl_easy_strerror(res) << "\n";
        } else {
            try {
                result = json::parse(response);
            } catch (const json::parse_error& e) {
                std::cerr << "❌ JSON parse error: " << e.what() << "\n";
            }
        }
        curl_easy_cleanup(curl);
    }
    return result;
}

// Get list of Ollama models
std::vector<ModelInfo> getOllamaModels() {
    std::vector<ModelInfo> models;
    json data = fetchJsonFromUrl("http://localhost:11434/api/tags");

    if (data.is_object() && data.contains("models") && data["models"].is_array()) {
        for (const auto& modelJson : data["models"]) {
            if (modelJson.contains("name") && modelJson.contains("size")) {
                ModelInfo model;
                model.name = modelJson["name"];
                model.size = modelJson["size"];
                models.push_back(model);
            }
        }
    }
    return models;
}

// Find a default model (e.g., starts with 'llama2')
std::string findDefaultModel(const std::vector<ModelInfo>& models) {
    for (const auto& model : models) {
        if (model.name.rfind("llama2", 0) == 0) {
            return model.name;
        }
    }
    return !models.empty() ? models[0].name : "";
}

// Convert size in bytes to human-readable string
std::string formatSize(uint64_t bytes) {
    const char* units[] = {"B", "KB", "MB", "GB"};
    int unitIndex = 0;
    double size = bytes;

    while (size >= 1024 && unitIndex < 3) {
        size /= 1024;
        ++unitIndex;
    }
    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%.1f %s", size, units[unitIndex]);
    return std::string(buffer);
}

// Print available models
void printAvailableModels(const std::vector<ModelInfo>& models, const std::string& defaultModel) {
    std::cout << "\n📦 Available Models:\n";
    for (size_t i = 0; i < models.size(); ++i) {
        const auto& model = models[i];
        std::string prefix = (model.name == defaultModel) ? "➡️  " : "   ";
        std::cout << prefix << (i + 1) << ". " << model.name << " (" << formatSize(model.size) << ")\n";
    }
}

// Prompt user to select a model
std::string selectModel(const std::vector<ModelInfo>& models, const std::string& defaultModel) {
    while (true) {
        std::cout << "\nEnter model number (default is " << defaultModel << ", press Enter): ";
        std::string input;
        std::getline(std::cin, input);
        if (input.empty()) {
            return defaultModel;
        }
        if (input == "q" || input == "Q") {
            return "";
        }
        try {
            int num = std::stoi(input);
            if (num >= 1 && num <= models.size()) {
                return models[num - 1].name;
            } else {
                std::cout << "❌ Invalid choice. Please try again.\n";
            }
        } catch (...) {
            std::cout << "❌ Invalid input. Please enter a number.\n";
        }
    }
}

// Send prompt to Ollama
std::string queryOllama(const std::string& prompt, const std::string& model) {
    std::string response;
    CURL* curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:11434/api/generate");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_POST, 1L);

        // Create JSON payload
        json payload = {
            {"model", model},
            {"prompt", prompt},
            {"stream", false}
        };
        std::string payloadStr = payload.dump();

        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payloadStr.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, payloadStr.size());

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "❌ Error querying Ollama: " << curl_easy_strerror(res) << "\n";
            response.clear();
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
    return response;
}

int main() {
    std::cout << "🧠 Ollama Interactive Query Tool\n";
    std::cout << "Initializing... Connecting to Ollama server.\n";

    std::vector<ModelInfo> models = getOllamaModels();

    if (models.empty()) {
        std::cout << "❌ No models available. Please pull a model in Ollama first.\n";
        return 1;
    }

    std::string defaultModel = findDefaultModel(models);
    printAvailableModels(models, defaultModel);

    std::string selectedModel = selectModel(models, defaultModel);
    if (selectedModel.empty()) {
        std::cout << "👋 Exiting Ollama Query Tool. Goodbye!\n";
        return 0;
    }

    while (true) {
        std::cout << "\n" << std::string(60, '-') << "\n";
        std::cout << "Enter your query (type 'q' to quit): ";
        std::string userInput;
        std::getline(std::cin, userInput);

        if (userInput == "q" || userInput == "Q") {
            std::cout << "\n👋 Exiting Ollama Query Tool. Goodbye!\n";
            break;
        }

        if (userInput.empty()) {
            std::cout << "⚠️  Empty input. Please enter a valid query.\n";
            continue;
        }

        std::string jsonResponse = queryOllama(userInput, selectedModel);
        if (jsonResponse.empty()) {
            std::cout << "❌ No response from Ollama.\n";
            continue;
        }

        try {
            json responseJson = json::parse(jsonResponse);
            std::string result = responseJson.value("response", "No response returned.");
            std::cout << "\n" << result << "\n";
        } catch (const json::parse_error& e) {
            std::cout << "❌ Error parsing response: " << e.what() << "\n";
        }
    }

    return 0;
}
