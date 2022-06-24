#!/usr/bin/env python3
from pathlib import Path
from PIL import Image
from colorama import init, Fore, Back, Style

init()

def info(msg: str) -> None:
    print(f"{Fore.BLUE}Info:{Fore.RESET} {msg}")

# -------------------------------------------------------- #
#                           START                          #
# -------------------------------------------------------- #
DOWNLOADS_DIR = '../downloads'
size = 150

for webp_image in Path(DOWNLOADS_DIR).rglob("*.webp"):
	png_image = str(webp_image.parent / webp_image.stem) + ".png"
	png_image_small = str(webp_image.parent / webp_image.stem) + "_small.png"

	info(f"Processing {webp_image}")
	with Image.open(webp_image) as im:
		if not Path(png_image).exists():
			im.save(png_image)

		# if not Path(png_image_small).exists():
		im.resize((size, size)).save(png_image_small)
