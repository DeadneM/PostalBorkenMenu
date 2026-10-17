#include "LevelTimerFormat.h"
#include <cstring>
int main(){
    struct Example{unsigned int ms;const char* text;};
    Example cases[]={{0,"00:00.00"},{59999,"00:59.99"},{60000,"01:00.00"},{3600000,"60:00.00"},{4294967295u,"71582:47.29"}};
    for(auto test:cases){char out[48];FormatLevelTimer(test.ms,out,sizeof(out));if(std::strcmp(out,test.text))return 1;}
    return 0;
}
