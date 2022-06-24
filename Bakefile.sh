# shellcheck shell=bash

task.browsec() {
	cd ./browser-c
	make
}

task.browseweb() {
	cd ./browser-web
	pnpm run dev
}

task.collect() {
	cd ./tools/collect
	poetry run python main.py "$@"
}

task.download() {
	cd ./tools/downloader
	node index.js
}

task.generate_sizes() {
	cd ./tools/resizer
	poetry run ./main.py
}
