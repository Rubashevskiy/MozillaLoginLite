# MozillaLoginLite

Mozilla Login Lite(mll) - приложение для автоматизации работы с сохраненными  
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

Основные требование:  

1. Для корректной работы необходимо настроить среду окружение.  
     - в LD_LIBRARY_PATH должен быть прописан путь до библиотек NSS.  
2. Библиотеки NSS.  
   Для тестирования использовались библиотеки  NSS версии 3.39_x64 (Ubuntu 18.04.4) собранные из  
   официального репозитория Mozilla. (бинари находятся в архиве в каталоге lib)  
   Так же можно использовать библиотеки NSS хранящиеся в каталогах:  
     - Для браузера Firefox например: "/usr/lib/firefox/" (Ubuntu 18.04.4)  
     - Для почтового клиента Thunderbird например: "/usr/lib/thunderbird/" (Ubuntu 18.04.4)  
   Поискать в системе 'несложно' например командой "locate libnss3.so", но существует  
   "подводный камень" - у меня в системе был установлен пакет NSS более поздней версии чем  
   использовал браузер и почтовый клиент, что приводило к ошибкам в работе.  
3. Профиль пользователя.  
     Для выборки учетных данных необходим доступ "чтение" на каталог профиля пользователя,  
   для обновление необходим доступ "чтение-запись" на каталог профиля пользователя.  




     Рекомендую перед 'эксперементами' на обновление скопировать файл 'logins.json'  
   из каталога профиля пользователя в 'укромное место'.  
   Профиль пользователя можно найти:  
     - Для браузера Firefox например: "~/.mozilla/firefox/" (Ubuntu 18.04.4)  
     - Для почтового клиента Thunderbird например: "~/.thunderbird/" (Ubuntu 18.04.4)  
     - Поиском файла 'logins.json'.  
4. Тестировани.  
     Перед началом работы рекомендуется проверить работоспособность с помощью теста,  
   а так же провести выборку данных.
     Рекомендую в начале 'эксперементировать' с фильтрами на одном хосте, проверить
   результат с помощью родного 'Менеджера паролей', пото уже приступать к пакетным изменениям.


Сборка приложения:

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



СПРАВКА

Возможно использовать только одну 'команду' при запуске

Для всех команд кроме '-h --help' требуется:
    - установить путь к библиотекам NSS(-l, --lib)
    - установить путь к библиотекам NSS(LD_LIBRARY_PATH)
    - путь к профилю пользователя(-p --profile)
    - мастер-пароль если установлен(-m --master)

Использование: mll [КОМАДА] [АРГУМЕНТ]<Значение>...

Команды:
    -t --test             Тесты:
                            - чтения базы паролей пользователя
                            - библиотеки NSS.
    -s --select           Выборка и отображение по маске учетных данных
                          пользователя по заданным фильтрам.
    -u --update           Пакетное обновление  учетных данных пользователя
                          по заданным фильтрам.
    -h --help             Показать эту справку и выйти.

Аргументы:
    -l --lib [Path]       Путь до библиотек NSS.
                          В LD_LIBRARY_PATH должен прсутствовать путь[Path]
                          до используемых библиотек NSS.
    -p --profile [Path]   Путь до профиля пользователя.
    -m --master [pass]    Мастер-пароль профиля.
                          Если пароль не установлен можно опустить.
    -d --debug            Вывод дополнительной информации при ошибке

Аргументы(Фильтры):
    --fhost [Host]        Фильтр по хосту[Host].
    --fuser [User]        Фильтр по имени пользователя[User].
                          Если нужно установить 'пустую строку'
                          в имени пользователя необходима записать 'NULL'.
    --fpass [Pass]        Фильтр по паролю пользователя[Pass].
                          Если нужно установить 'пустую строку'
                          в пароле необходима записать 'NULL'.
С помощью фильтров можно отсечь учетными данными оперируя --fhost,
--fuser, --fpassw в различны комбинациях до требуемого результата
ВНИМАНИЕ: При обновлении данных должен быть задан минимум один фильтр
из-за соображения безопасности.

Аргументы(Обновление):
    --uhost [Host]        Установка нового хоста[Host].
    --uuser [User]        Установка нового имени пользователя[User].
                          Если нужно установить 'пустую строку'
                          в имени пользователя необходима записать 'NULL'.
    --upass [Pass]        Установка нового пароля[Pass].
                          Если нужно установить 'пустую строку'
                          в пароле необходима записать 'NULL'.
ВНИМАНИЕ: При обновлении данных должен быть задан минимум одно значение
для обновления

Аргументы(Маска):
    -v --view [Mask]      Установка маски[Mask] при выборке данных
                          или отображения результата обновления.
Маска - Строка содержащая зарезервированные символы и текст оформления.
Зарезервированные символы:
    %n                    Номер по порядку.
    %h                    Хост
    %u                    Имя пользователя.
    %p                    Пароль.
Маски по умолчанию:
    - Выборка             %n. Адрес: %h Пользователь: %u Пароль: <скрыт>
    - Обновление          %n. Адрес: %h Пользователь: %u Пароль: <скрыт>

Коды выхода:
    0                    Всё отлично.
    1                    Серьёзная проблема (например, недоступен аргумент
                         командной строки).

Автор:                    Юрий Рубашевский (r9182230628@gmail.com)
Лицензия:                 GPLv3+
Автору на 'печеньки':     http://yasobe.ru/na/prilozhenie_mozilla_login_lite
