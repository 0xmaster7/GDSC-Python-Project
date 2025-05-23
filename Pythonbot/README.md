# Discord Bot – Features & Commands

This Discord bot includes powerful utilities such as reminders, polls, and an AI-powered question-answering system using Google's Gemini API. Below is a breakdown of the available commands and how to use them.

---

## 🧠 AI Chat Assistant (`!ask`)

### Description:
Interact with Google's **Gemini 2.0 Flash** model to ask questions or request summaries.

### Usage:
```
!ask <your question>
```

### Example:
```
!ask What are black holes?
```

### Notes:
- If the response exceeds Discord's 2000 character limit, the bot automatically summarizes it.

---

## ⏰ Reminder System

### Set a Reminder – `!remindme`

**Description:** Set a reminder that will notify you in the same channel after a given time in seconds.

**Usage:**
```
!remindme <time in seconds> <reminder message>
```

**Example:**
```
!remindme 60 Drink water
```

---

### Modify a Reminder – `!modify`

**Description:** Modify an existing reminder's time and message.

**Usage:**
```
!modify <new time in seconds> <new reminder message>
```

**Example:**
```
!modify 120 Take a break
```

> Only the most recent reminder can be modified per user.

---

### Cancel a Reminder – `!cancelreminder`

**Description:** Cancel your active reminder.

**Usage:**
```
!cancelreminder
```

---

## 📊 Poll System

### Create a Poll – `!poll`

**Description:** Create a simple reaction-based poll with up to 10 options.

**Usage:**
```
!poll <question> | <option 1> | <option 2> | ... (max 10 options)
```

**Example:**
```
!poll What's your favorite color? | Red | Blue | Green
```

**Result:**
The bot will display a poll message with reactions like `1️⃣`, `2️⃣`, `3️⃣` corresponding to options.

---

### End a Poll – `!endpoll`

**Description:** End an active poll and show the results.

**Usage:**
```
!endpoll <poll message ID>
```

**Example:**
```
!endpoll 123456789012345678
```

**How to get Poll Message ID:**
- Enable Developer Mode in Discord (User Settings → Advanced).
- Right-click the poll message → Copy Message ID.

---

## ⚙️ Setup & Environment

### Required `.env` Variables:
- `GENAI_API_KEY` — Your Google Gemini API Key.
- `TOKEN` — Discord bot token.

---

## 💬 Sample Interaction

**User:** `!remindme 30 Check the oven`  
**Bot:** `Reminder set for 30 seconds.`  
*(30 seconds later...)*  
**Bot:** `Reminder for @user: Check the oven`

---

## 🤖 Technologies Used

- `discord.py` – Discord API wrapper
- `google.generativeai` – Gemini AI integration
- `asyncio` – Non-blocking reminders
- `dotenv` – Environment variable management

---