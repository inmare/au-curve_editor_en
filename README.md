# Curve Editor v0.4.3 for AviUtl
![graph](https://user-images.githubusercontent.com/106879397/195975459-356f0a5f-0433-4f10-b4e4-0fda20c2e2d6.png)  

ベジェ曲線のイージングをウィンドウ上で編集できるAviUtlプラグインです。  

## ダウンロード
**v0.4.3**(Alpha版)：https://github.com/mimaraka/aviutl-plugin-curve_editor/releases/tag/v0.4.3  
※プリセット機能以外の機能が実装されています。

## インストール
同梱されている`curve_editor.auf`を`exedit.auf`のあるフォルダもしくは`plugins`フォルダ内に入れ、`@Curve Editor.tra`を`script`フォルダ**直下**に入れてください。

## 使用方法
AviUtlを起動後、メインウィンドウの「表示」メニューから「Curve Editorの表示」をクリックすると、Curve EditorのGUIが表示されます。

本プラグインでは、以下で紹介する「Valueモード」と「IDモード」の2つの編集モードでカーブを編集することができます。

- #### Valueモード
    Valueモードでは、カーブを1~10桁の整数で識別します。値とカーブがそれぞれ対応しています。

- #### IDモード
    IDモードでは、0~1023までのグラフのIDを使って、最大1024個のカーブをプロジェクトファイルに保存できます。  
    
    このモードでは、制御点を追加することでマルチベジェ軌道のように複雑なカーブを作成することもできます。  
    グラフ上の何もないところをダブルクリックすると制御点が追加され、制御点上で再度ダブルクリックすると削除されます。  

| | Valueモード | IDモード |
| :---: | :---: | :---: |
| マルチベジェ軌道 | × | 〇|
| カーブの一括変更 | × | 〇|
| exa, exo出力 | 〇 | ×|
| .flowファイル読み込み | 〇 | 〇|
| .flowファイル書き出し | 〇 | ×|
| .cepファイル読み込み | 〇 | 〇|
| .cepファイル書き出し | 〇 | 〇|
| プリセットパネルから直接D&D | 〇 | ×|
| その他 | 制御点のY座標に制限あり | カーブの個数に制限あり|

ウィンドウ左上のValue/IDスイッチまたは右クリックメニューの「編集モード」から2つのモードを切り替えることができます。

### グラフ
エディタパネルに表示されているグラフは、マウスの中央ボタンを押してドラッグすることで移動することができ、マウスホイールを回転または`Ctrl`キーを押しながら中央ボタンを押してドラッグすることで縮尺を変更することができます。

また、`Shift`や`Ctrl`、`Alt`のキーを押しながらハンドルをドラッグすると次のような操作をすることができます。(現在Valueモードのみ)  
この辺りの仕様はFlowとほぼ同じです。  

| `Shift` | `Alt` | `Ctrl` | `Shift`+`Ctrl` |
| :---: | :---: | :---: | :---: |
| 制御点をグラフ上下の辺にスナップ | ハンドルの角度を固定 | ハンドルの長さを固定 | 2つの制御点を同時に動かす |

### 値を読み取る
矢印マークのボタンをクリックし，変換後の値を入力するとグラフにカーブが反映されます。(Valueモードのみ)

### プリセットとして保存(未実装)
マークのボタンをクリックすると設定したカーブをプリセットとして保存でき，プリセットウィンドウから参照することができます。   
設定したプリセットは`curve_editor.bin`に保存されます。

### 右クリックメニュー
![rclickmenu](https://user-images.githubusercontent.com/106879397/197139725-c3f89eb0-d228-46be-bece-7a93ec52d239.png)

エディタパネル内で右クリックをすると次のようなオプションを選択できます：

- #### カーブの値をコピー(Valueモードのみ)
    カーブの値をクリップボードにコピーします。コピーボタンと動作は同じです。

- #### カーブの値を入力(Valueモードのみ)
    カーブの値を入力して、値からカーブに逆変換をすることができます。

- #### 保存(未実装)
    カーブをプリセットとして保存します。保存ボタンと動作は同じです。

- #### 値をコピー(4次元)
    カーブの値を`0.70,0.27,0.16,1.00`の形式でクリップボードにコピーします。

- #### カーブを反転
    カーブが、グラフの中心について点対称の形になります。つまり、EaseInのカーブはEaseOutに、EaseOutのカーブはEaseInになります。

### ショートカットキー
カーブ編集時には、以下のショートカットキーが使用できます：

- #### `A`キー
    ペアのハンドルとの角度を固定するかどうかを切り替えます。(IDモードのみ)

- #### `R`キー
    カーブを反転させます。

- #### `S`キー
    ハンドルの表示状態を切り替えます。

- #### `Ctrl`+`C`キー
    カーブの値をコピーします。(Valueモードのみ)

- #### `Ctrl`+`S`キー
    カーブをプリセットに保存します。(未実装)

- #### `Home`キー
    カーブをグラフウィンドウにフィットさせます。

- #### `<` / `>`キー
    前/次のIDのカーブに移動します。(IDモードのみ)

## プリセット(未実装)

## 設定
各種設定をダイアログから行うことができます。  
右クリックメニュー→設定からアクセスしてください。  

### 全般
- #### テーマ
    テーマをダーク / ライトの2種類から選ぶことができます。

- #### アラートを表示
    確認や警告等のポップアップウィンドウを表示します。  

- #### 変更前のカーブをグラフに表示
    1ステップ前のカーブの編集状態を表示します。

### 編集

- #### 値を自動でコピー(Valueモード)
    ハンドルを動かしたときにカーブの値が自動でクリップボードにコピーされます。



## その他
### Valueモードの注意点
- グラフでの制御点のY座標(グラフの下端を0、上端を1としたとき)は-2.74~3.74まで有効です。これより外の値は設定できません。  

- Valueモードではグラフの一括変更ができないため，カーブを使い回す場合はあらかじめ調整をしてからの方が良いかもしれません。  

- Valueモードでは小数点以下2桁刻みでカーブを設定するため，`.flow`ファイルを読み込む際，値が小数点以下3桁まで記述されている場合は四捨五入されます。  

### IDモードの注意点
- IDモードで設定したカーブは，エイリアス(`.exa`)やオブジェクトファイル(`.exo`)として出力しても保存されません。  

- IDモードでは，`.flow`ファイルの読み込みはできますが書き出しができない仕様となっています。  

- `Value@Curve Editor`はプラグインと連動していないため，プラグインを入れていなくても動作します。   

## 動作環境
| OS | AviUtl | 拡張編集 | 
| :---: | :---: | :---: |
| Windows 7~11 | ~1.10 | ~0.93rc(0.92推奨) |

【動作確認済み】  
Windows 11  
AviUtl 1.00, 1.10  
拡張編集 0.92  

## 免責事項
当プラグイン・スクリプトを使用したことによっていかなる損害が生じても，製作者は一切の責任を負わないものとします。  


