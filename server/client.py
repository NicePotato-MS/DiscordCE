import config
import discord

client = discord.Client()

@client.event
async def on_ready():

    print(f'Logged on as {client.user}!')

@client.event
async def on_message(message: discord.Message):
    pass
    
async def start_client():
    await client.start(config.DISCORD_TOKEN, reconnect=True)
