#include "cmdlineparserlite.hpp"

using namespace CMDPARS;

CmdLineParserLite::CmdLineParserLite(int argc,  char* argv[]) {
  // Если параметры переданны подготовим для парсинга
  if (argc > 1) {
    for (int i = 1; i < argc; i++) {
      // Костыльно но нужно распарсить
      if (strlen(argv[i]) > 2) { // Или команда или длинный параметр или параметр вида -xy..
        if (argv[i][0] != '-') { // Команда
          line_v.push_back(std::string(argv[i]));
          continue;
        }
        if (argv[i][1] == '-') { // Длинный параметр
          line_v.push_back(std::string(argv[i]));
          continue;
        }
        // группа коротких ключей
        for (int j = 1; j < strlen(argv[i]); j++) {
          std::string key{'-'};
          key.push_back(argv[i][j]);
          line_v.push_back(key);
        }
        continue;
      }
      line_v.push_back(std::string(argv[i]));
    }
  }
}

CmdLineParserLite::~CmdLineParserLite() {}

void CmdLineParserLite::addParam(Param *param) {
  // Проверка не являются ли список имен пустым
  if (param->name.empty()) {
    throw std::runtime_error("ERROR: <name> is empty");
  }
  // Проверка на присутствие пробелов в имени и не является ли пустым именем
  for (std::string n : param->name) {
    if ( (n.empty()) || (std::string::npos != n.find(' ')) ) {
      throw std::runtime_error("ERROR: <name> is empty or used 'space'");
    }
  }
  // Поиск дублируемых ключей
  bool find{false};
  getParam(param->name, find);
  if (find) {
    throw std::runtime_error("ERROR: duplicate params");
  }
  // Проверки пройденны добавляем в массив
  params_v.push_back(*param);
}

Param& CmdLineParserLite::getParam(std::list<std::string> name, bool &find) {
  // Наверняка установим флаг
  find = false;
  // Проход по объектам
  for (auto t : params_v) {
    // Приводим к базовому типу
    Param &testParam = t.get();
    // Проверка листов на одинаковые имена
    for (std::string pv : name) {
      for (std::string tv : testParam.name) {
        if (pv == tv) {
          find = true;
          return testParam;
        }
      }
    }
  }
}

bool CmdLineParserLite::parse() {
  // Проход по всем переданным параметрам
  for (int i = 0; i < line_v.size(); i++) {
    bool isFind{false}; ///< Переменная для поиска параметров в массиве
    auto &param = getParam({line_v[i]}, isFind); ///< Поиск параметра по имени
    if (!isFind) {
      error = "ERROR: Not used param: " + line_v[i];
      return false;
    }
    param.set = true; ///< Параметр найден отмечаем
    // флаг последего значения
    bool isEnd = (i+1 >= line_v.size());
    // Получение статуса следующего параметра
    bool isNext{false};
    if (!isEnd) {
      getParam({line_v[i+1]}, isNext);
    }
    
    // Проход по типу
    switch (param.type) {
      case Arg::No: break; // Параметр без значений
      case Arg::Required: { // Параметр должен быть не пустой
        /* Проверка не является ли параметр последним
        * и не является ли следующий аргумент другим параметром
        */
        if ((isEnd)  || (isNext)) {
          error = "ERROR: is null option: " + line_v[i];
          return false;
        }
        // Установка значений
        param.value = line_v[i+1];
        i++;
        break;
      }
      case Arg::Optional: { // Параметр опциональный
        /* Проверка не является ли параметр последним
         * и не является ли следующий аргумент параметром
         */
        if ((isEnd)  || (isNext)) {
          // Знечение не переданно
          break;
        }
        if ((!isEnd)  && (!isNext)) {
          // Знечение  переданно
          param.value = line_v[i+1];
          i++;
          break;
        }
      }
    }
  }
  return true;
}

bool CmdLineParserLite::isNull() {
  return (line_v.empty()) ? (true) : (false);
}

std::string CmdLineParserLite::lastErr() { 
  return error;
}








