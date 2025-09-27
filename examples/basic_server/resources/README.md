# HTTP Server 動作確認用サイト

## 目的
- HTML/CSS/JS/画像/JSON の配信確認
- キャッシュ挙動確認（クエリでのバイパス）
- 404 ページの配信確認（サーバ側でのルーティング設定が必要）

## 構成
- `/index.html` トップページ
- `/about.html` 簡単な別ページ
- `/404.html` 404 用テンプレート（手動確認用）
- `/assets/css/styles.css` 最小スタイル
- `/assets/js/script.js` 動作確認用 JS
- `/assets/img/logo.svg` ロゴ
- `/assets/img/sample.png` テスト画像
- `/assets/img/favicon.png` Favicon
- `/version.json` 静的 JSON
- `/robots.txt`, `/sitemap.xml` 付帯ファイル

## 起動例
### Python
```bash
cd http_test_site
python -m http.server 8080
# http://127.0.0.1:8080/
```

### Node.js (serve)
```bash
npx serve . -l 8080
```

## 確認項目
1. `/` が表示される
2. CSS/JS が 200 で配信される
3. 「/version.json を取得」ボタンで JSON が表示される
4. 画像のボタンで同一 URL 再読込しキャッシュ挙動を観察
5. 存在しないパスにアクセスしサーバ側の 404 応答を確認
