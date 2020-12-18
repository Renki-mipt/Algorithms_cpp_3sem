#include <iostream>
#include <vector>
#include <algorithm>
#include <stack>
#include <fstream>
#include <cmath>
#include <set>
#include <map>

const long double INF = 1e9 + 3;
const long double eps = 0.0000000001;

struct Point
{
	long double x_;
	long double y_;
	long double z_;
	Point() {}
	Point(long double x, long double y, long double z) : x_(x), y_(y), z_(z) {}
	Point& operator*=(const long double& d);
};

bool operator==(Point& A, Point& B)
{
	return A.x_ == B.x_ && A.y_ == B.y_ && A.z_ == B.z_;
}

bool operator!=(Point& A, Point& B)
{
	return !(A == B);
}

Point& Point::operator*=(const long double& d)
{
	x_ *= d;
	y_ *= d;
	z_ *= d;
	return *this;

}

std::istream& operator>>(std::istream& i, Point& A)
{
	i >> A.x_ >> A.y_ >> A.z_;
	return i;
}

std::ostream& operator<<(std::ostream& out, const Point& A)
{
	out << A.x_ << ' ' << A.y_ << ' ' << A.z_;
	return out;
}

struct Segment
{
	Point first_end_;
	Point second_end_;
	Segment() {}
	Segment(Point A, Point B) : first_end_(A), second_end_(B) {}
	long double len_() const;
};

struct Vector
{
	long double x_;
	long double y_;
	long double z_;
	Vector() {}
	Vector(Point A, Point B) {
		x_ = B.x_ - A.x_;
		y_ = B.y_ - A.y_;
		z_ = B.z_ - A.z_;
	}
	Vector(Segment sec)
	{
		Point begin_ = sec.first_end_;
		Point end_ = sec.second_end_;
		x_ = end_.x_ - begin_.x_;
		y_ = end_.y_ - begin_.y_;
		z_ = end_.z_ - begin_.z_;
	}
	Vector(long double x, long double y, long double z) : x_(x), y_(y), z_(z) {}
	long double len_() {
		return sqrt(x_ * x_ + y_ * y_ + z_ * z_);
	}
	Vector operator*(long double d) const;
	Vector operator+(const Vector& other) const;
	Vector operator-(const Vector& other) const;
	Vector normalize()
	{
		Vector new_vec;
		new_vec.x_ = x_ / len_();
		new_vec.y_ = y_ / len_();
		new_vec.z_ = z_ / len_();
		return new_vec;
	}
};

Vector Vector::operator*(long double d) const
{
	Vector V = *this;
	V.x_ *= d;
	V.y_ *= d;
	V.z_ *= d;
	return V;
}

Vector Vector::operator+(const Vector& other) const
{
	long double x_res = x_ + other.x_;
	long double y_res = y_ + other.y_;
	long double z_res = z_ + other.z_;
	return Vector(x_res, y_res, z_res);
}

Vector Vector::operator-(const Vector& other) const
{
	return *this + (other * -1.);
}

long double dot_product(Vector A, Vector B) {
	return A.x_ * B.x_ + A.y_ * B.y_ + A.z_ * B.z_;
}

Vector cross_product_(Vector PA, Vector PB) {
	long double x = PA.y_ * PB.z_ - PA.z_ * PB.y_;
	long double y = PA.z_ * PB.x_ - PA.x_ * PB.z_;
	long double z = PA.x_ * PB.y_ - PA.y_ * PB.x_;
	return Vector(x, y, z);
}

std::ostream& operator<<(std::ostream& os, Vector& V)
{
	os << V.x_ << ' ' << V.y_ << ' ' << V.z_;
	return os;
}

Point operator+(const Point& A, const Vector& B)
{
	return Point(A.x_ + B.x_, A.y_ + B.y_, A.z_ + B.z_);
}

long double Segment::len_() const {
	Vector V(first_end_, second_end_);
	return V.len_();
}

std::istream& operator>>(std::istream& i, Segment& sec)
{
	i >> sec.first_end_ >> sec.second_end_;
	return i;
}

std::ostream& operator<<(std::ostream& os, Segment& sec)
{
	os << sec.first_end_ << ' ' << sec.second_end_;
	return os;
}

struct Plane
{
	Point first_Point;
	Point second_Point;
	Point third_Point;
	Plane() {}
	Plane(Point A, Point B, Point C) : first_Point(A), second_Point(B), third_Point(C) {}
};

long double cos_angle_Vectors(Vector AB, Vector CD)
{
	return dot_product(AB, CD) / (AB.len_() * CD.len_());
}

Vector projection(Vector V1, Vector V2)
{
	return V2 * dot_product(V2, V1) * (1.0 / dot_product(V2, V2));
}

Vector ProjectionOnPlane(Vector V, Plane Pl)
{
	Vector V1(Pl.first_Point, Pl.second_Point);
	Vector V2(Pl.first_Point, Pl.third_Point);
	return projection(V, V2 - projection(V2, V1)) + projection(V, V1);
}

long double AnlgePlanes(Point K, Segment LM_sec, Point N)
{
	Vector LM(LM_sec);
	Point L = LM_sec.first_end_;
	Vector LK(LM_sec.first_end_, K), MN(LM_sec.second_end_, N);
	Vector N1 = LK - projection(LK, LM);
	Vector N2 = cross_product_(N1, LM);
	Plane normal_Plane(L, L + N1, L + N2);
	Vector LK_projection_on_normal_Plane = ProjectionOnPlane(LK, normal_Plane);
	Vector MN_projection_on_normal_Plane = ProjectionOnPlane(MN, normal_Plane);
	return cos_angle_Vectors(LK_projection_on_normal_Plane, MN_projection_on_normal_Plane);
}

int NextFace(std::vector<Point>& points, Segment edge, Point p_for_edge)
{
	long double min_cosinus = 2;
	int index = 0;
	for (int i = 0; i < points.size(); ++i)
	{
		if (points[i] == edge.first_end_ || points[i] == edge.second_end_)
		{
			continue;
		}
		long double cosinus = AnlgePlanes(p_for_edge, edge, points[i]);
		if (cosinus < min_cosinus)
		{
			index = i;
			min_cosinus = cosinus;
		}
	}
	return index;
}

struct Indices
{
	int a;
	int b;
	int c;
	Indices(int a, int b, int c) : a(a), b(b), c(c) {}
};

void FindFirstThreePoints(int& A_index, int& B_index, int& C_index, int& D_index,
	Point& A, Point& B, Point& C, Point& D, std::vector<Point>& points)
{
	A = points[A_index];
	Point P1;
	if (B_index == -1)
	{
		Vector V1(A, Point(A.x_ + 1, A.y_, A.z_));
		Vector V2(A, Point(A.x_, A.y_ + 1, A.z_));
		P1 = A + V1;
		Point P2 = A + V2;
		Segment edge(A, P1);
		B_index = NextFace(points, edge, P2);
	}
	else
	{
		P1 = A + Vector(1, 0, 0);
		if (cross_product_(Vector(P1, A), Vector(points[B_index], A)).len_() < eps)
			P1 = P1 + Vector(0, 1, 0);
	}
	B = points[B_index];
	if (C_index == -1)
	{
		Segment edge(A, B);
		C_index = NextFace(points, edge, P1);
	}
	C = points[C_index];
	if (B_index < A_index)
	{
		std::swap(A, B);
		std::swap(A_index, B_index);
	}
	if (C_index < A_index)
	{
		std::swap(A, C);
		std::swap(A_index, C_index);
	}
	for (int i = 0; i < points.size(); ++i)
		if (i != A_index && i != B_index && i != C_index)
		{
			D_index = i;
			break;
		}
	D = points[D_index];
}

void Horizontal(int& A_index, int& B_index, int& C_index,
	std::vector<Point>& points)
{
	long double x_min = INF, y_min = INF, z_min = INF;
	for (int i = 0; i < points.size(); ++i)
		if (z_min > points[i].z_ || (z_min == points[i].z_ && y_min > points[i].y_)
			|| (z_min == points[i].z_ && y_min == points[i].y_ && x_min > points[i].x_))
		{
			A_index = i;
			x_min = points[i].x_;
			y_min = points[i].y_;
			z_min = points[i].z_;
		}
	for (int i = 0; i < points.size(); ++i)
		if (points[A_index].z_ == points[i].z_ && i != A_index)
		{
			B_index = i;
			break;
		}
	for (int i = 0; i < points.size(); ++i)
		if (points[A_index].z_ == points[i].z_ && i != A_index && i != B_index)
		{
			C_index = i;
			break;
		}

}

void FirstPlane(std::vector<Point>& points, std::vector<Indices>& faces, 
	std::map<std::pair<int, int>, int>& map_for_edges)
{
	int A_index = -1;
	int B_index = -1;
	int C_index = -1;
	int D_index = -1;
	Horizontal(A_index, B_index, C_index, points);
	Point A, B, C, D;
	FindFirstThreePoints(A_index, B_index, C_index, D_index, A, B, C, D, points);
	
	Vector just_norm(A, D);
	Vector V1(A, B);
	Vector V2(B, C);
	Vector V12 = V1 - projection(V1, V2);
	Vector norm = just_norm - projection(just_norm, V2) - projection(just_norm, V12);
	if (norm.x_ * (V1.y_ * V2.z_ - V1.z_ * V2.y_) - V1.x_ * (norm.y_ * V2.z_ - norm.z_ * V2.y_) +
		V2.x_ * (norm.y_ * V1.z_ - norm.z_ * V1.y_) > 0)
	{
		std::swap(B, C);
		std::swap(B_index, C_index);
	}
	map_for_edges[std::make_pair(A_index, B_index)] = C_index;
	map_for_edges[std::make_pair(B_index, C_index)] = A_index;
	map_for_edges[std::make_pair(C_index, A_index)] = B_index;
	faces.push_back(Indices(A_index, B_index, C_index));
}


std::vector<Indices> Hull(std::vector<Point>& points)
{
	std::vector<Indices> faces;
	std::map<std::pair<int, int>, int> map_for_edges;
	FirstPlane(points, faces, map_for_edges);
	while (!map_for_edges.empty())
	{
		std::pair<int, int> p = map_for_edges.begin()->first;
		Point C = points[map_for_edges.begin()->second];
		map_for_edges.erase(map_for_edges.begin());	
		Segment AB(points[p.first], points[p.second]);
		int index = NextFace(points, AB, C);
		if (map_for_edges.find(std::make_pair(p.first, index)) == map_for_edges.end() && 
			map_for_edges.find(std::make_pair(index, p.first)) == map_for_edges.end())
				map_for_edges[std::make_pair(p.first, index)] = p.second;
		else
		{
			map_for_edges.erase(std::make_pair(p.first, index));
			map_for_edges.erase(std::make_pair(index, p.first));
		}
		if (map_for_edges.find(std::make_pair(p.second, index)) == map_for_edges.end() &&
			map_for_edges.find(std::make_pair(index, p.second)) == map_for_edges.end())
			map_for_edges[std::make_pair(index, p.second)] = p.first;
		else
		{
			map_for_edges.erase(std::make_pair(p.second, index));
			map_for_edges.erase(std::make_pair(index, p.second));
		}
		if (p.first < p.second && p.first < index)
			faces.push_back(Indices(p.first, index, p.second));
		if (p.second < p.first && p.second < index)
			faces.push_back(Indices(p.second, p.first, index));
		if (index < p.first && index < p.second)
			faces.push_back(Indices(index, p.second, p.first));
	}
	return faces;	
}

bool CmpForEdges(Indices first, Indices second)
{
	if ((first.a < second.a) || (first.a == second.a && first.b < second.b)
		|| (first.a == second.a && first.b == second.b && first.c < second.c))
		return 1;
	else
		return 0;
}

int main()
{
	std::ifstream fin("C:\\Users\\ivras\\Documents\\ФизтехУчеба\\AKOS\\input.txt");
	std::vector<Point> points;
	int test_counter;
	int n;
	Point p;
	fin >> test_counter;
	for (int i = 0; i < test_counter; ++i)
	{
		fin >> n;
		points.resize(0);
		for (int i = 0; i < n; ++i)
		{
			fin >> p;
			points.push_back(p);
		}
		std::vector<Indices> cov = Hull(points);
		std::sort(cov.begin(), cov.end(), CmpForEdges);
		std::cout << cov.size() << '\n';
		for (int i = 0; i < cov.size(); ++i)
			std::cout << 3 << ' ' << cov[i].a << ' ' << cov[i].b << ' ' << cov[i].c << '\n';
	}
	return 0;
}


