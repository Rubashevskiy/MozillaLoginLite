#include "mozillaloginlite.hpp"

using namespace Mozilla;

Login::Login() {}

Login::Login(std::string Host, std::string User, std::string Psw) 
            : host(Host), user(User), password(Psw) {}

void Login::update(const Login& other) {
  if (!other.user.empty()) {
    if (other.user == "NULL") {
      this->user.clear();
    }
    else {
      this->user = other.user;
    }
  }
  if (!other.password.empty()) {
    if (other.password == "NULL") {
      this->password.clear();
    }
    else {
      this->password = other.password;
    }
  }
  if (!other.host.empty()) {
      this->host = other.host;
  }
}

bool Login::equivalently(const Login& other) {
  if (!other.user.empty()) {
    if (other.user == "NULL") {
      if ( this->user != "" ) return false;
    }
    else {
      if (other.user != this->user) return false;
    }
  }
    
  if (!other.password.empty()) {
    if (other.password == "NULL") {
      if ( this->password != "" ) return false;
    }
    else {
      if (other.password != this->password) return false;
    }
  }

  if (!other.host.empty()) {
    if (other.host != this->host) return false;
  }
  return true;
}

bool Login::isEmpty() {
  return ( (this->user.empty()) && (this->password.empty()) && (this->host.empty()) );
}


MozillaLoginLite::MozillaLoginLite(
                  const std::string &libPath,
                  const std::string &dbPath,
                  const std::string &password) {
  // Инициализируем модуль для работы с NSS
  nss = new NssCryptoLite(libPath, dbPath, password);
  // Сохраняем полный путь к файлу паролей
  json_logins_path = dbPath + DEF_JSON_LOGIN;
  // Инициализация файла паролей
  loadJson();
}

std::list<Mozilla::Login> MozillaLoginLite::getLogin(Mozilla::Login filter) {
  std::list<Mozilla::Login> result{};
  for (int index = 0; index < size(); index++) {
    Mozilla::Login lGet = get(index);
    if (filter.isEmpty()) result.push_back(lGet);
    else {
      if (lGet.equivalently(filter)) result.push_back(lGet);
    }
  }
  return result;
}

std::list<Mozilla::Login> MozillaLoginLite::setLogin(
                          Mozilla::Login filter, Mozilla::Login login) {

  if (filter.isEmpty() || login.isEmpty()) {
    throw getOtherError("Ошибка: <Фильтр> и/или <Данные для обновления> не заполненны",
                        "MozillaLoginLite -> setLogin");
  }
  std::list<Mozilla::Login> result{};
  for (int index = 0; index < size(); index++) {
    Mozilla::Login lSet = get(index);
    if (lSet.equivalently(filter)) {
      lSet.update(login);
      set(index, lSet);
      result.push_back(lSet);
    }
  }
  if (!result.empty()) {
    saveJson();
  }
  return result;
}

std::string MozillaLoginLite::runTest() {
  // Тест библиотеки NSS
  nss->runTest();
  return "ТЕСТ: Найденно логинов: " + std::to_string(size()) +". Тест NSS выполнен успешно.";
}

MozillaLoginLite::~MozillaLoginLite() {
  if (nss) {
    delete nss;
  }
}

Login MozillaLoginLite::get(int index) {
  Login result;
  auto &logins     = json_file.at("logins");
  result.host      = logins[index].at("hostname").get<std::string>();
  std::string encryptedUsername = logins[index].at("encryptedUsername").get<std::string>();
  nss->decryptBase64to3DEStoStr(encryptedUsername, result.user);
  std::string encryptedPassword = logins[index].at("encryptedPassword").get<std::string>();
  nss->decryptBase64to3DEStoStr(encryptedPassword, result.password);
  return result;
}

void  MozillaLoginLite::set(int index, Login value) {
  auto &logins     = json_file.at("logins");
  std::string newHost = value.host;
  std::string newUserCrypt{};
  nss->encryptStrTo3DEStoBase64(value.user, newUserCrypt);
  std::string newPasswCrypt{};
  nss->encryptStrTo3DEStoBase64(value.password, newPasswCrypt);

  uint64_t now = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
  logins[index].at("hostname") = newHost;
  logins[index].at("encryptedUsername") = newUserCrypt;
  logins[index].at("encryptedPassword") = newPasswCrypt;
  logins[index].at("timePasswordChanged") = now;
}

int  MozillaLoginLite::size() {
  auto &logins = json_file.at("logins");
  return logins.size();
}

void  MozillaLoginLite::loadJson() {
  std::ifstream jsonFile(json_logins_path);
  if (jsonFile.good()) {
    json_file.clear();
    jsonFile >> json_file;
    // Проверка корректности
    auto &logins = json_file.at("logins");
    if (!logins.is_array()) {
      throw getOtherError("Ошибка: Json файл некорректен: " + json_logins_path,
                          "MozillaLoginLite -> loadJson");
    }
  }
  else {
    throw getOtherError("Ошибка: Json файл некорректен: " + json_logins_path,
                        "MozillaLoginLite -> loadJson");
  }
}

void  MozillaLoginLite::saveJson() {
  std::ofstream jsonFile(json_logins_path);
  if (json_file.empty()) {
    throw getOtherError("Ошибка сохранения файла Json: Файл пустой",
                        "MozillaLoginLite -> saveJson");
  }
  if (jsonFile) {
    jsonFile << json_file;
  }
  if(jsonFile.bad()) {
    throw getOtherError("Ошибка сохранения файла Json " + json_logins_path,
                        "MozillaLoginLite -> saveJson");
  }
}

ExceptionCryptoLite MozillaLoginLite::getOtherError(std::string err, std::string ctx) {
  ExceptionCryptoLite result{};
  // Получения кода ошибки
  result.code = 0;
  // Получение имени ошибки
  result.name = "MozillaLoginLite -> Ошибка";
  // Получение текста ошибки
  result.context = ctx;
  result.userErr = err;
  // Вывод
  return result;
}