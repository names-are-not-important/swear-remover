#include <windows.h>
#include <fstream>
#include <iostream>
#include <thread>
#include <chrono>
#include "words.h"


void SendKey(WORD key, DWORD flags) {
    INPUT input = {};  // Create an INPUT structure

    input.type = INPUT_KEYBOARD;
    input.ki.wVk = key;  // Virtual key code for the key
    input.ki.dwFlags = flags;  // KEYEVENTF_KEYDOWN or KEYEVENTF_KEYUP

    // Send the input event (either key press or key release)
    SendInput(1, &input, sizeof(INPUT));
}



HHOOK hHook;


std::string currentword = "";
std::string currentletters = "";

// This callback gets called on every key event
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* pKeyBoard = (KBDLLHOOKSTRUCT*)lParam;
        DWORD vkCode = pKeyBoard->vkCode;
        if(vkCode == VK_ESCAPE){
            std::cout << "Bye!";
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            PostQuitMessage(0);
        }
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            char key;
            if(vkCode == VK_BACK){
                if(currentletters != ""){
                currentletters.pop_back();
                }
            }   
            
            // Only log printable characters
            if ((vkCode >= 0x30 && vkCode <= 0x5A) || // Letters/numbers
                (vkCode >= 0x60 && vkCode <= 0x6F) || vkCode == 32)  // Numpad
            {
                key = static_cast<char>(vkCode);

                if(vkCode == 32){ //32 is the spacebar
                
                currentword = currentletters;
                
                for(auto& c : currentword){
                     c = tolower(c); //make it lowercase
                }
                    int len = 0;
                    for(auto _: curses){
                      len++;
                    }
                for (int i = 0; i < len; i++){
                    if(currentword == curses[i][0]){
                        currentword = curses[i][1];
                        std::cout << curses[i][0] << " replaced with: " << curses[i][1] << "\n";
                        SendKey(VK_CONTROL, 0);  // Key down for Ctrl                 
                        SendKey(VK_BACK, 0);  // Key down for Backspace             
                        SendKey(VK_BACK, KEYEVENTF_KEYUP);  // Key up for Backspace
                        SendKey(VK_CONTROL, KEYEVENTF_KEYUP);  // Key up for Ctrl

                        for(auto b: curses[i][1]){
                        SendKey(VkKeyScan(b), 0);  // Key down for Backspace             
                        SendKey(VkKeyScan(b), KEYEVENTF_KEYUP);  // Key up for Backspace
                        }
                        
                    }
                }
                
                currentletters = "";
                currentword = "";
                }else{
                    currentletters = currentletters + key; //add next letter

                }

            } 
        }
    }

    return CallNextHookEx(hHook, nCode, wParam, lParam);
}

int main() {
    for (const auto& line : art) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        std::cout << line << std::endl;
    }
    std::cout << "Welcome to Curse remover!";
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << std::endl;
    std::cout << "Press esc to exit.";
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << std::endl;
    std::cout << "Your ready to go Just start typing";
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << std::endl;
    // Set the hook
    hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
    if (!hHook) {
        MessageBoxW(NULL, L"Failed to install hook!", L"Error", MB_ICONERROR);
        return 1;
    }

   

    // Message loop to keep the hook alive
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    // Cleanup
    UnhookWindowsHookEx(hHook);
    return 0;
}
