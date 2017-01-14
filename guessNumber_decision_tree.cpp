#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <list>
#include <string>
#include <unordered_set>
#include <utility>
#include <limits>

std::vector<std::pair<int, int> >all_AB;
std::list<std::string> all_possibilities;
class Answer
{
private:
	std::string str;
	std::set<char> inside;
public:
	Answer(const std::string& in): str(in) {
		for (int i=0; i<str.length(); i++) {
			inside.insert(str[i]);
		}
	};
	~Answer(){};

	std::pair<int, int> AB(const std::string& guess) {
		std::pair<int, int> ret; // ret.first: ?A, ret.second: ?B
		for (int i=0; i<guess.length(); i++) {
			if (guess[i] == str[i]) ret.first++;
			else if (inside.find(guess[i]) != inside.end()) ret.second++;
		}
		return ret;
	}
};
class GuessAI
{
public:
	std::list<std::string> remain;
	int length;
	GuessAI(int length, std::list<std::string>& remain): length(length) {
		this->remain = remain;
	}
	~GuessAI(){}
	void print_remain() { // for debug
		std::cout << "There are " << remain.size() << " element in remain\n";
		for (auto& str: remain) {
			std::cout << str << ' ';
		}
		std::cout << std::endl;
	}
	void reduce(const std::string& guess, std::pair<int, int> AB) {
		for (auto it = remain.begin(); it != remain.end();) {
			Answer assume(*it);
			if (assume.AB(guess) != AB) {	// the string it point to won't be correct answer
				it = remain.erase(it);
			} else {
				it++;
			}
		}
	}
	int getRemainSize() {
		return this->remain.size();
	}
	int getRemainSizeIf(const std::string& guess, std::pair<int, int> AB) {
		int ret = 0;
		for (auto it = remain.begin(); it != remain.end(); it++) {
			Answer assume(*it);
			if (assume.AB(guess) == AB) ret++;
		}
		return ret;
	}
	std::string next_guess() {
		if (remain.size() == 1) {
			return *(remain.begin());
		}
		std::string best_from_worst;
		int size_best_from_worst = std::numeric_limits<int>::max();
		for (auto& guess: all_possibilities) {
			int size_worst = std::numeric_limits<int>::min();
			for (auto& AB: all_AB) {
				size_worst = std::max(size_worst, this->getRemainSizeIf(guess, AB));
			}
			if (size_worst < size_best_from_worst) {
				size_best_from_worst = size_worst;
				best_from_worst = guess;
			}
		}
		return best_from_worst;
	}
};
class Dnode
{
public:
	std::string now_guess;
	std::map<std::pair<int, int>, Dnode*> decision;
	Dnode(const std::string& now_guess): now_guess(now_guess){};
	~Dnode();
	bool hasDecision(std::pair<int, int> AB) {
		if (decision.find(AB) == decision.end()) return false;
		return true;
	}
	void addDecision(std::pair<int, int> AB, const std::string& next_guess) {
		decision.insert(std::make_pair(AB, new Dnode(next_guess)));
	}
};
void testttt(void);
void construct_all_AB(int length);
void construct_all_possibilities(std::string& str, unsigned int total_len);
void round_print(int& rnd, std::string& next_guess, std::pair<int, int>AB);
void all_test();
int main(int argc, char const *argv[])
{
	// all_test();
	
	std::string in, temp;
	std::cin >> in;
	Answer correct(in);

	construct_all_AB( in.length() );
	construct_all_possibilities(temp, in.length());
	GuessAI guessAI( in.length(), all_possibilities );

	int rnd = 0;
	Dnode *root = new Dnode("0123");
	Dnode *cur = root;
	std::pair<int, int> AB = correct.AB(cur->now_guess);
	round_print(rnd, cur->now_guess, AB);

	while (AB != std::make_pair(4, 0)) {
		guessAI.reduce(cur->now_guess, AB);
		if (not cur->hasDecision(AB)) {
			cur->addDecision(AB, guessAI.next_guess());
		}
		cur = cur->decision[AB];
		AB = correct.AB(cur->now_guess);
		round_print(rnd, cur->now_guess, AB);
	}
	std::cout << std::endl;

	return 0;
}

void all_test(void)
{
	std::string temp;
	construct_all_AB( 4 );
	construct_all_possibilities(temp, 4);
	std::vector<std::pair<std::string, int> > time_guess;
	Dnode *root = new Dnode("0123");
	for (auto& in: all_possibilities) {
		std::cout << "correct answer: " << in << std::endl;
		Answer correct(in);
		GuessAI guessAI( in.length(), all_possibilities );
		int rnd = 0;
		Dnode *cur = root;
		std::pair<int, int> AB = correct.AB(cur->now_guess);
		round_print(rnd, cur->now_guess, AB);

		while (AB != std::make_pair(4, 0)) {
			guessAI.reduce(cur->now_guess, AB);
			// guessAI.print_remain();
			if (not cur->hasDecision(AB)) {
				cur->addDecision(AB, guessAI.next_guess());
			}
			cur = cur->decision[AB];
			AB = correct.AB(cur->now_guess);
			round_print(rnd, cur->now_guess, AB);
		}
		time_guess.push_back(std::make_pair(in, rnd));
		std::cout << std::endl;
	}

	for (auto& pair: time_guess) {
		printf("(%s: %d) ", pair.first.c_str(), pair.second);
	}
}
void testttt(void)
{
	Answer correct(std::string("123"));
	std::string temp;
	construct_all_AB( 3 );
	construct_all_possibilities(temp, 3);
	std::string guess("124");
	GuessAI guessAI(3, all_possibilities);	
	guessAI.reduce(guess, correct.AB(guess));
	guessAI.print_remain();

	std::string next_guess = guessAI.next_guess();
	std::cout << "next_guess == " << next_guess << std::endl;
	guessAI.reduce(next_guess, correct.AB(next_guess));
	guessAI.print_remain();

}
void construct_all_AB(int length)
{
	all_AB.push_back(std::make_pair(length, 0));
	all_AB.push_back(std::make_pair(length-1, 0));
	for (int i=length-2; i>=0; i--) {
		for (int j=0; j <= length-i; j++) {
			all_AB.push_back(std::make_pair(i, j));
		}
	}
	return;
}
inline bool str_exist(const std::string& str, char ch) {
	for (char c: str) {
		if (c == ch) return true;
	}
	return false;
}
void construct_all_possibilities(std::string& str, unsigned int total_len)
{
	if (str.length() == total_len) {
		all_possibilities.push_back(str);
		return;
	}
	for (char ch = '0'; ch <= '9'; ch++) {
		if (not str_exist(str, ch)) {
			str.append(1, ch);
			construct_all_possibilities(str, total_len);
			str.erase(str.end()-1);
		}
	}
}
void round_print(int& rnd, std::string& next_guess, std::pair<int, int>AB)
{
	std::cout 	<< "Round-" << ++rnd << " " << next_guess << " " 
				<< AB.first << "A" << AB.second << "B" << std::endl;
}