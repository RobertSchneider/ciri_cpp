import spacy
nlp = spacy.load('en')
doc = nlp(u'ciri would you please create a file called test')

for w in doc:
    print str(w.i) + " " + w.text + " " + w.pos_ + " " + str(w.dep) + " " + w.dep_ + " " + w.head.text
