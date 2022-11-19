# Hello World Partition Test sample (Unexpected Maker TinyS3 - 8MB Flash, 8MB PSRAM)
Iterating partitions from the firmware and printing to Serial

## Sample Partition file used
```csv
# ESP-IDF Partition Table
# Name,   Type, SubType, Offset,  Size, Flags
nvs,      data, nvs,     0x9000,  0x4000,
otadata,  data, ota,     0xd000,  0x2000,
phy_init, data, phy,     0xf000,  0x1000,
factory,  app,  factory, 0x10000,  1M,
ota_0,    app,  ota_0,   0x110000, 1M,
ota_1,    app,  ota_1,   0x210000, 1M,
test,    app,  test,   , 1M,
storage,  data, spiffs, , 1M,
```


## Output expected
```shell

Hello world!
I (2040) PART: Initializing SPIFFS
I (2140) PART: Performing SPIFFS_check().
I (6280) PART: SPIFFS_check() successful
I (6280) PART: Partition size: total: 956561, used: 502
E (6290) PART: Reading readme.txt from SPIFFS
I (6290) PART: Read from /spiffs/readme.txt : Hello world from SPIFF!
I (6300) PART: Iterating through app partitions...
I (6300) PART: 'nvs'            [ESP_PARTITION_TYPE_DATA - ESP_PARTITION_SUBTYPE_DATA_NVS] at offset 0x9000 with size 0x4000 (16K)
I (6310) PART: 'otadata'        [ESP_PARTITION_TYPE_DATA - ESP_PARTITION_SUBTYPE_APP_FACTORY] at offset 0xd000 with size 0x2000 (8K)
I (6330) PART: 'phy_init'       [ESP_PARTITION_TYPE_DATA - ESP_PARTITION_SUBTYPE_DATA_PHY] at offset 0xf000 with size 0x1000 (4K)
I (6340) PART: 'factory'        [ESP_PARTITION_TYPE_APP - ESP_PARTITION_SUBTYPE_APP_FACTORY] at offset 0x10000 with size 0x100000 (1024K)
I (6350) PART: 'ota_0'          [ESP_PARTITION_TYPE_APP - ESP_PARTITION_SUBTYPE_APP_OTA] at offset 0x110000 with size 0x100000 (1024K)
I (6360) PART: 'ota_1'          [ESP_PARTITION_TYPE_APP - ESP_PARTITION_SUBTYPE_APP_OTA] at offset 0x210000 with size 0x100000 (1024K)
I (6370) PART: 'test'           [ESP_PARTITION_TYPE_APP - ESP_PARTITION_SUBTYPE_APP_TEST] at offset 0x310000 with size 0x100000 (1024K)
I (6390) PART: 'storage'        [ESP_PARTITION_TYPE_DATA - ESP_PARTITION_SUBTYPE_DATA_SPIFFS] at offset 0x410000 with size 0x100000 (1024K)
This is esp32s3 chip with 2 CPU core(s), WiFi/BLE, silicon revision 0, 8MB external flash
Minimum free heap size: 8732239 bytes

```
