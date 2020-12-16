#include <iostream>
#include <fstream>
#include <stack>
#include <vector>
#include <set>

const int SIZE = 8;

struct Point
{
    int x_;
    int y_;
    Point() {}
    Point(const int& x, const int& y) : x_(x), y_(y) {}
    bool operator==(const Point& other) const;
    bool operator!=(const Point& other) const;
};

std::ostream& operator<<(std::ostream& out, const Point& point) {
    out << point.x_ << ' ' << point.y_ << "  ";
    return out;
}

bool Point::operator==(const Point& other) const
{
    return x_ == other.x_ && y_ == other.y_;
}

bool Point::operator!=(const Point& other) const
{
    return !(*this == other);
}

struct State
{
    Point runner_;
    Point terminator_;
    bool turn_;         // 0 - runner's turn, 1 - terminator's turn
    State() {}
    State(const Point& runner, const Point& terminator, bool turn) :
        runner_(runner), terminator_(terminator), turn_(turn) {}
};

class Board
{
public:
    Board(const std::vector<std::vector<bool>>& board): board_(board) {}
    bool is_shot_(Point terminator, Point runner) const;
    bool is_empty(Point request) const;
private:
    std::vector<std::vector<bool>> board_;   // 0 - empty
};

bool Board::is_empty(Point request) const
{ 
    return board_[request.x_][request.y_] == 0;
}

bool Board::is_shot_(Point terminator, Point runner) const
{
    int x_path = terminator.x_ - runner.x_;
    int y_path = terminator.y_ - runner.y_;
    if (x_path == y_path || x_path == y_path * (-1) || y_path == 0 || x_path == 0)
    {
        int len = 1;
        int x_direction = 0;
        if (x_path > 0)
            x_direction = 1;
        if (x_path < 0)
            x_direction = -1;
        int y_direction = 0;
        if (y_path > 0)
            y_direction = 1;
        if (y_path < 0)
            y_direction = -1;
        while (len < std::max(abs(x_path), abs(y_path)))
        {
            if (board_[terminator.x_ - len * x_direction]
                [terminator.y_ - len * y_direction] == 1)
                return 0;
            ++len;
        }
        return 1;
    }
    return 0;
}

struct cmp_for_states {
    bool operator()(const State& first, const State& second) const {
        if (first.runner_.x_ < second.runner_.x_ || 
            (first.runner_.x_ == second.runner_.x_ && first.runner_.y_ < second.runner_.y_))
            return true;
        if (first.runner_.x_ == second.runner_.x_ && first.runner_.y_ == second.runner_.y_ &&
            (first.terminator_.x_ < second.terminator_.x_ ||
                (first.terminator_.x_ == second.terminator_.x_ &&
                    first.terminator_.y_ < second.terminator_.y_)))
            return true;
        if (first.runner_.x_ == second.runner_.x_ && first.runner_.y_ == second.runner_.y_ &&
            first.terminator_.x_ == second.terminator_.x_ &&
            first.terminator_.y_ == second.terminator_.y_ && first.turn_ < second.turn_)
            return true;
        return false;
    }
};

bool IsCorrectPoint(Point& request, Board& board)
{
    return !(request.x_ < 0 || request.y_ < 0 || request.x_ == SIZE ||
        request.y_ == SIZE || !board.is_empty(request));
}

void terminator_win_steps(Board& board, 
    std::set <State, cmp_for_states>& win_cases, 
    std::stack<State>& win_in_process,
    std::vector<std::pair<int, int>>& directions, State& current)
{
    Point terminator = current.terminator_;
    Point runner = current.runner_;
    for (int i = 0; i < directions.size(); ++i)
    {
        Point prev_terminator(terminator.x_ + directions[i].first,
            terminator.y_ + directions[i].second);
        if  (!IsCorrectPoint(prev_terminator, board) ||
            board.is_shot_(runner, prev_terminator) ||
            win_cases.find(State(runner, prev_terminator, 1)) != win_cases.end())
            continue;
        bool win_step = 1;
        for (int j = 0; j < directions.size(); ++j)
        {
            Point possible_step(prev_terminator.x_ + directions[j].first,
                prev_terminator.y_ + directions[j].second);
            if (!IsCorrectPoint(possible_step, board))
                continue;
            if (win_cases.find(State(runner, possible_step, 0)) == win_cases.end())
                win_step = 0;
        }
        if (win_step)
        {
            win_cases.insert(State(runner, prev_terminator, 1));
            win_in_process.push(State(runner, prev_terminator, 1));
        }
    }
}


void runner_win_steps(Board& board,
    std::set < State, cmp_for_states > & win_cases,
    std::stack<State>& win_in_process,
    std::vector<std::pair<int, int>>& directions, State& current)
{
    Point terminator = current.terminator_;
    Point runner = current.runner_;
    for (int i = 0; i < directions.size(); ++i)
    {
        Point prev_runner(runner.x_ + directions[i].first,
            runner.y_ + directions[i].second);
        if (!IsCorrectPoint(prev_runner, board) ||
            board.is_shot_(prev_runner, terminator) ||
            win_cases.find(State(prev_runner, terminator, 0)) != win_cases.end())
            continue;
        bool win_step = 0;
        for (int j = 0; j < directions.size(); ++j)
        {
            Point possible_step(prev_runner.x_ + directions[j].first,
                prev_runner.y_ + directions[j].second);
            if (!IsCorrectPoint(possible_step, board))
                continue;
            if (win_cases.find(State(possible_step, terminator, 1)) != win_cases.end())
                win_step = 1;
        }
        if (win_step)
        {
            win_cases.insert(State(prev_runner, terminator, 0));
            win_in_process.push(State(prev_runner, terminator, 0));
        }
    }
}


bool is_safe(Point& runner_begin, Point& terminator_begin, Board& board)
{
    std::set<State, cmp_for_states> win_cases(cmp_for_states{});
    std::stack<State> win_in_process;
    for (int i = 0; i < SIZE; ++i)
    {
        for (int j = 0; j < SIZE; ++j)
            for (int k = 0; k < SIZE; ++k)
            {
                Point runner(SIZE - 1, i);
                Point terminator(j, k);
                if (runner != terminator && board.is_empty(runner) && board.is_empty(terminator)
                    && !board.is_shot_(runner, terminator))
                {
                    win_in_process.push(State(runner, terminator, 0));
                    win_cases.insert(State(runner, terminator, 0));
                }
            }
    }

    ///////////////////////////////////////////
    std::vector<std::pair<int, int>> directions;
    for (int i = -1; i <= 1; ++i)
        for (int j = -1; j <= 1; ++j)
            if (i != 0 || j != 0)
                directions.push_back(std::make_pair(i, j));
    ////////////////////////////////////////////
    while (win_in_process.size() != 0)
    {
        State current = win_in_process.top();
        win_in_process.pop();
        if (current.turn_ == 0)
            terminator_win_steps(board, win_cases, win_in_process, directions, current);
        else
            runner_win_steps(board, win_cases, win_in_process, directions, current);
    }
    return win_cases.find(State(runner_begin, terminator_begin, 0)) != win_cases.end();
}


int main()
{
    std::ifstream fin("C:\\Users\\ivras\\Documents\\ФизтехУчеба\\AKOS\\input.txt");
    Point runner_begin, terminator_begin;
    std::vector<std::vector<bool>> board_vector(SIZE);
    for (int i = 0; i < SIZE; ++i)
        for (int j = 0; j < SIZE; ++j)
        {
            char number;        
            fin >> number;
            int cage = number - '0';
            if (cage == 2)
            {
                cage = 0;
                runner_begin = Point(i, j);
            }
            if (cage == 3)
            {
                cage = 0;
                terminator_begin = Point(i, j);
            }
            board_vector[i].push_back(cage);
        }
    Board board(board_vector);
        if (is_safe(runner_begin, terminator_begin, board))
            std::cout << 1;
        else
            std::cout << -1;
    return 0;
}


