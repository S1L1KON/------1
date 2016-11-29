#include <iostream>
#include <thread>
#include <string>
#include <fstream>
#include <sstream>
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <vector>
#include <cstdlib>
#include <algorithm>

using namespace std;
#define MAX_PATH 512

bool Prime(long long a) //проверка на простое число
{
   unsigned long i, j, bound;
   if (a == 0 || a == 1)
      return 0;
   if (a == 2 || a == 3 || a == 5)
      return 1;
   if (a%2 == 0 || a%3 == 0 || a%5 == 0)
      return 0;
   bound = sqrt((double)a);
   i = 7; j = 11;
   while (j <= bound && a%i && a%j)
   {
       i += 6; j += 6;
   }
   if (j <= bound || i <= bound && a%i == 0)
      return 0;
   return 1;
}

bool is_dots(const char* dir) // Проверка на наличие в имени точек
{
	if (strcmp(dir, ".") == 0) return true;
	if (strcmp(dir, "..") == 0) return true;
	return false;
}

vector<string> scan_directory(const char *dir, const char *mask) // Сканирование каталога и подкаталогов
{
	vector<string> str;
	string *result = new string();

	char filemask[MAX_PATH];
	sprintf_s(filemask, "%s\\%s", dir, mask); // Полный путь поиска

	WIN32_FIND_DATA wf;

	int i = 0;
	
	HANDLE hf = FindFirstFile(filemask, &wf); // Поиск первого файла с данной маской
	if (hf != INVALID_HANDLE_VALUE) 
	{
		do
		{
			if (!is_dots(wf.cFileName))
			{
				if (wf.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					char buf[MAX_PATH];
					sprintf_s(buf, "%s\\%s", dir, wf.cFileName); // Добавление найденного файла
					scan_directory(buf, mask); // Очередной рекурсивный вызов
				}
				else
				{
					str.push_back(wf.cFileName); // Добавление в вектор имен файлов имени очередного найденного файла
				}
					
			}
		} while (FindNextFile(hf, &wf)); // Поиск ведется, пока не найдутся все файлы
		FindClose(hf);
	}
	return str; // Возврат результирующего вектора
}

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");
	
	if (argc > 1) // Проверка: был ли введен хотя бы один аргумент командной строки
	{
		ostringstream arg;
		arg << argv[1]; //Запись аргумента коммандной строки в переменную
		string command = arg.str();

		if (command == "generator") // Параметр "генератор" генерирует исходные данные
		{
			int min, max; 
			bool t = false;
			ofstream fout;			

	       // проверка нижнего диапозона
			cout << "Введите нижний диапазон значений степени m (натуральное числа от 1 до 62): ";
			do {
		cin.clear();
		cin.sync();				
		cin >> min;
		t = false;
		if ((min < 1) || (min > 62)) { cout << "Ошибка ввода. Повторите попытку!" << endl;
					t = true;
				}
				} while (cin.fail() || t);

			// проверка верхнего диапозона
			cout << "Введите верхний диапазон значений степени m (натуральное числа от " << min << "  до 62): ";
			do {
		cin.clear();
		cin.sync();				
		cin >> max;
		t = false;
		if ((max < min) || (max > 62)) { cout << "Ошибка ввода. Повторите попытку!" << endl;
					t = true;
				}
				} while (cin.fail() || t);


			for (int i = min; i <= max; i++) //Создание исходных файлов
			{
				ostringstream t;
				t << i << ".txt"; // Имя файла будет иметь имя вида N.txt, N - номер файла
				string filename = t.str();
				fout.open(filename); // Открытие файла для записи
				
				long long k = pow(2, i) - 1;
				fout << k; // Запись сгенерированного числа в файл
				fout.close(); // Закрытие файла
			}

			cout << "\nГенератор создал " << max - min + 1 << " файлов.\nПараметры генератора: \nКоличество файлов: " << max - min + 1 << "\nМинимальное значение: " << min << "\nМаксимальное значение: " << max << endl;
		}		

		else if (command == "test") // Если параметр "тест", то программа проверяет исходные данные
		{
			vector<string> list = scan_directory(".", "*.txt"); // Вызов функции сканирования папки на наличие файлов с маской *.txt, имя каждого найденного файла записывается в вектор строк
			int l = list.size(); // Количество файлов

			auto check = [](int s, int n, vector<string> files) // Метод проверки числа
			{
				ofstream fout;
				fout.open("result.dat", std::ios::binary | std::ios::app); // Открытие файла с результатами для записи

				for (int i = s; i < n; i++) // Проверка очередной группы файлов
				{
					ifstream fin;
					fin.open(files[i]); // Открытие очередного файла
					
					vector<long long> res; // Обрабатываемые числа

					while (!fin.eof())
					{
						long long t;
						fin >> t;
						res.push_back(t);
					}

					sort(res.begin(), res.end());

					for (int j = 0; j < res.size(); j++)
					{
						if (Prime(res[j]))
							fout << res[j] << " - Простое число\n";
						else
							fout << res[j] << " - не является простым числом\n";
					}

					fout << "Данные из файла " << files[i] << " обработаны и отсортированы" << endl;

					fin.close();
				}
				cout << "Проверена группа из " << n - s << " файлов..." << endl;
				fout << "Проверена группа из " << n - s << " файлов..." << endl;
				fout.close();
			};

			int block = atoi(argv[2]); // Определение количества файлов в одном потоке из 2 параметра командной строки

			int k = l / block; // Получение числа потоков

			ofstream fout;
			fout.open("result.dat"); // Очистка файла с результатами
			fout.clear();
			fout.close();

			if (k <= 1 && (l - k * block) == 0) // Если файлов оказалось меньше, чем задано параметром
			{
				thread thread(check, 0, l, list); // Инициализация потока
				thread.join(); // Присоединение потока
			}
			else
			{
				
				for (int i = 0; i < k; i++) // Количество потоков с полным заданным числом файлов
				{					
					if (i != k)
						try
					{
						thread thread(check, i * block, i * block + block, list);  // Инициализация потока
						thread.join(); // Присоедениение потока
					}
					catch (int e)
					{
						cout << e;
					}					
										
				}
				int ost = l - k * block; // Определение остатка от деления
				thread thread(check, k * block, k * block + ost, list); // Инициализация потока, обрабатывающего оставшиеся файлы
				thread.join(); // Присоединение потока	
			}

			cout << "Обработка " << l << " файлов завершена успешно!" << endl;
		}

		system("pause");
	}

	else
	{
		cout << "Ошибка! Не заданы параметры работы программы!" << endl;
		system("pause");
		return 0;
	}

	return 0;
}