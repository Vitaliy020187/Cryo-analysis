cmake_minimum_required(VERSION 2.8)	 # Проверка версии CMake.
										# Если версия установленой программы
										# старее указаной, произайдёт аварийный выход.
project(average)
set(SOURCE main.cpp)		# Установка переменной со списком исходников для исполняемого файла

set(SOURCE_LIB1 Settings.cpp)		# Тоже самое, но для библиотеки
set(SOURCE_LIB2 Average.cpp)
set(SOURCE_LIB3 App.cpp)

add_library(Settings STATIC ${SOURCE_LIB1})	# Создание статической библиотеки
add_library(Average STATIC ${SOURCE_LIB2})
add_library(App STATIC ${SOURCE_LIB3})

add_executable(get_average_trace ${SOURCE})	# Создает исполняемый файл

target_link_libraries(get_average_trace App Average Settings)		# Линковка программы с библиотекой
