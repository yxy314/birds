set(src_dirs
            .
            IIC
            LCD
            Button
            LED
            SPI
            XL9555
            nvs)

set(include_dirs
            .
            IIC
            LCD
            Button
            LED
            SPI
            XL9555
            nvs)

set(requires
            driver)

idf_component_register(SRC_DIRS ${src_dirs} INCLUDE_DIRS ${include_dirs} REQUIRES ${requires} PRIV_REQUIRES nvs_flash)

component_compile_options(-ffast-math -O3 -Wno-error=format=-Wno-format)