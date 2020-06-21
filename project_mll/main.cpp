/**
 * @author Yuriy Rubashevskiy (r9182230628@gmail.com)
 * @brief Mozilla Login Lite(mll) Проект для работы с сохраненными логинами семейства Mozilla(Linux)
 * @version 0.2b
 * @date 2020-06-08
 * @copyright Copyright (c) 2020 Free   Software   Foundation,  Inc.
 *     License  GPLv3+:  GNU  GPL  version  3  or  later <http://gnu.org/licenses/gpl.html>.
 *     This is free software: you are free to change and redistribute it.  
 *     There is NO WARRANTY, to the  extent  permitted by law.
 */

// Стандартные библиотеки
#include <iostream>
#include <stdlib.h>
// Своя для работы с парсингом командной строки в объектном режиме
// https://github.com/Rubashevskiy/CmdLineParserLite
#include "../src/CmdLineParserLite/cmdlineparserlite.hpp"
// Своя для работы с сохраненными логинами семейства Mozilla
#include "../src/MozillaLoginLite/mozillaloginlite.hpp"

// Стандартная маска вывода логинов в режиме SELECT
#define DEF_SELECT_MASK "%n. Адрес: %h Пользователь: %u Пароль: <скрыт>"
// Стандартная маска вывода логинов в режиме UPDATE
#define DEF_UPDATE_MASK "%n. Адрес: %h Пользователь: %u Пароль: <скрыт>"

bool debugMod{false};

/** \enum Command
 * @brief Enum команд для удобства передачи параметров
*/
enum class Command {
  HELP,     ///> Справка
  TEST,     ///> Тстирование
  SELECT,   ///> Выборка логинов из базы
  UPDATE    ///> Обновленние данных логинов
};

/** \struct CMDLine
 * @brief Структура для хранения всех аргументов переданных через командную строку
 * 
*/
struct CMDLine {
  std::vector<Command> command; ///> Полный список переданных команд
  Mozilla::Login filter{};      ///> Фильтр для SELECT или UPDATE
  Mozilla::Login update{};      ///> Список данных для обновления
  std::string    lib{};         ///> Путь к библиотекам NSS
  std::string    profile{};     ///> Путь к профайлу(базы паролей)
  std::string    master{};      ///> Мастер-пароль к базе данных
  std::string    mask{};        ///> Маска вывода на печать
  std::string    lastErr{};     ///> Текс ошибки парсера
};

/**
 * @brief Вывод справки
*/
void help() {
  fputs (("\n\
                                       СПРАВКА\n\n\
Mozilla Login Lite(mll) - приложение для автоматизации работы с сохраненными\n\
учетными данными ПО семейства Mozilla(Firefox, Thunderbird).\n\n\
Для тестирования использовались библиотеки  NSS версии 3.39\n\n\
Возможно использовать только одну 'команду' при запуске\n\n\
Для всех команд кроме '-h --help' требуется:\n\
    - установить путь к библиотекам NSS(-l, --lib)\n\
    - установить путь к библиотекам NSS(LD_LIBRARY_PATH)\n\
    - путь к профилю пользователя(-p --profile)\n\
    - мастер-пароль если установлен(-m --master)\n\
\n"), stdout);

  fputs (("\
Использование: mll [КОМАДА] [АРГУМЕНТ]<Значение>...\n\
\n"), stdout);


  fputs (("\
Команды:\n\
    -t --test             Тесты:\n\
                            - чтения базы паролей пользователя\n\
                            - библиотеки NSS.\n\
    -s --select           Выборка и отображение по маске учетных данных\n\
                          пользователя по заданным фильтрам.\n\
    -u --update           Пакетное обновление  учетных данных пользователя\n\
                          по заданным фильтрам.\n\
    -h --help             Показать эту справку и выйти.\n\
\n"), stdout);

  fputs (("\
Аргументы:\n\
    -l --lib [Path]       Путь до библиотек NSS.\n\
                          В LD_LIBRARY_PATH должен прсутствовать путь[Path]\n\
                          до используемых библиотек NSS.\n\
    -p --profile [Path]   Путь до профиля пользователя.\n\
    -m --master [pass]    Мастер-пароль профиля.\n\
                          Если пароль не установлен можно опустить.\n\
    -d --debug            Вывод дополнительной информации при ошибке\n\
\n"), stdout);

  fputs (("\
Аргументы(Фильтры):\n\
    --fhost [Host]        Фильтр по хосту[Host].\n\
    --fuser [User]        Фильтр по имени пользователя[User].\n\
                          Если нужно установить 'пустую строку'\n\
                          в имени пользователя необходима записать 'NULL'.\n\
    --fpass [Pass]        Фильтр по паролю пользователя[Pass].\n\
                          Если нужно установить 'пустую строку'\n\
                          в пароле необходима записать 'NULL'.\n\
С помощью фильтров можно отсечь учетными данными оперируя --fhost,\n\
--fuser, --fpassw в различны комбинациях до требуемого результата\n\
ВНИМАНИЕ: При обновлении данных должен быть задан минимум один фильтр\n\
из-за соображения безопасности.\n\
\n"), stdout);

  fputs (("\
Аргументы(Обновление):\n\
    --uhost [Host]        Установка нового хоста[Host].\n\
    --uuser [User]        Установка нового имени пользователя[User].\n\
                          Если нужно установить 'пустую строку'\n\
                          в имени пользователя необходима записать 'NULL'.\n\
    --upass [Pass]        Установка нового пароля[Pass].\n\
                          Если нужно установить 'пустую строку'\n\
                          в пароле необходима записать 'NULL'.\n\
ВНИМАНИЕ: При обновлении данных должен быть задан минимум одно значение\n\
для обновления\n\
\n"), stdout);

  fputs (("\
Аргументы(Маска):\n\
    -v --view [Mask]      Установка маски[Mask] при выборке данных\n\
                          или отображения результата обновления.\n\
Маска - Строка содержащая зарезервированные символы и текст оформления.\n\
Зарезервированные символы:\n\
    %n                    Номер по порядку.\n\
    %h                    Хост\n\
    %u                    Имя пользователя.\n\
    %p                    Пароль.\n\
Маски по умолчанию:\n\
    - Выборка             %n. Адрес: %h Пользователь: %u Пароль: <скрыт>\n\
    - Обновление          %n. Адрес: %h Пользователь: %u Пароль: <скрыт>\n\
\n"), stdout);

  fputs (("\
Коды выхода:\n\
    0                    Всё отлично.\n\
    1                    Серьёзная проблема (например, недоступен аргумент\n\
                         командной строки).\n\
\n"), stdout);

  fputs (("\
Автор:                    Юрий Рубашевский (r9182230628@gmail.com)\n\
Лицензия:                 GPLv3+\n\
Автору на 'печеньки':     http://yasobe.ru/na/avtoru_na_pe4enki\n\
\n"), stdout);
}

/**
 * @brief Тестирование
 * @param mll Указатель на Mozilla::MozillaLoginLite
 */
void test(Mozilla::MozillaLoginLite *mll) {
  try {
    std::string test_result = mll->runTest();
    std::cout << "Тест выполнен успешно: " << test_result << std::endl;
  }
  catch(const ExceptionCryptoLite& e) {
    std::cerr << "Ошибка тестирования: " << e.userErr << std::endl;
    if (debugMod) {
      std::cerr << "  Код: " << e.code << std::endl;
      std::cerr << "  Имя: " << e.name << std::endl;
      std::cerr << "  Контекст ошибки: " << e.context << std::endl;
    }
    exit(1);
  }
}

/**
 * @brief Печать логинов в соответствии с маской
 * @param logins List логинов
 * @param mask   Маска
 */
void printLoginMask(std::list<Mozilla::Login> logins, std::string mask) {
  int num{0};
  for (Mozilla::Login login : logins) {
    num++;
    std::string shMask{mask};
    if (std::string::npos != mask.find("%n"))
      shMask.replace(shMask.find("%n"), 2, std::to_string(num));
    if (std::string::npos != shMask.find("%h")) 
      shMask.replace(shMask.find("%h"), 2, login.host);
    if (std::string::npos != shMask.find("%u")) 
      shMask.replace(shMask.find("%u"), 2, login.user);
    if (std::string::npos != shMask.find("%p")) 
      shMask.replace(shMask.find("%p"), 2, login.password);
    std::cout << shMask << std::endl;
  }
}

/**
 * @brief Функция для вывода логинов по заданным критериям
 * @param mll Указатель на Mozilla::MozillaLoginLite
 * @param filter Условия выборки
 * @param mask   Маска вывода
 */
void select(Mozilla::MozillaLoginLite *mll, Mozilla::Login filter, std::string mask) {
  std::string shMask = (mask.empty()) ? (DEF_SELECT_MASK) : (mask);
  try {
    printLoginMask(mll->getLogin(filter), shMask);
  }
  catch(const ExceptionCryptoLite& e) {
    std::cerr << "Ошибка выборки: " << e.userErr << std::endl;
    if (debugMod) {
      std::cerr << "  Код: " << e.code << std::endl;
      std::cerr << "  Имя: " << e.name << std::endl;
      std::cerr << "  Контекст ошибки: " << e.context << std::endl;
    }
    exit(1);
  }
}

/**
 * @brief Функция для обновления логинов по заданным критериямнов
 * @param mll    Указатель на Mozilla::MozillaLoginLite
 * @param filter Условия выборки
 * @param update  Данные для изменения
 */
void update(Mozilla::MozillaLoginLite *mll, Mozilla::Login filter, Mozilla::Login update, std::string mask = "") {
  std::string shMask = (mask.empty()) ? (DEF_UPDATE_MASK) : (mask);
  try {
    std::cout << "Результат обновления данных:" << std::endl;
    printLoginMask(mll->setLogin(filter, update), shMask);
  }
  catch(const ExceptionCryptoLite& e) {
    std::cerr << "Ошибка обновления данных: " << e.userErr << std::endl;
    if (debugMod) {
      std::cerr << "  Код: " << e.code << std::endl;
      std::cerr << "  Имя: " << e.name << std::endl;
      std::cerr << "  Контекст ошибки: " << e.context << std::endl;
    }
    exit(1);
  }
}

/**
 * @brief Фукция для парсинга коммандной строки
 * @param argc   Количество аргументов
 * @param argv   Масив аргументов
 * @param cmd    Переменная для сохранения результатов парсинга
 * @return true  Парсинг  успешно выполнен
 * @return false Ошибка
 */
bool ParseCommandLine(int argc, char **argv, CMDLine &cmd) {
  // Инициализируем парсер командной строки
  CMDPARS::CmdLineParserLite parser(argc, argv);
  // Проверка на пустую командную строку
  if (parser.isNull()) {
    cmd.lastErr = "Ошибка: Отсутствую команды для выполнения";
    return false;
  }
  // Создаем объекты командной строки
  CMDPARS::Param help({"-h", "--help"}, CMDPARS::Arg::No);
  CMDPARS::Param test({"-t", "--test"}, CMDPARS::Arg::No);
  CMDPARS::Param select({"-s", "--select"}, CMDPARS::Arg::No);
  CMDPARS::Param update({"-u", "--update"}, CMDPARS::Arg::No);
  CMDPARS::Param debug({"-d", "--debug"}, CMDPARS::Arg::No);
  CMDPARS::Param libPath({"-l", "--lib"}, CMDPARS::Arg::Required);
  CMDPARS::Param profPath({"-p", "--profile"}, CMDPARS::Arg::Required);
  CMDPARS::Param master({"-m", "--master"}, CMDPARS::Arg::Required);
  CMDPARS::Param f_host({"--fhost"}, CMDPARS::Arg::Required);
  CMDPARS::Param f_user({"--fuser"}, CMDPARS::Arg::Required);
  CMDPARS::Param f_password({"--fpass"}, CMDPARS::Arg::Required);
  CMDPARS::Param u_host({"--uhost"}, CMDPARS::Arg::Required);
  CMDPARS::Param u_user({"--uuser"}, CMDPARS::Arg::Required);
  CMDPARS::Param u_password({"--upass"}, CMDPARS::Arg::Required);
  CMDPARS::Param view_mask({"-v", "--view"}, CMDPARS::Arg::Required);
  // Добавляем в парсер требуемые элементы для парсинга
  parser.addParam(&help);
  parser.addParam(&test);
  parser.addParam(&select);
  parser.addParam(&update);
  parser.addParam(&debug);
  parser.addParam(&libPath);
  parser.addParam(&profPath);
  parser.addParam(&master);
  parser.addParam(&f_host);
  parser.addParam(&f_user);
  parser.addParam(&f_password);
  parser.addParam(&u_host);
  parser.addParam(&u_user);
  parser.addParam(&u_password);
  parser.addParam(&view_mask);
  // Парсинг
  if (!parser.parse()) {
    cmd.lastErr = parser.lastErr();
    return false;
  }
  // Проход по элементам и занесение данных в переменную
  debugMod = debug.set;
  if (help.set) cmd.command.push_back(Command::HELP);
  if (test.set) cmd.command.push_back(Command::TEST);
  if (select.set) cmd.command.push_back(Command::SELECT);
  if (update.set) cmd.command.push_back(Command::UPDATE);
  cmd.lib = libPath.value;
  cmd.profile = profPath.value;
  cmd.master = master.value;
  cmd.filter.host = f_host.value;
  cmd.filter.user = f_user.value;
  cmd.filter.password = f_password.value;
  cmd.update.host = u_host.value;
  cmd.update.user = u_user.value;
  cmd.update.password = u_password.value;
  cmd.mask = view_mask.value;
  return true;
}

int main(int argc, char **argv) {
  CMDLine cmd{};
  // Парсим командную строку
  if (!ParseCommandLine(argc, argv, cmd)) {
    std::cerr << cmd.lastErr << std::endl;
    std::cerr << "Для получения справки введите -h или --help" << std::endl;
    return 1;
  }
  // Можно использовать только одну команду
  if ( cmd.command.size() != 1) {
    std::cerr << "Ошибка: Используется более одной команды или отсутствую команды для выполнения" << std::endl;
    std::cerr << "Для получения справки введите -h или --help" << std::endl;
    return 1;
  }
  // Если команда HELP сразу же выводим
  if (cmd.command[0] == Command::HELP) {
    help();
    return 0;
  }
  // Для дальнейшей работы должны быть обязательно переданны пути к библиотекам и файлу логинов
  if ((cmd.lib.empty())  || (cmd.profile.empty())) {
    std::cerr << "Ошибка: Не указан путь к библиотекам или профайлу пользователя" << std::endl;
    std::cerr << "Для получения справки введите -h или --help" << std::endl;
    return 1;
  }
  // В переменной среды окружения(LD_LIBRARY_PATH) должен быть путь к библиотекам
  {
    char* c_ld_path = getenv("LD_LIBRARY_PATH");
    std::string ld_lib_path = (c_ld_path == NULL) ? ("") : (ld_lib_path);
    if (std::string::npos == ld_lib_path.find(cmd.lib)) {
      std::cerr << "Ошибка: Не указан путь к библиотекам NSS в переменных окружения(LD_LIBRARY_PATH)" << std::endl;
      std::cerr << "Для получения справки введите -h или --help" << std::endl;
      return 1;
    }
  }
  // Инициализируем модуль MozillaLoginLite
  try {
    Mozilla::MozillaLoginLite mll(cmd.lib, cmd.profile, cmd.master);
    // Выполнение команды
    switch (cmd.command[0]) {
      case Command::HELP: return 0; // Отбойник для компилятора
      case Command::TEST: test(&mll); return 0; // Тестирование
      case Command::SELECT: select(&mll, cmd.filter, cmd.mask); return 0; //Выборка данных
      case Command::UPDATE: update(&mll, cmd.filter, cmd.update, cmd.mask); return 0; // Обновление данных
    }
  }
  catch(const ExceptionCryptoLite& e) {
    std::cerr << "Ошибка: " << e.userErr << std::endl;
    if (debugMod) {
      std::cerr << "  Код: " << e.code << std::endl;
      std::cerr << "  Имя: " << e.name << std::endl;
      std::cerr << "  Контекст ошибки: " << e.context << std::endl;
    }
    return 1;
  }
  return 0;
}
