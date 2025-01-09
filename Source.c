#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <locale.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#define MAX_DATA_SIZE 1000 

// ��������� ��� ���� ������
const char* DATA_FILE = "data.txt";
const char* RESULT_FILE = "result.txt";

/*
 * ��������� ��� �������� ���������� �����:
 * K - ����� �����
 * ts - ����������� � �������� �������
 * mp - �������� � ���
 * minp - ����������������
 * indicator - �������� ���������� ����������
 */
typedef struct
{
    int K;
    int ts;
    float mp;
    int minp;
    double indicator;
} params;

params data_array[MAX_DATA_SIZE]; // ������ ��� �������� ������ � ������
int data_count = 0;               // ������� ������ �������

// ��������� �������
double indicator(params params1);
int write_array_to_file(const char* filename);
int manual_input(int minp, int* err);
int monitoring(int minp, int* err, const char* filename);
int generate_data(const char* filename);
int write_array(params params1);
int clear_result_file(const char* filename);
int sort_data_array();

/*
 * �������� ������� ���������
 * �������������� ����������������, ������������ ����� ������������
 * � �������� ��������������� ������� � ����������� �� ������
 */
int main()
{
    int minp;     // ����������������
    int err = 0;  // ������� ������
    int int_menu; // ���������� ��� ������ ������ ����
    setlocale(LC_CTYPE, "rus"); // ��������� ������ ��� ��������� �������� �����
    printf("[$ ��������� ������������ �������� �������� ������ mk6 $]\n");
    printf("\t\t����� ����������!\n");
    printf("��� ������ ������� ���������������� (100 - ��������, 10 - ����� � ��): ");
    scanf("%d", &minp);
    while (1)
    {
        // ����� ����
        printf("1) ������ ����\n");
        printf("2) ����� �����������\n");
        printf("3) ����������� ����������\n");
        printf("4) ������� ���������� ������\n");
        printf("5) ��������� ��������� ��������\n");
        printf("6) �������� ���� �����������\n");
        printf("7) �������� ������ � ����\n");
        printf("8) ������������� ������\n");
        printf("9) �����\n");
        scanf("%d", &int_menu);
        switch (int_menu)
        {
        case 1:
        {
            int result = manual_input(minp, &err);                   // ������ ����
            if (result > 0)
            {
                printf("������������ ����");
            }
            else if (result == -1)
            {
                printf("������ ������ ����������\n");
            }
            break;
        }
        case 2:
            if (monitoring(minp, &err, DATA_FILE) == -1)             // ����� �����������
            {
                printf("������ ��� �����������\n");
            }
            break;
        case 3:
            printf("������� ���������������� (100 - ��������): ");   // ��������� ����������������
            scanf("%d", &minp);
            break;
        case 4:
            printf("���������� ������ ��� �������� ������ = %d\n", err);    // ����� ���������� ������
            break;
        case 5:
            if (generate_data(DATA_FILE) == -1)                      // ��������� �������� � DATA_FILE
            {
                printf("������ ��� ��������� ������\n");
            }
            break;
        case 6:
            if (clear_result_file(RESULT_FILE) == -1)                // ������� RESULT_FILE
            {
                printf("������ ��� ������� ����� �����������\n");
            }
            break;
        case 7:
            if (write_array_to_file(RESULT_FILE) == -1)              // ������ ������ � RESULT_FILE
            {
                printf("������ ��� ������ ������ � ����\n");
            }
            break;
        case 8:
            if (sort_data_array() == -1)                             // ���������� �������, ����������� ������
            {
                printf("������ ������ ����\n");
            }
            break;
        case 9:
            printf("mk6, ���������� ������...");
            exit(0);                                                 // ����� �� ���������                             
        default:
            printf("������� ��������������� ���� �����\n");
            break;
        }
    }
    return 0;
}

/*
 * ������� ��� ������� ����� ������
 * ��������� ������������ ������� ��������� ����� �������
 * � ������ ������ ����� ����������� ������� ������
 * ���������� ���������� ������, ���� ���� �����������, � -1 ��� ������������ �������
 */
int manual_input(int minp, int* err)
{
    while (1)
    {
        int K, ts, choice;
        float mp;
        char pokaz[20];
        printf("������� ���������� (K_5#2.0mp#150ts) (0<=ts<=373): ");
        scanf("%20s", pokaz);

        // ������� ��������� ������ ��� ���������� ����������
        int count = sscanf(pokaz, "K_%d#%fmp#%dts", &K, &mp, &ts);
        params params1 = { K, ts, mp, minp, 0.0 };

        // �������� ������������ �����
        if (count != 3)
        {
            ++*err; // ���������� �������� ������ ��� �������� �����
            return *err;
        }
        params1.indicator = indicator(params1); // ���������� ����������
        printf("���� %d: ���������� %.1lf ���������\n", K, params1.indicator);
        if (write_array(params1) == -1)
        {
            return -1;
        }

        // ������ �� ����������� �����
        puts("������ ����� ����������? 1 - ��, 2 - ��� (����)\n");
        scanf("%d", &choice);
        if (choice == 2)
        {
            break;
        }
    }
    return 0; // ������ ��� => ���������� 0
}

/*
 * ������� ��� ����������� ������ �� �����
 * ������ ������ �� ����� � ��������� ���������� ��� ������� �����
 * � ������ ������ � ������ ����������� ������� ������
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

    // ������ ����� ���������
    while (fgets(pokaz, sizeof(pokaz), file))
    {
        // ������� ������ ��� ���������� ����������
        int count = sscanf(pokaz, "K_%d#%fmp#%dts", &K, &mp, &ts);
        if (count == 3)
        {
            params params1 = { K, ts, mp, minp, 0.0 };
            params1.indicator = indicator(params1); // ���������� ����������
            printf("���� %d: ���������� %.1lf ���������\n", K, params1.indicator);
            if (write_array(params1) == -1)
            {
                fclose(file);
                return -1;
            }
        }
        else
        {
            // ���������� �������� ������ ��� �������� ������
            ++*err;
        }
    }
    fclose(file);
    return 1;
}

/*
 * ������� ��� ���������� ���������� ����������
 * �� ������ ���������� ����� ��������� ���������� � ���������
 */
double indicator(params params1) // �������� ��������� �� ��������
{
    // ���������� ����������� ����������� ��������� �������� ���� tsf � x
    double tsf = (params1.ts + 273.15) / 647.14;
    double x = 1 - tsf;

    // ���������� ������� ����� ��� ������� ��������
    double drob = (-7.85823 + 1.83991 * pow(x, 1.5) - 11.7811 * pow(x, 3) + 22.6705 * pow(x, 3.5) - 15.9393 * pow(x, 4) + 1.77516 * pow(x, 7.5)) / params1.ts;

    // ������ ����������� �������� ��������� ����
    double ps = 22.064 * exp(drob);

    // ������ ���������� � ��������� � ������ ����������������
    double razl = fabs(ps - params1.mp) / fabs(params1.mp) * 100 * (params1.minp / 100.0);
    return razl;
}

/*
 * ������� ��� ��������� ��������� ������
 * ���������� ��������� �������� ���������� ������ � ����
 */
int generate_data(const char* filename)
{
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        return -1;
    }

    // ������������� ���������� ��������� �����
    srand(time(NULL));

    // ��������� ���������� ����� ����� �� 1 �� 200
    int num_lines = rand() % 200 + 1;

    // ������ ��������� ������ � ����
    for (int i = 1; i <= num_lines; i++)
    {
        float mp = (rand() % 20001) / 1000.0f + 1.0f;  // ��������� �������� mp �� 1.0 �� 20.0
        int ts = rand() % 374;  // ��������� �������� ts �� 0 �� 373
        fprintf(file, "K_%d#%.1fmp#%dts\n", i, mp, ts);
    }
    fclose(file);
    printf("������ ������������� � %s\n", filename);
    return num_lines;
}

/*
 * ������� ��� ������ ������ � ������
 * ��������� ������ � ����� � ������, ���� ���� �����
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
 * ������� ��� ������ ������ �� ������� � ����
 * ���������� ������ � ������ � ����
 */
int write_array_to_file(const char* filename)
{
    FILE* file = fopen(filename, "a");
    if (file == NULL)
    {
        return -1;
    }

    // ������ ������ �� ������� � ����
    for (int i = 0; i < data_count; i++)
    {
        fprintf(file, "���� %d: ���������� %.1lf ���������\n", data_array[i].K, data_array[i].indicator);
    }
    fclose(file);
    printf("������ �������� � ���� %s\n", filename);
    return 1;
}

/*
 * ������� ��� ������� ����� �����������
 * ������� ���������� �����
 */
int clear_result_file(const char* filename)
{
    FILE* file = fopen(filename, "w");
    if (file == NULL)
    {
        return -1;
    }
    fclose(file);
    printf("���������� ����� %s �������\n", filename);
    return 1;
}

/*
 * ������� ��� ���������� ������� ������
 * ��������� ������ �� ����������� �������� ���������� ����������
 * ���������� 1 � ������ ������, -1 ���� ������ ����
 */
int sort_data_array()
{
    // �������� �� ������� �������
    if (data_count == 0) {
        return -1;
    }

    // ���������� ���������
    for (int i = 0; i < data_count - 1; i++)
    {
        for (int j = i + 1; j < data_count; j++)
        {
            if (data_array[i].indicator > data_array[j].indicator)
            {
                // ����� ���������� � �������
                params temp = data_array[i];
                data_array[i] = data_array[j];
                data_array[j] = temp;
            }
        }
    }
    printf("������ ������������� �� �����������\n");
    return 1;
}