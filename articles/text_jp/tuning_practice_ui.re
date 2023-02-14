={practice_ui} Tuning Practice - UI
Unity標準のUIシステムであるuGUIと、画面にテキストを描画する仕組みであるTextMeshProについて、
チューニングプラクティスを紹介します。

=={practice_ui_rebuild} Canvasの分割

uGUIでは@<code>{Canvas}内の要素に変化があったとき、
@<code>{Canvas}全体のUIのメッシュを再構築する処理（リビルド）が走ります。
変化とは、アクティブ切り替えや移動やサイズの変更など、
見た目が大きく変わるようなものから一見ではわからないような細かいものまで、あらゆる変更を指します。
リビルドの処理のコストは高いため、実行される回数が多かったり
@<code>{Canvas}内のUIの数が多かったりするとパフォーマンスに悪影響を及ぼします。

これに対して、ある程度のUIのまとまりごとに@<code>{Canvas}を分割することで、
リビルドのコストを抑えることができます。
たとえば、アニメーションで動くUIと何も動かないUIがあったとき、
それらを別の@<code>{Canvas}の下に配置することで、
アニメーションによるリビルドの対象となるものを最小限にできます。

ただし、@<code>{Canvas}を分割すると描画のバッチが効かなくなるため、
どのように分割すればよいかに関しては注意深く考える必要があります。

//info{

@<code>{Canvas}の分割は、@<code>{Canvas}の配下に@<code>{Canvas}を入れ子で配置する場合でも有効です。
子の@<code>{Canvas}に含まれる要素が変化しても、子の@<code>{Canvas}のリビルドが走るだけで親の@<code>{Canvas}のリビルドは走りません。
ただし詳しく確認したところ、@<code>{SetActive}によって子の@<code>{Canvas}内のUIをアクティブ状態に切り替えたときは事情が違うようです。
このとき、親の@<code>{Canvas}内にUIが大量に配置されている場合は高負荷になる現象があるようです。
なぜそのような挙動になるのかの詳細は分かりませんが、入れ子の@<code>{Canvas}内のUIのアクティブ状態を切り替えるときは注意が必要そうです。

//}

=={practice_ui_unity_white} UnityWhite

UIの開発をしていると、単純な長方形型のオブジェクトを表示したいということがよくあります。
そこで注意するべきなのが、UnityWhiteの存在です。
UnityWhiteは、@<code>{Image}コンポーネントや@<code>{RawImage}コンポーネントで
利用する画像を指定しなかったとき（@<img>{none_image}）に使われるUnity組み込みのテクスチャです。
UnityWhiteが使われている様子はFrame Debuggerで確認できます（@<img>{unity_white}）。
この仕組みを使うと白色の長方形を描画できるため、
これに乗算する色を組み合わせることによって単純な長方形型の表示を実現できます。

//image[none_image][UnityWhiteの利用][scale=0.8]
//image[unity_white][UnityWhiteが使われている様子][scale=0.8]

しかし、UnityWhiteはプロジェクトで用意したSpriteAtlasとは別のテクスチャであるため、
描画のバッチが途切れてしまうという問題が起こります。
これによって、ドローコールが増加し描画効率が悪化してしまいます。

そのため、SpriteAtlasに小さい（たとえば4 × 4ピクセルの）白色正方形の画像を追加し、
そのSpriteを利用して単純な長方形を描画するようにするべきです。
これによって、同じSpriteAtlasを使っていれば同一マテリアルになるため、バッチを効かせることができます。

=={practice_ui_layout_component} Layoutコンポーネント

uGUIにはオブジェクトをきれいに整列させるための機能を持つLayoutコンポーネントが用意されています。
たとえば縦方向に整列するなら@<code>{VerticalLayoutGroup}、
グリッド上に整列するなら@<code>{GridLayoutGroup}が使われます（@<img>{layout_group}）。

//image[layout_group][左側が@<code>{VerticalLayoutGroup}、右側が@<code>{GridLayoutGroup}を使った例][scale=0.99]

Layoutコンポーネントを利用すると、対象のオブジェクトを生成したときや、特定のプロパティを編集したときにLayoutのリビルドが発生します。
Layoutのリビルドも、メッシュのリビルドと同様にコストの高い処理です。

Layoutのリビルドによるパフォーマンス低下を避けるには、
Layoutコンポーネントを極力使わないというのが有効です。

たとえば、テキストの内容に応じて配置が変わるといった動的な配置が必要ないのであれば、
Layoutコンポーネントを使う必要がありません。
本当に動的な配置が必要な場合も、画面上で多く使用される場合などは、
独自のスクリプトで制御したほうがよい場合もあります。
また、親のサイズが変わっても親から見て特定の位置に配置したいという要件であれば、
@<code>{RectTransform}のアンカーを調整することで実現できます。
プレハブを作成するときに配置に便利だからという理由でLayoutコンポーネントを使った場合は、
必ず削除して保存するようにしましょう。

=={practice_ui_raycast_target} Raycast Target
@<code>{Image}や@<code>{RawImage}のベースクラスである@<code>{Graphic}には、
Raycast Targetというプロパティがあります（@<img>{raycast_target}）。
このプロパティを有効にすると、その@<code>{Graphic}がクリックやタッチの対象になります。
画面をクリックしたりタッチしたりしたとき、このプロパティが有効なオブジェクトが
処理の対象となるため、できる限りこのプロパティを無効にすることでパフォーマンスを向上できます。

//image[raycast_target][Raycast Targetプロパティ][scale=0.8]

このプロパティはデフォルトで有効ですが、
実際のところ多くの@<code>{Graphic}ではこのプロパティを有効にする必要がありません。
一方、Unityではプリセット@<fn>{preset}と呼ばれる機能があり、デフォルトの値をプロジェクトで変更することが可能です。
具体的には、@<code>{Image}コンポーネントと@<code>{RawImage}コンポーネントに対してそれぞれプリセットを作成し、
それをProject Settingsのプリセットマネージャーからデフォルトのプリセットとして登録します。
この機能を使ってRaycast Targetプロパティをデフォルトで無効にしてもよいかもしれません。

//footnote[preset][@<href>{https://docs.unity3d.com/ja/current/Manual/Presets.html}]

=={practice_ui_mask} マスク

uGUIでマスクを表現するには、@<code>{Mask}コンポーネントか@<code>{RectMask2d}コンポーネントを利用します。

@<code>{Mask}ではステンシルを利用してマスクを実現しているため、コンポーネントが増えるたびに描画コストが大きくなります。
それに対して@<code>{RectMask2d}はシェーダーのパラメーターでマスクを実現しているため描画コストの増加が抑えられています。
ただし、@<code>{Mask}は好きな形でくり抜ける一方、@<code>{RectMask2d}は長方形でしかくり抜けないという制約があります。

利用できるなら@<code>{RectMask2d}を選択するべきだというのが通説ですが、
最近のUnityでは@<code>{RectMask2d}の利用にも注意が必要です。

具体的には、@<code>{RectMask2d}が有効のとき、そのマスク対象が増えるに連れ、
それに比例して毎フレームカリングのCPU負荷が発生します。
UIを何も動かさなくても毎フレーム負荷が発生するこの現象は、uGUIの内部実装のコメントを見る限りUnity 2019.3で入ったとあるissue@<fn>{mask_issue}の修正の
副作用によるもののようです。
//footnote[mask_issue][@<href>{https://issuetracker.unity3d.com/issues/rectmask2d-diffrently-masks-image-in-the-play-mode-when-animating-rect-transform-pivot-property}]

そのため、@<code>{RectMask2d}も極力使わないようにする、
使ったとしても必要ない状態のときは@<code>{enabled}を@<code>{false}にする、
マスク対象は必要最低限にするなどの対策を取ることが有効です。

=={practice_ui_text_mesh_pro} TextMeshPro
TextMeshProでテキストを設定する一般的な方法は@<code>{text}プロパティにテキストを代入する方法ですが、
それとは別に@<code>{SetText}というメソッドを使う方法があります。

@<code>{SetText}には多くのオーバーロードが存在しますが、
たとえば文字列と@<code>{float}型の値を引数に取るものがあります。
このメソッドを @<list>{setText} のように利用すると、第2引数の値を表示できます。
ただし、@<code>{label}は@<code>{TMP_Text}（もしくはそれを継承した）型、
@<code>{number}は@<code>{float}型の変数であるとします。

//listnum[setText][SetTextの利用例][csharp]{
label.SetText("{0}", number);
//}

この方法の利点は、文字列の生成コストを抑えられるという点です。

//listnum[noSetText][SetTextを使わない例][csharp]{
label.text = number.ToString();
//}

@<list>{noSetText} のように@<code>{text}プロパティを使う方法では、
@<code>{float}型の@<code>{ToString()}が実行されるのでこの処理が実行されるたびに文字列の生成コストが発生します。
それに対して@<code>{SetText}を使った方法は、文字列を極力生成しないような工夫が行われているため、
とくに頻繁に表示するテキストが変わるような場合、パフォーマンス的に有利です。

またこのTextMeshProの機能は、ZString@<fn>{zstring}と組み合わせると非常に強力なものになります。
ZStringは文字列生成におけるメモリアロケーションを削減できるライブラリです。
ZStringは@<code>{TMP_Text}型に対する多くの拡張メソッドを提供しており、
それらのメソッドを使うことで文字列の生成コストを抑えつつ柔軟なテキスト表示を実現できます。

//footnote[zstring][@<href>{https://github.com/Cysharp/ZString}]

=={practice_ui_active} UIの表示切り替え

uGUIのコンポーネントは、@<code>{SetActive}によるオブジェクトのアクティブ切り替えのコストが大きいという特徴があります。
これは、@<code>{OnEnable}で各種リビルドのDirtyフラグを立てたり、マスクに関する初期化を行ったりしていることが原因です。
そのため、UIの表示非表示の切り替えの方法として、@<code>{SetActive}による方法以外の選択肢も検討することが重要です。

まず1つ目の方法は、@<code>{Canvas}の@<code>{enabled}を@<code>{false}にするという方法です（@<img>{canvas_disable}）。
これによって、@<code>{Canvas}配下のオブジェクトがすべて描画されなくなります。
そのためこの方法は、@<code>{Canvas}配下のオブジェクトを丸ごと非表示にしたい場合のみにしか使えないという欠点があります。

//image[canvas_disable][@<code>{Canvas}を無効にする][scale=0.8]

もう1つの方法は、@<code>{CanvasGroup}を使った方法です。
@<code>{CanvasGroup}には、その配下のオブジェクトの透明度を一括で調整できる機能があります。
この機能を利用して、透明度を0にしてしまえば、
その@<code>{CanvasGroup}配下のオブジェクトをすべて非表示にできます（@<img>{canvas_group}）。

//image[canvas_group][@<code>{CanvasGroup}の透明度を0にする][scale=0.8]

これらの方法は@<code>{SetActive}による負荷を避けることが期待できますが、
@<code>{GameObject}はアクティブ状態のままとなるため注意が必要な場合もあります。
たとえば@<code>{Update}メソッドが定義されている場合には、その処理は非表示の状態でも実行され続けるため、
思わぬ負荷の向上に繋がってしまうかもしれないことに気をつけましょう。

参考までに、@<code>{Image}コンポーネントを付けた1280個の@<code>{GameObject}に対して、
それぞれの手法で表示非表示の切り替えをしたときの処理時間を計測しました（@<table>{activation_time}）。
処理時間はUnityエディターで計測し、Deep Profileは用いていません。
実際に切り替えを行ったまさにその処理の実行時間@<fn>{activation_profile}と、
そのフレームでの@<code>{UIEvents.WillRenderCanvases}の実行時間を足し合わせたものを
その手法の処理時間としています。
@<code>{UIEvents.WillRenderCanvases}の実行時間を足し合わせているのは、
この中でUIのリビルドが実行されるためです。

//footnote[activation_profile][たとえば@<code>{SetActive}なら、@<code>{SetActive}メソッドを呼び出す部分を@<code>{Profiler.BeginSample}と@<code>{Profiler.EndSample}で囲って計測しています。]

#@# SetActive 210.41+113.38=323.79 208.24+1.69=209.93
#@# Canva 8.99+52.26=61.25 9.07+52.16=61.23
#@# CanvasGroup 0.95+2.69=3.64 0.97+2.43=3.40

//tsize[|latex||l|r|r|]
//table[activation_time][表示切り替えの処理時間]{
手法	処理時間（表示）	処理時間（非表示）
-------------------------------------------------------------
@<code>{SetActive}	323.79ms	209.93ms
@<code>{Canvas}の@<code>{enabled}	61.25ms	61.23ms
@<code>{CanvasGroup}の@<code>{alpha}	3.64ms	3.40ms
//}

@<table>{activation_time}の結果から、
今回試したシチュエーションではCanvasGroupを使った手法が圧倒的に処理時間が短いことがわかりました。
