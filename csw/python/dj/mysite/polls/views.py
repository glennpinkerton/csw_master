from django.shortcuts import render
from datetime import datetime

# Create your views here.

from django.http import HttpResponse


# To show this view in a web browser, type localhost:8000/polls/index
# for the browser url.

def index(request):
    return HttpResponse("Hello, world. You're at the polls index.")


# The html template needs to be uniquely named for the site.
# A reasonable way to do that is to prepend the app name to
# the template file name.  Instead of hello.html, polls_hello.html.

# To show this view in a web browser, type localhost:8000/polls/hello
# for the browser url.

def hello(request):
   today = datetime.now().date()

   daysOfWeek = ['Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat', 'Sun']
   return render(request, "polls_hello2.html", {"today" : today,\
       "days_of_week" : daysOfWeek})

# To show this view in a web browser, type localhost:8000/polls/hello3
# for the browser url.

def hello3(request):
   today = datetime.now().date()

   daysOfWeek = ['Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat', 'Margred']
   return render(request, "polls_hello3.html", {"today" : today,\
       "days_of_week" : daysOfWeek})

# To show this view in a web browser, type localhost:8000/polls/aloha
# for the browser url.

def aloha(request):
   text = """<h2>Aloha my friend to the Polls App!</h2>"""
   return HttpResponse(text)


# To show this view in a web browser, type localhost:8000/polls/morning
# for the browser url.

def morning(request):
   text = """<h2>Top o the morning to ya Lad  from the Polls App</h2>"""
   return HttpResponse(text)

