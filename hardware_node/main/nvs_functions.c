/**
 * @file nvs_functions.c
 * @brief Contains helper nvs functions implementation
 *
 * @author Danijel Camdzic
 * @date 10 Apr 2023
 */

#include "nvs_functions.h"

esp_err_t nvs_init() 
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    return err;
}

esp_err_t nvs_store_string(const char *key, const char *value) 
{
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) return err;

    err = nvs_set_str(my_handle, key, value);
    if (err != ESP_OK) return err;

    return nvs_commit(my_handle);
}

esp_err_t nvs_read_string(const char *key, char *value, size_t *length) 
{
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READONLY, &my_handle);
    if (err != ESP_OK) return err;

    err = nvs_get_str(my_handle, key, value, length);
    return err;
}

void nvs_store(const char* key, const void* value, size_t length)
{
    nvs_handle_t my_handle;
    ESP_ERROR_CHECK(nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle));
    ESP_ERROR_CHECK(nvs_set_blob(my_handle, key, value, length));
    ESP_ERROR_CHECK(nvs_commit(my_handle));
    nvs_close(my_handle);
}

void* nvs_read(const char* key)
{
    nvs_handle_t my_handle;
    size_t length;
    void *read_value;

    ESP_ERROR_CHECK(nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle));

    /* Get the length of the binary data */
    ESP_ERROR_CHECK(nvs_get_blob(my_handle, key, NULL, &length));

    /* Allocate memory for read_value */
    read_value = malloc(length);

    /* Read the binary data */
    ESP_ERROR_CHECK(nvs_get_blob(my_handle, key, read_value, &length));

    nvs_close(my_handle);

    return read_value;
}