#include <algorithm>
#include <cmath>
#include <iostream>
#include <fstream>
#include <stack>
#include <vector>

const long double INF = 1e9 + 3;

struct point
{
	long double x;
	long double  y;
	point() {}
	point(long double x, long double y) : x(x), y(y) {}
};

bool operator==(const point& A, const point& B) 
{
	return A.x == B.x && A.y == B.y;
}

bool operator!=(const point& A, const point& B) 
{
	return !(A == B);
}


std::istream& operator>>(std::istream& i, point& A)
{
	i >> A.x >> A.y;
	return i;
}

std::ostream& operator<<(std::ostream& out, const point& A)
{
	out << A.x << ' ' << A.y;
	return out;
}

struct Vector
{
	point A;
	point B;
	long double x;
	long double y;
	Vector() {}
	Vector(point A, point B) : A(A), B(B) {
		this->x = B.x - A.x;
		this->y = B.y - A.y;
	}
	inline long double len()
	{
		return sqrt(x * x + y * y);
	}
};

long double dot_product(const Vector& A,const Vector& B) {
	return A.x * B.x + A.y * B.y;
}

long double cross_product(const Vector& A, const Vector& B) {
	return A.x * B.y - A.y * B.x;
}

bool cmp_up(const point& A, const point& B)
{
	return (A.x > B.x || (A.x == B.x && A.y > B.y));
}

bool cmp_down(const point& A, const point& B)
{
	return (A.x < B.x || (A.x == B.x && A.y < B.y));
}

std::vector<point> Graham_convex_hull(const std::vector<point>& points)
{
	if (points.size() == 2)
	{
		std::vector<point> result = { points[1] , points[0] };
		return result;
	}
	std::vector<point> min_cover;
	std::stack<point> st;
	st.push(points[points.size() - 1]);
	st.push(points[0]);
	for (int i = 1; i < points.size(); ++i)
	{
		bool flag = false;
		while (!flag)
		{
			point B = st.top();
			st.pop();
			point A = st.top();
			st.pop();
			Vector V1(A, B);
			Vector V2(B, points[i]);
			if (cross_product(V1, V2) > 0)
			{
				flag = true;
				st.push(A);
				st.push(B);
			}
			else
				st.push(A);
		}
		st.push(points[i]);
	}
	while (st.size() != 1)
	{
		point A = st.top();
		min_cover.push_back(A);
		st.pop();
	}
	return min_cover;
}

std::pair<int, int> find_most_right_and_most_left_point(std::vector<point>& points)
{
	int left_point_number = 0;
	int right_point_number = 0;
	long double max_right = INF * (-1);
	long double max_left = INF;
	for (int i = 0; i < points.size(); ++i)
	{
		if (points[i].x > max_right ||
			(points[i].x == max_right && points[i].y > points[right_point_number].y))
		{
			max_right = points[i].x;
			right_point_number = i;
		}
		if (points[i].x < max_left ||
			(points[i].x == max_left && points[i].y < points[left_point_number].y))
		{
			max_left = points[i].x;
			left_point_number = i;
		}
	}
	return std::make_pair(left_point_number, right_point_number);
}

std::vector<point> Andru_convex_hull(std::vector<point>& points)
{
	std::pair<int, int> left_right = find_most_right_and_most_left_point(points);
	int left_point_number = left_right.first;
	int right_point_number = left_right.second;
	std::vector<point> up_half;
	std::vector<point> down_half;
	up_half.push_back(points[left_point_number]);
	up_half.push_back(points[right_point_number]);
	down_half.push_back(points[right_point_number]);
	down_half.push_back(points[left_point_number]);
	std::swap(points[left_point_number], points[points.size() - 1]);
	std::swap(points[right_point_number], points[points.size() - 2]);
	points.resize(points.size() - 2);
	for (int i = 0; i < points.size(); ++i)
	{
		Vector A(down_half[0], down_half[1]);
		Vector B(down_half[0], points[i]);
		if (cross_product(B, A) > 0)
			up_half.push_back(points[i]);
		if (cross_product(B, A) < 0)
			down_half.push_back(points[i]);
	}
	std::sort(up_half.begin(), up_half.end(), cmp_up);
	std::sort(down_half.begin(), down_half.end(), cmp_down);
	std::vector<point> answer_up = Graham_convex_hull(up_half);
	std::vector<point> answer_down = Graham_convex_hull(down_half);
	std::vector<point> min_cover;
	for (int i = 0; i < answer_up.size() - 1; ++i)
		min_cover.push_back(answer_up[i]);
	for (int i = 0; i < answer_down.size() - 1; ++i)
		min_cover.push_back(answer_down[i]);
	return min_cover;
}

std::vector<point> clear_from_reiterations(std::vector<point>& points)
{
	std::vector<point> points_with_out_coincidences;
	std::sort(points.begin(), points.end(), cmp_up);  //dosen't matter cmp_up or cmp_down
	points_with_out_coincidences.push_back(points[0]);
	for (int i = 1; i < points.size(); ++i)
	{
		if (points[i] != points[i - 1])
			points_with_out_coincidences.push_back(points[i]);
	}
	return points_with_out_coincidences;
}

long double fence_length(const std::vector<point>& min_cover)
{
	long double length = 0;
	for (int i = 0; i < min_cover.size() - 1; ++i)
	{
		Vector V(min_cover[i], min_cover[i + 1]);
		length += V.len();
	}
	return length;
}

int main()
{
	std::ifstream fin("input.txt");
	std::vector<point> points;
	int n = 0;
	fin >> n;
	point A;
	for (int i = 0; i < n; ++i)
	{
		fin >> A;
		points.push_back(A);
	}
	std::vector<point> points_with_out_coincidences = clear_from_reiterations(points);
	std::vector<point> min_cover = Andru_convex_hull(points_with_out_coincidences);
	
	std::cout.precision(50);
	min_cover.push_back(min_cover[0]);
	std::cout << fence_length(min_cover);
}