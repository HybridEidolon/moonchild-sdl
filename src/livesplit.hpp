#ifndef MCSR_LIVESPLIT_HPP
#define MCSR_LIVESPLIT_HPP

#include <cstdint>

namespace livesplit {

void Init(const char* host, uint16_t port);
bool Running();
void SendStart();
void SendSplit(uint32_t time);
void SendPause();
void SendReset();
void SendPauseGameTime();
void SendUnpauseGameTime();
void Quit();

}

#endif // MCSR_LIVESPLIT_HPP
