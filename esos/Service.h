#ifndef SERVICE_H
#define SERVICE_H

#include <sim800.h>
#include "Settings.h"
#include "log.h"
#include "Clocks.h"

// SIM800
extern Sim800 server = Sim800(Serial1, APN, USERNAME, PASSWORD);

void ServiceBegin();

#endif