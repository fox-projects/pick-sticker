# shellcheck shell=bash

task.collect() {
	cd collect
	poetry run python main.py "$@"
}

task.download() {
	cd ./downloader
	node index.js
}

task.generate_sizes() {
	cd ./resizer
	poetry run ./main.py
}
