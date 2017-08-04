import speech_recognition as sr

r = sr.Recognizer()
r.energy_threshold = 800

with sr.Microphone() as source:
	audio = r.listen(source)

try:
	print r.recognize_google(audio, language='en-AU')
except sr.UnknownValueError as er:
	pass