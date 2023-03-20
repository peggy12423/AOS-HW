#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <arpa/inet.h>
#include <sstream>
#include <fstream>
#include <vector>
#include <sys/stat.h>

#define MAXLINE 4096 // maximum buffer size

using namespace std;

int main()
{
    int   sockfd, n;
    char  recvline[MAXLINE], sendline[MAXLINE];
//    char user_name[100] = {};
    struct sockaddr_in  servaddr;
//    string member_list="Peggy, Alan, Denial, Jim ,Weilber, Sam";

    if((sockfd =socket(AF_INET, SOCK_STREAM, 0)) == -1)  // create socket
    {
        cout << "create socket error" << endl;
    }

    memset(&servaddr, 0, sizeof(servaddr)); // clean servaddr
    servaddr.sin_family = AF_INET; // for IPv4
    servaddr.sin_port = htons(8888); // destination port
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // destination ipv4

    if(connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) == -1)
    { // connect to server
        cout << "connection error" << endl;
    }
	
    cout << "Entry your name: ";

    while(1) // loop for conversation
    { 
        //Send a message to server
        char user_name[100] = {};
        char receiveMessage[100] = {};
        char user_list[100] = {"Peggy , Alan , Denial , Jim , Weilber , Sam"};
        cin.getline(user_name, 100); //  user name get input (including space)

	//判斷user name是否已在可存取的名單內
	//user_name[strlen(user_name) -1] = '\0';
//	char *loc = strstr(user_list,user_name);
	   
	//divide user's command into vector
            stringstream ss(user_name);
            string tmp;
            vector<string> command;
            while(ss >> tmp) // split string with space
            { 
                command.push_back(tmp);
            }

            if(command[0] == "read")  // request download file
            {
                send(sockfd, user_name, sizeof(user_name), 0); // send message to server
                recv(sockfd, receiveMessage, sizeof(receiveMessage),0); // receive message from server
                if(strcmp(receiveMessage, "Permission denial") == 0)  // permisssion denial
                {
                    cout << receiveMessage << endl;
                    continue;
                }
                else if(strcmp(receiveMessage, "The file doesn't exist") == 0)  // file doesn't exitst
                {
                    cout << receiveMessage << endl;
                    continue;
                }
    
                fstream file;
                file.open(command[1], ios::out | ios::trunc | ios::binary); // open file(overwrite if file exists)
                file << receiveMessage; // input received message into file
                file.close();
                cout << "Read successfully" << endl;
                continue;

            }
            else if(command[0] == "write") 
            {
                struct stat buf;
//                if(stat(command[1].c_str(), &buf) == -1) // determine if file already existed
//                { 
//                    cout << "file doesn't existed" << endl;
//                    continue;
//                }
    
                send(sockfd, user_name, sizeof(user_name), 0); // send message to server
                recv(sockfd, receiveMessage, sizeof(receiveMessage),0); // receive message from server
                if(strcmp(receiveMessage, "Permission denial") == 0)  // permisssion denial
                {
                    cout << receiveMessage << endl;
                    continue;
                }
                else if(strcmp(receiveMessage, "The file doesn't exist") == 0)  // file doesn't exitst
                {
                    cout << receiveMessage << endl;
                    continue;
                }
                
//                if(receiveMessage != "") cout << receiveMessage << endl;
    
                fstream file;
                file.open(command[1], ios::in | ios::binary); // open file with readable and binary mode
    
                std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                send(sockfd, contents.c_str(), contents.length(), 0); // send file content to server
                file.close();
                cout << "Write successfully" << endl;
                continue;
            }
            else if(command[0] =="--help")
            {
                cout<<"1.create [file] rwrwrw\n";
            	cout<<"2.read [file]\n";
            	cout<<"3.write [file] o/a\n";
        	cout<<"4.mode [file] rwrwrw\n";
        	continue;
            }

        send(sockfd,user_name,sizeof(user_name), 0);//sen user_name to server
        recv(sockfd, receiveMessage, sizeof(receiveMessage),0); // receive message from server
        cout << receiveMessage << endl;
        if(strcmp(user_name, "exit") == 0) break; // if user input exit, then close connection    
            
        }
    close(sockfd);
    return 0;
}
