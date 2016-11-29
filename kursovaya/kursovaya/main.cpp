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

bool Prime(long long a) //�������� �� ������� �����
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

bool is_dots(const char* dir) // �������� �� ������� � ����� �����
{
	if (strcmp(dir, ".") == 0) return true;
	if (strcmp(dir, "..") == 0) return true;
	return false;
}

vector<string> scan_directory(const char *dir, const char *mask) // ������������ �������� � ������������
{
	vector<string> str;
	string *result = new string();

	char filemask[MAX_PATH];
	sprintf_s(filemask, "%s\\%s", dir, mask); // ������ ���� ������

	WIN32_FIND_DATA wf;

	int i = 0;
	
	HANDLE hf = FindFirstFile(filemask, &wf); // ����� ������� ����� � ������ ������
	if (hf != INVALID_HANDLE_VALUE) 
	{
		do
		{
			if (!is_dots(wf.cFileName))
			{
				if (wf.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					char buf[MAX_PATH];
					sprintf_s(buf, "%s\\%s", dir, wf.cFileName); // ���������� ���������� �����
					scan_directory(buf, mask); // ��������� ����������� �����
				}
				else
				{
					str.push_back(wf.cFileName); // ���������� � ������ ���� ������ ����� ���������� ���������� �����
				}
					
			}
		} while (FindNextFile(hf, &wf)); // ����� �������, ���� �� �������� ��� �����
		FindClose(hf);
	}
	return str; // ������� ��������������� �������
}

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");
	
	if (argc > 1) // ��������: ��� �� ������ ���� �� ���� �������� ��������� ������
	{
		ostringstream arg;
		arg << argv[1]; //������ ��������� ���������� ������ � ����������
		string command = arg.str();

		if (command == "generator") // �������� "���������" ���������� �������� ������
		{
			int min, max; 
			bool t = false;
			ofstream fout;			

	       // �������� ������� ���������
			cout << "������� ������ �������� �������� ������� m (����������� ����� �� 1 �� 62): ";
			do {
		cin.clear();
		cin.sync();				
		cin >> min;
		t = false;
		if ((min < 1) || (min > 62)) { cout << "������ �����. ��������� �������!" << endl;
					t = true;
				}
				} while (cin.fail() || t);

			// �������� �������� ���������
			cout << "������� ������� �������� �������� ������� m (����������� ����� �� " << min << "  �� 62): ";
			do {
		cin.clear();
		cin.sync();				
		cin >> max;
		t = false;
		if ((max < min) || (max > 62)) { cout << "������ �����. ��������� �������!" << endl;
					t = true;
				}
				} while (cin.fail() || t);


			for (int i = min; i <= max; i++) //�������� �������� ������
			{
				ostringstream t;
				t << i << ".txt"; // ��� ����� ����� ����� ��� ���� N.txt, N - ����� �����
				string filename = t.str();
				fout.open(filename); // �������� ����� ��� ������
				
				long long k = pow(2, i) - 1;
				fout << k; // ������ ���������������� ����� � ����
				fout.close(); // �������� �����
			}

			cout << "\n��������� ������ " << max - min + 1 << " ������.\n��������� ����������: \n���������� ������: " << max - min + 1 << "\n����������� ��������: " << min << "\n������������ ��������: " << max << endl;
		}		

		else if (command == "test") // ���� �������� "����", �� ��������� ��������� �������� ������
		{
			vector<string> list = scan_directory(".", "*.txt"); // ����� ������� ������������ ����� �� ������� ������ � ������ *.txt, ��� ������� ���������� ����� ������������ � ������ �����
			int l = list.size(); // ���������� ������

			auto check = [](int s, int n, vector<string> files) // ����� �������� �����
			{
				ofstream fout;
				fout.open("result.dat", std::ios::binary | std::ios::app); // �������� ����� � ������������ ��� ������

				for (int i = s; i < n; i++) // �������� ��������� ������ ������
				{
					ifstream fin;
					fin.open(files[i]); // �������� ���������� �����
					
					vector<long long> res; // �������������� �����

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
							fout << res[j] << " - ������� �����\n";
						else
							fout << res[j] << " - �� �������� ������� ������\n";
					}

					fout << "������ �� ����� " << files[i] << " ���������� � �������������" << endl;

					fin.close();
				}
				cout << "��������� ������ �� " << n - s << " ������..." << endl;
				fout << "��������� ������ �� " << n - s << " ������..." << endl;
				fout.close();
			};

			int block = atoi(argv[2]); // ����������� ���������� ������ � ����� ������ �� 2 ��������� ��������� ������

			int k = l / block; // ��������� ����� �������

			ofstream fout;
			fout.open("result.dat"); // ������� ����� � ������������
			fout.clear();
			fout.close();

			if (k <= 1 && (l - k * block) == 0) // ���� ������ ��������� ������, ��� ������ ����������
			{
				thread thread(check, 0, l, list); // ������������� ������
				thread.join(); // ������������� ������
			}
			else
			{
				
				for (int i = 0; i < k; i++) // ���������� ������� � ������ �������� ������ ������
				{					
					if (i != k)
						try
					{
						thread thread(check, i * block, i * block + block, list);  // ������������� ������
						thread.join(); // �������������� ������
					}
					catch (int e)
					{
						cout << e;
					}					
										
				}
				int ost = l - k * block; // ����������� ������� �� �������
				thread thread(check, k * block, k * block + ost, list); // ������������� ������, ��������������� ���������� �����
				thread.join(); // ������������� ������	
			}

			cout << "��������� " << l << " ������ ��������� �������!" << endl;
		}

		system("pause");
	}

	else
	{
		cout << "������! �� ������ ��������� ������ ���������!" << endl;
		system("pause");
		return 0;
	}

	return 0;
}