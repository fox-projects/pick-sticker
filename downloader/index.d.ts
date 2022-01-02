type TgetStickerResultStickerThumb = {
	file_id: string
	file_unique_id: string
	file_size: number
	width: number
	height: number
}

type TgetStickerResultSticker = {
	width: number
	height: number
	emoji: string
	set_name: string
	is_animated: boolean
	thumb: TgetStickerResultStickerThumb
	file_id: string
	file_unique_id: string
	file_size: number
}

type TgetStickerResult = {
	name: string
	title: string
	is_animated: boolean
	contains_masks: boolean
	stickers: TgetStickerResultSticker[]
}

export type TgetStickerSetResponse = {
	ok: boolean
	result: TgetStickerResult
}

type StickerThumbType = {
	file_id: string
	file_unique_id: string
	file_size: number
	width: number
	height: number
}

type TgetFileResult = {
	file_id: string
	file_unique_id: string
	file_size: number
	file_path: string
}

export type TgetFileResponse = {
	ok: boolean
	result: TgetFileResult
}
