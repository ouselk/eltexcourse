#ifndef IPROFLS_H
#define IPROFLS_H

#include <stdint.h>
#include <stdbool.h>

#define IpAddress uint32_t

IpAddress genIpAddress(void);
bool addressInSubnet(IpAddress address, IpAddress gateway, IpAddress mask);
int parseIp(const char *str, IpAddress *out);
bool isCorrectMask(IpAddress mask);
IpAddress prefixToMask(int prefix);
int countOnes(IpAddress mask);
void ipToString(IpAddress addr, char *buf);

#endif