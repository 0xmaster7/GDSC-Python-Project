# Python Bot

Firstly, we start by importing all the libraries we need for this project. The Gemini and Bot Tokens are stored in a ``.env`` file which will not be shared for security purposes.

We then choose the LLM we want to call. For this code, ``gemini-2.0-flash`` has been chosen. Intially, we set certain Intents to be ``True``. Intents are what the bot listens for. They also help improve performace by allowing the bot to listen for specific events. 

We define the ``on_ready()`` function to know if the bot has started succesfully when we run the code.

Then we define the ``on_message()`` function where we check if the message starts with 
                            - ``!ask`` 
                            - ``!remindme``
                            - ``!modify``
                            - ``!cancelreminder``
                            - ``!poll``

- ``!ask`` -> This is the command used for calling Gemini. Usage: ``!ask <question>``
- ``!remindme`` -> Command used for setting reminders. Usage: ``!remindme <time in seconds> <reminder>``
- ``!modify`` -> Command used for modifying reminders. Usage: 
