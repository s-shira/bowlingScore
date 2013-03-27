#include <iostream>

#include "BowlingScore.h"

int main()
{
	BowlingScore bowling;

	bowling.putFrameScore( 10, 0 );
	bowling.putFrameScore(  1, 9 );
	bowling.putFrameScore( 10, 0 );
	bowling.putFrameScore(  2, 8 );
	bowling.putFrameScore( 10, 0 );
	bowling.putFrameScore(  3, 7 );
	bowling.putFrameScore( 10, 0 );
	bowling.putFrameScore(  4, 6 );
	bowling.putFrameScore( 10, 0 );
	bowling.putFrameScore(  5, 5, 10 );

	std::cout << "total score = " << bowling.score() << std::endl;

	return 0;
}
