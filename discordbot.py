import discord
import os
from discord.ext import commands
import youtube_dl
import yt_dlp
import asyncio #asynchronous functions can basiclaly run 2 calls at the same time without any delay
import google.generativeai as genai
from dotenv import load_dotenv #cus for some reason cudnt access .env file
generalheh = 1346143970992062548 #u can also do os.getenv and get it if u want to hide from user
# Load environment variables from .env file
load_dotenv()
genai.configure(api_key=os.getenv('GENAI_API_KEY'))  
model = genai.GenerativeModel("gemini-2.0-flash")

intents = discord.Intents.default()
intents.message_content = True  #ensures that bot can lisen to messages
intents.reactions = True  # Allows bot to track reactions
intents.members = True  # Allows bot to track members

client = discord.Client(intents=intents)  # Pass intents when creating the client + connection to discord

reminders = {} #hashmap for storing reminders
polls = {}
emoji_numbers = ["1️⃣", "2️⃣", "3️⃣", "4️⃣", "5️⃣", "6️⃣", "7️⃣", "8️⃣", "9️⃣", "🔟"] 


async def handle_reminder(userid, time,message, channel):
  await asyncio.sleep(time)
  await channel.send(f"Reminder for {userid}: {message}")
  del reminders[userid]


@client.event #to register an event
async def on_ready(): #to check if bot is ready to be used
  print('We have logged in as {0.user}'.format(client)) #0 gets replaced by client so its client.user
@client.event  
async def on_member_join(member):
  channel = client.get_channel(generalheh)  # Get the channel
  if channel:
      await channel.send(f"🎉 Welcome, {member.mention}! We're glad to have you here! 🎉")
@client.event
async def on_message(message): #triggers each time message is recieved
  if message.author == client.user:
    return

  text = message.content.lower()
  if (text.startswith('!ask')): #for gemini ap
    query = text.replace('!ask', '').strip()
    response = model.generate_content(query)
    fullresponse = response.text
    if len(fullresponse) >2000:
      summary = model.generate_content(f"Summarize this under 2000 characters: {fullresponse}").text
      await message.channel.send(summary)
    else:
      await message.channel.send(fullresponse)
     #if u dont give .text it will give u a response object

  elif text.startswith('!remindme'): #to set reminder
    #i need to make it print the reminder in the channel
    try:
      sentence = text.split(" ",2)
      if len(sentence) < 3:
        print("Usage: '!remindme <time in seconds> <reminder>'")
        return
      time = int(sentence[1])
     #print(time)
      reminder = sentence[2]
      #print(reminder)
      task = asyncio.create_task(handle_reminder(message.author.id, time, reminder, message.channel))
      reminders[message.author.id] = {"time": time, "message": reminder, "task": task}
      await message.channel.send(f"Reminder set for {time} seconds.")
    except ValueError:
      await message.channel.send("Invalid time format. Please use a valid integer.")

  elif text.startswith("!modify"):
    try:
      if message.author.id not in reminders:
        await message.channel.send("You have no reminders set.")
        return
      parts = text.split(" ", 2)
      if len(parts) < 3:
          await message.channel.send("Usage: `!modify <new time in seconds> <new reminder>`")
          return
      time = int(parts[1])
      new_reminder = parts[2]
      #del reminders[message.author.id] simply deleting wont work cus the task is running anyway so it wont make a difference
    
      #old_time, old_reminder = reminders[message.author.id]
      reminders[message.author.id]["task"].cancel()

      # Set new reminder
      new_task = asyncio.create_task(handle_reminder(message.author.id, time, new_reminder, message.channel))
      reminders[message.author.id] = {"message": new_reminder, "task": new_task}
      await message.channel.send(f"Reminder modified. New time: {time} seconds.")
    
      #await message.channel.send(f"Modified reminder: '{old_reminder}' to '{new_reminder}' in {time} seconds.")
    
    except ValueError:
        await message.channel.send("Invalid time format. Please use a valid integer.")

  elif text.startswith("!cancelreminder"): #to delete reminder
    if message.author.id in reminders:
      reminders[message.author.id]["task"].cancel()
      await message.channel.send("Reminder canceled.")
      await message.channel.send(f"Reminder set for {time} seconds.")
      asyncio.create_task(handle_reminder(message.author.id, time, reminder, message.channel))
    else:
      await message.channel.send("You don't have any active reminders.")

  elif text.startswith("!poll"):
    parts = message.content.split("|") #using.content cus messgae is an object probably
    if len(parts) < 3:
      await message.channel.send("Usage: `!poll <question> | <option1> | <option2> | ...` (max 10 options)")
      return
    question = parts[0].replace("!poll","").strip()
    options = [option.strip() for option in parts[1:]]
    if len(options) > 10:
      await message.channel.send("You can only have up to 10 options.")
      return
    poll_message = f"📊 **{question}**\n"
    for i, option in enumerate(options):
        poll_message += f"{emoji_numbers[i]} {option}\n"
    poll_msg = await message.channel.send(poll_message)
    # Add reactions for voting
    for i in range(len(options)):
        await poll_msg.add_reaction(emoji_numbers[i]) #this wont add reaction 1 to the question it dosent work like that so ye
    polls[poll_msg.id] = {"question": question, "options": options, "author": message.author.id} #stores data
  elif text.startswith("!endpoll"):
    parts = text.split(" ")
    if len(parts) < 2:
        await message.channel.send("Usage: `!result <poll message ID>`")
        return
    try:
      poll_id = int(parts[1])
      if poll_id not in polls:
          await message.channel.send("Invalid poll ID or poll not found.")
          return
      poll_message = await message.channel.fetch_message(poll_id)
      poll_data = polls[poll_id]

          # Count reactions
      results = []
      for i, option in enumerate(poll_data["options"]):
        reaction = discord.utils.get(poll_message.reactions, emoji=emoji_numbers[i])
        if reaction:
          results.append(f"{option}: {reaction.count - 1} votes")  # Subtract 1 to ignore bot's reaction

      result_text = f"📊 **Poll Results: {poll_data['question']}**\n" + "\n".join(results)
      await message.channel.send(result_text)

    except ValueError:
          await message.channel.send("Invalid poll ID format.")
    except discord.NotFound:
          await message.channel.send("Poll message not found.")
    



#code for setting up music bot!


class MusicBot:
    def __init__(self, bot):
        self.bot = bot
        self.queue = {}

    async def join(self, ctx):
        if ctx.author.voice: #chekcs if user is in vc
            await ctx.author.voice.channel.connect() #connects to tht vc
        else:
            await ctx.send("You need to be in a voice channel.")

    async def leave(self, ctx):
        if ctx.voice_client:
            await ctx.voice_client.disconnect()
        else:
            await ctx.send("I'm not in a voice channel.")

  
    async def play(self, ctx, url: str):
        if not ctx.author.voice:
            await ctx.send("You need to be in a voice channel to play music!")
            return
  
        if not ctx.voice_client:
            await ctx.author.voice.channel.connect()
  
        YDL_OPTIONS = {'format': 'bestaudio/best', 'noplaylist': 'True'}
        FFMPEG_OPTIONS = {'options': '-vn'}
  
        with yt_dlp.YoutubeDL(YDL_OPTIONS) as ydl:
            info = ydl.extract_info(url, download=False)
            url2 = info['url']
  
        ctx.voice_client.stop()
        ctx.voice_client.play(discord.FFmpegPCMAudio(url2, **FFMPEG_OPTIONS))

    def play_next(self, ctx):
        if ctx.guild.id in self.queue and self.queue[ctx.guild.id]:
            next_url = self.queue[ctx.guild.id].pop(0)
            asyncio.run_coroutine_threadsafe(self.play(ctx, next_url), self.bot.loop)

    async def add(self, ctx, url):
        if ctx.guild.id not in self.queue:
            self.queue[ctx.guild.id] = []
        self.queue[ctx.guild.id].append(url)
        await ctx.send(f"Added to queue: {url}")

    async def skip(self, ctx):
        if ctx.voice_client and ctx.voice_client.is_playing():
            ctx.voice_client.stop()
            await ctx.send("Skipped current song.")

    async def queue_list(self, ctx):
        if ctx.guild.id in self.queue and self.queue[ctx.guild.id]:
            queue_msg = "\n".join(self.queue[ctx.guild.id])
            await ctx.send(f"Current Queue:\n{queue_msg}")
        else:
            await ctx.send("Queue is empty.")

bot = commands.Bot(command_prefix="!", intents=intents)
music_bot = MusicBot(bot)

@bot.command()
async def join(ctx):
    await music_bot.join(ctx)

@bot.command()
async def leave(ctx):
    await music_bot.leave(ctx)

@bot.command()
async def play(ctx, url):
    await music_bot.play(ctx, url)

@bot.command()
async def add(ctx, url):
    await music_bot.add(ctx, url)

@bot.command()
async def skip(ctx):
    await music_bot.skip(ctx)

@bot.command()
async def queue(ctx):
    await music_bot.queue_list(ctx)

client = bot



client.run(os.getenv('TOKEN')) #takes token as parameter