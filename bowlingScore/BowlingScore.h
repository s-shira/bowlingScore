//------------------------------------------------------------------------------
// �{�[�����O�̃X�R�A�v�Z������N���X��`
//------------------------------------------------------------------------------
#pragma once

#include <vector>

typedef unsigned short frame_t;		// �t���[���ԍ��p�̌^
typedef unsigned short score_t;		// �X�R�A�p�̌^

//------------------------------------------------------------------------------
// �{�[�����O�̃X�R�A�v�Z������N���X��`
//------------------------------------------------------------------------------
class BowlingScore
{
public:
	static const score_t LAST_FRAME_NUM = 10;	// �Ō�̃t���[���ԍ� (10�t��)
	static const frame_t MAX_FRAME_NUM = 10;	// �ő�̃t���[���ԍ�
	static const score_t MAX_FRAME_SCORE = 10;	// ��t���[���̃X�R�A�̍ő�l

public:
	BowlingScore(void);
	virtual ~BowlingScore(void);

	bool putFrameScore( const score_t& first,
						const score_t& second,
						const score_t& third = 0 );

	score_t score( const frame_t& frame = 0 ) const;

protected:
	enum EFrameScoreState	// 1�t���[���̃X�R�A�̏��
	{
		EScoreST_NONE =	0		// ��ԂȂ� (�����l)
	,	EScoreST_OPEN			// �I�[�v��
	,	EScoreST_SPARE			// �X�y�A
	,	EScoreST_STRIKE			// �X�g���C�N
	,	EScoreST_LAST_FRAME		// �ŏI�t���[���̃X�R�A
	};

	typedef struct _frameScore		// 1�t���[���ɂ�����X�R�A�p�\����
	{
		score_t first;		// �꓊�ڂ̃X�R�A [-] (0 - 10)
		score_t second;		// �񓊖ڂ̃X�R�A [-] (0 - 10)
		score_t third;		// �O���ڂ̃X�R�A [-] (0 - 10)
		EFrameScoreState state;	// �X�R�A�̏�� [-] (-)

		_frameScore()
			: first( 0u ), second( 0u ), third( 0u), state( EScoreST_NONE )
		{}
	} FrameScoreT;

protected:
	EFrameScoreState checkScoreState( const frame_t& frame, const FrameScoreT& score ) const;
	void updateScore();
	score_t getNextThrowScore( const frame_t& frame, const unsigned int& cnt, bool* flag ) const;

private:
	frame_t		m_frame;			// ���݂̃X�R�A�҂��̃t���[���ԍ� [-] (1 - 10)
	std::vector<FrameScoreT> m_frameScoreList;	// �t���[�����̃X�R�A [-] (-)

	frame_t		m_fixedFrame;		// �X�R�A���m�肵���t���[���ԍ� [-] (0 - 10);
	std::vector<score_t>	m_fixedScoreList;	// �m��X�R�A�̃��X�g [-] (-)
};
