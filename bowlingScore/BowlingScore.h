//------------------------------------------------------------------------------
// ボーリングのスコア計算をするクラス定義
//------------------------------------------------------------------------------
#pragma once

#include <vector>

typedef unsigned short frame_t;		// フレーム番号用の型
typedef unsigned short score_t;		// スコア用の型

//------------------------------------------------------------------------------
// ボーリングのスコア計算をするクラス定義
//------------------------------------------------------------------------------
class BowlingScore
{
public:
	static const score_t LAST_FRAME_NUM = 10;	// 最後のフレーム番号 (10フレ)
	static const frame_t MAX_FRAME_NUM = 10;	// 最大のフレーム番号
	static const score_t MAX_FRAME_SCORE = 10;	// 一フレームのスコアの最大値

public:
	BowlingScore(void);
	virtual ~BowlingScore(void);

	bool putFrameScore( const score_t& first,
						const score_t& second,
						const score_t& third = 0 );

	score_t score( const frame_t& frame = 0 ) const;

protected:
	enum EFrameScoreState	// 1フレームのスコアの状態
	{
		EScoreST_NONE =	0		// 状態なし (初期値)
	,	EScoreST_OPEN			// オープン
	,	EScoreST_SPARE			// スペア
	,	EScoreST_STRIKE			// ストライク
	,	EScoreST_LAST_FRAME		// 最終フレームのスコア
	};

	typedef struct _frameScore		// 1フレームにおけるスコア用構造体
	{
		score_t first;		// 一投目のスコア [-] (0 - 10)
		score_t second;		// 二投目のスコア [-] (0 - 10)
		score_t third;		// 三投目のスコア [-] (0 - 10)
		EFrameScoreState state;	// スコアの状態 [-] (-)

		_frameScore()
			: first( 0u ), second( 0u ), third( 0u), state( EScoreST_NONE )
		{}
	} FrameScoreT;

protected:
	EFrameScoreState checkScoreState( const frame_t& frame, const FrameScoreT& score ) const;
	void updateScore();
	score_t getNextThrowScore( const frame_t& frame, const unsigned int& cnt, bool* flag ) const;

private:
	frame_t		m_frame;			// 現在のスコア待ちのフレーム番号 [-] (1 - 10)
	std::vector<FrameScoreT> m_frameScoreList;	// フレーム毎のスコア [-] (-)

	frame_t		m_fixedFrame;		// スコアが確定したフレーム番号 [-] (0 - 10);
	std::vector<score_t>	m_fixedScoreList;	// 確定スコアのリスト [-] (-)
};
