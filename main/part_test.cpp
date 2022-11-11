/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
const char *TAG = "PART";
#include <stdio.h>
#include "sdkconfig.h"
#include <esp_log.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_flash.h"
#include "esp_spiffs.h"
#include "esp_partition.h"
#include "nvs_flash.h"
#include <string.h>

// Get the string name of type enum values used in this example
static const char* get_type_str(esp_partition_type_t type)
{
    switch(type) {
        case ESP_PARTITION_TYPE_APP:
            return "ESP_PARTITION_TYPE_APP";
        case ESP_PARTITION_TYPE_DATA:
            return "ESP_PARTITION_TYPE_DATA";
        default:
            return "UNKNOWN_PARTITION_TYPE"; // type not used in this example
    }
}

// Get the string name of subtype enum values used in this example
static const char* get_subtype_str(esp_partition_subtype_t subtype)
{
    switch(subtype) {
        case ESP_PARTITION_SUBTYPE_DATA_NVS:
            return "ESP_PARTITION_SUBTYPE_DATA_NVS";
        case ESP_PARTITION_SUBTYPE_DATA_PHY:
            return "ESP_PARTITION_SUBTYPE_DATA_PHY";
        case ESP_PARTITION_SUBTYPE_APP_FACTORY:
            return "ESP_PARTITION_SUBTYPE_APP_FACTORY";
        case ESP_PARTITION_SUBTYPE_DATA_FAT:
            return "ESP_PARTITION_SUBTYPE_DATA_FAT";
        case ESP_PARTITION_SUBTYPE_APP_OTA_0:
        case ESP_PARTITION_SUBTYPE_APP_OTA_1:
        case ESP_PARTITION_SUBTYPE_APP_OTA_2:   // Till 15
            return "ESP_PARTITION_SUBTYPE_APP_OTA";
        case ESP_PARTITION_SUBTYPE_DATA_SPIFFS:
            return "ESP_PARTITION_SUBTYPE_DATA_SPIFFS";
        case ESP_PARTITION_SUBTYPE_DATA_ESPHTTPD:
            return "ESP_PARTITION_SUBTYPE_DATA_ESPHTTPD";
        case ESP_PARTITION_SUBTYPE_APP_TEST:
            return "ESP_PARTITION_SUBTYPE_APP_TEST";
        default:
            return "UNKNOWN_PARTITION_SUBTYPE"; // subtype not used in this example
    }
}

void list_partitions()
{

    esp_partition_iterator_t it;

    ESP_LOGI(TAG, "Iterating through app partitions...");

    it = esp_partition_find(ESP_PARTITION_TYPE_ANY, ESP_PARTITION_SUBTYPE_ANY, NULL);
    // Loop through all matching partitions, in this case, all with the type 'data' until partition with desired
    // label is found. Verify if its the same instance as the one found before.
    for (; it != NULL; it = esp_partition_next(it)) {
        const esp_partition_t *part = esp_partition_get(it);
        ESP_LOGI(TAG, "'%s' \t[%s - %s] at offset 0x%x with size 0x%x (%dK)", 
                    part->label, get_type_str(part->type), get_subtype_str(part->subtype), 
                    part->address, part->size,part->size/1024);
    }

    // Release the partition iterator to release memory allocated for it
    esp_partition_iterator_release(it);

}


esp_err_t init_spiff()
{
    ESP_LOGI(TAG, "Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = NULL,
      .max_files = 5,
      .format_if_mount_failed = true
    };

    // Use settings defined above to initialize and mount SPIFFS filesystem.
    // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Performing SPIFFS_check().");
    ret = esp_spiffs_check(conf.partition_label);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPIFFS_check() failed (%s)", esp_err_to_name(ret));
        return ESP_FAIL;
        } else {
        ESP_LOGI(TAG, "SPIFFS_check() successful");
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }        
    return ESP_OK;

    // esp_vfs_spiffs_unregister(NULL);
    // ESP_LOGI(TAG, "SPIFFS unmounted");    
}

static void print_readme_txt(void)
{
    // Not error but to highlight in output
    ESP_LOGE(TAG, "Reading readme.txt from SPIFFS");

    // Open for reading hello.txt
    FILE* f = fopen("/spiffs/readme.txt", "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open /spiffs/readme.txt");
        return;
    }

    char buf[100];
    memset(buf, 0, sizeof(buf));
    fread(buf, 1, sizeof(buf), f);
    fclose(f);

    // Display the read contents from the file
    ESP_LOGI(TAG, "Read from /spiffs/readme.txt : %s", buf);
}

extern "C" void app_main(void)
{
    printf("Hello world!\n");

    //Initialize NVS
    ESP_ERROR_CHECK(nvs_flash_init());

    // Init SPIFF & print readme.txt from the root
    init_spiff();
    print_readme_txt();

    list_partitions();

    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU core(s), WiFi%s%s, ",
            CONFIG_IDF_TARGET,
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());

  





}
