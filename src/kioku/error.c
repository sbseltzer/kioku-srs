#include "kioku/error.h"

#define srsERROR_NAME_SIZE    128
#define srsERROR_MESSAGE_SIZE 1024

static char srsError_Name[srsERROR_NAME_SIZE]       = {0};
static char srsError_Message[srsERROR_MESSAGE_SIZE] = {0};

srsTHREADLOCAL static srsERROR_DATA srsError_Last = {srsRESULT_OK, srsError_Name, srsError_Message};
