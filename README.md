MMP-PMXViewer
=============

MikuMikuPenguinプロジェクトのPMXViewerの開発に注力するプロジェクト

当面の目標
==========

* PMXViewerのコードとlibmmpのコードを統合
* PMXViewerのリファクタリングおよび新機能の追加

ビルド方法
==========

1. リポジトリをクローン
2. cmake .
3. make

使い方
======

./pmx <pmxファイル> <vmdファイル>

依存ライブラリ
=============

* libsoil
* libglfw
* libglew
* libbullet
* libglm


フォーク元
==========

sn0w75/MikuMikuPenguin

注意事項
========

* MasterへのコミットはPullRequestのマージコミットだけ
* MikuMikuPenguinのライセンスを尊重して本プロジェクトもGPLv3で利用．

あとは煮るなり焼くなり好きにしてください．
