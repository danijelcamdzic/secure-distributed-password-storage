#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <gfshare.h>

std::vector<std::vector<uint8_t>> split_secret(const std::string& secret, uint8_t n, uint8_t m)
{
    GfshareCtx* gctx = gfshare_ctx_init_enc(n, m, GFSHARE_FILE_PAD_LEN);
    uint8_t* secret_data = new uint8_t[m];
    memcpy(secret_data, secret.c_str(), secret.size());

    gfshare_ctx_enc_setsecret(gctx, secret_data);

    std::vector<std::vector<uint8_t>> shares(n, std::vector<uint8_t>(m));
    for (uint8_t i = 0; i < n; ++i)
    {
        gfshare_ctx_enc_getshare(gctx, i, shares[i].data());
    }

    gfshare_ctx_free(gctx);
    delete[] secret_data;
    return shares;
}

std::string recover_secret(const std::vector<std::vector<uint8_t>>& shares, uint8_t m)
{
    GfshareCtx* gctx = gfshare_ctx_init_dec(shares.size(), m, GFSHARE_FILE_PAD_LEN);

    for (size_t i = 0; i < shares.size(); ++i)
    {
        gfshare_ctx_dec_giveshare(gctx, i, shares[i].data());
    }

    uint8_t* secret_data = new uint8_t[m];
    gfshare_ctx_dec_newshares(gctx, secret_data);

    std::string secret(reinterpret_cast<char*>(secret_data), m);
    gfshare_ctx_free(gctx);
    delete[] secret_data;
    return secret;
}

int main()
{
    std::string secret = "This is a secret message.";
    uint8_t n = 5;
    uint8_t m = 3;

    std::vector<std::vector<uint8_t>> shares = split_secret(secret, n, m);

    std::vector<std::vector<uint8_t>> recovery_shares(shares.begin(), shares.begin() + m);

    std::string recovered_secret = recover_secret(recovery_shares, m);
    std::cout << "Recovered secret: " << recovered_secret << std::endl;

    return 0;
}

