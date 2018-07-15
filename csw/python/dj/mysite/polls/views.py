from django.shortcuts import render
from datetime import datetime

# Create your views here.

from django.http import HttpResponse


# To show this view in a web browser, type localhost:8000/polls/index
# for the browser url.

from .models import Question

def index(request):

#  latest_question_list is a normal python list of Question objects

    latest_question_list = Question.objects.order_by('-pub_date')[:5]

#  context is a map with 'tag': object pairs.  The html file tags
#  specify where the objects will be used.  See polls/index.html
#  for comments about the tag usaage.

    context = {'latest_question_list': latest_question_list}

#  The render method creates html from the template file (polls/index.html)
#  and the context map.

    return render(request, 'polls/index.html', context)


# The html template needs to be uniquely named for the site.
# A reasonable way to do that is to make a directory of the app name
# in the template directory.  Instead of hello.html, polls/hello.html.
# The template directory under the app (in this case the polls)
# directory is automatically searched for a match.

# To show this view in a web browser, type localhost:8000/polls/hello
# for the browser url.

def hello(request):
   today = datetime.now().date()

   daysOfWeek = ['Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat', 'Sun']
   return render(request, "polls/hello2.html", {"today" : today,\
       "days_of_week" : daysOfWeek})

# To show this view in a web browser, type localhost:8000/polls/hello3
# for the browser url.

def hello3(request):
   today = datetime.now().date()

   daysOfWeek = ['Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat', 'Margred']
   return render(request, "polls/hello3.html", {"today" : today,\
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



#def detail(request, question_id):
#    return HttpResponse("You're looking at question %s." % question_id)


# If the question id exists, send the question object
# to the render function.  The html template will be used 
# by render in conjunction with the question object to 
# return an HttpResponse object.

def detail(request, question_id):
    try:
        question = Question.objects.get(pk=question_id)
    except Question.DoesNotExist:
        raise Http404("Question does not exist")
    return render(request, 'polls/detail.html', {'question': question})





def results(request, question_id):
    response = "You're looking at the results of question %s."
    return HttpResponse(response % question_id)

def vote(request, question_id):
    return HttpResponse("You're voting on question %s." % question_id)
