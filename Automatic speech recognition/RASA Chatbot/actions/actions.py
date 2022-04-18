from typing import Any, Text, Dict, List
import random
from matplotlib.pyplot import text
import tweepy
 
from rasa_sdk import Action, Tracker, FormValidationAction
from rasa_sdk.executor import CollectingDispatcher

from rasa_sdk.types import DomainDict
 
class ActionPlayRPS(Action):
   
    def name(self) -> Text:
        return "action_play_rps"
 
    def run(self, dispatcher: CollectingDispatcher, tracker: Tracker, domain: Dict[Text, Any]) -> List[Dict[Text, Any]]:
        
        # Obtener eleccion del usuario
        user_choice = tracker.get_slot("user_choice_rps")

        if (user_choice not in ["rock", "paper", "scissors"]):
            dispatcher.utter_message(text=f"{user_choice} is not a valid option!")
        else:
            #Generamos aleatoriamente la eleccion del bot
            cpu_choice = ["rock", "paper", "scissors"][random.randint(0,2)]

            # Comparamos que han sacado el usuario y el bot y damos veredicto
            result = "The bot, (I mean ME) I won!"
            if (user_choice == "rock" and cpu_choice == "scissors") or (user_choice == "paper" and cpu_choice == "rock") or (user_choice == "scissors" and cpu_choice == "paper"):
                result = "User won this round!"
            elif user_choice == cpu_choice:
                result =  "It was a Draw!"

            #Mostramos los movimientos de sendos jugadores y mostramos el veredicto
            dispatcher.utter_message(text=f"You type:  {user_choice}")
            dispatcher.utter_message(text=f"CPU: {cpu_choice}")
            dispatcher.utter_message(text=result)
    
        return []

class ActionPerformTweet(Action):
   
    def name(self) -> Text:
        return "action_perform_tweet"
 
    def run(self, dispatcher: CollectingDispatcher, tracker: Tracker, domain: Dict[Text, Any]) -> List[Dict[Text, Any]]:
        
        # Obtener eleccion del usuario
        texto = tracker.get_slot("txt_tw")

        # personal details
        consumer_key ="------CONSUMER_KEY-----"
        consumer_secret ="-----CONSUMER_SECRET-----"

        access_token ="-----ACCESS_TOKEN-----"
        access_token_secret ="-----ACCESS_TOKEN_SECRET-----"
        
        # authentication of consumer key and secret
        auth = tweepy.OAuthHandler(consumer_key, consumer_secret)
        
        # authentication of access token and secret
        auth.set_access_token(access_token, access_token_secret)
        api = tweepy.API(auth)
  
        dispatcher.utter_message(text=f"Sending: '{texto}' to twitter!")

        # Create a tweet
        api.update_status(texto)
    
        return []  

class ValidateHmForm(FormValidationAction):

    wordToGuess = ""
    wordGuessed = ""
   
    def name(self) -> Text:
        return "validate_hm_form"
 
    def validate_txt_hm(
        self,
        slot_value: Any,
        dispatcher: CollectingDispatcher,
        tracker: Tracker,
        domain: DomainDict,
    ) -> Dict[Text, Any]:
        """Validate `txt_hm` value."""

        if len(ValidateHmForm.wordToGuess) == 0:
            ValidateHmForm.wordToGuess = "GLASSES"
            ValidateHmForm.wordGuessed = " _ " * len(ValidateHmForm.wordToGuess)
            dispatcher.utter_message(text=f"I've prepared a new challenge for you!, try to guess this {len(ValidateHmForm.wordToGuess)} letter word!")
            dispatcher.utter_message(text=f"{ValidateHmForm.wordGuessed}")
            return {"txt_hm": None}

        texto =  tracker.get_slot("txt_hm").upper()
        ocurrences_indexes = [i for i in range(len(ValidateHmForm.wordToGuess)) if ValidateHmForm.wordToGuess.startswith(texto, i)]

        if len(ocurrences_indexes) == 0:
             dispatcher.utter_message(text=f"There are no matches for {texto}")
        else:
            dispatcher.utter_message(text=f"GOOD JOB, there are matches for {texto}")
            prevGuessed =  ValidateHmForm.wordGuessed.replace(" ", "")
            ValidateHmForm.wordGuessed = ""
            i = 0
            while i < len( ValidateHmForm.wordToGuess ):
                if i in ocurrences_indexes:
                    for j in range(0, len(texto)):
                        ValidateHmForm.wordGuessed += f" {texto[j]} "
                        i+=1
                elif (prevGuessed[i] == ValidateHmForm.wordToGuess[i]):
                    ValidateHmForm.wordGuessed += f" {prevGuessed[i]} "
                    i+=1
                else:
                    ValidateHmForm.wordGuessed += f" _ "
                    i+=1
                
                
        
        dispatcher.utter_message(text=f" Current word:  {ValidateHmForm.wordGuessed}")

        if (ValidateHmForm.wordGuessed.replace(" ", "") == ValidateHmForm.wordToGuess):
             ValidateHmForm.wordToGuess = ""
             dispatcher.utter_message(text=f"HURRAYYYY, you are correct!")
             return {"txt_hm": texto}
        else:
            return {"txt_hm": None}



class ValidateGnForm(FormValidationAction):

    number_to_guess = random.randint(1,100)
   
    def name(self) -> Text:
        return "validate_gn_form"
 
    def validate_number_gn(
        self,
        slot_value: Any,
        dispatcher: CollectingDispatcher,
        tracker: Tracker,
        domain: DomainDict,
    ) -> Dict[Text, Any]:
        """Validate `number_gn` value."""

        if  ValidateGnForm.number_to_guess == -1:
             dispatcher.utter_message(text=f"I've prepared a new challenge for you!, try to guess: ")
             ValidateGnForm.number_to_guess = random.randint(1,100)
             return {"number_gn": None}

        numero =  tracker.get_slot("number_gn")

        if int(numero) == ValidateGnForm.number_to_guess:
            dispatcher.utter_message(text=f"You are correct!, the number was: {numero}")
            ValidateGnForm.number_to_guess = -1
            return {"number_gn": numero}
        elif int(numero) > ValidateGnForm.number_to_guess:
            dispatcher.utter_message(text=f"Noooo!, you introduces {numero} but in fact, the real number is lower")
            return {"number_gn": None}
        elif int(numero) < ValidateGnForm.number_to_guess:
            dispatcher.utter_message(text=f"Noooo!, you introduces {numero} but in fact, the real number is grater")
            return {"number_gn": None}