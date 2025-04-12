<p align="center">
  <img src="https://github.com/user-attachments/assets/8ce290eb-718b-49d6-9ea9-cec69a221b0a" width="250" alt="Демо NoEngine">
</p>

<div align="center">
  
# 🎮 NoEngine - 3D-движок на OpenGL
  
</div>

<p align="center">
NoEngine - прототип игрового 3D-движка с открытым исходным кодом. Предоставляет базовый функционал редактора сцен и управления примитивами. В основе рендеринга лежит графический API - OpenGL.
</p>

<div align="center">
  
## 💻 Демо-проекты
  
</div>

<p align="center">
Движок позволяет создавать разные по сложности проекты: от простых 3D-сцен до полноценных моделей физических процессов. Вот небольшая подборка готовых работ: 
</p>


<p align="center">
  <img src="https://github.com/user-attachments/assets/f7753ce6-cd3b-4ba8-9651-1112d4a57234" width="45%">
  <img src="https://github.com/user-attachments/assets/4869f44e-fee3-4086-8419-a234f9b8f967" width="45%">
</p>

<div align="center">
  <div style="margin-top: 20px;">
    <a href="https://rutube.ru/video/private/43e362a3aa597fd115dd18115a698b41/?r=wd&p=z0HPN3Zm2BjLQ3zuTmBEVA" target="_blank">
      <img src="https://github.com/user-attachments/assets/c934d679-72de-4b26-8ee1-e3fa7f206190" width="90%">
    </a>
  </div>
</div>

## 🔥 Возможности

* 📦 Редактор 3D-объектов (кубы, сферы, плоскости, источники света)

* 💡 Динамическое освещение (Directional, Point, Spot lights)

* 🎥 Управление FPS камерой

* 🛠 Встроенный редактор материалов (цвет, ambient/diffuse/specular)

* ⚙️ Поддержка пользовательских скриптов для объектов

## 🔄 Установка

### Зависимости 
* OpenGL 4.3+
* C++17 и выше
* VC++ Redistributable 2022
* ОС Windows 10/11

### 1. Установка готовой сборки проекта 
Наиболее простой способ - это использование установщика проекта. Чтобы скачать установщик, нужно перейти по [ссылке](https://github.com/MokArtiy/NoEngine/releases/tag/v0.1.0) и загрузить на устройство файл **NoEngineEditor_Setup.exe**. Далее запустить установщик и следовать шагам установки.

### 2. Самостоятельная сборка проекта
Если нужно полностью контролировать процесс установки или установка первым способом вызвала ошибки, можно собрать проект самостоятельно. Для этого на устройстве должны быть установлены:

* **CMake** - утилита сборки проектов  
Проверить наличие:  
```bash
 cmake --version
```  
Если команда не работает, скачайте с [официального сайта](https://cmake.org/)

* **Компилятор C++** (GCC/Clang/MSVC *- предпочтительнее*)  
Минимальная версия: C++17

После всех подготовительных этапов [скачайте](https://github.com/MokArtiy/NoEngine/releases/tag/v0.1.0) и распакуйте архив с проектом. После установки нужно перейти в каталог проекта и создать папку `build`:
```bash
 cd путь_к_проекту
 mkdir build
 cd build
```
После запустить сборку проекта с помощью CMake:
```cmd
:: Сгенерировать файлы сборки (для Visual Studio 2022)
cmake .. -G "Visual Studio 17 2022" -A x64

:: Альтернативные генераторы:
:: Для MinGW: cmake .. -G "MinGW Makefiles"
:: Для Ninja: cmake .. -G "Ninja"

cmake --build . --config Release
```
Исполняемый файл появится в каталоге `build\bin\Release`.

## 📚 Использование

### Перемещение по сцене
* WASD - движение камеры впрао/влево/вперед/назад
* SPACE - движение камеры вверх
* SHIFT - движение камеры вниз
* ПКМ - вращение камеры
* ЛКМ + ПКМ - движение вправо/влево/вверх/вниз

### Взаимодействие с интерфейсом
* ЛКМ - взаимодействие с интерфейсом
* ЛКМ + ALT - медленное изменение значения (для слайдеров)
* ЛКМ + SHIFT - быстрое изменение значения (для слайдеров)
* ЛКМ + CTRL - ввести значение вручную (для слайдеров)

### Навигация по интерфейсу
<table>
  <tr>
    <td valign="top" width="50%">
      <img src="https://github.com/user-attachments/assets/4d033baf-35ab-44fb-b18a-c471405f7d0d" style="height: 250px; width: 100%; object-fit: contain;"><br>
      <strong>Настройки камеры</strong><br>
      Скорость передвижения, позиция, поворот, FOV, плоскости
    </td>
    <td valign="top" width="50%">
      <img src="https://github.com/user-attachments/assets/558b085e-e069-4daa-9f84-2aaf824c7948" style="height: 250px; width: 100%; object-fit: contain;"><br>
      <strong>Настройка направленного освещения</strong><br>
      Направление, характеристики света
    </td>
  </tr>
  <tr>
    <td valign="top" width="50%">
      <img src="https://github.com/user-attachments/assets/c17818bb-820c-4917-a522-57a5a7af6709" style="height: 300px; width: 100%; object-fit: contain;"><br>
      <strong>Настройка "Прожекторного" освещение</strong><br>
      В движке представлен в виде фонарика. Настройка характеристик света
    </td>
    <td valign="top" width="50%">
      <img src="https://github.com/user-attachments/assets/9630eafb-455c-4d75-a93a-d956dd1127f1" style="height: 300px; width: 100%; object-fit: contain;"><br>
      <strong>Настройка и редактирование объектов</strong><br>
      Основные трансформации над объектом (перемещение, поворот, масштабирование), редактирование материала объекта, удаление и изменение функции обновления
    </td>
  </tr>
</table>

### Функциональые кнопки

<table>
  <tr>
    <td valign="top" width="25%" style="padding: 10px; text-align: center;">
      <img src="https://github.com/user-attachments/assets/dceda73a-e049-4941-a94a-3ed8322364c8" style="height: 128px; width: 128px; object-fit: contain; display: block; margin: 0 auto;"><br>
      <strong>Добавление объекта</strong>
    </td>
    <td valign="top" width="25%" style="padding: 10px; text-align: center;">
      <img src="https://github.com/user-attachments/assets/74313a0c-7225-4755-b216-8244c4bd8658" style="height: 128px; width: 128px; object-fit: contain; display: block; margin: 0 auto;"><br>
      <strong>Вкл/Выкл отоброжение сеток</strong>
    </td>
    <td valign="top" width="25%" style="padding: 10px; text-align: center;">
      <img src="https://github.com/user-attachments/assets/2b69d1d0-586b-403c-aa0c-8e0e81c4389d" style="height: 128px; width: 128px; object-fit: contain; display: block; margin: 0 auto;"><br>
      <strong>Переключение режимов камеры</strong>
    </td>
    <td valign="top" width="25%" style="padding: 10px; text-align: center;">
      <img src="https://github.com/user-attachments/assets/e447ab19-8e0c-46e4-b61e-144acb628e54" style="height: 128px; width: 200px; object-fit: contain; display: block; margin: 0 auto;"><br>
      <strong>Управление состояниями сцены</strong>
    </td>
  </tr>
</table>

### Настройка материалов и света

В основе освещения в движке лежит модифицированная модель освещения Фонга, поэтому значения параметров материалов и света задаются, опираясь на законы этой модели. Для получения реалистичных результатов рекомендуется пользоваться специальными таблицами с подобранными значениями. Примерами таких таблиц являются [Таблица материалов](http://devernay.free.fr/cours/opengl/materials.html) и [Таблица настройки источников света](https://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation). 

### EDIT UPDATE FUNCTION

Edit Update Function - изменение функции обновления объекта - одна из ключевых особенностей движка, позволяющая изменять значения основных характеристик объекта (позиция, поворот, масштаб). С её помощью, например, можно заставить объект перемещаться по сцене по траектории, заданной математической функцией.
Для того чтобы открыть меню редактирования, нужно выделить объект и нажать на кнопку **EDIT UPDATE FUNCTION** в основном меню редактирования объектов. Откроется следующее меню:

![image](https://github.com/user-attachments/assets/659cb9fc-1e5d-45b6-b1c5-52caa0d34312)

Здесь можно выбрать, какая характеристика будет изменяться (Position, Rotation, Scale) и в какой координате будут изменения (X, Y, Z). 

`TIME` `DELTA` - переменные времени - чтобы заставить двигаться объект, **обязательно** нужно включить их в своё уравнение. 

`POS_X` `POS_Y` `POS_Z` (для изменения Position) - переменные, хранящие текущие значения позиции объекта (для Position). 

`Close` `Save` `Reset` - функциональные кнопки взаимодействия с окном и текущими изменениями.

В своей формуле можно использовать основные символы математических операций и функций (+, -, /, sin, cos, и т.д.).

Вот пример формулы задания движения объекта по окружности в центре координат и радиусом 3: 
```cmd
:: POS_X
3 * cos(TIME)
:: POS_Y
3 * sin(TIME)
```
После того, как формула была записана, её нужно сохранить `Save` и переключить состояние сцены на **RUN**, если всё сделано правильно, объект начнет изменять свои значения по заданной функции. Если допущена синтаксическая ошибка, информация об этом появится в **окне с логами**.

### Логирование

![image](https://github.com/user-attachments/assets/79ded76f-bc47-4ce0-80ed-f8952ef61739)

Помимо окна с редактором запускается командная строка с логами. В ней будут отображаться информационные логи (например логи о текущей системе в начале), предупреждения и ошибки. Если в процессе работы что-то ведёт себя неправильно или программа ломается, в первую очередь нужно смотреть именно туда.

