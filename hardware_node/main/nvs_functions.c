/**
 * @file nvs_functions.c
 * @brief Contains necessary defines, variables definitions
 * and function definitions for using the NVS in esp32
 *
 * @author Danijel Camdzic
 * @date 1 May 2023
 */

#include "nvs_functions.h"

/* ------------------------- FUNCTION DEFINITIONS ------------------------------------ */

/**
 * @brief Initialize the NVS flash.
 *
 * @return esp_err_t Error code indicating the result of the operation.
 */
esp_err_t nvs_init() 
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    return err;
}

/**
 * @brief Store binary data in the NVS storage.
 *
 * @param key     Key for the binary data to be stored.
 * @param value   Pointer to the binary data to be stored.
 * @param length  Length of the binary data in bytes.
 */
void nvs_store(const char* key, const void* value, size_t length)
{
    nvs_handle_t my_handle;
    ESP_ERROR_CHECK(nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle));
    ESP_ERROR_CHECK(nvs_set_blob(my_handle, key, value, length));
    ESP_ERROR_CHECK(nvs_commit(my_handle));
    nvs_close(my_handle);
}

/**
 * @brief Read binary data from the NVS storage.
 *
 * @param key  Key for the binary data to be read.
 *
 * @return void* Pointer to the read binary data. The caller is responsible for freeing the memory.
 */
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