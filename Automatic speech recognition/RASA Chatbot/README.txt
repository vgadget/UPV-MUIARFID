This is a bot capable of playing rock-paper-scissors, number guessing and hangman.

When it starts playing, the bot will wait for your greeting, then it will give you the options to play the game of your choice.

	-> Rock Paper and Scissors: It will ask you for a move and randomly calculate its own move.
	-> Number guessing game between 1 and 100: It will ask you for numbers until you find the hidden number.
	-> Hangman game: A word is proposed, you can offer letters or complete sequences of characters, or even the complete word if you know it.

It is also able to tweet: if you say to it 
	-> Tweet this <text>
	-> Tweet that <text>
	-> publish <text>
	-> etc
It will tweet what you tell it to the twitter account: @upvgamerbot

---- HOW TO START THE BOT -----
1. In the chatbot folder, open a new terminal and run:
	rasa run actions --cors "*" --debug
   this will start the custom actions server.

2. In the same chatbot folder, open another terminal and run
	rasa train
	rasa run -m models --enable-api --cors "*" --debug
    This way, we will train the chatbot and start it up. 


----------------------------------------------------------------------

Graphical interface based on: https://github.com/JiteshGaikwad/Chatbot-Widget
