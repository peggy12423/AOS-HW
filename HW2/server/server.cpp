#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>
#include <fstream>
#include "unistd.h"
#include <ctime>
#include <mutex>

#define MAXLINE 4096

using namespace std;


void show_capability_list(void);
void show_file_list(void);

struct capability      //使用者的權限
{
    string object;
    bool r, w;
};

struct file_detail     //儲存file相關細節
{
    string permission;
    string owner;
    string group;
    int size;
    string date;
    string name;
    mutex locker;       //互斥鎖

    int reading = 0;
    
    file_detail &operator=(const file_detail &o)   //以防互斥鎖未被正常使用
    { 
        permission = o.permission;
        owner = o.owner;
        group = o.group;
        size = o.size;
        date = o.date;
        name = o.name;
        reading = 0;
        return *this;          //返回函式的物件的拷貝(臨時變數)
    }
   
};
 


unordered_map<string, pair<string, vector<struct capability>> > CAPABILITY_LIST; //儲存檔案細節至CAPABILITY_LIST
unordered_map<string, struct file_detail> FILE_LIST;      //儲存檔案細節至FILE_LIST

const int MEMBER_SIZE = 6;
string MEMBER[MEMBER_SIZE] = {"Peggy", "Alan", "Denial", "Jim" ,"Weilber", "Sam"};
void initial_member()     //將一開始的六位成員加入名單
{
    CAPABILITY_LIST["Peggy"];
    CAPABILITY_LIST["Peggy"].first = "AOS";
    CAPABILITY_LIST["Peggy"].second = vector<struct capability>();
    CAPABILITY_LIST["Alan"];
    CAPABILITY_LIST["Alan"].first = "AOS";
    CAPABILITY_LIST["Alan"].second = vector<struct capability>();
    CAPABILITY_LIST["Denial"];
    CAPABILITY_LIST["Denial"].first = "AOS";
    CAPABILITY_LIST["Denial"].second = vector<struct capability>();
    CAPABILITY_LIST["Jim"];
    CAPABILITY_LIST["Jim"].first = "CSE";
    CAPABILITY_LIST["Jim"].second = vector<struct capability>();
    CAPABILITY_LIST["Weilber"];
    CAPABILITY_LIST["Weilber"].first = "CSE";
    CAPABILITY_LIST["Weilber"].second = vector<struct capability>();
    CAPABILITY_LIST["Sam"];
    CAPABILITY_LIST["Sam"].first = "CSE";
    CAPABILITY_LIST["Sam"].second = vector<struct capability>();
}


string create_file(string file_name,string  user,string  permission)     // for creating file
{
    fstream file;
    string path;
    if(user=="Peggy" || user=="Alan" || user=="Denial")
    {
        path = "../AOS/" + file_name;
    }
    else if(user=="Jim" || user=="Weilber" || user=="Sam")
    {
        path = "../CSE/" + file_name;
    }
    else
    {
        path = "../others/" + file_name;
    }

    struct stat buf;

    if(stat(path.c_str(), &buf) != -1) return "The file has already existed!"; // 判斷檔案是否已經存在

    file.open(path, ios::out);   //文件以輸出方式打開（內存數據輸出到文件）
    file.seekg(0, ios::end);    //將讀取位置設置為從文件末尾的第1個字節(字節0)
    int file_size = file.tellg();     //得到檔案的size
    file.close();

    struct file_detail t;    //宣告名為file_detail的structure並存入t
    t.permission = permission;
    t.owner = user;
    t.group = CAPABILITY_LIST[user].first;
    t.size = file_size;

    // the date of file to be created
    time_t now = time(0);        //紀錄檔案被創建的時間
    char *dt = ctime(&now);
    t.date = dt;      //將dt存入t
    stringstream ss(t.date);
    string tmp;
    vector<string> da;
    while(ss >> tmp)     //將data分開在不同區域 
    { 
        da.push_back(tmp);
    }
    t.date = da[1] + " " + da[2] + " " + da[4];

    t.name = file_name;
    FILE_LIST[file_name] = t; //將檔案加入file_list
    cout << "permission user group date file_name\n";
    cout << "Create file: " << FILE_LIST[file_name].permission << " " << FILE_LIST[file_name].owner << " " << FILE_LIST[file_name].group << " " << FILE_LIST[file_name].size << " " << FILE_LIST[file_name].date << " " << FILE_LIST[file_name].name << endl;

    //更新 capability_list
    for(int i=0; i<MEMBER_SIZE; i++) { // for every member
        struct capability tmp;
        tmp.object = file_name;
        if(MEMBER[i] == user)    //相同的owner
        { 
            tmp.r = t.permission[0] == 'r' ? true : false;
            tmp.w = t.permission[1] == 'w' ? true : false;
        }
        else if(CAPABILITY_LIST[MEMBER[i]].first == CAPABILITY_LIST[user].first)     //相同的group
        { 
            tmp.r = t.permission[2] == 'r' ? true : false;
            tmp.w = t.permission[3] == 'w' ? true : false;
        }
        else
        { 
            tmp.r = t.permission[4] == 'r' ? true : false;
            tmp.w = t.permission[5] == 'w' ? true : false;
        }
        CAPABILITY_LIST[MEMBER[i]].second.push_back(tmp);   //利用push_back在tmp末尾插入新資料(owner、group)

    }

    return "Create file command has finished!";
}

string transfer_file(int connfd, string file_name, string user) { // for transferring file to user

    string path;
    if(user=="Peggy" || user=="Alan" || user=="Denial")
    {
        path = "../AOS/" + file_name;
    }
    else if(user=="Jim" || user=="Weilber" || user=="Sam")
    {
        path = "../CSE/" + file_name;
    }
    else
    {
        path = "../others/" + file_name;
    }

    struct stat buf;

    if(stat(path.c_str(), &buf) == -1) return "file doesn't exist"; //判斷file是否存在

    int len = CAPABILITY_LIST[user].second.size();
    for(int i=0; i<len; i++)    //依序搜尋capability
    { 
        if(CAPABILITY_LIST[user].second[i].object == file_name) // find object
        { 
            if(CAPABILITY_LIST[user].second[i].r) 
            {
                break;
            } 
            else 
            {
                return "Permission denial";
            }
        }
    }


    if(FILE_LIST[file_name].reading == 0)    //代表無人正在reading
    { 
        FILE_LIST[file_name].locker.lock();
    }

    mutex R_LOCKER;      //reading的互斥鎖
    R_LOCKER.lock(); //互斥鎖上鎖
    FILE_LIST[file_name].reading += 1;     //更新file_list內的reading成員數量
    R_LOCKER.unlock();
    sleep(30); // for test synchronization

    // critical section

    fstream file;
    file.open(path, ios::in | ios::binary);    //使用binary模式(換行符為單字符)打開檔案並閱讀

    std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    send(connfd, contents.c_str(), contents.length(), 0);
    file.close();
    cout << "transfer" << " " << file_name << " to " << user << endl;

    R_LOCKER.lock(); //reading的互斥鎖上鎖
    FILE_LIST[file_name].reading -= 1; //更新file_list內的reading成員數量
    R_LOCKER.unlock();

    if(FILE_LIST[file_name].reading == 0) 
    {
        FILE_LIST[file_name].locker.unlock();   //檔案unlock
    }

    return "";
}

string write_file(string file_name, string user) //determine file existence and permission
{ 
    string path;
    if(user=="Peggy" || user=="Alan" || user=="Denial")
    {
        path = "../AOS/" + file_name;
    }
    else if(user=="Jim" || user=="Weilber" || user=="Sam")
    {
        path = "../CSE/" + file_name;
    }
    else
    {
        path = "../others/" + file_name;
    }

    struct stat buf;

    if(stat(path.c_str(), &buf) == -1) return "The file isn't exist!"; // determine if file already existed

    int len = CAPABILITY_LIST[user].second.size();
    for(int i=0; i<len; i++)    //搜尋capability清單
    { 
        if(CAPABILITY_LIST[user].second[i].object == file_name) // find object
        { 
            if(CAPABILITY_LIST[user].second[i].w) 
            { // if user has writing permission for this file
                break;
            }
            else
            {
                return "Permission denial";
            }
        }
    }

    return "Wrie command is successful!";

}

void write_file(string file_name, string user, string mode, string content) {

    FILE_LIST[file_name].locker.lock(); // lock the file
    cout << user << ": write lock finished" << endl;
    sleep(60); // for synchronization

    // critical section

    string path;
    if(user=="Peggy" || user=="Alan" || user=="Denial")
    {
        path = "../AOS/" + file_name;
    }
    else if(user=="Jim" || user=="Weilber" || user=="Sam")
    {
        path = "../CSE/" + file_name;
    }
    else
    {
        path = "../others/" + file_name;
    }
    if(mode == "o")    // overwrite mode
    { 
        fstream file;
        file.open(path, ios::out | ios::trunc | ios::binary); //使用binary模式(換行符為單字符)打開檔案並覆寫
        file << content; //使用複寫(overwriting)模式寫content到file
        file.close();

        file.open(path, ios::in);
        file.seekg(0, ios::end);
        int file_size = file.tellg(); // find file size
        file.close();

        FILE_LIST[file_name].size = file_size;  //更新file_list內的file size
        cout << "overwrite " << file_name << " successfully" << endl;

    }
    else if(mode == "a")    // append mode
     { 
        fstream file;
        file.open(path, ios::out | ios::app | ios::binary);  //使用binary模式(換行符為單字符)打開檔案並從後面繼續寫
        file << content; //使用複寫(overwriting)模式寫content到file
        file.close();

        file.open(path, ios::in);
        file.seekg(0, ios::end);
        int file_size = file.tellg(); // find file size
        file.close();

        FILE_LIST[file_name].size = file_size;  //更新file_list內的file size
        cout << "append " << file_name << " successfully" << endl;
    }

    FILE_LIST[file_name].locker.unlock(); //解開互斥鎖

    return;
}

string changemode(string file_name, string user, string permission) // changing模式
{ 
    string path;
    if(user=="Peggy" || user=="Alan" || user=="Denial")
    {
        path = "../AOS/" + file_name;
    }
    else if(user=="Jim" || user=="Weilber" || user=="Sam")
    {
        path = "../CSE/" + file_name;
    }
    else
    {
        path = "../others/" + file_name;
    }

    struct stat buf;

    if(stat(path.c_str(), &buf) == -1) return "file doesn't exist"; // file是否已經存在

    if(FILE_LIST[file_name].owner != user) return "Permission denial"; // determine whether is the owner of file

    FILE_LIST[file_name].locker.lock(); // 鎖住file
    cout << user << ": changemode lock finished" << endl;
    sleep(60); // for synchronization

    // critical section

    FILE_LIST[file_name].permission = permission;

    for(int i=0; i<MEMBER_SIZE; i++) // 更新 CAPABILITY_LIST
    { 
        int len = CAPABILITY_LIST[MEMBER[i]].second.size();
        for(int j=0; j<len; j++) // member's capabilities
        { 
            if(CAPABILITY_LIST[MEMBER[i]].second[j].object != file_name) {
                continue;
            }else {
                if(MEMBER[i] == user)    //owner相同
                { 
                    CAPABILITY_LIST[MEMBER[i]].second[j].r = permission[0] == 'r' ? true : false;
                    CAPABILITY_LIST[MEMBER[i]].second[j].w = permission[1] == 'w' ? true : false;
                } 
                else if(CAPABILITY_LIST[MEMBER[i]].first == CAPABILITY_LIST[user].first) // 相同的group
                { 
                    CAPABILITY_LIST[MEMBER[i]].second[j].r = permission[2] == 'r' ? true : false;
                    CAPABILITY_LIST[MEMBER[i]].second[j].w = permission[3] == 'w' ? true : false;
                } 
                else    // other user
                { 
                    CAPABILITY_LIST[MEMBER[i]].second[j].r = permission[4] == 'r' ? true : false;
                    CAPABILITY_LIST[MEMBER[i]].second[j].w = permission[5] == 'w' ? true : false;
                }
            }
        }
    }

    FILE_LIST[file_name].locker.unlock(); // 解開file的互斥鎖

    return "change mode successfully";
}

void show_capability_list(void) // 顯示CAPABILITY_LIST detail
{ 
    for(int i=0; i<MEMBER_SIZE; i++) {
        cout << MEMBER[i] <<": ";
        int len = CAPABILITY_LIST[MEMBER[i]].second.size();
        for(int j=0; j<len; j++) {
            cout << "  " << CAPABILITY_LIST[MEMBER[i]].second[j].object << "    " << CAPABILITY_LIST[MEMBER[i]].second[j].r << " " <<CAPABILITY_LIST[MEMBER[i]].second[j].w << " --- ";
        }
        cout << endl;
    }
}

void show_file_list(void) { // 顯示FILE_LIST 細節
    for(auto it = FILE_LIST.begin(); it != FILE_LIST.end(); it++) {
        cout << it->second.permission << " " << it->second.owner << " " << it->second.group << " " << it->second.size << " " << it->second.date << " " << it->second.name << endl;
    }
}

void thread_task(int connfd)  // the task which every thread should execute
{
    int n, cnt=0;
    string user;
    char buffer[MAXLINE];
    while(1) // 使用迴圈收到使用者訊息並返回response
    { 
        n = recv(connfd, buffer, MAXLINE, 0);
        buffer[n] = '\0';
        if(cnt++ == 0) user = buffer;
        if(user.length()==0)
        {
            cout << "Connection failed!"<<endl;
            break;
        }
        else
        {
            cout << user << " " <<": receive message:" << buffer << endl;
//            cout << "Connect successful!" <<endl;
        }
        
        //分類user的命令到vector
        stringstream ss(buffer);
        string tmp;
        vector<string> command;
        while(ss >> tmp) {
            command.push_back(tmp);
        }

        //判斷user的命令
        if(command[0] == "create") // create file
        { 
            string mes = create_file(command[1], user, command[2]);
            send(connfd, mes.c_str(), mes.length(), 0);
            continue;
        }
        else if(command[0] == "read")  // transfer file
        {
            string mes =  transfer_file(connfd ,command[1], user);
            send(connfd, mes.c_str(), mes.length(), 0);
            continue;
        }
        else if(command[0] == "write") // upload file
        { 
            string mes =  write_file(command[1], user);
            send(connfd, mes.c_str(), mes.length(), 0);
            n = recv(connfd, buffer, MAXLINE, 0);
            buffer[n] = '\0';

            string content = buffer;

            write_file(command[1], user, command[2], content);

            continue;
        }
        else if(command[0] == "mode") // change file mode
        { 
            string mes = changemode(command[1], user, command[2]);
            send(connfd, mes.c_str(), mes.length(), 0);
            continue;
        }
        else if(command[0] == "exit") // exit connection
        { 
            string mes = "close connection";
            send(connfd, mes.c_str(), mes.length(), 0);
            break;
        }
        else if(command[0] == "show" && command[1] == "-f") // show file detail
        { 
            cout << "-----------------file detail-----------------\n";
            cout << "user file_name\n";
            show_file_list();
        }
        else if(command[0] == "show" && command[1] == "-c") // show capability detail
        { 
            cout << "-----------------capability detail-----------------\n";
            cout << "permission user group date file_name\n";
            show_capability_list();
        }
        else if(command[0] == "--help")
        {
            continue;
        }

        
        string mes = "Sent ack packet!";
        send(connfd, mes.c_str(), mes.length(), 0);
    }

        cout << "close: " << user <<endl;
        close(connfd);
        return;
}


int main()
{
    int listenfd, connfd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[MAXLINE];
    int n;

    initial_member() ;//將一開始的六位成員加入名單


    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ) //create a socket for listening
    { 
        cout << "create socket error" << endl;
        return 0;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 接收來自不同interface的訊息
    server_addr.sin_port = htons(8888); // server port number

    if(bind(listenfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) // bind socket with server socket pair
    { 
        cout << "bind socket error" << endl;
        return 0;
    }

    if(listen(listenfd, 10) == -1)   // 聆聽socket connection
    { 
        cout << "listen socket error" << endl;
        return 0;
    }

    cout << "Client is requesting..." << endl;

    thread t[10]; // user最大的connection數量
    int cnt = 0;
    
    while(1) // 等待user connection
    { 
        unsigned int client_addrlen = sizeof(client_addr);
        int connfd;
        
        if((connfd = accept(listenfd, (struct sockaddr*) &client_addr, &client_addrlen)) != -1)
        {
            t[cnt] = thread(thread_task, connfd);
            t[cnt++].detach(); // execute thread task without blocking main thread
        }
    }

    close(listenfd);


    cout <<"Server Hello world!" << endl;
    return 0;
}
