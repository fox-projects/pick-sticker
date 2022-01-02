#!/usr/bin/env bash
set -eo pipefail
shopt -s nullglob

die() {
	local msg=$1

	error "$msg"
	exit 1
}

error() {
	local msg=$1

	printf 'ERROR: %s\n' "$msg" >&2
}

main() {
	if ! command -v dwebp &>/dev/null; then
		die "Failed to find command 'dwebp'"
	fi

	cd downloads
	for dir in */; do
		if ! cd "$dir"; then
			error "Skipping directory '$dir'"
			continue
		fi

		for file in *.webp; do
			local dest="${file%.*}.png"
			if [ -e "$dest" ]; then
				continue
			fi

			echo "Converting file '$dir$file'"
			if ! convert "$file" "$dest"; then
				error "Skipping '$dir$file'"
			fi
		done

		cd ..
	done
}

main "$@"
