import sys
import win32com.client as wincl
speak = wincl.Dispatch("SAPI.SpVoice")
speak.Speak(sys.argv[1])