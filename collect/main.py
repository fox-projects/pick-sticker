import os
import re
import sys
from pathlib import Path
from typing import Callable

import requests
from dotenv import load_dotenv
from telethon import TelegramClient, errors, functions, types


def die(msg: str) -> None:
    print(f'Error: {msg}. Exiting')
    exit(1)

async def collectTelegramMessagesExec(client, fd):
    channelName = os.environ.get('CHANNEL_NAME', '')
    if not channelName:
        die('channelName must have a valid value')

    async for message in client.iter_messages(channelName, 99999999999999999999999):
        if message.sticker:
            # Each sent sticker message is associated with a sticker set
            stickerSetId = None
            for attr in message.document.attributes:
                if isinstance(attr, types.DocumentAttributeSticker):
                    stickerSetId = attr.stickerset
            if stickerSetId is None:
                die('Could not find stickerSetId in attributes')

            # Get the stickers in the sticker set
            try:
                stickers = await client(functions.messages.GetStickerSetRequest(
                    stickerset=stickerSetId
                ))
                print(f'Found: {stickers.set.title} ({stickers.set.count} total) (https://t.me/addstickers/{stickers.set.short_name})')
                fd.write(stickers.set.short_name + '\n')
                fd.flush()
            except (errors.rpcerrorlist.StickersetInvalidError, errors.rpcerrorlist.EmoticonStickerpackMissingError) as err:
                print(err)
            # await client.download_media(stickers.documents[0])

def collectTelegramMessages(fd):
    api_id = os.environ.get('API_ID', '')
    api_hash = os.environ.get('API_HASH', '')
    client = TelegramClient('session_name', api_id, api_hash)

    with client:
        client.loop.run_until_complete(collectTelegramMessagesExec(client, fd))

def collectPullexMessages(fd):
    res = requests.get('https://www.pulexart.com/stickers.html')
    regex = re.compile('https://t\.me/addstickers/(?P<id>.*?)[\'"]', re.UNICODE)
    for match in regex.findall(res.text):
        fd.write(match + '\n')

def collect(collectFn: Callable):
    collectFileName = '../common/collectOutput.txt'
    collectFile = Path(collectFileName)

    if collectFile.exists():
        die(f'File {collectFileName} already exists. Rename or delete to continue')

    with collectFile.open('a+') as fd:
        collectFn(fd)

if __name__ == '__main__':
    if Path(__file__).parent.name != 'collect':
        die("Must be ran in directory 'collect'")

    load_dotenv()

    subcmd = sys.argv[1]
    if subcmd == 'telegram':
        collect(collectTelegramMessages)
    elif subcmd == 'pulex':
        collect(collectPullexMessages)
    else:
        die('Subcommand not valid')

