#include "BowlingScore.h"

#define RANGE_IN( num, min, max )	((min) <= (num) && (num) <= (max))

//------------------------------------------------------------------------------
// コンストラクタ
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
// デストラクタ
//------------------------------------------------------------------------------
BowlingScore::~BowlingScore(void)
{
}

//------------------------------------------------------------------------------
// １フレーム分のスコアの登録
// 引数: first    <I> - 一投目のスコア [-] (0 - 10)
//       second   <I> - 二投目のスコア [-] (0 - 10)
//       third    <I> - 三投目のスコア [-] (0 - 10)
// 戻り値: bool - 登録成否 [-] (true:成功, false:失敗)
//------------------------------------------------------------------------------
bool BowlingScore::putFrameScore(	const score_t& first,
									const score_t& second,
									const score_t& third )
{
	if ( m_frame > MAX_FRAME_NUM )		return false;

	// スコアの判定
	FrameScoreT score;

	score.first = first;
	score.second = second;
	score.third = third;

	score.state = checkScoreState( m_frame, score );
	if ( EScoreST_NONE == score.state )   return false;

	// スコアの登録
	m_frameScoreList[ m_frame ] = score;
	m_frame += 1;

	// スコアの更新
	updateScore();

	return 0;
}

//------------------------------------------------------------------------------
// スコアの取得
// 引数: frame    <I> - このフレームまでのスコアを取得 [-] (0 - 10)
// 戻り値: score_t - スコア [-] (0 - 300)
// 備考: frame = 0 または m_frameより大きい場合、m_frameまでのスコアを取得
//------------------------------------------------------------------------------
score_t BowlingScore::score( const frame_t& frame ) const
{
	frame_t getFrame = ( ( 0 == frame ) || ( frame > m_frame ) ) ? m_frame : frame;

	// リミットチェック
	getFrame = std::min<score_t>( MAX_FRAME_NUM, getFrame );
	getFrame = std::max<score_t>( 1u, getFrame );

	// スコアの取得
	score_t ret = 0u;

	if ( getFrame <= m_fixedFrame )	ret = m_fixedScoreList[ getFrame ];
	else {
		// m_fixedFrame ～ getFrame までのスコアを計算
		ret = m_fixedScoreList[ m_fixedFrame ];

		// スペアまたはストライクでボーナスが確定していない分を加算
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
// スコア状態の判定
// 引数: frame    <I> - フレーム番号 [-] (-)
//       score    <I> - 判定対象のスコア [-] (-)
// 戻り値: EFrameScoreState - 判定結果 [-] (-)
//------------------------------------------------------------------------------
BowlingScore::EFrameScoreState
	BowlingScore::checkScoreState( const frame_t& frame, const FrameScoreT& score ) const
{
	EFrameScoreState state = EScoreST_NONE;

	if ( RANGE_IN( frame, 1u, LAST_FRAME_NUM - 1) )
	{
		score_t total2 = score.first + score.second;

		// 1 ～ 9フレの場合
		if ( ( score.third != 0u ) ||				// ３投目が 0 ではない
			 ( score.first > MAX_FRAME_SCORE ) ||	// １投で 10 を超える
			 ( total2 > MAX_FRAME_SCORE ) )			// ２投の合計が 10を超える
		{
			state = EScoreST_NONE;	// エラー状態
		}
		else if ( score.first == MAX_FRAME_SCORE )	// １投目が 10
		{
			state = EScoreST_STRIKE;	// ストライク
		}
		else if ( total2 == MAX_FRAME_SCORE )		// ２投の合計が 10
		{
			state = EScoreST_SPARE;		// スペア
		}
		else
		{
			state = EScoreST_OPEN;		// オープン
		}
	}
	else if ( frame == LAST_FRAME_NUM )
	{
		score_t total12 = score.first + score.second;  // 1, 2投目の合計
		score_t total23 = score.second + score.third;  // 2, 3投目の合計

		// 10フレの場合
		state = EScoreST_LAST_FRAME;

		// エラー状態判定
		if ( ( score.first > MAX_FRAME_SCORE )	||	// １投で 10を超える
			 ( score.second > MAX_FRAME_SCORE ) ||	
			 ( score.third  > MAX_FRAME_SCORE ) )
		{
			state = EScoreST_NONE;
		}
		else if ( score.first == MAX_FRAME_SCORE )	// １投目がストライク
		{
			if ( ( score.second != MAX_FRAME_SCORE ) &&  // ２投目がストライクではなく
				 ( total23 > MAX_FRAME_SCORE ) )		 // 2, 3投目の合計が 10を超える
			{
				state = EScoreST_NONE;
			}
		}
		else if ( total12 > MAX_FRAME_SCORE )		// ２投の合計が 10を超える
		{
			state = EScoreST_NONE;
		}
		else if ( ( total12 < MAX_FRAME_SCORE ) &&	// 1, 2投でスペアでなく
				  ( score.third != 0u ) )			// ３投目が 0でない
		{
			state = EScoreST_NONE;
		}
	}

	return state;
}

//------------------------------------------------------------------------------
// スコアの更新
// 引数: なし
// 戻り値: なし
//------------------------------------------------------------------------------
void BowlingScore::updateScore()
{
	int fnum = m_fixedFrame + 1;	// スコア計算中のフレーム

	for(;;)
	{
		score_t frTotal = 0u;		// 1フレームのトータル
		score_t bonus   = 0u;		// ボーナス
		bool    isFixed = true;		// フレームの合計が確定するか [-] (true:確定, false:不確定)

		FrameScoreT& fScore = m_frameScoreList[ fnum ]; // 現在のフレームスコア

		switch( fScore.state )
		{
		case EScoreST_OPEN:
			frTotal = ( fScore.first + fScore.second );
			break;

		case EScoreST_SPARE:
			frTotal = MAX_FRAME_SCORE;
			bonus = getNextThrowScore( fnum, 1u, &isFixed );	// １投のボーナス
			break;

		case EScoreST_STRIKE:
			frTotal = MAX_FRAME_SCORE;
			bonus = getNextThrowScore( fnum, 2u, &isFixed );	// ２投のボーナス
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
			score_t preTotal = m_fixedScoreList[ fnum - 1 ];	// 前フレームまでのトータルスコア
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
// 次のN投の合計スコアの取得
// 引数: frame    <I> - フレーム番号 [-] (1 - 9)
//       cnt      <I> - 取得する投球数 [-] (0 - 2)
//       flag     <I> - cnt投球数分取得できたかどうか [-] (true:取得できた, false:取得できない)
// 戻り値: score_t - 次のN投の合計スコア [-] (-)
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

		// １投目の加算
		ret += frScore.first;
		remainCnt -= 1;
		if ( remainCnt <= 0 )	break;

		if ( EScoreST_STRIKE == frScore.state )		continue;

		// ２投目の加算
		ret += frScore.second;
		remainCnt -= 1;
		if ( remainCnt <= 0 )  break;

		if ( EScoreST_LAST_FRAME != frScore.state )	continue;

		// ３投目の加算
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
