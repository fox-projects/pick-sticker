# shellcheck shell=bash

task.download() {
	bake.assert_cmd 'node'

	cd ./downloader
	node index.js
}

task.generate_sizes() {
	bake.assert_cmd 'poetry'

	cd ./resizer
	poetry run ./main.py
}
