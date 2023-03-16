import unittest
import review2xml

class TestFunc(unittest.TestCase):
    def test_convert_review(self):
        input = """//image[dotween_inspector_game_object][[DOTween\] GameObject][scale=0.5]""".splitlines()
        expected = """<image dotween_inspector_game_object><bracket>DOTween</bracket> GameObject</image><parameter scale=0.5/><br/>""".splitlines()
        self._test(input, expected)

    def test_table_code(self):
        input = """//tsize[|latex||l|r|r|]
//table[activation_time][表示切り替えの処理時間]{
手法	処理時間（表示）	処理時間（非表示）
-------------------------------------------------------------
@<code>{SetActive}	323.79ms	209.93ms
@<code>{Canvas}の@<code>{enabled}	61.25ms	61.23ms
@<code>{CanvasGroup}の@<code>{alpha}	3.64ms	3.40ms
//}""".splitlines()
        expected = """<tsize |latex||l|r|r|/><br/>
<table activation_time>表示切り替えの処理時間</table><block><br/>
<td>手法</td><td>処理時間（表示）</td><td>処理時間（非表示）</td><br/>
-------------------------------------------------------------<br/>
<td><code>SetActive</code></td><td>323.79ms</td><td>209.93ms</td><br/>
<td><code>Canvas</code>の<code>enabled</code></td><td>61.25ms</td><td>61.23ms</td><br/>
<td><code>CanvasGroup</code>の<code>alpha</code></td><td>3.64ms</td><td>3.40ms</td><br/>
</block><br/>""".splitlines()
        self._test(input, expected)

    def test_unorderd_list(self):
        input = """ * 不要な描画領域を削減する
 ** テクスチャが完全に透明な領域も描画対象になるためなるべく減らす
 * オーバードローが発生する可能性のあるオブジェクトにはなるべく軽量なシェーダーを使用する
 * 半透明マテリアルはなるべく使用しない
 ** 不透明マテリアルで擬似的に半透明のような見た目を再現できる@<kw>{ディザリング}という手法も検討する""".splitlines()
        expected = """<ul1>不要な描画領域を削減する</ul1><br/>
<ul2>テクスチャが完全に透明な領域も描画対象になるためなるべく減らす</ul2><br/>
<ul1>オーバードローが発生する可能性のあるオブジェクトにはなるべく軽量なシェーダーを使用する</ul1><br/>
<ul1>半透明マテリアルはなるべく使用しない</ul1><br/>
<ul2>不透明マテリアルで擬似的に半透明のような見た目を再現できる<kw>ディザリング</kw>という手法も検討する</ul2><br/>""".splitlines()
        self._test(input, expected)

    def test_unorderd_list2(self):
        input = """ * フレームレート
 **	インゲームは60フレーム、アウトゲームはバッテリー消費の観点から30フレームとする。
 * メモリ
 ** 遷移時間の高速化のために、インゲーム中にアウトゲームの一部リソースも保持しておく設計とする。最大使用量を1GBとする。
 * 遷移時間
 ** インゲームやアウトゲームへの遷移時間は競合と同じレベルを目指す。時間にすると3秒以内。
 * 熱
 ** 競合と同じレベル。検証端末で連続1時間は熱くならない。（充電していない状態）
 * バッテリー
 ** 競合と同じレベル。検証端末で連続1時間でバッテリー消費は20%ほど。""".splitlines()
        expected = """<ul1>フレームレート</ul1><br/>
<ul2>インゲームは60フレーム、アウトゲームはバッテリー消費の観点から30フレームとする。</ul2><br/>
<ul1>メモリ</ul1><br/>
<ul2>遷移時間の高速化のために、インゲーム中にアウトゲームの一部リソースも保持しておく設計とする。最大使用量を1GBとする。</ul2><br/>
<ul1>遷移時間</ul1><br/>
<ul2>インゲームやアウトゲームへの遷移時間は競合と同じレベルを目指す。時間にすると3秒以内。</ul2><br/>
<ul1>熱</ul1><br/>
<ul2>競合と同じレベル。検証端末で連続1時間は熱くならない。（充電していない状態）</ul2><br/>
<ul1>バッテリー</ul1><br/>
<ul2>競合と同じレベル。検証端末で連続1時間でバッテリー消費は20%ほど。</ul2><br/>""".splitlines()
        self._test(input, expected)

    def test_table_br(self):
        input = """//table[animator_culling_mode][カリングモードの説明]{
種類	意味
--------------------
Always Animate	画面外にいても常に更新を行います。（デフォルト設定）
Cull Update Transform	画面外にいるときにIKやTransformの書き込みを行ないません。@<br>{}ステートマシンの更新は行います。
Cull Completely	画面外にいるとステートマシンの更新を行ないません。@<br>{}アニメーションが完全に止まります。
//}""".splitlines()
        expected = """<table animator_culling_mode>カリングモードの説明</table><block><br/>
<td>種類</td><td>意味</td><br/>
--------------------<br/>
<td>Always Animate</td><td>画面外にいても常に更新を行います。（デフォルト設定）</td><br/>
<td>Cull Update Transform</td><td>画面外にいるときにIKやTransformの書き込みを行ないません。<br></br>ステートマシンの更新は行います。</td><br/>
<td>Cull Completely</td><td>画面外にいるとステートマシンの更新を行ないません。<br></br>アニメーションが完全に止まります。</td><br/>
</block><br/>""".splitlines()
        self._test(input, expected)

    def test_column(self):
        input = """=====[column] バージョンによる表記違い
2020.2以降では「GC」と表示されていますが、2020.1以下のバージョンまでは「Mono」と記載されています。
どちらもマネージドヒープの占有量を指しています。
=====[/column]""".splitlines()
        expected = """<column5><title>バージョンによる表記違い</title><br/>
2020.2以降では「GC」と表示されていますが、2020.1以下のバージョンまでは「Mono」と記載されています。<br/>
どちらもマネージドヒープの占有量を指しています。<br/>
</column5><br/>""".splitlines()
        self._test(input, expected)

    def test_list_code_comment(self):
        input = """//listnum[material_dynamic][動的に生成したMaterialの削除例][csharp]{
Material material;

void Awake()
{
    material = new Material();  // マテリアルを動的生成
}

void OnDestroy()
{
    if (material != null)
    {
        Destroy(material);  // 使い終わったマテリアルをDestroy
    }
}
//}""".splitlines()
        expected = """<listnum material_dynamic>動的に生成したMaterialの削除例</listnum><parameter csharp/><block><br/>
<code>Material material;</code><br/>
<code></code><br/>
<code>void Awake()</code><br/>
<code>{</code><br/>
<code>    material = new Material();  // </code><comment>マテリアルを動的生成</comment><br/>
<code>}</code><br/>
<code></code><br/>
<code>void OnDestroy()</code><br/>
<code>{</code><br/>
<code>    if (material != null)</code><br/>
<code>    {</code><br/>
<code>        Destroy(material);  // </code><comment>使い終わったマテリアルをDestroy</comment><br/>
<code>    }</code><br/>
<code>}</code><br/>
</block><br/>""".splitlines()
        self._test(input, expected)

    def _test(self, input, expected):
        actual = review2xml.convert_xml(input)
        actual = "".join(actual).splitlines()
        self.assertEqual(expected, actual)

if __name__ == '__main__':
    unittest.main()