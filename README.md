# MozillaLoginLite

**Mozilla Login Lite(mll)** - приложение для автоматизации работы с сохраненными  
учетными данными ПО семейства Mozilla(Firefox, Thunderbird).  
Основная цель приложения - проверка и обновление (хоста, логина или пароля)  
в файле с сохраненными учетными данными пользователя.  
    
Например: У Вас имеется "группа сайтов" для авторизации в которых требуется  
одни и те-же учетные данные (логин пароль). Политикой безопасности установленна  
смена пароля каждые (n) дней. Через (n) дней Вам приходится в ручную пересохранять  
пароли для каждого сайта в этой "групе".  
С помощью данного приложение можно используя фильтр (старый пароль) сменить на новый  
одной командой для всей "группы сайтов". Так же можно обновить пароли и для Thunderbird.  

Особая просьба: не использовать код или сборку, для создание "стиллеров" или других  
противозаконных инструментов.  

## Основные требования:  

1. Для корректной работы необходимо настроить среду окружение.  
     - в LD_LIBRARY_PATH должен быть прописан путь до библиотек NSS.  
2. Библиотеки NSS.  
   Для тестирования использовались библиотеки  NSS версии 3.39_x64 (Ubuntu 18.04.4) собранные из  
   официального репозитория Mozilla. (бинари находятся в архиве в каталоге lib)  
   Так же можно использовать библиотеки NSS хранящиеся в каталогах:  
     - Для браузера Firefox например: "/usr/lib/firefox/" (Ubuntu 18.04.4)  
     - Для почтового клиента Thunderbird например: "/usr/lib/thunderbird/" (Ubuntu 18.04.4)  
3. Профиль пользователя.  
   Профиль пользователя можно найти:  
     - Для браузера Firefox например: "~/.mozilla/firefox/" (Ubuntu 18.04.4)  
     - Для почтового клиента Thunderbird например: "~/.thunderbird/" (Ubuntu 18.04.4)  
     - Поиском файла 'logins.json'.  
4. Тестирование.  
   Рекомендую перед 'эксперементами' на обновление скопировать файл 'logins.json'  
   из каталога профиля пользователя в 'укромное место'.  
   Перед началом работы рекомендуется проверить работоспособность с помощью теста,  
   а так же провести выборку данных.  
   Рекомендую в начале 'эксперементировать' с фильтрами на одном хосте, проверить  
   результат с помощью родного 'Менеджера паролей', уже потом приступать к пакетным изменениям.  

## Сборка приложения:  

  Простая сборка - запуск в терминале build.sh (cmake)  

   1. Создается каталог "build"  
   2. В каталоге build производится сборка тестового приложения  

  Ручная установка (cmake)  
   1. Перейти в консоли корневой каталог проекта.  
   2. Создать и перейти в каталог например "build", что бы не "захламлять" основной проект  
   3. Генерируем Makefile командой "cmake ../cmake"  
   4. Собираем проект командой "make"  

  Класическая сборка (g++)  
   1. Перейти в консоли корневой каталог проекта.  
   2. Создать и перейти в каталог например "build", что бы не "захламлять" основной проект  
   3. Выполнить "g++ ../mll/main.cpp ../src/CmdLineParserLite/cmdlineparserlite.cpp ../src/MozillaLoginLite/mozillaloginlite.cpp ../src/NSSCryptoLite/nsscryptolite.cpp -ldl -o mll -std=c++17"  

Автор:                    Юрий Рубашевский (r9182230628@gmail.com)  
Автору на 'печеньки':     <http://yasobe.ru/na/prilozhenie_mozilla_login_lite>
