#include <string>
#include <iostream>

using namespace std;

class Main {
public:
    Main(int i);
    int getZahl();
private:
    std::string inputTxt;
    bool sendingText;
    int start;
    int txtLength;
    void setSendingText();
};

Main::Main(int i) {
    std::cout << "Hallo";
    inputTxt = "Nein. Ich mache das nicht! Halt stop!";
    sendingText = true;
    start = 0;
    txtLength = inputTxt.length();
}

int Main::getZahl() {
    return 19;
}

int main() {
    Main carObj(1);
    carObj.getZahl();

}
