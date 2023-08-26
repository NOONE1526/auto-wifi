#include <vector>
#include <string>
#include <fstream>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <thread>

std::vector<std::string> spliting(std::string answer)
{
    std::vector<std::string> res;
    boost::split(res, answer, boost::is_any_of("\n\r"));
    return res;
}

std::vector<int> getSignal() {
    const char* command = "netsh wlan show networks mode=bssid > output.txt";
    std::system(command);

    std::ifstream file("output.txt");
    std::string result((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    bool zte = 0, notZte = 0, ctrl = 0;
    std::vector<std::string> temp;
    std::vector<std::string> signal{ "00", "00" };

    std::vector<std::string> res = spliting(result);
    for (int i = 0; i < res.size(); i++) {
        if (res[i].find("SSID") != std::string::npos) { ctrl = 1; }
        if (res[i].find("FIRST NAME OF WIFI") != std::string::npos && notZte == 0 && ctrl == 1) {
            for (; i < res.size(); i++) {
                if (res[i].find("Signal") != std::string::npos) {
                    boost::split(temp, res[i], boost::is_any_of(" "));
                    for (int k = 0; k < temp.size(); k++) {
                        if (temp[k] == ":") { signal[0] = temp[k + 1]; notZte = 1; ctrl = 1; break; }
                    }
                    break;
                }
            }
        }
        else if (res[i].find("SECOND NAME OF WIFI") != std::string::npos && zte == 0 && ctrl == 1) {
            for (; i < res.size(); i++) {
                if (res[i].find("Signal") != std::string::npos) {
                    boost::split(temp, res[i], boost::is_any_of(" "));
                    for (int k = 0; k < temp.size(); k++) {
                        if (temp[k] == ":") { signal[1] = temp[k + 1]; zte = 1; ctrl = 1; break; }
                    }
                    break;
                }
            }
        }
    }

    // connecting to best network below
    int n1 = stoi(signal[0].erase(signal[0].size() - 1));
    int n2 = stoi(signal[1].erase(signal[1].size() - 1));
    std::vector<int> result2 {n1, n2};
    std::remove("output.txt");
    return result2;
}

int logic(std::vector<int> res) {
    //0 -> connect to FIRST WIFI, 1 -> connect to SECOND WIFI
    const char* command = "netsh wlan show interfaces > output.txt";
    std::system(command); 
    std::vector <std::string> out;
    std::ifstream file("output.txt"); 
    std::string result((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>()); 
    boost::split(out, result, boost::is_any_of("\n\r"));
    int var = -1;
    for (int i = 0; i < out.size(); i++) {
        if (out[i].find("SSID") != std::string::npos) {
            if (out[i].find("SECOND WIFI") != std::string::npos) { var = 1; }
            else if (out[i].find("FIRST WIFI") != std::string::npos) { var = 0; }
        }
    }
    std::remove("output.txt");
    if (res[0] > res[1] && var != 0) { return 0; }
    else if (res[0] < res[1] && var != 1) { return 1; }
    return 3;
    }

int main() {
    using namespace std::chrono_literals;
    bool ctrl = 1;
    std::vector<int> answer;
    while ( ctrl ) {
        answer = getSignal();
        int res = logic(answer);
        if (res == 0) { std::system("netsh wlan connect name=FIRST WIFI"); }
        else if (res == 1) { std::system("netsh wlan connect name=SECOND WIFI"); }
        const auto start = std::chrono::high_resolution_clock::now();
        std::this_thread::sleep_for(2s);
        const auto end = std::chrono::high_resolution_clock::now();
    }
    return 0;
}
