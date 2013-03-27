#include "BowlingScore.h"

#define RANGE_IN( num, min, max )	((min) <= (num) && (num) <= (max))

//------------------------------------------------------------------------------
// �R���X�g���N�^
//------------------------------------------------------------------------------
BowlingScore::BowlingScore(void)
	: m_frame( 1u )
	, m_frameScoreList()
	, m_fixedFrame( 0u )
	, m_fixedScoreList()
{
	m_frameScoreList.resize( MAX_FRAME_NUM + 1 );
	m_fixedScoreList.resize( MAX_FRAME_NUM + 1 );

	m_frameScoreList[ 0 ] = FrameScoreT();
	m_fixedScoreList[ 0 ] = 0;
}

//------------------------------------------------------------------------------
// �f�X�g���N�^
//------------------------------------------------------------------------------
BowlingScore::~BowlingScore(void)
{
}

//------------------------------------------------------------------------------
// �P�t���[�����̃X�R�A�̓o�^
// ����: first    <I> - �꓊�ڂ̃X�R�A [-] (0 - 10)
//       second   <I> - �񓊖ڂ̃X�R�A [-] (0 - 10)
//       third    <I> - �O���ڂ̃X�R�A [-] (0 - 10)
// �߂�l: bool - �o�^���� [-] (true:����, false:���s)
//------------------------------------------------------------------------------
bool BowlingScore::putFrameScore(	const score_t& first,
									const score_t& second,
									const score_t& third )
{
	if ( m_frame > MAX_FRAME_NUM )		return false;

	// �X�R�A�̔���
	FrameScoreT score;

	score.first = first;
	score.second = second;
	score.third = third;

	score.state = checkScoreState( m_frame, score );
	if ( EScoreST_NONE == score.state )   return false;

	// �X�R�A�̓o�^
	m_frameScoreList[ m_frame ] = score;
	m_frame += 1;

	// �X�R�A�̍X�V
	updateScore();

	return 0;
}

//------------------------------------------------------------------------------
// �X�R�A�̎擾
// ����: frame    <I> - ���̃t���[���܂ł̃X�R�A���擾 [-] (0 - 10)
// �߂�l: score_t - �X�R�A [-] (0 - 300)
// ���l: frame = 0 �܂��� m_frame���傫���ꍇ�Am_frame�܂ł̃X�R�A���擾
//------------------------------------------------------------------------------
score_t BowlingScore::score( const frame_t& frame ) const
{
	frame_t getFrame = ( ( 0 == frame ) || ( frame > m_frame ) ) ? m_frame : frame;

	// ���~�b�g�`�F�b�N
	getFrame = std::min<score_t>( MAX_FRAME_NUM, getFrame );
	getFrame = std::max<score_t>( 1u, getFrame );

	// �X�R�A�̎擾
	score_t ret = 0u;

	if ( getFrame <= m_fixedFrame )	ret = m_fixedScoreList[ getFrame ];
	else {
		// m_fixedFrame �` getFrame �܂ł̃X�R�A���v�Z
		ret = m_fixedScoreList[ m_fixedFrame ];

		// �X�y�A�܂��̓X�g���C�N�Ń{�[�i�X���m�肵�Ă��Ȃ��������Z
		for( frame_t fr = m_fixedFrame + 1; fr <= getFrame; ++fr )
		{
			FrameScoreT frScore = m_frameScoreList[ fr ];

			ret += MAX_FRAME_SCORE;

			switch( frScore.state )
			{
			case EScoreST_SPARE:
				ret += getNextThrowScore( fr, 1u, NULL );
				break;

			case EScoreST_STRIKE:
				ret += getNextThrowScore( fr, 2u, NULL );
				break;

			default:
				break;
			}
		}
	}

	return ret;
}

//------------------------------------------------------------------------------
// �X�R�A��Ԃ̔���
// ����: frame    <I> - �t���[���ԍ� [-] (-)
//       score    <I> - ����Ώۂ̃X�R�A [-] (-)
// �߂�l: EFrameScoreState - ���茋�� [-] (-)
//------------------------------------------------------------------------------
BowlingScore::EFrameScoreState
	BowlingScore::checkScoreState( const frame_t& frame, const FrameScoreT& score ) const
{
	EFrameScoreState state = EScoreST_NONE;

	if ( RANGE_IN( frame, 1u, LAST_FRAME_NUM - 1) )
	{
		score_t total2 = score.first + score.second;

		// 1 �` 9�t���̏ꍇ
		if ( ( score.third != 0u ) ||				// �R���ڂ� 0 �ł͂Ȃ�
			 ( score.first > MAX_FRAME_SCORE ) ||	// �P���� 10 �𒴂���
			 ( total2 > MAX_FRAME_SCORE ) )			// �Q���̍��v�� 10�𒴂���
		{
			state = EScoreST_NONE;	// �G���[���
		}
		else if ( score.first == MAX_FRAME_SCORE )	// �P���ڂ� 10
		{
			state = EScoreST_STRIKE;	// �X�g���C�N
		}
		else if ( total2 == MAX_FRAME_SCORE )		// �Q���̍��v�� 10
		{
			state = EScoreST_SPARE;		// �X�y�A
		}
		else
		{
			state = EScoreST_OPEN;		// �I�[�v��
		}
	}
	else if ( frame == LAST_FRAME_NUM )
	{
		score_t total12 = score.first + score.second;  // 1, 2���ڂ̍��v
		score_t total23 = score.second + score.third;  // 2, 3���ڂ̍��v

		// 10�t���̏ꍇ
		state = EScoreST_LAST_FRAME;

		// �G���[��Ԕ���
		if ( ( score.first > MAX_FRAME_SCORE )	||	// �P���� 10�𒴂���
			 ( score.second > MAX_FRAME_SCORE ) ||	
			 ( score.third  > MAX_FRAME_SCORE ) )
		{
			state = EScoreST_NONE;
		}
		else if ( score.first == MAX_FRAME_SCORE )	// �P���ڂ��X�g���C�N
		{
			if ( ( score.second != MAX_FRAME_SCORE ) &&  // �Q���ڂ��X�g���C�N�ł͂Ȃ�
				 ( total23 > MAX_FRAME_SCORE ) )		 // 2, 3���ڂ̍��v�� 10�𒴂���
			{
				state = EScoreST_NONE;
			}
		}
		else if ( total12 > MAX_FRAME_SCORE )		// �Q���̍��v�� 10�𒴂���
		{
			state = EScoreST_NONE;
		}
		else if ( ( total12 < MAX_FRAME_SCORE ) &&	// 1, 2���ŃX�y�A�łȂ�
				  ( score.third != 0u ) )			// �R���ڂ� 0�łȂ�
		{
			state = EScoreST_NONE;
		}
	}

	return state;
}

//------------------------------------------------------------------------------
// �X�R�A�̍X�V
// ����: �Ȃ�
// �߂�l: �Ȃ�
//------------------------------------------------------------------------------
void BowlingScore::updateScore()
{
	int fnum = m_fixedFrame + 1;	// �X�R�A�v�Z���̃t���[��

	for(;;)
	{
		score_t frTotal = 0u;		// 1�t���[���̃g�[�^��
		score_t bonus   = 0u;		// �{�[�i�X
		bool    isFixed = true;		// �t���[���̍��v���m�肷�邩 [-] (true:�m��, false:�s�m��)

		FrameScoreT& fScore = m_frameScoreList[ fnum ]; // ���݂̃t���[���X�R�A

		switch( fScore.state )
		{
		case EScoreST_OPEN:
			frTotal = ( fScore.first + fScore.second );
			break;

		case EScoreST_SPARE:
			frTotal = MAX_FRAME_SCORE;
			bonus = getNextThrowScore( fnum, 1u, &isFixed );	// �P���̃{�[�i�X
			break;

		case EScoreST_STRIKE:
			frTotal = MAX_FRAME_SCORE;
			bonus = getNextThrowScore( fnum, 2u, &isFixed );	// �Q���̃{�[�i�X
			break;

		case EScoreST_LAST_FRAME:
			frTotal = ( fScore.first + fScore.second + fScore.third );
			break;

		default:
			isFixed = false;
			break;
		}

		if ( isFixed )
		{
			score_t preTotal = m_fixedScoreList[ fnum - 1 ];	// �O�t���[���܂ł̃g�[�^���X�R�A
			m_fixedScoreList[ fnum ] = preTotal + frTotal + bonus;
			m_fixedFrame += 1;
			fnum += 1;

			if ( fnum > MAX_FRAME_NUM )	break;
		}
		else
		{
			break;
		}
	}
}

//------------------------------------------------------------------------------
// ����N���̍��v�X�R�A�̎擾
// ����: frame    <I> - �t���[���ԍ� [-] (1 - 9)
//       cnt      <I> - �擾���铊���� [-] (0 - 2)
//       flag     <I> - cnt���������擾�ł������ǂ��� [-] (true:�擾�ł���, false:�擾�ł��Ȃ�)
// �߂�l: score_t - ����N���̍��v�X�R�A [-] (-)
//------------------------------------------------------------------------------
score_t BowlingScore::getNextThrowScore( const frame_t& frame, 
										 const unsigned int& cnt, 
										 bool* flag ) const
{
	score_t ret = 0u;

	if ( false == RANGE_IN( frame, 1u, LAST_FRAME_NUM - 1 ) )
	{
		if ( NULL != flag ) *flag = false;
		return ret;
	}

	unsigned int remainCnt = cnt;
	for( frame_t fnum = frame + 1; fnum < m_frame; ++fnum )
	{
		const FrameScoreT& frScore = m_frameScoreList[ fnum ];

		if ( EScoreST_NONE == frScore.state )	break;

		// �P���ڂ̉��Z
		ret += frScore.first;
		remainCnt -= 1;
		if ( remainCnt <= 0 )	break;

		if ( EScoreST_STRIKE == frScore.state )		continue;

		// �Q���ڂ̉��Z
		ret += frScore.second;
		remainCnt -= 1;
		if ( remainCnt <= 0 )  break;

		if ( EScoreST_LAST_FRAME != frScore.state )	continue;

		// �R���ڂ̉��Z
		ret += frScore.third;
		remainCnt -= 1;
		if ( remainCnt <= 0 )  break;
	}

	if ( NULL != flag )
	{
		if ( remainCnt <= 0 )  *flag = true;
		else				   *flag = false;
	}

	return ret;
}
