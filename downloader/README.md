# pick-sticker-downloader

Contains a convenient downloader that pulls all the stickers available from [Pulex's site](https://www.pulexart.com). I wish to acknowledge [this sticker downloader](https://github.com/Cartmanishere/telegram-sticker-downloader) for making it easier to write this program

## Usage

1. Get a [Telegram Bot](https://core.telegram.org/bots) token

2. Paste it in `.env` (see `.env.example`)

3. Install [NodeJS](https://nodejs.org)

4. Run the scraper

```sh
npm install --global pnpm
pnpm install
node index.js
```

5. Convert files to PNG

```sh
./convert.sh
```
