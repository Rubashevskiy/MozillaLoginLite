/**
 * @author Yuriy Rubashevskiy (r9182230628@gmail.com)
 * @brief Библиотека для работы с сохраненными логинами семейства Mozilla
 * @version 0.1
 * @date 2020-06-08
 * @copyright Copyright (c) 2020 Free   Software   Foundation,  Inc.
 *     License  GPLv3+:  GNU  GPL  version  3  or  later <http://gnu.org/licenses/gpl.html>.
 *     This is free software: you are free to change and redistribute it.  
 *     There is NO WARRANTY, to the  extent  permitted by law.
 */

#ifndef MOZILLA_LOGIN_LITE
#define MOZILLA_LOGIN_LITE

/*Стандартные библиотеки:*/
#include <string>
#include <fstream>
#include <list>
#include <chrono>
/* Своя для работы с NSS*/
#include "../NSSCryptoLite/nsscryptolite.hpp"
// Библиотека для работы с JSON от nlohmann https://github.com/nlohmann/json
#include "../Json/json.hpp"


using namespace std::chrono;
using namespace NSS;
using json = nlohmann::json;

/*! \namespace Mozilla
 * @brief Пространство Mozilla
 */
namespace Mozilla {
 
  /* Имя файла паролей по умолчанию для семейства Mozilla*/
  #define DEF_JSON_LOGIN std::string("logins.json")
  
  /** \struct Login
   * @brief Структура для работы с "логинами" семейства Mozilla(FireFox/Thunderbird)
   * @details Используется как контейнер для передачи данных после декодирования,
   * а также как фильтр при получении и записи паролей.
   */
  struct Login {
    /**
     * @brief Пустой конструктор
     */
    Login();
    /**
     * @brief Конструктор с заполнением данных
     * @param[in] Host Адрес
     * @param[in] User Имя пользователя
     * @param[in] Psw  Пароль
     */
    Login(std::string Host, std::string User, std::string Psw);
    std::string host{};     // Адрес
    std::string user{};     // Имя пользователя
    std::string password{}; // Пароль

    /**
     * @brief Обновление структуры Login из другой структуры Login
     * @param[in] other Другая структура Login
     * @details Если в структуре "other" поле не пустое оно записывается в структуру.
     * Внимание если нужно установить "пустую строку" необходима записать "NULL".
     */
    void update(const Login& other);
    /**
     * @brief Нечеткое сравнение структур
     * @param[in] other Другая структура Login
     * @details Если в структуре "other" поле "не пустое" оно сравнивается.
     * Внимание если нужно сравнить "пустую строку" необходима записать "NULL".
     * @return true  Равны
     * @return false Не равны
     */
    bool equivalently(const Login& other);
    /**
     * @brief По элементная проверка структуры на пустоту
     * @return true  Структура пустая
     * @return false Структура не пустая
     */
    bool isEmpty();
  };

  /*! \class MozillaLoginLite
   *  @brief  Клас для работы с логинами семейства Mozilla(FireFox/Thunderbird)
   */
  class MozillaLoginLite {
  public:
    /**
     * @brief Конструктор
     * @param[in] libPath  Абсолютный путь до библиотек NSS
     * @param[in] dbPath   Абсолютный путь до каталога пользователя
     * @param[in] password Мастер пароль пользователя
     * @throw ExceptionCryptoLite
     */
    MozillaLoginLite(const std::string &libPath,
                        const std::string &dbPath,
                        const std::string &password = "");
    
    /**
     * @brief Вычитка и декодирование логинов из файла паролей
     * @param[in] filter Фильтр
     * @details С помощью фильтра можно отсечь логины оперируя host, user, password
     * в различны комбинациях.
     * Внимание если нужно отсечь "пустую строку" необходима записать "NULL".
     * @return std::list<Mozilla::Login>
     * @throw ExceptionCryptoLite
     */
    std::list<Mozilla::Login> getLogin(Mozilla::Login filter = Mozilla::Login());

    /**
     * @brief Кодирование и запись логинов в файл паролей
     * @param[in] filter Фильтр
     * @param[in] login  Данные для изменения
     * @details С помощью фильтра нужно отсечь логины оперируя host, user, password
     * в различны комбинациях. Фильтр не может быть пустым из-за соображения безопасности.
     * Внимание! если нужно отсечь\изменить "пустую строку" необходима записать "NULL".
     * Внимание! если нужно записать "пустую строку" необходима записать "NULL".
     * @return std::list<Mozilla::Login> Список хостов в которых было произведенно изменение.
     * @throw ExceptionCryptoLite
     */
    std::list<Mozilla::Login> setLogin(Mozilla::Login filter, Mozilla::Login login);

    /**
     * @brief Запуск проверочнго теста
     * @param[out] info Возврат результата теста
     * @details Тест произведет проверку NSS, произведет вычитку паролей.
     * @throw ExceptionCryptoLite
     */
    std::string runTest();

    /**
     * @brief Деструктор.
    */
    ~MozillaLoginLite();
  private: // private функции
    
    /**
     * @brief Декодирование и получение "логина" по его индексу
     * @param[in] index Индекс логина в масиве (отсчет с 0)
     * @return Login "логин"
     * @throw ExceptionCryptoLite
     */
    Login get(int index);

    /**
     * @brief Кодирование и запись "логина" по его индексу
     * @param[in] index Индекс логина в масиве (отсчет с 0)
     * @param[in] value "логин" для записи
     * @throw ExceptionCryptoLite
     */
    void  set(int index, Login value);

    /**
     * @brief Получение количества "логинов в базе паролей"
     * @return int количество "логинов" в базе паролей
     */
    int   size();

    /**
     * @brief Загрузка файла паролей как Json файла для дальнейшей работы
     * @details Путь к файлу формируется в конструкторе
     * @throw ExceptionCryptoLite
     */
    void  loadJson();

    /**
     * @brief Сохранение файла паролей как Json файла.
     * @details Путь к файлу формируется в конструкторе
     * @throw ExceptionCryptoLite
     */
    void  saveJson();

    ExceptionCryptoLite getOtherError(std::string err, std::string ctx = "");
  private: // private переменные
    NssCryptoLite *nss  = nullptr;  // Указательна на NSS
    std::string json_logins_path{}; // Абсолютный путь к файлу паролей пользователя
    json json_file;                 // Переменная для хранения Json файла в памяти
  };


} // namespace Mozilla

#endif //MOZILLA_LOGIN_LITE

