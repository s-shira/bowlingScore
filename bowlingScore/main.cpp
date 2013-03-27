#define  _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string>

#include "BowlingScore.h"

int main()
{
	BowlingScore bowling;

	for( frame_t fr = 0; fr < BowlingScore::LAST_FRAME_NUM; ++fr )
	{
		std::string lineBuff;
		if ( ! std::getline( std::cin, lineBuff ) )  break;

		score_t sc[3] = { 0u, 0u, 0u };
		sscanf( lineBuff.c_str(), "%hu %hu %hu", &sc[0], &sc[1], &sc[2] );

		bowling.putFrameScore( sc[0], sc[1], sc[2] );
	}

	std::cout << bowling.score() << std::endl;

	return 0;
}
