idf_component_register(SRCS "part_test.cpp"
                    INCLUDE_DIRS "")

spiffs_create_partition_image(storage ${PROJECT_DIR}/fatfs FLASH_IN_PROJECT)