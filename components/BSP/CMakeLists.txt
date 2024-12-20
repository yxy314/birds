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
            driver
            nvs_flash)

idf_component_register(SRC_DIRS ${src_dirs} INCLUDE_DIRS ${include_dirs} REQUIRES ${requires})
                        #这里               #和这里 都是将这里函数声明出去  #这里添加依赖，添加那些没有被声明出去的函数
                                                                         #比如上文中用到的driver文件夹里面的函数
                                                                         #nvs_flash头文件

component_compile_options(-ffast-math -O3 -Wno-error=format=-Wno-format)