#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <vector>
#include <set>
#include <fstream>

using namespace std;

struct Checker {
	set<int> reqA;
	set<int> reqB;
	set<int> electives;
	set<int> my_electives;
	int mode;

	void init(string track) {
		mode = -1;

		ifstream track_file (track);

		if (track_file.is_open()) {
			int num, course;
			track_file >> num;
			for (int i = 0; i < num; ++i) {
				track_file >> course;
				reqA.insert(course);
			}
			track_file >> num;
			for (int i = 0; i < num; ++i) {
				track_file >> course;
				reqB.insert(course);
			}
			track_file >> num;
			for (int i = 0; i < num; ++i) {
				track_file >> course;
				electives.insert(course);
			}
		}
	}

	void course_input() {
		int num, cnt = 0;
		while (cin >> num) {
			/* Due to significant overlap,
			   students can receive credits
			   for only one of these courses
			   (either COMS W4771 Machine Learning
			   or COMS W4721 Machine Learning for Data Science).
			 */
			if (num > 0) {
				if (num == 4771 || num == 4721) {
					if (!cnt++)
						courses.insert(num);
				} else
					courses.insert(num);
			} else {
				waived.insert(-num);
			}
		}
	}

	void check_required() {
		int cntA = 0, cntB = 0;
		for (auto &course: courses) {
			if (reqA.find(course) != reqA.end())
				cntA++;
			if (reqB.find(course) != reqB.end())
				cntB++;
		}
		for (auto &course: waived) {
			if (reqA.find(course) != reqA.end()) {
				cntA++;
			}
			if (reqB.find(course) != reqB.end()) {
				cntB++;
			}
		}
		if (cntA >= 2 || (cntA >= 1 && cntB >= 1)) {
			cout << "Passed: required courses are satisfied!" << endl;
			if (cntA >= 2)
				mode = 1;
			else
				mode = 2;
		} else
			cout << "Error: required courses are not satisfied!" << endl;
	}

	void check_breadth() {
		/* Required track and track elective courses taken
		   at Columbia can satisfy breadth requirements
		   if they also appear on the approved lists below.
		   All breadth courses must be taken at Columbia and
		   cannot be waived.
		 */
		// no check for coms w 4995 - Advanced Algorithm
		int cnt[3];
		memset(cnt, 0, sizeof(cnt));
		for (auto &course: courses) {
			int prefix2, prefix3;
			prefix2 = course / 100;
			prefix3 = course / 10;
			// for theory
			if (prefix2 == 42 && course != 4246)
				cnt[0]++;
			// for systems
			if (prefix2 == 41 && course != 4121 && prefix3 != 416 &&
			    prefix3 != 417)
				cnt[1]++;
			if (prefix2 == 48)
				cnt[1]++;
			if (course == 4444)
				cnt[1]++;
			// CSEE 4119, CSEE 4823, CSEE 4824, CSEE 4840, CSEE 4868
			// are included above
			if (course == 4340)
				cnt[1]++;
			// for AI & Applications
			// CBMF 4761 is included
			if (prefix2 == 47 && course != 4721 && course != 4776)
				cnt[2]++;
			if (prefix3 == 416 || prefix3 == 417)
				cnt[2]++;
		}
		string out = "";
		bool state = true;
		int sum = 0;
		for (int i = 0; i < 3; ++i) {
			if (cnt[i] < 1) {
				string group;
				if (i == 0)
					group = "Theory";
				if (i == 1)
					group = "System";
				if (i == 2)
					group = "AI & Application";
				out +=  "\t - group " + group + " is not satisfied\n";
				state = false;
			}
			sum += cnt[i];
		}
		if (sum < 4) {
			out += "\t - total number is not satisfied\n";
			state = false;
		}
		if (state)
			out = "Passed: breadth courses are satisfied!\n" + out;
		else
			out = "Error: breadth courses are not satisfied!\n" + out;
		cout << out;
	}

	void check_track_elective() {
		set<int> not_available;
		int cntA, cntB, num_my_elec = 0;
		string out;
		if (mode == 1) {
			cntA = 2;
		} else if (mode == 2) {
			cntA = 1;
			cntB = 1;
		}
		for (auto &course: courses) {
			if (mode == 1) {
				if (cntA && reqA.find(course) != reqA.end()) {
					cntA--;
					not_available.insert(course);
				}
			} else if (mode == 2) {
				if (cntA && reqA.find(course) != reqA.end()) {
					cntA--;
					not_available.insert(course);
				}
				if (cntB && reqB.find(course) != reqB.end()) {
					cntB--;
					not_available.insert(course);
				}
			} else {
				not_available.insert(course);
			}
		}
		for (auto &course: courses) {
			if (not_available.find(course) == not_available.end() &&
			    electives.find(course) != electives.end()) {
				my_electives.insert(course);
				if (++num_my_elec >= 2)
					break;
			}
		}
		if (num_my_elec >= 2) {
			out = "Passed: track elective course requirement is satisfied!\n" + out;
		} else {
			out = "Error: track elective course requirement is not satisfied!\n";
			out = out + "\t - You need to have 2 or more track elective courses! You only have " + to_string(num_my_elec) + "/2 now \n";
		}
		cout << out;
	}

	void check_general_elective() {
		return;
	}

	void check_6000() {
		string out;
		int cnt = 0;
		int in_elective = 0;
		for (auto &course: courses) {
			int prefix1 = course / 1000;
			if (prefix1 == 6) {
				cnt++;
				if (electives.find(course) != electives.end())
					in_elective++;
			}
		}
		if (cnt < 2 || in_elective < 1) {
			out = "Error: 6000-level courses are not satisfied!\n";
			if (cnt < 2)
				out += "\t - you need to choose 2 or more 6000-level courses, you only have " + to_string(cnt) + "/2 currently\n";
			if (in_elective < 1)
				out += "\t - you need to choose at least 1 6000-level course in your track electives\n";
		} else {
			out = "Passed: 6000-level courses are satisfied!\n";
		}
		cout << out;
	}

	void check() {
		check_required();
		check_breadth();
		check_track_elective();
		check_general_elective();
		check_6000();
	}

	set<int> courses;
	set<int> waived;
};

int main()
{
	ios::sync_with_stdio(false);
	string track;
	cin >> track;
	Checker checker;
	checker.init(track);
	checker.course_input();
	checker.check();
	return 0;
}
