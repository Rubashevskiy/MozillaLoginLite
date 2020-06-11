#include "nsscryptolite.hpp"

using namespace NSS;

/*public function*/

NssCryptoLite::NssCryptoLite(std::string libPath,
                             std::string dbPath,
                             std::string password)
                        : psw(password) {
  // Загрузка динамической библиотеки и загрузка  функции
  loadLib(libPath);
  // Инициализируем  NSS
  if ((NSS_Init(dbPath.c_str())) != SECSuccess) {
    throw getNSSError();
  }
  // Переменная для хранения пароля пользователя
  secuPWData pwdata(SourceType::PW_PLAINTEXT, (char*)psw.c_str());
  // Переменная для хранения слота привязки пользователя к базе
  PK11SlotInfo *slot = PK11_GetInternalKeySlot();
  // Проверки инициализации слота, принадлежность слота базе, проверка пароля пользователя
  if ( (!slot) ||
       ((PK11_NeedUserInit(slot) != 0)) ||
       (PK11_CheckUserPassword(slot, pwdata.data) != SECSuccess) )
  {
    throw getNSSError();
  }
  // Выгрузка слота привязки
  PK11_FreeSlot(slot);
};

void NssCryptoLite::encryptStrTo3DEStoBase64(std::string in, std::string &out) {
  // Переменная для хранения слота привязки пользователя к базе
  PK11SlotInfo *slot = PK11_GetInternalKeySlot();
  // Переменная для хранения пароля пользователя
  secuPWData pwdata(SourceType::PW_PLAINTEXT, (char*)psw.c_str());
  // Проверки инициализации слота, принадлежность слота базе, установка пароля пользователя
  if ( (!slot) || ((PK11_NeedUserInit(slot) != 0)) ||
       (PK11_CheckUserPassword(slot, pwdata.data) != SECSuccess) )
  {
    throw getNSSError();
  }
  // Переменная для хранения id пользователя
  SECItem keyid(0, 0, siBuffer);
  // Переменная для хранения данных для кодирования
  SECItem data((unsigned char *)in.c_str(), strlen(in.c_str()), siBuffer);
  // Переменная для результата кодирования в 3-DES
  SECItem to_3des_result(0, 0, siBuffer);
  // Кодировие из SECItem(unsigned *char) в SECItem(3-DES)
  if ((PK11SDR_Encrypt(&keyid, &data, &to_3des_result, &pwdata)) != SECSuccess) {
    throw getNSSError();
  }
  // Переменная для результата кодирования в Base64
  char *base64_result = nullptr;
  // Кодирование строки SECItem(3-DES) в  char* (Base64)
  base64_result = NSSBase64_EncodeItem(NULL, NULL, 0, &to_3des_result);
  if (base64_result == NULL) {
    throw getNSSError();
  }
  // Преобразовывыем данные
  out = base64_result;
  // Залепа библиотеки (в строку иногда вставляется перенос строки - нужно убрать)
  out.erase(std::remove(out.begin(), out.end(), 10), out.end());
  out.erase(std::remove(out.begin(), out.end(), 13), out.end());
  // Выгрузка переменных
  SECITEM_ZfreeItem(NULL, &keyid, 1);
  SECITEM_ZfreeItem(NULL, &data, 1);
  SECITEM_ZfreeItem(NULL, &to_3des_result, 1);
  PK11_FreeSlot(slot);
}

void NssCryptoLite::decryptBase64to3DEStoStr(std::string in, std::string &out) {
  // Декодирование строки из Base64 в SECItem(3-DES)
  SECItem * request = NSSBase64_DecodeBuffer(NULL, NULL, in.c_str(), strlen(in.c_str()));
  if (!request) {
    throw getNSSError();
  }
  // Переменная для хранения результата декодирования 3-DES -> unsigned *char
  SECItem reply(0, 0, siBuffer);
  // Декодировие из SECItem(3-DES) в SECItem(unsigned *char)
  if (PK11SDR_Decrypt(request, &reply, NULL) != SECSuccess) {
    throw getNSSError();
  }
  else {
    // Преобразование unsigned *char в std::string
    char * cResult = new char [reply.len+1];
    std::strncpy(cResult, (char *)reply.data, reply.len);
    (cResult)[reply.len] = '\0';
     // Возвращаем данные
    out = cResult;
    delete[] cResult;
  }
  // Выгрузка переменных
  SECITEM_ZfreeItem(NULL, request, 1);
  SECITEM_ZfreeItem(NULL, &reply, 1);
}

void NssCryptoLite::runTest() {
  // Строка для тестирования
  std::string testString = DEF_TEST_STRING;
  // Строка для получения кодированного текста
  std::string cryptString{};
  // Кодирование тестовой строки
  encryptStrTo3DEStoBase64(testString, cryptString);
  // Строка для получения декодированного текста
  std::string encryptString{};
  // Декодирование строки
  decryptBase64to3DEStoStr(cryptString, encryptString);
  // Сравнение результатов
  if (testString != encryptString) {
    throw getOtherError("Критическая ошибка при тестировании библиотеки NSS: Строка -> Кодирование -> Декодирование != Строка", "Run-Test");
  }
}

NssCryptoLite::~NssCryptoLite() {
  // Закрываем работу с NSS
  NSS_Shutdown();
  // Выгрузка динамической библиотеки
  unloadLib();
};

/* private function */

void NssCryptoLite::loadLib(std::string &lib_path) {
  //  Линкуем библиотеку "на лету"
  std::string path_lib_nss = lib_path + DEF_LIB_NAME_NSS;
  if (!(nss_h = dlopen(path_lib_nss.c_str(), RTLD_LAZY))) {
    throw getLibError();
  }
  // Получаем адреса требуемых функций из библиотеки:
  // Инициализация/Закрытие
  NSS_Init                = (NSSInit) dlsym(nss_h, "NSS_Init");
  NSS_Shutdown            = (NSSShutdown) dlsym(nss_h, "NSS_Shutdown");
  // Авторизация
  PK11_GetInternalKeySlot = (PK11GetInternalKeySlot) dlsym(nss_h, "PK11_GetInternalKeySlot");
  PK11_CheckUserPassword  = (PK11CheckUserPassword) dlsym(nss_h, "PK11_CheckUserPassword");
  PK11_NeedUserInit       = (PK11NeedUserInit) dlsym(nss_h, "PK11_NeedUserInit");
  // Шифрвание
  PK11SDR_Decrypt         = (PK11SDRDecrypt) dlsym(nss_h, "PK11SDR_Decrypt");
  PK11SDR_Encrypt         = (PK11SDREncrypt) dlsym(nss_h, "PK11SDR_Encrypt");
  NSSBase64_DecodeBuffer  = (NSSBase64DecodeBuffer) dlsym(nss_h, "NSSBase64_DecodeBuffer");
  NSSBase64_EncodeItem    = (NSSBase64EncodeItem) dlsym(nss_h, "NSSBase64_EncodeItem");
  // Освобождение ресурсов
  PK11_FreeSlot           = (PK11FreeSlot) dlsym(nss_h, "PK11_FreeSlot");
  SECITEM_ZfreeItem       = (SECITEMZfreeItem) dlsym(nss_h, "SECITEM_ZfreeItem");
  // Получение ошибок
  PORT_GetError           = (PORTGetError) dlsym(nss_h, "PORT_GetError");
  PR_ErrorToName          = (PRErrorToName) dlsym(nss_h, "PR_ErrorToName");
  PR_ErrorToString        = (PRErrorToString) dlsym(nss_h, "PR_ErrorToString");
  // Проверка полученных адресов
  if ( (!NSS_Init) || (!NSS_Shutdown) ||
       (!PK11_GetInternalKeySlot) || (!PK11_CheckUserPassword) || (!PK11_NeedUserInit) ||
       (!PK11SDR_Decrypt) || (!PK11SDR_Encrypt) ||
       (!NSSBase64_DecodeBuffer) || (!NSSBase64_EncodeItem) ||
       (!PK11_FreeSlot) || (!SECITEM_ZfreeItem) ||
       (!PORT_GetError) || (!PR_ErrorToName) || (!PR_ErrorToString) )
  {
    throw getLibError();
  }
}

void NssCryptoLite::unloadLib() {
  // Если динамическая библиотека загруженна - сброс адресов, выгрузка библиотеки
  if (nss_h) {
    NSS_Init  = nullptr;
    NSS_Shutdown  = nullptr;
    PK11_GetInternalKeySlot  = nullptr;
    PK11_CheckUserPassword  = nullptr;
    PK11_NeedUserInit  = nullptr;
    PK11SDR_Decrypt  = nullptr;
    PK11SDR_Encrypt  = nullptr;
    NSSBase64_DecodeBuffer = nullptr;
    NSSBase64_EncodeItem = nullptr;
    PK11_FreeSlot  = nullptr;
    SECITEM_ZfreeItem  = nullptr;
    PORT_GetError  = nullptr;
    PR_ErrorToName  = nullptr;
    PR_ErrorToString  = nullptr;
    if (dlclose(nss_h) != 0) {
      throw getLibError();
    }
  }
}

ExceptionCryptoLite NssCryptoLite::getNSSError() {
  ExceptionCryptoLite result{};
  // Получения кода ошибки
  result.code = PORT_GetError();
  // Получение имени ошибки
  char *name = PR_ErrorToName(result.code);
  // Получение текста ошибки
  char *text = PR_ErrorToString(result.code, 0);
  // Преобразование и форматирование
  result.name = (!name) ? ("NULL") : (name);
  result.context = (!text) ? ("NULL") : (text);
  // Если не распарсится ошибка выведем исходник
  result.userErr = result.context;
  // Парсим контекст
  /* Чаще всего ошибка возникает при неправильном указании пути к профилю*/
  if (std::string::npos != result.context.find("bad database")) {
    result.userErr = "Путь к профайлу некоректен или криптобаза поврежденна";
  }
  if (std::string::npos != result.context.find("password entered is incorrect")) {
    result.userErr = "Неправильно указан мастер-пароль к базе данных паролей";
  }
  //
  // Вывод
  return result;
}

ExceptionCryptoLite NssCryptoLite::getLibError() {
  ExceptionCryptoLite result{};
  // Получения кода ошибки
  result.code = 0;
  // Получение имени ошибки
  result.name = "Ошибка библиотек";
  // Получение текста ошибки
  result.context = dlerror();
  // Если не распарсится ошибка выведем исходник
  result.userErr = result.context;
  // Парсим контекст
  /* Чаще всего ошибка возникает если не указан LD_LIBRARY_PATH 
    и линкуются библиотеки из системы которые могут быть более ранние чем требуется*/
  if ( (std::string::npos != result.context.find("version")) || 
       (std::string::npos != result.context.find("undefined symbol")))
  {
    result.userErr = "Версия библиотеки NSS не корректна";
  }
  /* Чаще всего ошибка возникает при неправильном указании каталога с библиотеками*/
  if (std::string::npos != result.context.find("cannot open shared object file")) {
    result.userErr = "Подходящая библиотека NSS не найденна";
  }
  // Вывод
  return result;
}

ExceptionCryptoLite NssCryptoLite::getOtherError(std::string err, std::string ctx) {
  ExceptionCryptoLite result{};
  // Получения кода ошибки
  result.code = 0;
  // Получение имени ошибки
  result.name = "NssCryptoLite -> Ошибка";
  // Получение текста ошибки
  result.context = ctx;
  result.userErr = err;
  // Вывод
  return result;
}