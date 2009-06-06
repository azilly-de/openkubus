#ifndef OPENKUBUS_H
#define OPENKUBUS_H

#include <stdint.h>

int32_t openkubus_authenticate(const char *pad, const char *pw, uint16_t offset, uint16_t num);

#endif //OPENKUBUS_H
