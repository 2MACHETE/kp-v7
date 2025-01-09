#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <locale.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#define MAX_DATA_SIZE 1000 

// Константы для имен файлов
const char* DATA_FILE = "data.txt";
const char* RESULT_FILE = "result.txt";

/*
 * Структура для хранения параметров котла:
 * K - номер котла
 * ts - температура в градусах Цельсия
 * mp - давление в МПа
 * minp - чувствительность
 * indicator - значение индикатора отклонения
 */
typedef struct
{
    int K;
    int ts;
    float mp;
    int minp;
    double indicator;
} params;

params data_array[MAX_DATA_SIZE]; // Массив для хранения данных о котлах
int data_count = 0;               // Счетчик данных массива

// Прототипы функций
double indicator(params params1);
int write_array_to_file(const char* filename);
int manual_input(int minp, int* err);
int monitoring(int minp, int* err, const char* filename);
int generate_data(const char* filename);
int write_array(params params1);
int clear_result_file(const char* filename);
int sort_data_array();

/*
 * Основная функция программы
 * Инициализирует чувствительность, обрабатывает выбор пользователя
 * и вызывает соответствующие функции в зависимости от выбора
 */
int main()
{
    int minp;     // Чувствительность
    int err = 0;  // Счетчик ошибок
    int int_menu; // Переменная для выбора пункта меню
    setlocale(LC_CTYPE, "rus"); // Установка локали для поддержки русского языка
    printf("[$ Программа отслеживания значений тепловых котлов mk6 $]\n");
    printf("\t\tДобро пожаловать!\n");
    printf("Для начала введите чувствительность (100 - стандарт, 10 - малая и тд): ");
    scanf("%d", &minp);
    while (1)
    {
        // Вывод меню
        printf("1) Ручной ввод\n");
        printf("2) Режим мониторинга\n");
        printf("3) Модификация параметров\n");
        printf("4) Вывести количество ошибок\n");
        printf("5) Генерация случайных значений\n");
        printf("6) Очистить файл результатов\n");
        printf("7) Записать данные в файл\n");
        printf("8) Отсортировать данные\n");
        printf("9) Выход\n");
        scanf("%d", &int_menu);
        switch (int_menu)
        {
        case 1:
        {
            int result = manual_input(minp, &err);                   // Ручной ввод
            if (result > 0)
            {
                printf("Некорректный ввод");
            }
            else if (result == -1)
            {
                printf("Массив данных переполнен\n");
            }
            break;
        }
        case 2:
            if (monitoring(minp, &err, DATA_FILE) == -1)             // Режим мониторинга
            {
                printf("Ошибка при мониторинге\n");
            }
            break;
        case 3:
            printf("Введите чувствительность (100 - стандарт): ");   // Изменение чувствительности
            scanf("%d", &minp);
            break;
        case 4:
            printf("Количество ошибок при передачи данных = %d\n", err);    // Вывод количества ошибок
            break;
        case 5:
            if (generate_data(DATA_FILE) == -1)                      // Генерация значений в DATA_FILE
            {
                printf("Ошибка при генерации данных\n");
            }
            break;
        case 6:
            if (clear_result_file(RESULT_FILE) == -1)                // Очистка RESULT_FILE
            {
                printf("Ошибка при очистке файла результатов\n");
            }
            break;
        case 7:
            if (write_array_to_file(RESULT_FILE) == -1)              // Запись данных в RESULT_FILE
            {
                printf("Ошибка при записи данных в файл\n");
            }
            break;
        case 8:
            if (sort_data_array() == -1)                             // Сортировка массива, накопленных данных
            {
                printf("Массив данных пуст\n");
            }
            break;
        case 9:
            printf("mk6, завершение работы...");
            exit(0);                                                 // Выход из программы                             
        default:
            printf("Введите соответствующий меню пункт\n");
            break;
        }
    }
    return 0;
}

/*
 * Функция для ручного ввода данных
 * Позволяет пользователю вводить параметры котла вручную
 * В случае ошибки ввода увеличивает счетчик ошибок
 * Возвращает количество ошибок, если ввод некорректен, и -1 при переполнении массива
 */
int manual_input(int minp, int* err)
{
    while (1)
    {
        int K, ts, choice;
        float mp;
        char pokaz[20];
        printf("Введите показатели (K_5#2.0mp#150ts) (0<=ts<=373): ");
        scanf("%20s", pokaz);

        // Парсинг введенной строки для извлечения параметров
        int count = sscanf(pokaz, "K_%d#%fmp#%dts", &K, &mp, &ts);
        params params1 = { K, ts, mp, minp, 0.0 };

        // Проверка корректности ввода
        if (count != 3)
        {
            ++*err; // Увеличение счетчика ошибок при неверном вводе
            return *err;
        }
        params1.indicator = indicator(params1); // Вычисление индикатора
        printf("Котёл %d: отклонение %.1lf процентов\n", K, params1.indicator);
        if (write_array(params1) == -1)
        {
            return -1;
        }

        // Запрос на продолжение ввода
        puts("Ввести новые показатели? 1 - да, 2 - нет (меню)\n");
        scanf("%d", &choice);
        if (choice == 2)
        {
            break;
        }
    }
    return 0; // Ошибок нет => возвращаем 0
}

/*
 * Функция для мониторинга данных из файла
 * Читает данные из файла и вычисляет отклонение для каждого котла
 * В случае ошибки в строке увеличивает счетчик ошибок
 */
int monitoring(int minp, int* err, const char* filename)
{
    int K, ts;
    float mp;
    char pokaz[50];
    FILE* file = fopen(filename, "r");
    if (file == NULL)
    {
        return -1;
    }

    // Чтение файла построчно
    while (fgets(pokaz, sizeof(pokaz), file))
    {
        // Парсинг строки для извлечения параметров
        int count = sscanf(pokaz, "K_%d#%fmp#%dts", &K, &mp, &ts);
        if (count == 3)
        {
            params params1 = { K, ts, mp, minp, 0.0 };
            params1.indicator = indicator(params1); // Вычисление индикатора
            printf("Котёл %d: отклонение %.1lf процентов\n", K, params1.indicator);
            if (write_array(params1) == -1)
            {
                fclose(file);
                return -1;
            }
        }
        else
        {
            // Увеличение счетчика ошибок при неверной строке
            ++*err;
        }
    }
    fclose(file);
    return 1;
}

/*
 * Функция для вычисления индикатора отклонения
 * На основе параметров котла вычисляет отклонение в процентах
 */
double indicator(params params1) // Передаем структуру по значению
{
    // Вычисление приведенной температуры насыщения водяного пара tsf и x
    double tsf = (params1.ts + 273.15) / 647.14;
    double x = 1 - tsf;

    // Вычисление дробной части для расчета давления
    double drob = (-7.85823 + 1.83991 * pow(x, 1.5) - 11.7811 * pow(x, 3) + 22.6705 * pow(x, 3.5) - 15.9393 * pow(x, 4) + 1.77516 * pow(x, 7.5)) / params1.ts;

    // Расчет абсолютного давления насыщения пара
    double ps = 22.064 * exp(drob);

    // Расчет отклонения в процентах с учетом чувствительности
    double razl = fabs(ps - params1.mp) / fabs(params1.mp) * 100 * (params1.minp / 100.0);
    return razl;
}

/*
 * Функция для генерации случайных данных
 * Записывает случайные значения параметров котлов в файл
 */
int generate_data(const char* filename)
{
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        return -1;
    }

    // Инициализация генератора случайных чисел
    srand(time(NULL));

    // Генерация случайного числа строк от 1 до 200
    int num_lines = rand() % 200 + 1;

    // Запись случайных данных в файл
    for (int i = 1; i <= num_lines; i++)
    {
        float mp = (rand() % 20001) / 1000.0f + 1.0f;  // Случайное значение mp от 1.0 до 20.0
        int ts = rand() % 374;  // Случайное значение ts от 0 до 373
        fprintf(file, "K_%d#%.1fmp#%dts\n", i, mp, ts);
    }
    fclose(file);
    printf("Данные сгенерированы в %s\n", filename);
    return num_lines;
}

/*
 * Функция для записи данных в массив
 * Добавляет данные о котле в массив, если есть место
 */
int write_array(params params1)
{
    if (data_count < MAX_DATA_SIZE)
    {
        data_array[data_count] = params1;
        data_count++;
        return 1;
    }
    else {
        return -1;
    }
}

/*
 * Функция для записи данных из массива в файл
 * Записывает данные о котлах в файл
 */
int write_array_to_file(const char* filename)
{
    FILE* file = fopen(filename, "a");
    if (file == NULL)
    {
        return -1;
    }

    // Запись данных из массива в файл
    for (int i = 0; i < data_count; i++)
    {
        fprintf(file, "Котёл %d: отклонение %.1lf процентов\n", data_array[i].K, data_array[i].indicator);
    }
    fclose(file);
    printf("Данные записаны в файл %s\n", filename);
    return 1;
}

/*
 * Функция для очистки файла результатов
 * Очищает содержимое файла
 */
int clear_result_file(const char* filename)
{
    FILE* file = fopen(filename, "w");
    if (file == NULL)
    {
        return -1;
    }
    fclose(file);
    printf("Содержимое файла %s очищено\n", filename);
    return 1;
}

/*
 * Функция для сортировки массива данных
 * Сортирует данные по возрастанию значения индикатора отклонения
 * Возвращает 1 в случае успеха, -1 если массив пуст
 */
int sort_data_array()
{
    // Проверка на пустоту массива
    if (data_count == 0) {
        return -1;
    }

    // Сортировка пузырьком
    for (int i = 0; i < data_count - 1; i++)
    {
        for (int j = i + 1; j < data_count; j++)
        {
            if (data_array[i].indicator > data_array[j].indicator)
            {
                // Обмен значениями в массиве
                params temp = data_array[i];
                data_array[i] = data_array[j];
                data_array[j] = temp;
            }
        }
    }
    printf("Данные отсортированы по возрастанию\n");
    return 1;
}