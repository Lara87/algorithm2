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

chrono::time_point<chrono::steady_clock> start;//steady_clock –  монотонные часы - рекомендации учебников
void beginProfile()
{
	start = chrono::steady_clock::now();
}

void endProfile(string message)
{
	/* auto: при инициализации переменной оно может использоваться вместо типа переменной,
	чтобы сообщить компилятору, что он должен присвоить тип переменной исходя из инициализируемого значения.
	Это называется выводом типа (или автоматическим определением типов данных компилятором).*/

	auto finish = chrono::steady_clock::now();
	auto t = chrono::duration_cast<chrono::milliseconds>(finish - start);
	cout << message << ' ' << t.count() << " msec" << endl;
}

int N = 10000;
default_random_engine gen1(unsigned(time(0)));//генератор псевдослучайных чисел - на основе сочетания производительности, размера, качества случайности

tuple<int, int> tupleGenerator()//генерируем рост и вес
{
	uniform_int_distribution<> height_d(150, 200);// случайные целые значения, равномерно распределенные на замкнутом интервале [a, b]
	uniform_int_distribution<> weight_d(50, 100);
	return tuple<int, int>(height_d(gen1), weight_d(gen1));
}

int main(int argc, char *argv[])
{
	setlocale(LC_ALL, "Russian");
	cout << "Лабораторная работа выполнена студенткой Черниковой Л.В. группа ЗИНРБ - 31" << endl;
	system("pause");


	vector<tuple<int, int>> container;//создали кортеж

	default_random_engine gen(unsigned(time(0)));//генератор псевдослучайных чисел - на основе сочетания производительности, размера, качества случайности

	uniform_int_distribution<> height_d(150, 200);// случайные целые значения, равномерно распределенные на замкнутом интервале [a, b] рост

	uniform_int_distribution<> weight_d(50, 100);// случайные целые значения, равномерно распределенные на замкнутом интервале [a, b] вес

	//container.reserve(N);


	cout << "а) заполнить контейнер случайными данными (методы emplace_back() и push_back())" << endl;
	beginProfile();
	for (int i = 0; i < N; i++)
		container.push_back(tuple<int, int>(height_d(gen), weight_d(gen)));//добавляет элемент в конец контейнера:1)как копия 2)перемещение
	endProfile("Push back");

	beginProfile();
	for (int i = 0; i < N; i++)
		container.emplace_back(height_d(gen), weight_d(gen));//создает новый элемент в конце контейнера(без лишнего копирования или перемещения)
	endProfile("Emplace back");


	cout << "б) заполнить контейнер случайными данными (алгоритмы generate(), generate_n())" << endl;
	beginProfile();
	auto genLambda = [&gen, &height_d, &weight_d]() {return tuple<int, int>(height_d(gen), weight_d(gen)); };
	generate(container.begin(), container.end(), tupleGenerator);//заполняет диапазон, ограничен. парой first/last, вызывая последовательно gen
	endProfile("Generate");

	beginProfile();
	generate_n(container.begin(), N, tupleGenerator);//заполняет последоватльность начиная с first, n раз вызывая gen
	endProfile("Generate N");


	cout << "в) сортировать (алгоритм sort()). Реализовать функтор сравнения. Для списка применить метод sort()" << endl;
	beginProfile();
	auto cmp = [](tuple<int, int>x, tuple<int, int>y) {
		int cx = get<0>(x) / get<1>(x);//рост/вес
		int cy = get<0>(y) / get<1>(y);//рост/вес
		return cx == cy ? get<0>(x) < get<0>(y) : cx < cy;
	};
	sort(container.begin(), container.end(), cmp);//сортирует элементы в диапазоне [first, last)по возрастанию 
	/*элементы сравниваются с помощью заданной двоичной функции сравнения;
	cmp - объект функции сравнения, который возвращает true, если первый аргумент меньше (т. е. упорядочен до) второго*/
	endProfile("Sort");

	cout << "г) удалить дубликаты (алгоритм unique()) и уменьшить размер (метод erase()). Для списка применить метод unique()" << endl;
	beginProfile();
	auto cmpEq = [](tuple<int, int>x, tuple<int, int>y) {return get<0>(x) == get<0>(y) && get<1>(x) == get<1>(y); };
	auto nEnd = unique(container.begin(), container.end(), cmpEq);//удаляет все последовательно повторяющиеся элементы из диапазона
	container.erase(nEnd, container.end());//удаляет указанные элементы из контейнера
	endProfile("Unique & Erase");

	cout << "д) случайно переставить элементы (алгоритм shuffle())" << endl;
	beginProfile();
	shuffle(container.begin(), container.end(), gen);//изменяет порядок элементов в заданном диапазоне
	endProfile("Shuffle");

	cout << "е) посчитать количество элементов, удовлетворяющих заданному условию (алгоритм count_if())" << endl;
	beginProfile();
	int cmpVal = 114;//значение для сравнения
	auto ch = [cmpVal](tuple<int, int>x) {return sqrt(get<0>(x)*get<1>(x)) == cmpVal; };//корень квадратный из произведения рост и вес
	int count = count_if(container.begin(), container.end(), ch);//возвращает кол-во элементов в диапазоне, удовлетвор. определенному условию
	endProfile("Count_if");

	cout << "заменить элементы (алгоритм replace_if()). Реализовать критерий для замены как лямбда-функцию" << endl;
	beginProfile();
	tuple<long, long> sum1 = accumulate(container.begin(), container.end(), tuple<long, long>(0, 0), [](tuple<long, long>acc, tuple<int, int>x)//считает сумму значений и всех элементов в диапазоне
	{
		return tuple<long, long>(get<0>(acc) + get<0>(x), get<1>(acc) + get<1>(x));
	});
	tuple<int, int>mean = make_tuple(get<0>(sum1) / container.size(), get<1>(sum1) / container.size());//среднее арифметическое
	replace_if(container.begin(), container.end(), ch, mean);//заменяет все элементы в диапазоне, удовлет. определенному условию(для которых возвращает true), на new_value
	endProfile("Replace_if");

	system("pause");

	return 0;
}

transform(container.begin(), container.end(), container.begin(), [&ch, &mean](tuple<int, int>x)
	{
		if (ch(x))
			return tuple<int, int>(get<0>(mean) + get<0>(x), get<1>(mean) + get<1>(x));
		else
			return x;
	});
	container;
	endProfile("Replace_if");
