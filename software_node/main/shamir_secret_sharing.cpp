#include "shamir_secret_sharing.h"

void sss_split_password_into_shares(const std::string& password, std::vector<sss_Share>& shares) 
{
    // Ensure the shares vector is the correct size
    if (shares.size() != SHAMIR_NUM_SHARES) {
        throw std::runtime_error("Shares vector size must be equal to SHAMIR_NUM_SHARES.");
    }

    // Check if length of password exceeds the max length
    if (password.size() > sss_MLEN) {
        throw std::runtime_error("Password length must not exceed sss_MLEN.");
    }

    // Convert password to a fixed-length array
    uint8_t password_data[sss_MLEN];
    memset(password_data, 0, sizeof(password_data));
    memcpy(password_data, password.c_str(), password.size());

    // Split the password into shares
    sss_create_shares(shares.data(), password_data, SHAMIR_NUM_SHARES, SHAMIR_THRESHOLD);
}
