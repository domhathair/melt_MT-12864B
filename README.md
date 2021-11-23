# melt_MT-12864B-3FL
Библиотека базовых функций для работы с дисплеем ___МЭЛТ MT-12864B___ по интерфейсу SPI.  
В дисплее используется контроллер ___NT75451___.  
Ссылка на документацию производителя: http://www.melt.com.ru/docs/MT-12864B.pdf    

Библиотека легко модифицируется под нужный контроллер. Для этого нужно отредактировать функцию _Init_LCD_Hardware_ в файле __LCD.c__ для инициализации SPI и указать макросы для работы с логическим уровнем пинов в файле __LCD.h__.  
В папке __example__ приведён пример инициализации дисплея в связке с контроллером _STM32F205_.    
