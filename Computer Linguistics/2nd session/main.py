from os import error
import nltk
from nltk.util import pr
# nltk.download()
# nltk.download('punkt')
from nltk.corpus import cess_esp
from nltk.tag import hmm
from nltk.tag import tnt
from random import shuffle
import matplotlib.pyplot as plt
import itertools

from numpy import sqrt


def split(a, n):
    k, m = divmod(len(a), n)
    return (a[i*k+min(i, m):(i+1)*k+min(i+1, m)] for i in range(n))


print("\n<-------------- EXERCISE 1 -------------->\n")

corpus_sentences = cess_esp.tagged_sents()

procesed_sentences = []

for sentence in corpus_sentences:

    words = []

    for word in sentence:

        if ("*0*" not in word[0]):

            lenght = 2

            if (word[1][0].lower() == "v" or word[1][0].lower() == "f"):
                lenght = 3

            lenght = min(lenght, len(word[1]))

            words.append((word[0], word[1][0:lenght]))

    procesed_sentences.append(words)

train_size = int(len(procesed_sentences)*0.9)

train = procesed_sentences[0:train_size]
test = procesed_sentences[train_size:len(procesed_sentences)]

print("Dataset preview: " + str(procesed_sentences[0][0:10]))

print(f"\nOriginal dataset size: {len(procesed_sentences)}")
print(f"Train dataset size: {len(train)}")
print(f"Test dataset size: {len(test)}")

print("\n<-------------- EXERCISE 2 -------------->\n")

hmm_tagger = hmm.HiddenMarkovModelTagger.train(train)
tnt_tagger = tnt.TnT()
tnt_tagger.train(train)

hmm_eval = hmm_tagger.evaluate(test)
tnt_eval = tnt_tagger.evaluate(test)

phrase = "Salió a saludarme el perro de tu padre"
tokens=nltk.word_tokenize(phrase,language='spanish')

print(f"HMM - Acc: {hmm_eval} ")
print(hmm_tagger.tag(tokens))
print(f"\nTnT - Acc: {tnt_eval} ")
print(tnt_tagger.tag(tokens))



