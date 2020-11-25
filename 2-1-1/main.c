#include <iostream>
#include <vector>
#include <algorithm>
#include <stack>
#include <fstream>
#include <cmath>
#include <set>
#include <iomanip>

const long double EPS = 0.00000000000001;

struct point
{
	long double x;
	long double  y;
	long double  z;
	point() {}
	point(long double x, long double y, long double z) : x(x), y(y), z(z) {}
	point& operator*=(const long double& d);
};

point& point::operator*=(const long double& d)
{
	this->x *= d;
	this->y *= d;
	this->z *= d;
	return *this;

}

bool operator==(point p1, point p2)
{
	return p1.x == p2.x && p1.y == p2.y;
}

struct Vector
{
	long double x;
	long double y;
	long double z;
	Vector() {}
	Vector(point A, point B) {
		this->x = B.x - A.x;
		this->y = B.y - A.y;
		this->z = B.z - A.z;
	}
	Vector(long double x, long double y, long double z) : x(x), y(y), z(z) {}
	long double len() {
		return sqrt(x * x + y * y + z * z);
	}
	Vector operator*(long double d);
	Vector normalize()
	{
		Vector new_vec;
		new_vec.x = x / len();
		new_vec.y = y / len();
		new_vec.z = z / len();
		return new_vec;
	}
};

Vector Vector::operator*(long double d)
{
	return Vector(x * d, y * d, z * d);
}

long double dot_product(Vector A, Vector B) {
	return A.x * B.x + A.y * B.y + A.z * B.z;
}

long double cross_product(Vector PA, Vector PB) {
	long double x = PA.y * PB.z - PA.z * PB.y;
	long double y = PA.z * PB.x - PA.x * PB.z;
	long double z = PA.x * PB.y - PA.y * PB.x;
	return Vector(x, y, z).len();
}

point operator+(const point& A, const Vector& B)
{
	return point(A.x + B.x, A.y + B.y, A.z + B.z);
}

struct section
{
	point A;
	point B;
	section() {}
	section(point A, point B) : A(A), B(B) {}
	long double len() {
		Vector V(A, B);
		return V.len();
	}
};

std::istream& operator>>(std::istream& i, section& sec)
{
	i >> sec.A >> sec.B;
	return i;
}

std::ostream& operator<<(std::ostream& os, section& sec)
{
	os << sec.A << ' ' << sec.B;
	return os;
}

long double path_from_point_to_section(point& P, section& sec)
{
	Vector PA(P, sec.A), PB(P, sec.B);
	long double high = cross_product(PA, PB) / sec.len();
	long double check1 = dot_product(Vector(sec.A, sec.B), Vector(sec.A, P));
	long double check2 = dot_product(Vector(sec.B, sec.A), Vector(sec.B, P));
	if (check1 >= 0 && check2 >= 0)
		return high;
	else
		return std::min(PA.len(), PB.len());
}

long double path(section AB, section CD)
{
	if (AB.len() < EPS && CD.len() < EPS)
		return section(AB.A, CD.A).len();
	if (AB.len() < EPS)
		return path_from_point_to_section(AB.A, CD);
	if (CD.len() < EPS)
		return path_from_point_to_section(CD.A, AB);
	point from = AB.A;
	point to = AB.B;
	Vector direction(from, to);
	direction = direction.normalize();
	Vector current_section(from, to);
	long double accuracy = current_section.len();
	long double cef = 1.0 / 3.0;
	while (accuracy > EPS)
	{
		point middle1 = from + direction * (accuracy * cef);
		point middle2 = from + direction * (accuracy * cef * 2);
		long double path1 = path_from_point_to_section(middle1, CD);
		long double path2 = path_from_point_to_section(middle2, CD);
		if (path2 > path1)
			to = middle2;
		else
			from = middle1;
		current_section = Vector(from, to);
		accuracy = current_section.len();
	}
	return path_from_point_to_section(from, CD);
}

int main()
{
	std::ifstream fin("C:\\Users\\ivras\\Documents\\ФизтехУчеба\\AKOS\\input.txt");
	section AB, CD;
	fin >> AB >> CD;
	std::cout << std::fixed << std::setprecision(20) << path(AB, CD);
	return 0;
}
