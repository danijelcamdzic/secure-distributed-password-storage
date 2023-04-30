/**
 * @file shamir_secret_sharing_functions.h
 * @brief Contains helper shamir secret sharing functions prototypes
 *
 * @author Danijel Camdzic
 * @date 10 Apr 2023
 */

#ifndef SHAMIR_SECRET_SHARING_FUNCTIONS_H
#define SHAMIR_SECRET_SHARING_FUNCTIONS_H

#include <iostream>
#include <thread>
#include <fstream>
#include <vector>
#include <string.h>
#include "sss.h"
#include "randombytes.h"

#define SHAMIR_NUM_SHARES   1
#define SHAMIR_THRESHOLD    1

void sss_split_password_into_shares(const std::string& password, std::vector<sss_Share>& shares);
std::string sss_recombine_password_from_shares(const std::vector<sss_Share>& shares);

#endif /* SHAMIR_SECRET_SHARING_FUNCTIONS_H */