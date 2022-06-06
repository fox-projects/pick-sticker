import fs from 'fs'
import path from 'path'
import http from 'https'

import fetch from 'node-fetch'
import dotenv from 'dotenv'
import c from 'ansi-colors'
import ora from 'ora'
import minimist from 'minimist'

function die(
	/** @type {string} */ msg,
	/** @type {Record<string, any>[] | undefined} */ ...data
) {
	error(msg, ...data)
	process.exit(1)
}

function error(
	/** @type {string} */ msg,
	/** @type {Record<string, any>[] | undefined} */ ...data
) {
	console.error(`${c.red('Error')}: ${msg}`)
	if (data !== void 0) {
		console.error(`${c.red('Data')}`, data)
	}
}

function info(/** @type {string} */ msg) {
	console.info(`${c.blue('Info')}: ${msg}`)
}

function spinnerFactory(/** @type {string} */ msg) {
	const spinner = ora(msg)
	spinner.start()

	return {
		stop() {
			spinner.stop()
			console.info(`✔️ ${msg}`)
		},
	}
}

async function doDiscord() {
	const discord_token = process.env.DISCORD_TOKEN
	if (!discord_token) {
		die("Variable 'DISCORD_TOKEN' is empty")
	}

	const guildId = '741433237263351889'
	const data = await fetch(`https://discordapp.com/api/v6/guilds/${guildId}`, {
		headers: {
			Authorization: discord_token,
		},
	})
	console.info(await data.json())
}

function chunkInto(
	/** @type Array<any> */ arr,
	/** @type {number} */ chunkSize
) {
	const res = []

	while (arr.length > 0) {
		res.push(arr.splice(0, chunkSize))
	}

	return res
}

async function mkdirp(/** @type {fs.PathLike} */ dir) {
	try {
		await fs.promises.mkdir(dir, { recursive: true })
	} catch (err) {
		if (err.code !== 'EEXIST') throw err
	}
}

/** @typedef {import('./index').TgetStickerSetResponse} TgetStickerSetResponse */
/** @typedef {import('./index').TgetFileResponse} TgetFileResponse */
async function downloadStickerSet(/** @type {string} */ stickerSetName) {
	/** @type {Promise<Response>[]} */
	let promises = []
	{
		const res = await fetch(
			`https://api.telegram.org/bot${telegram_token}/getStickerSet?name=${stickerSetName}`
		)
		/** @type {TgetStickerSetResponse} */
		const json = await res.json()
		if (!json.ok) {
			error("Fetch 'getStickerSet' failed", stickerSetName, json)
			return
		}

		for (const sticker of json.result.stickers) {
			const promise = fetch(
				`https://api.telegram.org/bot${telegram_token}/getFile?file_id=${sticker.file_id}`
			)
			promises.push(promise)
		}
	}

	let responses = await Promise.all(promises)
	/** @type {TgetFileResponse[]} */
	let fileObjects = await Promise.all(responses.map((item) => item.json()))

	// Now, we have each image to download of a particular sticker pack
	// Commence even more downloads!

	return fileObjects.map((item) => {
		return new Promise(async (resolve, reject) => {
			if (!item.ok) reject(`Item not ok: ${JSON.stringify(item, null, 2)}`)

			const fileName = path.basename(item.result.file_path)
			const filePath = path.join(DOWNLOADS_DIR, `${stickerSetName}/${fileName}`)
			try {
				await mkdirp(path.dirname(filePath))

				const file = fs.createWriteStream(filePath)
				const req = http.get(
					`https://api.telegram.org/file/bot${telegram_token}/${item.result.file_path}`,
					(res) => {
						if (res.statusCode !== 200)
							reject(`Item not ok: ${JSON.stringify(req, null, 2)}`)

						res.pipe(file)
					}
				)
				req.on('error', (err) => {
					fs.rm(filePath, (rmErr) => {
						reject(err)
					})
				})
				req.on('end', () => {
					resolve()
				})
			} catch (err) {
				fs.rm(filePath, (rmErr) => {
					reject(err)
				})
			}
		})
	})
}

/* ------------------------------------------------------ */
/*                          START                         */
/* ------------------------------------------------------ */
const DOWNLOADS_DIR = '../downloads'

/* Read dotenv */
dotenv.config()

const argv = minimist(process.argv.slice(2))
if (argv.discord) {
	await doDiscord()
	process.exit(1)
} else if (argv.telegram) {
} else {
	die('Must pass either --discord or --telegram')
}

const telegram_token = process.env.TELEGRAM_BOT_TOKEN
if (!telegram_token) {
	die("Variable 'TELEGRAM_BOT_TOKEN' is empty")
}

/* Test telegram_token */
{
	const s = spinnerFactory('Testing authentication token')
	const res = await fetch(`https://api.telegram.org/bot${telegram_token}/getMe`)
	s.stop()
	const json = await res.json()
	if (!json.ok) {
		die('Telegram authentication failed', json)
	}
}

/** @type {Array<string>} */
let stickerSets = []
{
	const s = spinnerFactory('Fetching list of all sticker sets')
	const res = await fetch('https://www.pulexart.com/stickers.html')
	s.stop()
	const text = await res.text()
	stickerSets = [
		...text.matchAll(/https:\/\/t\.me\/addstickers\/(?<id>.*?)['"]/gu),
	].map((item) => item.groups.id)
}

if (stickerSets.length === 0) {
	die("Could not retrieve list of sticker sets from Pulex's site")
}

info(`Downloading ${stickerSets.length} sticker sets...`)
for (const chunk of chunkInto(stickerSets, 20)) {
	const s = spinnerFactory(`Downloading ${JSON.stringify(chunk)}`)

	const promises = []
	for (const stickerSet of chunk) {
		// Skip if sticker set already exists
		try {
			await fs.promises.stat(path.join(DOWNLOADS_DIR, stickerSet))
			continue
		} catch (err) {
			if (err.code !== 'ENOENT') throw err
		}

		promises.concat(await downloadStickerSet(stickerSet))
	}

	await Promise.all(promises)

	s.stop()
}
info('Done.')
