#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <locale.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#define MAX_DATA_SIZE 1000 

/*
 * ��������� ��� �������� ���������� �����:
 * ts - ����������� � �������� �������
 * mp - �������� � ���
 * minp - ����������������
 */
typedef struct
{
    int ts;
    float mp;
    int minp;
} params;

int K_array[MAX_DATA_SIZE];            // ������ ��� �������� ������� ������
double indicator_array[MAX_DATA_SIZE]; // ������ ��� �������� �������� ����������
int data_count = 0;                    // ������� ������ � ��������

// ��������� �������
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
 * �������� ������� ���������
 * �������������� ����������������, ������������ ����� ������������
 * � �������� ��������������� ������� � ����������� �� ������
 */
void main()
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
            manual_input(minp, &err); // ������ ���� ������
            break;
        case 2:
            monitoring(minp, &err);   // ����� �����������
            break;
        case 3:
            sensitive(&minp);         // ��������� ����������������
            break;
        case 4:
            put_error(err);           // ����� ���������� ������
            break;
        case 5:
            generate_data();          // ��������� ��������� ������
            break;
        case 6:
            clear_result_file();      // ������� ����� �����������
            break;
        case 7:
            write_array_to_file();    // ������ ������ � ����
            break;
        case 8:
            sort_data_array();        // ���������� ������
            break;
        case 9:
            printf("mk6, ���������� ������...");
            exit(1);                 // ����� �� ���������
        default:
            printf("������� ��������������� ���� �����\n");
            break;
        }
    }
}

/*
 * ������� ��� ������� ����� ������
 * ��������� ������������ ������� ��������� ����� �������
 * � ������ ������ ����� ����������� ������� ������
 */
int manual_input(int minp, int* err)
{
    while (1)
    {
        int K, ts, choice;
        float mp;
        char pokaz[20];
        printf("������� ���������� (K_5#2.0mp#150ts) (0<=ts<=373): ");
        /*
        * ��� ���������� ������ ��������� ���������� �������
        * �������� ts � ��������� ������ ���������
        * ��� ������ �� ������ � ������� indicator ����� ���������
        * ������������� X, ������� ����� ������� � ������� pow
        * � ����� �� ��� ���������� � ������������� �������
        * ����� � ��������� ������� pow ���������� NaN
        */
        scanf("%20s", pokaz);

        // ������� ��������� ������ ��� ���������� ����������
        int count = sscanf(pokaz, "K_%d#%fmp#%dts", &K, &mp, &ts);
        params params1 = { ts, mp, minp };

        // �������� ������������ �����
        if (count != 3)
        {
            ++*err; // ���������� �������� ������ ��� �������� �����
        }
        printf("���� %d: ���������� %.1lf ���������\n", K, indicator(params1));
        write_array(K, params1);

        // ������ �� ����������� �����
        puts("������ ����� ����������? 1 - ��, 2 - ��� (����)\n");
        scanf("%d", &choice);
        if (choice == 2)
        {
            break;
        }
    }
    return 0;
}

/*
 * ������� ��� ����������� ������ �� �����
 * ������ ������ �� ����� "data.txt" � ��������� ���������� ��� ������� �����
 * � ������ ������ � ������ ����������� ������� ������
 */
int monitoring(int minp, int* err)
{
    int K, ts;
    float mp;
    char pokaz[50];
    FILE* file = fopen("data.txt", "r");
    if (file == NULL)
    {
        printf("������ �������� �����\n");
        return 0;
    }

    // ������ ����� ���������
    while (fgets(pokaz, sizeof(pokaz), file))
    {
        // ������� ������ ��� ���������� ����������
        int count = sscanf(pokaz, "K_%d#%fmp#%dts", &K, &mp, &ts);
        if (count == 3)
        {
            params params1 = { ts, mp, minp };
            printf("���� %d: ���������� %.1lf ���������\n", K, indicator(params1));
            write_array(K, params1);
        }
        else
        {
            // ����� ������ � �������
            printf("������ � ������: %s\n", pokaz);
            ++*err;
        }
    }
    fclose(file);
    return 0;
}

/*
 * ������� ��� ��������� ����������������
 * ��������� ������������ ������ ����� �������� ����������������
 */
int sensitive(int* minp)
{
    printf("������� ���������������� (100 - ��������): ");
    scanf("%d", minp);
    return 0;
}

/*
 * ������� ��� ������ ���������� ������
 * ������� ������� �������� �������� ������
 */
int put_error(int err)
{
    printf("���������� ������ = %d\n", err);
    return 0;
}

/*
 * ������� ��� ���������� ���������� ����������
 * �� ������ ���������� ����� ��������� ���������� � ���������
 */
double indicator(params params1)
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
 * ���������� ��������� �������� ���������� ������ � ���� "data.txt"
 */
void generate_data()
{
    FILE* file = fopen("data.txt", "w");
    if (file == NULL) {
        printf("������ �������� �����\n");
        return;
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
    printf("������ ������������� � data.txt\n");
    return;
}

/*
 * ������� ��� ������ ������ � �������
 * ��������� ������ � ����� � �������, ���� ���� �����
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
        printf("������� ������ ����������\n");
    }
    return;
}

/*
 * ������� ��� ������ ������ �� �������� � ����
 * ���������� ������ � ������ � ���� "result.txt"
 */
void write_array_to_file()
{
    FILE* file = fopen("result.txt", "a");
    if (file == NULL)
    {
        printf("������ �������� �����\n");
        return;
    }

    // ������ ������ �� �������� � ����
    for (int i = 0; i < data_count; i++)
    {
        fprintf(file, "���� %d: ���������� %.1lf ���������\n", K_array[i], indicator_array[i]);
    }
    fclose(file);
    printf("������ �������� � ���� result.txt\n");
    return;
}

/*
 * ������� ��� ������� ����� �����������
 * ������� ���������� ����� "result.txt"
 */
void clear_result_file()
{
    FILE* file = fopen("result.txt", "w");
    if (file == NULL)
    {
        printf("������ �������� �����\n");
        return;
    }
    fclose(file);
    printf("���������� �������\n");
    return;
}

/*
 * ������� ��� ���������� �������� ������
 * ��������� ������ �� ����������� �������� ���������� ����������
 */
void sort_data_array()
{
    // ���������� ���������
    for (int i = 0; i < data_count - 1; i++)
    {
        for (int j = i + 1; j < data_count; j++)
        {
            if (indicator_array[i] > indicator_array[j])
            {
                // ����� ���������� � ��������
                int temp_K = K_array[i];
                K_array[i] = K_array[j];
                K_array[j] = temp_K;

                double temp_indicator = indicator_array[i];
                indicator_array[i] = indicator_array[j];
                indicator_array[j] = temp_indicator;
            }
        }
    }
    printf("������ ������������� �� �����������\n");
    return;
}