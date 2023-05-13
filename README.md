# TraningTracer_Sample_code_for_RoboTracer

著者が制作した，株式会社アールティ社の教材用ロボットである[トレーニングトレーサー](https://rt-net.jp/products/rt-tracer/)の走行用プログラムです．
マイクロマウスコンテストで開催されるロボトレース競技に参加できるコードが一通り記載してあります．

![tracer](https://rt-net.jp/wp-content/uploads/2020/05/RT-Tracer.png)  
（写真：アールティ社WEBページより）

## 開発環境
STM32CubeIDE 1.7.0

## フォルダ・ソフト構成
* Core  
　各種コード
* Debug  
　ビルド済みファイル同胞
* Drivers  
* 他  
  STM32CubeIDE 1.7.0の構成に準じる

## キットからのハード改造点
マイコンのハードウェア機能としてPWMを使用できるようにするため  
PA11 -> DRV8835 AIN2接続  
PA12 -> ブザー接続  
↓（パターンカットと配線で入れ替え改造）  
PA11 -> ブザー接続  
PA12 -> DRV8835 AIN2接続 

## 元となったソフト構成
オープンとなっているマイクロマウスキット　Zirconia　の下位層のソフト構成をベースとしている．　　
https://bitbucket.org/kohiro/zirconia_sample/src/master/    
https://drive.google.com/file/d/1Cd2tDbJwFvDJpaGqB1d-GE1OD7vOHeNP/view


## ライセンス

(C) 2020 RT Corporation \<support@rt-net.jp\>
各ファイルはライセンスがファイル中に明記されている場合、そのライセンスに従います。特に明記されていない場合は、Apache License, Version 2.0に基づき公開されています。  
ライセンスの全文は[LICENSE](./LICENSE)または[https://www.apache.org/licenses/LICENSE-2.0](https://www.apache.org/licenses/LICENSE-2.0)から確認できます。
