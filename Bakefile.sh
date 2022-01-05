# shellcheck shell=bash

task.convert() {
	bake.assert_cmd 'inkscape'

	cd ./assets/feather
	for svg_file in *.svg; do
		local png_file="${svg_file%.svg}.png"

		bake.info "Processing file '$svg_file'"

		rm -f "$png_file"
		inkscape -w 100 -h 100 "$svg_file" -o "$png_file"
	done
}
