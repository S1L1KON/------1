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

using namespace std;
#define MAX_PATH 512

bool mersenn(int a)
{
	int t = a + 1; // Прибавляем 1 к исходному числу
	if (!(t & (t - 1))) // Является ли число степенью 2?
		return true;
	else
		return false; // Не является
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
	sprintf_s(filemask, "%s\\%s", dir, mask);

	WIN32_FIND_DATA wf;

	int i = 0;

	HANDLE hf = FindFirstFile(filemask, &wf);
	if (hf != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (!is_dots(wf.cFileName))
			{
				if (wf.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					char buf[MAX_PATH];
					sprintf_s(buf, "%s\\%s", dir, wf.cFileName);
					scan_directory(buf, mask);
				}
				else
				{
					str.push_back(wf.cFileName);
				}
					
			}
		} while (FindNextFile(hf, &wf));
		FindClose(hf);
	}
	return str;
}

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");
	
	if (argc > 1) // Проверка: был ли введен хотя бы один аргумент командной строки
	{
		ostringstream arg;
		arg << argv[1];
		string command = arg.str();

		if (command == "generator") // Параметр "генератор" генерирует исходные данные
		{
			long long min, max;
			int n;
			ofstream fout;			

			cout << "Введите диапазон значений чисел (два натуральных числа): ";
			cin >> min >> max;
			cout << "\nВведите количество сгенерированных файлов: ";
			cin >> n;

			srand(time(0));

			for (int i = 0; i < n; i++) //Создание исходных файлов
			{
				ostringstream t;
				t << i << ".txt";
				string filename = t.str();
				fout.open(filename);
				
				long long k = rand() % (max - min + 1) - min;
				fout << k;
				fout.close();
			}

			cout << "\nГенератор создал " << n << " файлов.\nПараметры генератора: \nКоличество файлов: " << n << "\nМинимальное значение: " << min << "\nМаксимальное значение: " << max << endl;
		}		

		else if (command == "test")
		{
			vector<string> list = scan_directory(".", "*.txt");
			int l = list.size();

			auto check = [](int s, int n, vector<string> files)
			{
				ofstream fout;
				fout.open("result.dat", std::ios::binary | std::ios::app);
				for (int i = s; i < n; i++) // Проверка очередной группы файлов
				{
					ifstream fin;
					fin.open(files[i]);

					while (!fin.eof()) // Проверка всех чисел
					{
						long long t;
						fin >> t;
						if (mersenn(t))
							fout << t << " - число Мерсенна\n";
						else
							fout << t << " - не является числом Мерсенна\n";
					}
					fin.close();
				}
				cout << "Проверена группа из " << n - s << " файлов..." << endl;
				fout << "Проверена группа из " << n - s << " файлов..." << endl;
				fout.close();
			};

			int block = atoi(argv[2]);

			int k = l / block;

			ofstream fout;
			fout.open("result.dat");
			fout.clear();
			fout.close();

			if (k <= 1 && (l - k * block) == 0)
			{
				thread thread(check, 0, l, list);
				thread.join();
			}
			else
			{
				
				for (int i = 0; i < k; i++)
				{					
					if (i != k)
						try
					{
						thread thread(check, i * block, i * block + block, list);
						thread.join();
					}
					catch (int e)
					{
						cout << e;
					}					
										
				}
				int ost = l - k * block;
				thread thread(check, k * block, k * block + ost, list);
				thread.join();				
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