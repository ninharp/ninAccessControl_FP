/**
* @file general.h
*
* @brief This file includes the general program settings
*
* @author Michael Sauer <sauer.uetersen@gmail.com>
*
* @date 20.03.2014 - first Version
*
**/
#ifndef _GENERAL_H
#define _GENERAL_H

//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/unistd.h>

#include "delay.h"

//--------------------------------------------------------------
// Defines
//--------------------------------------------------------------

//--------------------------------------------------------------
// Structures
//--------------------------------------------------------------

extern uint32_t SystemCoreClock;

#endif // _GENERAL_H
