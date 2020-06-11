/**
 * @author Yuriy Rubashevskiy (r9182230628@gmail.com)
 * @brief Библиотека для работы с сохраненными логинами семейства Mozilla через API NSS
 * @version 0.2b
 * @date 2020-04-08
 * @copyright Copyright (c) 2020 Free   Software   Foundation,  Inc.
 *     License  GPLv3+:  GNU  GPL  version  3  or  later <http://gnu.org/licenses/gpl.html>.
 *     This is free software: you are free to change and redistribute it.  
 *     There is NO WARRANTY, to the  extent  permitted by law.
 */

#ifndef NSS_CRYPTO_LITE
#define NSS_CRYPTO_LITE

/*Стандартные библиотеки:*/
#include <string>
#include <cstring>
#include <algorithm>
#include <dlfcn.h>

/*! \namespace NSS
 * @brief Пространство NSS
 */
namespace NSS {

  /*Имя библиотеки NSS*/
  #define DEF_LIB_NAME_NSS std::string("/libnss3.so")
  /*Строка для проведения теста библиотеки NSS(смотри runTest())*/
  #define DEF_TEST_STRING std::string("!@#$%^&*()1234567890.,ABCDabcd");

  /*! \enum SECItemType
      @brief Тип контейнера SECItem
  */
  enum SECItemType {
    siBuffer,
    siClearDataBuffer,
    siCipherDataBuffer,
    siDERCertBuffer,
    siEncodedCertBuffer,
    siDERNameBuffer,
    siEncodedNameBuffer,
    siAsciiNameString,
    siAsciiString,
    siDEROID
  };

  /*! \struct SECItem
    @brief Основной контейнер для работы с библиотекой NSS
  */
  struct SECItem {
    SECItem() {}
    SECItem(unsigned char * cdata, unsigned int ilen, SECItemType itype)
                    : data(cdata), len(ilen), type(itype) {}
    SECItemType type{siBuffer}; // Тип контейнера
    unsigned char *data{0};     // Данные
    unsigned int len{0};        // Длина данных
};

  /*! \enum SECStatus
      @brief Статусы возвращаемые библиотекой NSS
  */
  enum SECStatus {
    SECWouldBlock = -2, // Заблокированно
    SECFailure = -1,    // Ошибка
    SECSuccess = 0      // Успех
  };

  /*! \enum SourceType
      @brief Тип данных
  */
  enum SourceType {
    PW_NONE = 0,
    PW_FROMFILE = 1,
    PW_PLAINTEXT = 2,
    PW_EXTERNAL = 3
  };

  /*! \struct secuPWData
      @brief Контейнер для работы с паролями
  */
  struct secuPWData {
    secuPWData() {}
    secuPWData(SourceType source_type, char * psw) : source(source_type), data(psw) {}
    SourceType source; // Тип данных
    char *data;        // Данные
  };


  /*! \struct PK11Slot_Info
      @brief Объявление скрытой структуры
  */
  typedef struct PK11Slot_Info PK11SlotInfo;

  /*Псевдонимы типа на указатели  функции*/
  typedef SECStatus     (*NSSInit)(const char *);
  typedef SECStatus     (*NSSShutdown)();
  typedef PK11SlotInfo* (*PK11GetInternalKeySlot)();
  typedef SECStatus     (*PK11CheckUserPassword)(PK11SlotInfo *, const char *);
  typedef int           (*PK11NeedUserInit)(PK11SlotInfo *);
  typedef SECStatus     (*PK11SDRDecrypt)(SECItem *, SECItem *, void *);
  typedef SECStatus     (*PK11SDREncrypt)(SECItem *, SECItem *, SECItem *, void *);
  typedef SECItem*      (*NSSBase64DecodeBuffer)(void *, void *, const char *inStr, unsigned int inLen);
  typedef char*         (*NSSBase64EncodeItem)(void *, char *, unsigned int , SECItem *);
  typedef void          (*PK11FreeSlot)(PK11SlotInfo *);
  typedef void          (*SECITEMZfreeItem)(PK11SlotInfo *, SECItem *, int);
  typedef int           (*PORTGetError)();
  typedef char*         (*PRErrorToName)(int);
  typedef char*         (*PRErrorToString)(int, unsigned int);
  
  /** \struct ExceptionCryptoLite
   * @brief Структура для формирования исключений
   */
  struct ExceptionCryptoLite {
    int code{};            ///> Код ошибки
    std::string name{};    ///> Имя ошибки
    std::string context{}; ///> Контекст ошибки
    std::string userErr{}; ///> Человекочитабельная ошибка
  };

  /*! \class NssCryptoLite
   *  @brief  Клас для работы с криптобиблиотекой NSS
   */
  class NssCryptoLite {
  public:

   /**
    * @brief Конструктор
    * @param[in] libPath  Абсолютный путь до библиотек NSS
    * @param[in] dbPath   Абсолютный путь до каталога пользователя
    * @param[in] password Мастер пароль пользователя
    * @throw ExceptionCryptoLite
    */
    NssCryptoLite(std::string libPath,
                  std::string dbPath,
                  std::string password = "");

    /**
     * @brief Кодирование данных (Строка) -> Base64(3-Des(Строка))
     * @param[in]  in  Строка
     * @param[out] out Кодированная строка
     * @throw ExceptionCryptoLite
     */
    void encryptStrTo3DEStoBase64(std::string in, std::string &out);


    /**
     * @brief Декодирование строки Base64(3-Des(Строка)) -> (Строка)
     * @param[in] in   Кодированная строка
     * @param[out] out Строка
     * @throw ExceptionCryptoLite
     */
    void decryptBase64to3DEStoStr(std::string in, std::string &out);
 
    /**
     * @brief Тест на зашифровку\расшифровку данных
     * @details ((Строка) -> Base64(3-Des(Строка))) -> (Base64(3-Des(Строка)) -> (Строка))
     * @throw ExceptionCryptoLite
     */
    void runTest();
  
     /**
      * @brief Деструктор.
      */
    ~NssCryptoLite();
  private: // private функции

    /**
     * @brief Линк библиотеки NSS "на лету", Загрузка библиотек.
     * @param[in] lib_path Абсолютный путь до библиотек NSS
     * @details Получние дескрипторов на функций из библиотеки.
     * @throw ExceptionCryptoLite
     */
    void loadLib(std::string &lib_path);

    /**
     * @brief Сброс дескрипторов функции и выгрузка библиотек.
     * @throw ExceptionCryptoLite
     */
     void unloadLib();
  
    /**
     * @brief Формирование информации об ошибке из библиотеки NSS
     * @return ExceptionCryptoLite
     */
    ExceptionCryptoLite getNSSError();

    /**
     * @brief Формирование информации об ошибке при работе с библиотеками
     * @return ExceptionCryptoLite
     */
    ExceptionCryptoLite getLibError();

    /**
     * @brief Формирование информации об ошибке в свободной форме
     * @param err Текст ошибки
     * @param ctx Контекст ошибки
     * @return ExceptionCryptoLite
     */
    ExceptionCryptoLite getOtherError(std::string err, std::string ctx = "");
  private: // private переменные
    // Дескриптор на библиотеку NSS
    void *nss_h = nullptr;
    // Мастер-Пароль пользователя
    std::string psw{};
    // Инициализация библиртеки NSS
    NSSInit NSS_Init = nullptr;
    // Закрытие библиотеки NSS
    NSSShutdown NSS_Shutdown = nullptr;
    // Получение слота для аундификации БД
    PK11GetInternalKeySlot PK11_GetInternalKeySlot = nullptr;
    // Установка пароля пользователя
    PK11CheckUserPassword PK11_CheckUserPassword = nullptr;
    // Привязка слота к данным пользователя
    PK11NeedUserInit PK11_NeedUserInit = nullptr;
    // Декодировие [SECItem]<unsigned *char>(3-DES) -> [SECItem]<unsigned *char>()
    PK11SDRDecrypt PK11SDR_Decrypt = nullptr;
    // Кодировие из [SECItem]<unsigned *char>() ->  [SECItem]<unsigned *char>(3-DES)
    PK11SDREncrypt PK11SDR_Encrypt = nullptr;
    // Декодировие [SECItem]<unsigned *char>(Base64) -> []<unsigned *char>()
    NSSBase64DecodeBuffer NSSBase64_DecodeBuffer = nullptr;
    // Кодировие [SECItem]<unsigned *char>() -> [SECItem]<unsigned *char>(Base64)
    NSSBase64EncodeItem NSSBase64_EncodeItem = nullptr;
    // Освобожднение слота аундификации БД
    PK11FreeSlot PK11_FreeSlot = nullptr;
    // Освобождение SECItem
    SECITEMZfreeItem SECITEM_ZfreeItem = nullptr;
    // Получение кода ошибки
    PORTGetError PORT_GetError = nullptr;
    // Получение имени ошибки
    PRErrorToName PR_ErrorToName = nullptr;
    // Получение текста ошибки
    PRErrorToString PR_ErrorToString = nullptr;
  };

} // namespace NSS

#endif // NSS_CRYPTO_LITE