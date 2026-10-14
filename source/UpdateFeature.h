#pragma once
#include <string>
namespace postal::update {
struct Status{bool busy=false,available=false,queued=false,beta=false;std::string tag,message="Choose a channel, then Check for Updates.";};
Status GetStatus();
void Check(bool beta);
void InstallAfterExit();
void RestoreAfterExit();
}
