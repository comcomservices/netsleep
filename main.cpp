/* Developed by Jon Mundall, FLEXSCADA Systems, ALL RIGHTS RESERVED */


#include <functional>
#include <chrono>
#include <future>
#include <cstdio>
#include <sysinfoapi.h>
#include <windows.h>
#include <iostream>
#include <string>

using namespace std;
string nicName;
uint32_t activityTimeout = 60;

void timer_start(std::function<void(void)> func, unsigned int interval)
{
  std::thread([func, interval]()
  {
    while (true)
    {
      auto x = std::chrono::steady_clock::now() + std::chrono::milliseconds(interval);
      func();
      std::this_thread::sleep_until(x);
    }
  }).detach();
}

bool adapterStatus = false;
int setAdapterStatus(bool status){
    if(adapterStatus == status){
        return 1;
    }
 char buffer[256];
    adapterStatus = status;
   if(status){
       sprintf(buffer,"netsh interface set interface \"%s\" admin=enable",nicName.c_str());
       return system(buffer);
       cout << "Enabling Adapter" << endl;
   } else {
        sprintf(buffer,"netsh interface set interface \"%s\" admin=disable",nicName.c_str());
        return system(buffer);
        cout << "Disabling Adapter" << endl;
   }

}



void checkUserActivity(void)
{

        DWORD dwIdleTime = 0;
        LASTINPUTINFO lii = { 0 };
        lii.cbSize = sizeof(lii);


        if (GetLastInputInfo(&lii))
        {

            dwIdleTime = GetTickCount() - lii.dwTime;
            setAdapterStatus(!(dwIdleTime > (activityTimeout*1000)) );

        }


}



int main(int argc, char* argv[])
{

    if(argc != 3){
        cout<< "Invalid Args, Use netsleep 10 \"Ethernet 2\"";
        return 0;
    }

    activityTimeout = atoi(argv[1]);
    nicName = string(argv[2]);

    timer_start(checkUserActivity,500);
    cin.ignore();
    return 1;
}
