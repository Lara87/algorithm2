#include <cstdlib>
#include <vector>
#include <numeric>
#include <iterator>
#include <ostream>
#include <random>
#include <ctime>
#include <iostream>
#include <chrono>
#include <string>
#include <list>
#include <deque>
#include <forward_list>
#include <algorithm>
#include <tuple>
#include <functional>
using namespace std;

chrono::time_point<chrono::steady_clock> start;//steady_clock �  ���������� ���� - ������������ ���������
void beginProfile()
{
	start = chrono::steady_clock::now();
}

void endProfile(string message)
{
	/* auto: ��� ������������� ���������� ��� ����� �������������� ������ ���� ����������,
	����� �������� �����������, ��� �� ������ ��������� ��� ���������� ������ �� ����������������� ��������.
	��� ���������� ������� ���� (��� �������������� ������������ ����� ������ ������������).*/

	auto finish = chrono::steady_clock::now();
	auto t = chrono::duration_cast<chrono::milliseconds>(finish - start);
	cout << message << ' ' << t.count() << " msec" << endl;
}

int N = 10000;
default_random_engine gen1(unsigned(time(0)));//��������� ��������������� ����� - �� ������ ��������� ������������������, �������, �������� �����������

tuple<int, int> tupleGenerator()//���������� ���� � ���
{
	uniform_int_distribution<> height_d(150, 200);// ��������� ����� ��������, ���������� �������������� �� ��������� ��������� [a, b]
	uniform_int_distribution<> weight_d(50, 100);
	return tuple<int, int>(height_d(gen1), weight_d(gen1));
}

int main(int argc, char *argv[])
{
	setlocale(LC_ALL, "Russian");
	cout << "������������ ������ ��������� ���������� ���������� �.�. ������ ����� - 31" << endl;
	system("pause");


	vector<tuple<int, int>> container;//������� ������

	default_random_engine gen(unsigned(time(0)));//��������� ��������������� ����� - �� ������ ��������� ������������������, �������, �������� �����������

	uniform_int_distribution<> height_d(150, 200);// ��������� ����� ��������, ���������� �������������� �� ��������� ��������� [a, b] ����

	uniform_int_distribution<> weight_d(50, 100);// ��������� ����� ��������, ���������� �������������� �� ��������� ��������� [a, b] ���

	//container.reserve(N);


	cout << "�) ��������� ��������� ���������� ������� (������ emplace_back() � push_back())" << endl;
	beginProfile();
	for (int i = 0; i < N; i++)
		container.push_back(tuple<int, int>(height_d(gen), weight_d(gen)));//��������� ������� � ����� ����������:1)��� ����� 2)�����������
	endProfile("Push back");

	beginProfile();
	for (int i = 0; i < N; i++)
		container.emplace_back(height_d(gen), weight_d(gen));//������� ����� ������� � ����� ����������(��� ������� ����������� ��� �����������)
	endProfile("Emplace back");


	cout << "�) ��������� ��������� ���������� ������� (��������� generate(), generate_n())" << endl;
	beginProfile();
	auto genLambda = [&gen, &height_d, &weight_d]() {return tuple<int, int>(height_d(gen), weight_d(gen)); };
	generate(container.begin(), container.end(), tupleGenerator);//��������� ��������, ���������. ����� first/last, ������� ��������������� gen
	endProfile("Generate");

	beginProfile();
	generate_n(container.begin(), N, tupleGenerator);//��������� ����������������� ������� � first, n ��� ������� gen
	endProfile("Generate N");


	cout << "�) ����������� (�������� sort()). ����������� ������� ���������. ��� ������ ��������� ����� sort()" << endl;
	beginProfile();
	auto cmp = [](tuple<int, int>x, tuple<int, int>y) {
		int cx = get<0>(x) / get<1>(x);//����/���
		int cy = get<0>(y) / get<1>(y);//����/���
		return cx == cy ? get<0>(x) < get<0>(y) : cx < cy;
	};
	sort(container.begin(), container.end(), cmp);//��������� �������� � ��������� [first, last)�� ����������� 
	/*�������� ������������ � ������� �������� �������� ������� ���������;
	cmp - ������ ������� ���������, ������� ���������� true, ���� ������ �������� ������ (�. �. ���������� ��) �������*/
	endProfile("Sort");

	cout << "�) ������� ��������� (�������� unique()) � ��������� ������ (����� erase()). ��� ������ ��������� ����� unique()" << endl;
	beginProfile();
	auto cmpEq = [](tuple<int, int>x, tuple<int, int>y) {return get<0>(x) == get<0>(y) && get<1>(x) == get<1>(y); };
	auto nEnd = unique(container.begin(), container.end(), cmpEq);//������� ��� ��������������� ������������� �������� �� ���������
	container.erase(nEnd, container.end());//������� ��������� �������� �� ����������
	endProfile("Unique & Erase");

	cout << "�) �������� ����������� �������� (�������� shuffle())" << endl;
	beginProfile();
	shuffle(container.begin(), container.end(), gen);//�������� ������� ��������� � �������� ���������
	endProfile("Shuffle");

	cout << "�) ��������� ���������� ���������, ��������������� ��������� ������� (�������� count_if())" << endl;
	beginProfile();
	int cmpVal = 114;//�������� ��� ���������
	auto ch = [cmpVal](tuple<int, int>x) {return sqrt(get<0>(x)*get<1>(x)) == cmpVal; };//������ ���������� �� ������������ ���� � ���
	int count = count_if(container.begin(), container.end(), ch);//���������� ���-�� ��������� � ���������, ����������. ������������� �������
	endProfile("Count_if");

	cout << "�������� �������� (�������� replace_if()). ����������� �������� ��� ������ ��� ������-�������" << endl;
	beginProfile();
	tuple<long, long> sum1 = accumulate(container.begin(), container.end(), tuple<long, long>(0, 0), [](tuple<long, long>acc, tuple<int, int>x)//������� ����� �������� � ���� ��������� � ���������
	{
		return tuple<long, long>(get<0>(acc) + get<0>(x), get<1>(acc) + get<1>(x));
	});
	tuple<int, int>mean = make_tuple(get<0>(sum1) / container.size(), get<1>(sum1) / container.size());//������� ��������������
	replace_if(container.begin(), container.end(), ch, mean);//�������� ��� �������� � ���������, �������. ������������� �������(��� ������� ���������� true), �� new_value
	endProfile("Replace_if");

	system("pause");

	return 0;
}

/*transform(container.begin(), container.end(), container.begin(), [&ch, &mean](tuple<int, int>x)
	{
		if (ch(x))
			return tuple<int, int>(get<0>(mean) + get<0>(x), get<1>(mean) + get<1>(x));
		else
			return x;
	});
	container;
	endProfile("Replace_if");*/