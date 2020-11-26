#include <iostream>
#include <vector>
#include <algorithm>
#include <stack>
#include <fstream>
#include <cmath>
#include <set>

const long double INF = 1e9 + 3;
const double EPS = 1E-9;

struct Point
{
	long double x_;
	long double y_;
	Point() {}
	Point(long double x, long double y) : x_(x), y_(y) {}
	Point& operator*=(const long double& d);
};

Point& Point::operator*=(const long double& d)
{
	x_ *= d;
	y_ *= d;
	return *this;

}

std::istream& operator>>(std::istream& i, Point& A)
{
	i >> A.x_ >> A.y_;
	return i;
}

std::ostream& operator<<(std::ostream& out, const Point& A)
{
	out << A.x_ << ' ' << A.y_;
	return out;
}

struct Segment
{
	Point left_end_;
	Point right_end;
	Segment() {}
	Segment(Point A, Point B) : left_end_(A), right_end(B) {}
	long double get_y_(long double x) const {
		if (abs(left_end_.x_ - right_end.x_) < EPS)  return left_end_.y_;
		return left_end_.y_ + (right_end.y_ - left_end_.y_) * 
			(x - left_end_.x_) / (right_end.x_ - left_end_.x_);
	}
};

std::istream& operator>>(std::istream& i, Segment& sec)
{
	i >> sec.left_end_ >> sec.right_end;
	return i;
}

std::ostream& operator<<(std::ostream& os, Segment& sec)
{
	os << sec.left_end_ << ' ' << sec.right_end;
	return os;
}

struct Vector
{
	Point A;
	Point B;
	long double x;
	long double y;
	Vector() {}
	Vector(Point A, Point B) : A(A), B(B) {
		this->x = B.x_ - A.x_;
		this->y = B.y_ - A.y_;
	}
	long double len(){
		return sqrt(x * x + y * y);
	}
	Vector normalize()
	{
		Vector new_vec;
		new_vec.x = x / len();
		new_vec.y = y / len();
		new_vec.A = A;
		new_vec.B = Point(A.x_ + new_vec.x, A.y_ + new_vec.y);
		return new_vec;
	}
};

long double DotProduct(Vector A, Vector B) {
	return A.x * B.x + A.y * B.y;
}

long double CrossProduct(Vector A, Vector B) {
	return A.x * B.y - A.y * B.x;
}

Point operator+(const Point& A, const Vector& B)
{
	return Point(A.x_ + B.x, A.y_ + B.y);
}

struct Segment_state
{
	Segment sec;
	int64_t index;
	bool open_close;
	Segment_state(){}
	Segment_state(Segment& sec, int64_t index, bool open_close) :
		sec(sec), index(index), open_close(open_close) {}
};

auto cmp1 = [](std::pair<Segment, int> a, std::pair<Segment, int> b) {
	long double a_y;
	long double b_y;
	long double x = std::max(a.first.left_end_.x_, b.first.left_end_.x_);
	a_y = a.first.get_y_(x);
	b_y = b.first.get_y_(x);
	return ((a_y < b_y) || ((a_y == b_y) && (a.second < b.second)));
};

auto cmp2 = [](Segment_state a, Segment_state b) {
	Point A, B;
	A = a.open_close == 0 ? a.sec.left_end_ : a.sec.right_end;
	B = b.open_close == 0 ? b.sec.left_end_ : b.sec.right_end;
	if (A.x_ < B.x_)
		return true;
	if (A.x_ == B.x_)
	{
		if (a.open_close == 0 && b.open_close == 1)
			return true;
		if (a.open_close == b.open_close && a.index < b.index)
			return true;
	}
	return false;

};

bool SegmentCross(Segment first, Segment second)
{
	//std::cout << first << ' ' << second << '\n';
	Vector V1(first.left_end_, first.right_end);
	Vector V2(first.left_end_, second.left_end_);
	Vector V3(first.left_end_, second.right_end);
	if (CrossProduct(V1, V2) * CrossProduct(V1, V3) > 0)
		return 0;
	V1 = Vector(second.left_end_, second.right_end);
	V2 = Vector(second.left_end_, first.left_end_);
	V3 = Vector(second.left_end_, first.right_end);
	if (CrossProduct(V1, V2) * CrossProduct(V1, V3) > 0)
		return 0;
	if (CrossProduct(V1, V2) * CrossProduct(V1, V3) == 0
		&& CrossProduct(V1, V2) * CrossProduct(V1, V3) == 0)
		if (std::min(first.left_end_.y_, first.right_end.y_) >
			std::max(second.left_end_.y_, second.right_end.y_) ||
			std::min(second.left_end_.y_, second.right_end.y_) >
			std::max(first.left_end_.y_, first.right_end.y_))
			return 0;
	return 1;
}

std::pair<int, int> AddSegment(int i, std::vector<Segment>& segments, 
	std::set<std::pair<Segment, int>, decltype(cmp1)>& Segment_sort,
	std::vector<Segment_state>& skanline)
{
	std::set <std::pair<Segment, int>>::iterator it, it_minus1, it_plus1;
	Segment_sort.insert(std::make_pair(skanline[i].sec, skanline[i].index));
	it = Segment_sort.find(std::make_pair(skanline[i].sec, skanline[i].index));
	it_plus1 = it;
	it_minus1 = it;
	++it_plus1;

	if (it != Segment_sort.begin())
	{
		--it_minus1;
		if (SegmentCross(segments[(*it_minus1).second], segments[(*it).second]))
			return std::make_pair((*it_minus1).second, (*it).second);
	}

	if (it_plus1 != Segment_sort.end())
	{
		if (SegmentCross(segments[(*it_plus1).second], segments[(*it).second]))
			return std::make_pair((*it_plus1).second, (*it).second);
	}
	return std::make_pair(-1, -1);
}

std::pair<int, int> DeleteSegment(int i, std::vector<Segment>& segments,
	std::set<std::pair<Segment, int>, decltype(cmp1)>& Segment_sort,
	std::vector<Segment_state>& skanline)
{
	std::set <std::pair<Segment, int>>::iterator it, it_minus1, it_plus1;
	it = Segment_sort.find(std::make_pair(skanline[i].sec, skanline[i].index));
	it_minus1 = it;
	it_plus1 = it;
	++it_plus1;
	if (it != Segment_sort.begin() && it_plus1 != Segment_sort.end())
	{
		--it_minus1;
		if (SegmentCross(segments[(*it_plus1).second], segments[(*it_minus1).second]))
			return std::make_pair((*it_plus1).second, (*it_minus1).second);
	}
	Segment_sort.erase(it);
	return std::make_pair(-1, -1);
}

std::pair<int, int> FindCrossSegments(std::vector<Segment>& segments)
{

	std::set<std::pair<Segment, int>, decltype(cmp1)> Segment_sort(cmp1);
	std::set <std::pair<Segment, int>>::iterator it, it_minus1, it_plus1;
	std::vector<Segment_state> skanline;
	for (int64_t i = 0; i < segments.size(); ++i)
	{
		skanline.push_back(Segment_state(segments[i], i, 0));
     	skanline.push_back(Segment_state(segments[i], i, 1));
	}
	std::sort(skanline.begin(), skanline.end(), cmp2);
	for (int64_t i = 0; i < skanline.size(); ++i)
	{
		if (skanline[i].open_close == 0)
		{
			std::pair<int, int> crossing_sections =
				AddSegment(i, segments, Segment_sort, skanline);
			if (crossing_sections != std::make_pair(-1, -1))
				return crossing_sections;
		}
		else
		{
			std::pair<int, int> crossing_sections =
				DeleteSegment(i, segments, Segment_sort, skanline);
			if (crossing_sections != std::make_pair(-1, -1))
				return crossing_sections;
		}
	}
	return std::make_pair(-1, -1);
}



int main()
{
	int64_t n;
	std::ifstream fin("C:\\Users\\ivras\\Documents\\ФизтехУчеба\\AKOS\\input.txt");
	std::vector<Segment> segments;
	Segment sec;
	fin >> n;
	for (int64_t i = 0; i < n; ++i)
	{
		fin >> sec;
		if (sec.left_end_.x_ > sec.right_end.x_)
			sec = Segment(sec.right_end, sec.left_end_);
		segments.push_back(sec);
	}
	std::pair<int, int> answer = FindCrossSegments(segments);
	if (answer.first > answer.second)
		answer = std::make_pair(answer.second, answer.first);
	if (answer == std::make_pair(-1, -1))
		std::cout << "NO";
	else
		std::cout << "YES\n" << answer.first + 1 << ' ' << answer.second + 1;
	return 0;
}