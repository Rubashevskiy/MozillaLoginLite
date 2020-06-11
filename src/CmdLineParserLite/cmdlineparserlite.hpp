/**
 * @author Yuriy Rubashevskiy (r9182230628@gmail.com)
 * @brief Парсинг командной строки в объектном режиме
 * @version 0.1
 * @date 2020-06-05
 * 
 * @copyright Copyright (c) 2020 Free   Software   Foundation,  Inc.
 *     License  GPLv3+:  GNU  GPL  version  3  or  later <http://gnu.org/licenses/gpl.html>.
 *     This is free software: you are free to change and redistribute it.  
 *     There is NO WARRANTY, to the  extent  permitted by law.
 */

#ifndef CMDLINEPARSERLITE_HPP
#define CMDLINEPARSERLITE_HPP

/*Стандартные библиотеки:*/
#include <functional>
#include <stdexcept>
#include <string>
#include <cstring>
#include <vector>
#include <list>

/*! \namespace CMDPARS
 *  @brief Область видимости CMDPARS
 */
namespace CMDPARS {

  /*! \enum Arg
   * @brief Тип параметра
   */
  enum class Arg {
    No,       ///< Без значения
    Required, ///< С обязательным значением
    Optional  ///< Опциональный параметр
  };
  
  /*! \struct Param
   * @brief Структура для работы с параметрами
   */
  struct Param {
    /**
     * @brief Конструктор
     * @param paramName Имена параметров(Команды[без - и --], короткие ключи[-], длинные[--])
     * @param has_arg   Тип параметра
     */
    Param(std::list<std::string> paramName, Arg has_arg) : name(paramName), type(has_arg) {};
    std::list<std::string> name{};     ///< Список имен
    Arg                    type{};     ///< Тип параметра
    std::string            value{};    ///< Значение параметра
    bool                   set{false}; ///< Установлен ли параметр
  };
  
  /*! \class CmdLineParserLite
   *  @brief Класс для парсинга командной строки в объектном режиме
   */
  class CmdLineParserLite {
  public:
    /*!
     * @brief Конструктор
     * @param argc Число аргументов
     * @param argv Масив аргументов
     */
    CmdLineParserLite(int argc,  char* argv[]);

    /**
     * @brief Деструктор
     * 
     */
    ~CmdLineParserLite();

    /*!
     * @brief Функция добавление объектов "Param" для парсинга
     * @param param Параметр
     * @throw std::runtime_error в случае добавление некоректных параметров(дубли, пробелы)
     */
    void addParam(Param *param);
    
    /*!
     * @brief Запуск парсинга командной строки
     * @return true  Успешно
     * @return false Ошибка
     */
    bool parse();
    
    /*!
     * @brief  Проверка были ли переданны параметры
     * @return true  Не переданны
     * @return false Переданны
     */
    bool isNull();

    /*!
     * @brief Возврат ошибок при парсинге
     * @return std::string Текс ошибки
     */
    std::string lastErr();
  private:
    /*!
     * @brief Возврат указателя на объект Param из масива по имени
     * @param name Имя параметра или имена
     * @param find    // Флаг что параметр найден
     * @return Param&  указателя на объект Param
     */
    Param& getParam(std::list<std::string> name, bool &find);
  private:
    std::vector<std::string> line_v{};                     ///< переданные параметры
    std::vector<std::reference_wrapper<Param>> params_v{}; ///< параметры для парсинга
    std::string error{};                                   ///< текст ошибки
  };
}  //namespace CMDPARS

#endif // CMDLINEPARSERLITE_HPP
