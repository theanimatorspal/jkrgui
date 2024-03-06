#include "CryptoSolver.hpp"

v<v<sv>> fk = {
	{"BASE", "BALL", "GAMES"},
	{"LOGIC", "LOGIC", "PROLOG"},
	{"ONE", "ONE", "TWO", "FOUR"},
	{"LETS", "WAVE", "LATER"},
	{"CROSS", "ROADS", "DANGER"},
	{"KYOTO", "OSAKA", "TOKYO"},
	{"RIGHT", "RIGHT", "WRONG"},
	{"APPLE", "GRAPE", "CHERRY"}
};

int main()
{
	for(auto& vv : fk)
	{
		CryptoSolver Solver;
		for(int i = 0; i < vv.size() - 1; i++ )
		{
			Solver.RegisterInput(vv[i]);
		}
		Solver.RegisterOutput(vv.back());
		Solver.SolveByBruteForce();
		*(Solver);
	}
	return 0;
}
