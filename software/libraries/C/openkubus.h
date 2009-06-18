#ifndef OPENKUBUS_H
#define OPENKUBUS_H

#include <stdint.h>

int openkubus_authenticate(const char *pad, const char *pw, int offset, int num);
void openkubus_gen_pad(const char *pw, int offset, int num, char *pad);

#endif //OPENKUBUS_H
