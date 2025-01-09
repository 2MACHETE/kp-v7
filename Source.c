#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <locale.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#define MAX_DATA_SIZE 1000 

/*
 * Структура для хранения параметров котла:
 * ts - температура в градусах Цельсия
 * mp - давление в МПа
 * minp - чувствительность
 */
typedef struct
{
    int ts;
    float mp;
    int minp;
} params;

int K_array[MAX_DATA_SIZE];            // Массив для хранения номеров котлов
double indicator_array[MAX_DATA_SIZE]; // Массив для хранения значений индикатора
int data_count = 0;                    // Счетчик данных в массивах

// Прототипы функций
double indicator(params params1);
void write_array_to_file();
int manual_input(int minp, int* err);
int monitoring(int minp, int* err);
int sensitive(int* minp);
int put_error(int err);
void generate_data();
void write_array(int K, params params1);
void clear_result_file();
void clear_data_array();
void sort_data_array();

/*
 * Основная функция программы
 * Инициализирует чувствительность, обрабатывает выбор пользователя
 * и вызывает соответствующие функции в зависимости от выбора
 */
void main()
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
            manual_input(minp, &err); // Ручной ввод данных
            break;
        case 2:
            monitoring(minp, &err);   // Режим мониторинга
            break;
        case 3:
            sensitive(&minp);         // Изменение чувствительности
            break;
        case 4:
            put_error(err);           // Вывод количества ошибок
            break;
        case 5:
            generate_data();          // Генерация случайных данных
            break;
        case 6:
            clear_result_file();      // Очистка файла результатов
            break;
        case 7:
            write_array_to_file();    // Запись данных в файл
            break;
        case 8:
            sort_data_array();        // Сортировка данных
            break;
        case 9:
            printf("mk6, завершение работы...");
            exit(1);                 // Выход из программы
        default:
            printf("Введите соответствующий меню пункт\n");
            break;
        }
    }
}

/*
 * Функция для ручного ввода данных
 * Позволяет пользователю вводить параметры котла вручную
 * В случае ошибки ввода увеличивает счетчик ошибок
 */
int manual_input(int minp, int* err)
{
    while (1)
    {
        int K, ts, choice;
        float mp;
        char pokaz[20];
        printf("Введите показатели (K_5#2.0mp#150ts) (0<=ts<=373): ");
        /*
        * Для корректной работы программы необходимо вводить
        * значение ts в указанном сверху диапазоне
        * При выходе за предел в функции indicator будет рассчитан
        * отрицательный X, который потом попадет в функцию pow
        * В языке Си при возведении в отрицательную степень
        * числа с плавающей запятой pow возвращает NaN
        */
        scanf("%20s", pokaz);

        // Парсинг введенной строки для извлечения параметров
        int count = sscanf(pokaz, "K_%d#%fmp#%dts", &K, &mp, &ts);
        params params1 = { ts, mp, minp };

        // Проверка корректности ввода
        if (count != 3)
        {
            ++*err; // Увеличение счетчика ошибок при неверном вводе
        }
        printf("Котёл %d: отклонение %.1lf процентов\n", K, indicator(params1));
        write_array(K, params1);

        // Запрос на продолжение ввода
        puts("Ввести новые показатели? 1 - да, 2 - нет (меню)\n");
        scanf("%d", &choice);
        if (choice == 2)
        {
            break;
        }
    }
    return 0;
}

/*
 * Функция для мониторинга данных из файла
 * Читает данные из файла "data.txt" и вычисляет отклонение для каждого котла
 * В случае ошибки в строке увеличивает счетчик ошибок
 */
int monitoring(int minp, int* err)
{
    int K, ts;
    float mp;
    char pokaz[50];
    FILE* file = fopen("data.txt", "r");
    if (file == NULL)
    {
        printf("Ошибка открытия файла\n");
        return 0;
    }

    // Чтение файла построчно
    while (fgets(pokaz, sizeof(pokaz), file))
    {
        // Парсинг строки для извлечения параметров
        int count = sscanf(pokaz, "K_%d#%fmp#%dts", &K, &mp, &ts);
        if (count == 3)
        {
            params params1 = { ts, mp, minp };
            printf("Котёл %d: отклонение %.1lf процентов\n", K, indicator(params1));
            write_array(K, params1);
        }
        else
        {
            // Вывод строки с ошибкой
            printf("Ошибка в строке: %s\n", pokaz);
            ++*err;
        }
    }
    fclose(file);
    return 0;
}

/*
 * Функция для изменения чувствительности
 * Позволяет пользователю ввести новое значение чувствительности
 */
int sensitive(int* minp)
{
    printf("Введите чувствительность (100 - стандарт): ");
    scanf("%d", minp);
    return 0;
}

/*
 * Функция для вывода количества ошибок
 * Выводит текущее значение счетчика ошибок
 */
int put_error(int err)
{
    printf("Количество ошибок = %d\n", err);
    return 0;
}

/*
 * Функция для вычисления индикатора отклонения
 * На основе параметров котла вычисляет отклонение в процентах
 */
double indicator(params params1)
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
 * Записывает случайные значения параметров котлов в файл "data.txt"
 */
void generate_data()
{
    FILE* file = fopen("data.txt", "w");
    if (file == NULL) {
        printf("Ошибка открытия файла\n");
        return;
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
    printf("Данные сгенерированы в data.txt\n");
    return;
}

/*
 * Функция для записи данных в массивы
 * Добавляет данные о котле в массивы, если есть место
 */
void write_array(int K, params params1)
{
    if (data_count < MAX_DATA_SIZE)
    {
        K_array[data_count] = K;
        indicator_array[data_count] = indicator(params1);
        data_count++;
    }
    else {
        printf("Массивы данных переполнен\n");
    }
    return;
}

/*
 * Функция для записи данных из массивов в файл
 * Записывает данные о котлах в файл "result.txt"
 */
void write_array_to_file()
{
    FILE* file = fopen("result.txt", "a");
    if (file == NULL)
    {
        printf("Ошибка открытия файла\n");
        return;
    }

    // Запись данных из массивов в файл
    for (int i = 0; i < data_count; i++)
    {
        fprintf(file, "Котёл %d: отклонение %.1lf процентов\n", K_array[i], indicator_array[i]);
    }
    fclose(file);
    printf("Данные записаны в файл result.txt\n");
    return;
}

/*
 * Функция для очистки файла результатов
 * Очищает содержимое файла "result.txt"
 */
void clear_result_file()
{
    FILE* file = fopen("result.txt", "w");
    if (file == NULL)
    {
        printf("Ошибка открытия файла\n");
        return;
    }
    fclose(file);
    printf("Содержимое очищено\n");
    return;
}

/*
 * Функция для сортировки массивов данных
 * Сортирует данные по возрастанию значения индикатора отклонения
 */
void sort_data_array()
{
    // Сортировка пузырьком
    for (int i = 0; i < data_count - 1; i++)
    {
        for (int j = i + 1; j < data_count; j++)
        {
            if (indicator_array[i] > indicator_array[j])
            {
                // Обмен значениями в массивах
                int temp_K = K_array[i];
                K_array[i] = K_array[j];
                K_array[j] = temp_K;

                double temp_indicator = indicator_array[i];
                indicator_array[i] = indicator_array[j];
                indicator_array[j] = temp_indicator;
            }
        }
    }
    printf("Данные отсортированы по возрастанию\n");
    return;
}