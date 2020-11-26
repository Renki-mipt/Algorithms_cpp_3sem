#include <iostream>
#include <vector>
#include <algorithm>
#include <stack>
#include <fstream>
#include <cmath>

const long double INF = 1e9 + 3;

struct Point
{
	long double x;
	long double  y;
	Point() {}
	Point(long double x, long double y) : x(x), y(y) {}
	Point& operator*=(const long double& d);
};

bool operator==(Point& A, Point& B)
{
	return A.x == B.x && A.y == B.y;
}

bool operator!=(Point& A, Point& B)
{
	return !(A == B);
}

Point& Point::operator*=(const long double& d)
{
	this->x *= d;
	this->y *= d;
	return *this;

}


std::istream& operator>>(std::istream& i, Point& A)
{
	i >> A.x >> A.y;
	return i;
}

std::ostream& operator<<(std::ostream& out, const Point& A)
{
	out << A.x << ' ' << A.y;
	return out;
}

struct Vector
{
	Point A;
	Point B;
	long double x;
	long double y;
	Vector() {}
	Vector(long double x, long double y) : x(x), y(y) {}
	Vector(Point A, Point B) : A(A), B(B) {
		this->x = B.x - A.x;
		this->y = B.y - A.y;
	}
	long double len()
	{
		return sqrt(x * x + y * y);
	}
};

long double dot_product(Vector A, Vector B) {
	return A.x * B.x + A.y * B.y;
}

long double cross_product(Vector A, Vector B) {
	return A.x * B.y - A.y * B.x;
}

Point operator+(const Point& A, const Vector& B)
{
	return Point(A.x + B.x, A.y + B.y);
}

Vector operator+(const Vector& A, const Vector& B)
{
	return Vector(A.x + B.x, A.y + B.y);
}

int MostDown(std::vector<Point>& polygon)
{
	int number = 0;
	long double min = INF;
	for (int i = 0; i < polygon.size(); ++i)
		if (polygon[i].y < min || (polygon[i].y == min && polygon[i].x > polygon[number].x))
		{
			number = i;
			min = polygon[i].y;
		}
	return number;
}

std::vector<Vector> VectorSort(int i, int j, std::vector<Point>& polygon1, 
	std::vector<Point>& polygon2)
{
	std::vector<Vector> vec_sort;
	Vector horizontal(Point(0, 0), Point(-1, 0));
	int i_plus = 0;
	int j_plus = 0;
	while (i_plus != polygon1.size() || j_plus != polygon2.size())
	{
		bool turn;
		if (i_plus == polygon1.size())
			turn = 1;
		else
			if (j_plus == polygon2.size())
				turn = 0;
			else
			{
				Vector V_first(polygon1[(i - 1 + polygon1.size()) % polygon1.size()], polygon1[i]);
				Vector V_second(polygon2[(j - 1 + polygon2.size()) % polygon2.size()], polygon2[j]);
				if (cross_product(V_first, horizontal) >= 0)
					if (cross_product(V_second, horizontal) >= 0)
						turn = (dot_product(V_first, horizontal) * V_second.len() <
							dot_product(V_second, horizontal) * V_first.len());
					else
						turn = 0;
				else
					if (cross_product(V_second, horizontal) >= 0)
						turn = 1;
					else
						turn = (dot_product(V_first, horizontal) * V_second.len() >
							dot_product(V_second, horizontal) * V_first.len());
			}
		if (!turn)
		{
			vec_sort.push_back(Vector(polygon1[(i - 1 + polygon1.size()) % polygon1.size()],
				polygon1[i]));
			i_plus += 1;
			i = (i + 1) % polygon1.size();
		}
		else
		{
			vec_sort.push_back(Vector(polygon2[(j - 1 + polygon2.size()) % polygon2.size()],
				polygon2[j]));
			j_plus += 1;
			j = (j + 1) % polygon2.size();
		}
	}
	return vec_sort;
}

std::vector<Point> MinkovskiySum(std::vector<Point>& polygon1, std::vector<Point>& polygon2)
{
	int most_down_in_first_number = MostDown(polygon1);
	int most_down_in_second_number = MostDown(polygon2);
	int i = (most_down_in_first_number + 1) % polygon1.size();
	int j = (most_down_in_second_number + 1) % polygon2.size();
	std::vector<Vector> vec_sort = VectorSort(i, j, polygon1, polygon2);
	std::vector<Point> new_polygon;
	Point new_polygon_Point(polygon1[most_down_in_first_number].x + polygon2[most_down_in_second_number].x,
		polygon1[most_down_in_first_number].y + polygon2[most_down_in_second_number].y);
	for (int i = 0; i < vec_sort.size(); ++i)
	{
		new_polygon_Point = new_polygon_Point + vec_sort[i];
		new_polygon.push_back(new_polygon_Point);
	}
	return new_polygon;
}

bool IsPointInPolygon(std::vector<Point>& polygon, Point centre)
{
	int minus_turn = 0;
	int plus_turn = 0;
	for (int i = 0; i < polygon.size(); ++i)
	{
		Vector Vec1(centre, polygon[i]);
		Vector Vec2(centre, polygon[(i + 1) % polygon.size()]);
		if (cross_product(Vec1, Vec2) > 0)
			++plus_turn;
		if (cross_product(Vec1, Vec2) < 0)
			++minus_turn;
	}
	return (plus_turn == 0 or minus_turn == 0);
}

int main()
{
	int n, m;
	Point p;
	std::vector<Point> polygon1;
	std::vector<Point> polygon2;
	std::ifstream fin("C:\\Users\\ivras\\Documents\\ФизтехУчеба\\AKOS\\input.txt");
	fin >> n;
	for (int i = 0; i < n; ++i)
	{
		fin >> p;
		p *= (-1);
		polygon1.push_back(p);
	}
	fin >> m;
	for (int i = 0; i < m; ++i)
	{
		fin >> p;
		polygon2.push_back(p);
	}
	std::vector<Point> new_polygon = MinkovskiySum(polygon1, polygon2);
	Point centre(0, 0);
	if (IsPointInPolygon(new_polygon, centre))
		std::wcout << "YES";
	else
		std::wcout << "NO";
	return 0;
}
