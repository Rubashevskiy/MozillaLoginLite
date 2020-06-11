#!/bin/bash
#Название проекта
project="mll"
#Каталог проекта
path=$PWD
#Каталог сборки
build="build"
#Расположение CmakeList.txt
cmakeList="cmake"

echo "Build APP : ${project}"
echo "Path build project : ${path}/${build}"

#Проверка наличия каталога, если нет создание.
#Аргументы: Полный путь к каталогу.
#Возвращает 0 - Успешно || 1 - Ошибка'
function foundOrCreateDir () {
  if ([ ! -n "$1" ]); then 
    echo "Error found or CreateDir: param is empty"
    exit 1
  fi
  if [ -d $1 ]; then
    return 0
  else
    if ( mkdir -p $1 ); then
      return 0
    else 
      echo "Error: create dir $1"
      return 1
    fi
  fi
}


#Сборка тестового проекта
function buildProject() {
  if ([ ! -n "$1" ]) || ([ ! -n "$2" ]); then 
    echo "Error buildProject: param is empty"
    exit
  fi
  #Переход в каталог сборки
  cd $1
  #Генерируем Makefile и собираем проект
  if (cmake $2) && (make); then
    echo "Build APP : ${project} OK"
  else
    echo "Build APP : ${project} ERROR"
    exit
  fi
  # Возврат в корень каталога
  cd ${path}
}


# Проверка существования папки build в корне проекта, если нет создаем
if (! foundOrCreateDir "${path}/${build}/"); then
  exit
fi

# Сборка тестового проекта
buildProject "${path}/${build}/" "${path}/${cmakeList}/"

# Эникей
read -n 1 -s -r -p "Press any key to exit"
echo
