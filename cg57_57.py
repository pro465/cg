## Python Version 3.9.7
## There was no `golfing' tomfoolery involved.
## Forsake your place in   clementine  heaven.
## For eso-limes guessed a puzzle   is solved.
## Please keep handy the metaphorical  heaven
## (That  rose  out  of   Shakespeare's  hand)
## While d(r)iving into this  serpent's  land:
## https://www.gutenberg.org/cache/epub/100/pg100.txt


import random # We will be using random for **spIkiness** :D

def generate_markov_chain(data, len_ctx): # Self-explanatory.
    res = {}
    for i in range(len_ctx, len(data)):
        window=data[i-len_ctx:i]
        res.setdefault(window, {})
        res[window].setdefault(data[i], 0)
        res[window][data[i]] += 1

    for prev_chars, next_chars in res.items():
        res[prev_chars] = list(next_chars.items()), sum(next_chars.values())

    return res

def generate_next_ch(markov_chain): # Self-explanatory.
    prob, total = markov_chain
    c = random.randrange(total)
    for i, p in prob:
        if c < p: return i
        c-=p
    return

def generate_text(markov_chain, prompt, len_gen_text): # Self-explanatory.
    lookback = len(list(markov_chain.keys())[0])
    if len(prompt) < lookback: 
        return "Error: prompt not long enough"

    for i in range(len(prompt), len_gen_text+1):
        ctx = prompt[i-lookback:i]
        if ctx not in markov_chain:
            return f"Error: prompt '{ctx}' wasn't found *inside* Markov"

        c = generate_next_ch(markov_chain[ctx])
        if c is None: break
        prompt += c
    return prompt

with open("corpus.txt", mode="r", encoding="utf-8") as file:
    m=generate_markov_chain(file.read(), 5)
    file.close()

    while True:
        t=generate_text(m, input(), 2000)
        print(t)
