from random import *

numberOfTricks = 1

numbers = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"}
colors = {"C","D", "H", "S" }
sides = ["N", "E", "S", "W"]
cards = []
for n in numbers:
    for c in colors:
        cards.append(n+c)
for i in range(0,numberOfTricks):
    shuffle(cards)
    shuffle(sides)
    print(str(randint(1,7))+ sides[0])
    for x in {0,13,26,39}:
        cardsStr = ""
        for c in cards[x:x+13]:
            cardsStr+=c
        print(cardsStr)

